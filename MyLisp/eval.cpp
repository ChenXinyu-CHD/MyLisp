#include "Grammer.hpp"
#include <set>
#include <stdexcept>

using namespace std;

ValPtr Defination::eval(std::shared_ptr<Enviroment> env) const {
	static set<string> key {		// ������ʹ�õ����йؼ���
		"define", "set", "lambda", "nil", "true", "false"
	};
	if (key.count(name) != 0 || !env->insert(name, value->eval(env))) {
		throw runtime_error("�����ض������");
	} else {
		return make_shared<Nil>();
	}
}
ValPtr Assignment::eval(std::shared_ptr<Enviroment> env) const {
	return make_shared<Nil>();		// ��ֵ������е㸴�ӣ��Ժ���ʵ��
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
		throw runtime_error("�������ʽ�У�����������boolean��");
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
		throw runtime_error("��ͼ����һ�����ɵ��õĶ���");
	} else {
		auto f = reinterpret_pointer_cast<Callable>(fun);
		if (f->argsSize != args.size()) {
			throw runtime_error("���������в��������������в���");
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
		throw runtime_error("��ͼ������δ����ı�ʶ��");
	} else {
		return env->get(name);
	}
}