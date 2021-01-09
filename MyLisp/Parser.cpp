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
			throw ParseErr("�﷨�����޷����������");
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
			throw ParseErr("�﷨�����޷����������");
		}
	}

	// ���������ڵ�ʣ�����У�����������ֹͣ����
	// ���迼��û�������ŵ�����µ����β�ַ�
	// lexer���϶���������������Ų�ƥ�䣬���׳��쳣
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
		// lexer�ᴦ�����Ų�ƥ��Ĵ���
		// �˴����迼��û�������ŵ�����µ����β
		while (token.type != Token::RB) {
			if (token.type != Token::ID) {
				throw ParseErr("�﷨���󣺺����ββ��ǺϷ��ı�ʶ��");
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
			throw ParseErr("�﷨���󣺺��������ǺϷ��ı�ʶ��");
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
			throw ParseErr("�﷨���󣺶������ĸ�ʽ����");
		}
		if (lexer.now().type != Token::RB) {
			throw ParseErr("�﷨����ȱ�����ĩβ������");
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
			throw ParseErr("�﷨����ȱ�����ĩβ������");
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
			throw ParseErr("�﷨����ȱ�����ĩβ������");
		} else {
			lexer.next();
			return result;
		}
	}

	Ptr<Lambda> parse_lam(Lexer& lexer) {
		if (lexer.next().type != Token::LB) {
			throw ParseErr("�﷨����Lambda���ȱ���β��б�");
		}
		lexer.next();
		auto args = parse_args(lexer);
		auto body = parse_atoms_seq(lexer);
		auto result = make_shared<Lambda>(
			move(args),move(body)
		);
		if (lexer.now().type != Token::RB) {
			throw ParseErr("�﷨����ȱ�����ĩβ������");
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
			throw ParseErr("�﷨����ȫ����������ֻ�ܴ��ں�����������������䡣");
		} else {
			result.push_back(parse_def(lexer));
		}
	}

	return result;
}
