#ifndef MY_LISP_GRAMMER_HPP
#define MY_LISP_GRAMMER_HPP

#include "Token.hpp"

#include <vector>

struct Grammer {
	enum Type { 
		def, lamb, cond, call, num, var, mod
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
	std::vector<Ptr> params;
	Calling(Ptr&& callable, std::vector<Ptr>&& params) :
		Grammer(call), callable(callable), params(params) {
	}
};

struct ConstNumber final : Grammer {
	Token::Number val;
	ConstNumber(Token::Number val) : Grammer(num), val(val) {}
};

struct Variable final : Grammer {
	std::string name;
	Variable(std::string&& str) : Grammer(var), name(str) {}
};

struct Model final : Grammer {
	std::vector<std::shared_ptr<Defination>> defs;
	Model(std::vector<std::shared_ptr<Defination>>&& defs) : Grammer(mod), defs(defs) {}
};

struct ParseErr : std::exception {
	ParseErr(char const* const what) : std::exception(what) {}
};

std::shared_ptr<Model> parse(Lexer& lexer);

#endif // !MY_LISP_GRAMMER_HPP
