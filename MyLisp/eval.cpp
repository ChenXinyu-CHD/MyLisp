#include "Grammer.hpp"
#include <set>
#include <stdexcept>

using namespace std;

ValPtr Defination::eval(std::shared_ptr<Enviroment> env) const {
	static set<string> key {		// 语言中使用的所有关键字
		"define", "set", "lambda", "nil", "true", "false"
	};
	if (key.count(name) != 0 || !env->insert(name, value->eval(env))) {
		throw runtime_error("变量重定义错误");
	} else {
		return make_shared<Nil>();
	}
}
ValPtr Assignment::eval(std::shared_ptr<Enviroment> env) const {
	return make_shared<Nil>();		// 赋值的情况有点复杂，以后再实现
}

ValPtr Lambda::eval(std::shared_ptr<Enviroment> env) const {
	auto& lambdaArgs = this->args;
	auto& lambdaBody = this->body;
	auto lambda = [lambdaArgs, lambdaBody, env](vector<ValPtr> params)->ValPtr {
		auto newEnv = make_shared<Enviroment>(env);
		for (vector<ValPtr>::size_type i = 0; i < params.size(); ++i) {
			newEnv->insert(lambdaArgs[i], params[i]);
		}
		ValPtr result;
		for (auto elem : lambdaBody) {
			result = elem->eval(newEnv);
		}
		return result;
	};
	auto result = make_shared<Callable>();
	result->argsSize = args.size();
	result->val = lambda;
	return result;
}
ValPtr Condition::eval(std::shared_ptr<Enviroment> env) const {
	auto cond = condition->eval(env);
	if (cond->type() != ValueType::boolean) {
		throw runtime_error("条件表达式中，条件必须是boolean型");
	} else {
		auto boolean = reinterpret_pointer_cast<Boolean>(cond);
		if (boolean->val == true) {
			return onTrue->eval(env);
		} else if (onFalse != nullptr){
			return onFalse->eval(env);
		} else {
			return make_shared<Nil>();
		}
	}
}

ValPtr Application::eval(std::shared_ptr<Enviroment> env) const {
	auto fun = callable->eval(env);
	if (fun->type() != ValueType::callable) {
		throw runtime_error("试图调用一个不可调用的对象");
	} else {
		auto f = reinterpret_pointer_cast<Callable>(fun);
		if (f->argsSize != args.size()) {
			throw runtime_error("函数调用中参数数量与声明中不符");
		} else {
			vector<ValPtr> params;
			for (auto arg : args) {
				params.push_back(arg->eval(env));
			}
			return f->val(move(params));
		}
	}
}

ValPtr Variable::eval(std::shared_ptr<Enviroment> env) const {
	if (!env->has(name)) {
		throw runtime_error("试图访问尚未定义的标识符");
	} else {
		return env->get(name);
	}
}