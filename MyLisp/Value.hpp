#ifndef MY_LISP_VALUE
#define MY_LISP_VALUE

#include <functional>
#include <exception>
#include <memory>
#include <vector>

#include "Token.hpp"
#include "Grammer.hpp"

struct Value {
	enum Type {
		number, callable, boolean, nil
	};
	// virtual Type type() const = 0;
	const Type type = nil;
	Value(Type t) : type(t) {}
	virtual ~Value() = default;
	static std::shared_ptr<Value> Nil() {
		static auto nil = std::make_shared<Value>(Value::nil);
		return nil;
	}
};

struct Number final : Value {
	Number(Token::Number val) : Value(number), val(val) {}
	Token::Number val = 0.0;
};

struct Boolean final : Value {
	bool val;
	Boolean(bool val) : Value(boolean), val(val) {}
	static std::shared_ptr<Value> True() {
		static auto val = std::make_shared<Boolean>(true);
		return val;
	}
	static std::shared_ptr<Value> False() {
		static auto val = std::make_shared<Boolean>(false);
		return val;
	}
};

struct Callable : Value {
	using Function = std::function<
		std::shared_ptr<Value>(
			std::vector<std::shared_ptr<Value>>& args
		)
	>;

	Function invoke;

	Callable(Function invoke) : Value(callable), invoke(invoke) {}
};

std::shared_ptr<Callable> analyze(std::shared_ptr<Model> definations);

struct RuntimeErr : std::exception {
	RuntimeErr(char const* const what) : std::exception(what) {}
};

#endif // !MY_LISP_VALUE
