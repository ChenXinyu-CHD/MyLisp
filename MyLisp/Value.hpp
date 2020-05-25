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
	// ���·�Ϊ���ú������Զ��庯������
	// ���ú�����C++��д�Ĵ��룬�������ʽ������ͬ
	// �Զ��庯��һ����ͨ��Ŀ�����Ե�lambda���ʽ���ɵ�
	// �������ͳһ��std::function����
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
