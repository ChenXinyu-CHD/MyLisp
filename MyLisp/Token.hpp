#ifndef MY_LISP_TOKEN_HPP
#define MY_LISP_TOKEN_HPP

#include <string>
#include <memory>
#include <variant>
#include <exception>
#include <stack>
#include <sstream>

struct Token {
	enum Type {
		NUM, LB, RB, ID, END, DEF, IF, LAM
	};
	Type type;
	std::variant<int, std::string> val;
	static Token Num(int val) {
		return Token(NUM, val);
	}
	static Token Lb() {
		return Token(LB, 0);
	}
	static Token Rb() {
		return Token(RB, 0);
	}
	static Token Id(std::string&& val) {
		return Token(ID, val);
	}
	static Token End() {
		return Token(END, 0);
	}
	static Token Key(Type t) {
		return Token(t, 0);
	}
private:
	Token(Type t, int val) : type(t), val(val) {}
	Token(Type t, std::string val) : type(t), val(val) {}
};

struct LexErr : std::exception {
	LexErr(char const* const what) : std::exception(what) {}
};

class Lexer final {
private:
	std::istringstream in;
	std::stack<char> brackets;
	Token token;
public:
	Lexer(std::string code);
	Token now() const;
	Token next();
};

#endif // !MY_LISP_TOKEN_HPP
