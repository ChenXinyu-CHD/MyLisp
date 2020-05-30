#pragma once

#include "Elem.hpp"
#include "Enviroment.hpp"
#include <vector>

enum class GrammerType {
	defination,
	assignment,
	lambda,
	condition,
	application,
	constant,
	variable
};

struct Grammer {
	virtual GrammerType type() const = 0;
	virtual ValPtr eval(std::shared_ptr<Enviroment> env) const = 0;
	virtual ~Grammer() = default;
};

using GramPtr = std::shared_ptr<Grammer>;

struct Defination final : Grammer {
	GrammerType type() const override {
		return GrammerType::defination;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override;
	std::string name;
	GramPtr value;
};

struct Assignment final : Grammer {
	GrammerType type() const override {
		return GrammerType::assignment;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override;
	GramPtr name;	// 左值可能比较复杂，不仅可以是个变量名
	GramPtr value;
};

struct Lambda final : Grammer {
	GrammerType type() const override {
		return GrammerType::lambda;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override;
	std::vector<std::string> args;
	std::vector<GramPtr> body;
};

struct Condition final : Grammer {
	GrammerType type() const override {
		return GrammerType::condition;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override;
	GramPtr condition;
	GramPtr onTrue;
	GramPtr onFalse;
};

struct Application final : Grammer {
	GrammerType type() const override {
		return GrammerType::application;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override;
	GramPtr callable;	// 其是否可调用需要后续步骤检验
	std::vector<GramPtr> args;
};

struct Constant final : Grammer {
	GrammerType type() const override {
		return GrammerType::constant;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override {
		return val;
	}
	ValPtr val;
};

struct Variable final : Grammer {
	GrammerType type() const override {
		return GrammerType::variable;
	}
	ValPtr eval(std::shared_ptr<Enviroment> env) const override;
	std::string name;
};

GramPtr analyze(ElemPtr elem);
