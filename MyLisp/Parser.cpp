#include "Grammer.hpp"

using namespace std;

template<typename T>
using Ptr = shared_ptr<T>;

namespace {
	Ptr<Defination> parse_def(Lexer& lexer);
	Ptr<Calling> parse_call(Lexer& lexer);
	Ptr<Condition> parse_if(Lexer& lexer);
	Ptr<Lambda> parse_lam(Lexer& lexer);

	Ptr<Grammer> parse_tuple(Lexer& lexer) {
		Token token = lexer.now();
		switch (token.type) {
		case Token::LB: case Token::ID:
			return parse_call(lexer);
		case Token::DEF:
			return parse_def(lexer);
		case Token::IF:
			return parse_if(lexer);
		case Token::LAM:
			return parse_lam(lexer);
		default:
			throw ParseErr("语法错误：无法解析的语句");
		}
	}

	Ptr<Grammer> parse_atom(Lexer& lexer) {
		Token token = lexer.now();
		lexer.next();
		switch (token.type) {
		case Token::NUM:
			return make_shared<Constant>(get<int>(token.val));
		case Token::ID:
			return make_shared<Variable>(get<string>(move(token).val));
		case Token::LB:
			return parse_tuple(lexer);
		default:
			throw ParseErr("语法错误：无法解析的语句");
		}
	}

	// 解析括号内的剩余序列，遇到右括号停止解析
	// 无需考虑没有右括号的情况下到达结尾字符
	// lexer会认定此种情况属于括号不匹配，并抛出异常
	vector<Ptr<Grammer>> parse_atoms_seq(Lexer& lexer) {
		vector<Ptr<Grammer>> result;
		while (lexer.now().type != Token::RB)
			result.push_back(parse_atom(lexer));
		return result;
	}

	Ptr<Defination> parse_def_var(Lexer& lexer) {
		string name = move(get<string>(lexer.now().val));
		lexer.next();
		return make_shared<Defination>(move(name), parse_atom(lexer));
	}

	vector<string> parse_args(Lexer& lexer) {
		Token token = lexer.now();
		vector<string> result;
		// lexer会处理括号不匹配的错误
		// 此处无需考虑没有右括号的情况下到达结尾
		while (token.type != Token::RB) {
			if (token.type != Token::ID) {
				throw ParseErr("语法错误：函数形参不是合法的标识符");
			} else {
				result.push_back(get<string>(lexer.now().val));
			}
			token = lexer.next();
		}
		lexer.next();
		return result;
	}

	Ptr<Defination> parse_def_fun(Lexer& lexer) {
		string name;
		Token token = lexer.next();
		if (token.type != Token::ID) {
			throw ParseErr("语法错误：函数名不是合法的标识符");
		} else {
			name = move(get<string>(token.val));
		}

		lexer.next();
		auto args = parse_args(lexer);
		auto body = parse_atoms_seq(lexer);
		return make_shared<Defination>(
			move(name),
			make_shared<Lambda>(
				move(args),
				move(body)
			)
		);
	}
	 
	shared_ptr<Defination> parse_def(Lexer& lexer) {
		Ptr<Defination> result;
		switch (lexer.next().type) {
		case Token::ID:
			result = parse_def_var(lexer);
			break;
		case Token::LB:
			result = parse_def_fun(lexer);
			break;
		default:
			throw ParseErr("语法错误：定义语句的格式错误");
		}
		if (lexer.now().type != Token::RB) {
			throw ParseErr("语法错误：缺少语句末尾的括号");
		} else {
			lexer.next();
			return result;
		}
	}

	Ptr<Calling> parse_call(Lexer& lexer) {
		// lexer.next();
		Ptr<Grammer> callable = parse_atom(lexer);
		Token token = lexer.now();
		vector<Ptr<Grammer>> params = parse_atoms_seq(lexer);
		if (lexer.now().type != Token::RB) {
			throw ParseErr("语法错误：缺少语句末尾的括号");
		} else {
			lexer.next();
			return make_shared<Calling>(move(callable), move(params));
		}
	}

	Ptr<Condition> parse_if(Lexer& lexer) {
		lexer.next();
		auto condition = parse_atom(lexer);
		auto onTrue = parse_atom(lexer);
		auto onFalse = parse_atom(lexer);
		auto result = make_shared<Condition>(
			move(condition),
			move(onTrue),
			move(onFalse)
		);
		if (lexer.now().type != Token::RB) {
			throw ParseErr("语法错误：缺少语句末尾的括号");
		} else {
			lexer.next();
			return result;
		}
	}

	Ptr<Lambda> parse_lam(Lexer& lexer) {
		if (lexer.next().type != Token::LB) {
			throw ParseErr("语法错误：Lambda语句缺少形参列表");
		}
		lexer.next();
		auto args = parse_args(lexer);
		auto body = parse_atoms_seq(lexer);
		auto result = make_shared<Lambda>(
			move(args),move(body)
		);
		if (lexer.now().type != Token::RB) {
			throw ParseErr("语法错误：缺少语句末尾的括号");
		} else {
			lexer.next();
			return result;
		}
	}
}

vector<Ptr<Defination>> parse(Lexer& lexer) {
	vector<Ptr<Defination>> result;
	while (
		lexer.now().type != Token::END &&
		lexer.now().type == Token::LB
	) {
		auto token = lexer.next();
		if (token.type != Token::DEF) {
			throw ParseErr("语法错误：全局作用域内只能存在函数定义或变量定义语句。");
		} else {
			result.push_back(parse_def(lexer));
		}
	}

	return result;
}
