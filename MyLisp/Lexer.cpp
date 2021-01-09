#include "Token.hpp"

#include <sstream>
#include <iostream>
#include <stack>
#include <map>

using namespace std;

Lexer::Lexer(string code):
	in(code), token(Token::End()) {
	this->next();
}

Token Lexer::now() const {
	return token;
}

namespace {
	bool is_com(char ch) {
		return ch == ';';
	}

	void skip_useless(istream& in) {
		while (in) {
			if (is_com(in.peek())) {
				while (in && in.get() != '\n')
					continue;
			} else if (isspace(in.peek())){
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

Token Lexer::next() {
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
			int val = 0;
			in >> val;
			token = Token::Num(val);
		} else {
			string str;
			while (
				!isspace(in.peek()) &&
				!is_lb(in.peek()) &&
				!is_rb(in.peek()) &&
				!is_com(in.peek())
			) {
				str += in.get();
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
