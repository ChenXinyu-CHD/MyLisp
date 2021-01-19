#ifndef MY_LISP_TOKEN_HPP
#define MY_LISP_TOKEN_HPP

#include <string>
#include <memory>
#include <variant>
#include <exception>
#include <stack>
#include <sstream>

struct Token {
	using Number = double;
	enum Type {
		NUM, LB, RB, ID, END, DEF, IF, LAM
	};
	Type type;
	std::variant<Number, std::string> val;
	static Token Num(Number val) {
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
	Token(Type t, Number val) : type(t), val(val) {}
	Token(Type t, std::string val) : type(t), val(val) {}
};

struct LexErr : std::exception {
	LexErr(char const* const what) : std::exception(what) {}
};

class Lexer final {
private:
	std::istream& in;
	std::stack<char> brackets;
	Token token;
public:
	Lexer(std::istream& in);
	const Token& now() const;
	const Token& next();
};

#endif // !MY_LISP_TOKEN_HPP
