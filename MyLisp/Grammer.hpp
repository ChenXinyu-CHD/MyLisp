#ifndef MY_LISP_GRAMMER_HPP
#define MY_LISP_GRAMMER_HPP

#include "Token.hpp"
#include <vector>

struct Grammer {
	enum Type {
		def, lamb, cond, call, cons, var,
	};
	using Ptr = std::shared_ptr<Grammer>;
	Type type;
	Grammer(Type t) : type(t) {}
	virtual ~Grammer() = default;
};

struct Defination final : Grammer {
	std::string name;
	Ptr value;
	Defination(std::string&& name, Ptr&& value) :
		Grammer(def), name(name), value(value) {
	}
};

struct Lambda final : Grammer {
	std::vector<std::string> args;
	std::vector<Ptr> body;
	Lambda(std::vector<std::string>&& args, std::vector<Ptr>&& body) :
		Grammer(lamb), args(args), body(body) {
	}
};

struct Condition final : Grammer {
	Ptr condition;
	Ptr onTrue;
	Ptr onFalse;

	Condition(Ptr condition, Ptr onTrue, Ptr onFalse) :
		Grammer(cond), condition(condition), onTrue(onTrue), onFalse(onFalse) {
	}
};

struct Calling final : Grammer {
	Ptr callable;
	std::vector<Ptr> args;
	Calling(Ptr&& callable, std::vector<Ptr>&& args) :
		Grammer(call), callable(callable), args(args) {
	}
};

struct Constant final : Grammer {
	int val;
	Constant(int val) : Grammer(cons), val(val) {}
};

struct Variable final : Grammer {
	std::string name;
	Variable(std::string&& str) : Grammer(var), name(str) {}
};

struct ParseErr : std::exception {
	ParseErr(char const* const what) : std::exception(what) {}
};

std::vector<std::shared_ptr<Defination>> parse(Lexer& lexer);

#endif // !MY_LISP_GRAMMER_HPP
