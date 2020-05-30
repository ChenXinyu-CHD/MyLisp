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
	using Type = double;
	ValueType type() const override {
		return ValueType::number;
	}
	Type val = 0.0;
};

struct Callable final : Value {
	ValueType type() const override {
		return ValueType::callable;
	}
	using Function = std::function<ValPtr(std::vector<ValPtr>)>;
	// ���·�Ϊ���ú������Զ��庯������
	// ���ú�����C++��д�Ĵ��룬�������ʽ������ͬ
	// �Զ��庯��һ����ͨ��Ŀ�����Ե�lambda���ʽ���ɵ�
	// �������ͳһ��std::function����
	Function val;
	size_t argsSize = 0;
};

struct Boolean final : Value {
	ValueType type() const override {
		return ValueType::boolean;
	}
	bool val;
	Boolean() = default;
	Boolean(bool val) : val(val) {}
};

struct Nil final : Value {
	ValueType type() const override {
		return ValueType::nil;
	}
};
