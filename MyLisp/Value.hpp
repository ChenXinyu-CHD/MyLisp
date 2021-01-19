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
	// ���·�Ϊ���ú������Զ��庯������
	// ���ú�����C++��д�Ĵ��룬�������ʽ������ͬ
	// �Զ��庯��һ����ͨ��Ŀ�����Ե�lambda���ʽ���ɵ�
	// �������ͳһ��std::function����
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

