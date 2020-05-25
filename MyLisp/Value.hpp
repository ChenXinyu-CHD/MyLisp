#pragma once

#include <functional>
#include <memory>
#include <vector>

enum class ValueType {
	number, callable, boolean, nil
};

struct Value {
	virtual ValueType type() const = 0;
	virtual ~Value() = default;
};

using ValPtr = std::shared_ptr<Value>;

struct Number final : Value {
	ValueType type() const override {
		return ValueType::number;
	}
	double val = 0.0;
};

struct Callable final : Value {
	ValueType type() const override {
		return ValueType::callable;
	}
	// 大致分为内置函数和自定义函数两类
	// 内置函数是C++编写的代码，其产生方式不尽相同
	// 自定义函数一定是通过目标语言的lambda表达式生成的
	// 方便起见统一用std::function抽象
	std::function<ValPtr(std::vector<ValPtr>)> val;
	size_t argsSize = 0;
};

struct Boolean final : Value {
	ValueType type() const override {
		return ValueType::boolean;
	}
	bool val;
};

struct Nil final : Value {
	ValueType type() const override {
		return ValueType::nil;
	}
};
