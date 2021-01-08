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
		NUM, LB, RB, ID, END
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
	static Token Id(std::string val) {
		return Token(ID, val);
	}
	static Token End() {
		return Token(END, 0);
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

/*
enum class ElemType { atom, list };

struct Elem {
	virtual ElemType type() const = 0;
	virtual ~Elem() = default;
};

using ElemPtr = std::shared_ptr<Elem>;

struct Atom final : Elem {
	Atom(
		std::string::const_iterator& begin,
		const std::string::const_iterator& end
	);
	ElemType type() const override {
		return ElemType::atom;
	}
	std::string val;
	Atom() = default;
};

struct List final : Elem {
	List(
		std::string::const_iterator& begin,
		const std::string::const_iterator& end
	);
	ElemType type() const override {
		return ElemType::list;
	}
	std::deque<ElemPtr> val;
	List() = default;
};

std::shared_ptr<List> parse_file(const std::string& code);
*/

#endif // !MY_LISP_TOKEN_HPP