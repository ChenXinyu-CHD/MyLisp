#ifndef MY_LISP_VALUE
#define MY_LISP_VALUE

#include <functional>
#include <memory>
#include <vector>

#include "Token.hpp"

struct Value {
	enum Type {
		number, callable, boolean, nil
	};
	virtual Type type() const = 0;
	virtual ~Value() = default;
};

using ValPtr = std::shared_ptr<Value>;

struct Number final : Value {
	Type type() const override {
		return Value::number;
	}
	Token::Number val = 0.0;
};

struct Callable final : Value {
	Type type() const override {
		return Value::callable;
	}
	using Function = std::function<ValPtr(std::vector<ValPtr>)>;
	// 大致分为内置函数和自定义函数两类
	// 内置函数是C++编写的代码，其产生方式不尽相同
	// 自定义函数一定是通过目标语言的lambda表达式生成的
	// 方便起见统一用std::function抽象
	Function val;
	size_t argsSize = 0;
};

struct Boolean final : Value {
	Type type() const override {
		return Value::boolean;
	}
	bool val;
	Boolean() = default;
	Boolean(bool val) : val(val) {}
};

struct Nil final : Value {
	Type type() const override {
		return Value::nil;
	}
};

#endif // !MY_LISP_VALUE

