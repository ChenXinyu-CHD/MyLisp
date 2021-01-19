#include "Token.hpp"

#include <istream>
#include <stack>
#include <map>

using namespace std;

Lexer::Lexer(istream& in):
	in(in), token(Token::End()) {
	this->next();
}

const Token& Lexer::now() const {
	return token;
}

namespace {
	bool is_com(char ch) {
		return ch == ';';
	}

	void skip_useless(istream& in) {
		while (in) {
			char ch = in.peek();
			if (is_com(ch)) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			} else if (isspace(ch)){
				while (in && isspace(in.peek()))
					in.get();
			} else {
				break;
			}
		}
	}

	bool is_lb(char ch) {
		return ch == '[' || ch == '(' || ch == '{';
	}

	bool is_rb(char ch) {
		return ch == ']' || ch == ')' || ch == '}';
	}

	bool match(char l, char r) {
		return	(l == '(' && r == ')') ||
				(l == '[' && r == ']') ||
				(l == '{' && r == '}');
	}
}

const Token& Lexer::next() {
	skip_useless(in);
	if (!in) {
		token = Token::End();
		if (!brackets.empty())
			throw LexErr("´Ê·¨´íÎó£ºÀ¨ºÅ²»Æ¥Åä");
	} else {
		auto ch = in.peek();
		if (is_lb(ch)) {
			brackets.push(ch);
			in.get();
			token = Token::Lb();
		} else if (is_rb(ch)) {
			if (brackets.empty() || !match(brackets.top(), ch)) {
				throw LexErr("´Ê·¨´íÎó£ºÀ¨ºÅ²»Æ¥Åä");
			} else {
				brackets.pop();
				in.get();
				token = Token::Rb();
			}
		} else if (isdigit(ch)) {
			Token::Number val = 0;
			in >> val;
			token = Token::Num(val);
		} else {
			string str;
			while (
				!isspace(ch) &&
				!is_lb(ch) &&
				!is_rb(ch) &&
				!is_com(ch)
			) {
				str += in.get();
				ch = in.peek();
			}
			static const map<string, Token::Type> keys{
				{"if", Token::IF},
				{"define", Token::DEF},
				{"lambda", Token::LAM},
			};
			if (keys.count(str) != 0) {
				token = Token::Key(keys.at(str));
			} else {
				token = Token::Id(move(str));
			}
		}
	} 

	return token;
}
