#include "Enviroment.hpp"
#include "Value.hpp"

using namespace std;

namespace {
	// ��ֵ���������ʹ�������ĺ���
	// analyze�����������ǽ��﷨��ת��Ϊ��ֵ��
	using Evaluator = function<
		shared_ptr<Value>(shared_ptr<Enviroment>)
	>;
	Evaluator analyze_call(shared_ptr<Calling> grammer);
	Evaluator analyze_def(shared_ptr<Defination> grammer);
	Evaluator analyze_lambda(shared_ptr<Lambda> grammer);
	Evaluator analyze_cond(shared_ptr<Condition> grammer);
	Evaluator analyze_cons(shared_ptr<ConstNumber> grammer);
	Evaluator analyze_var(shared_ptr<Variable> grammer);

	Evaluator analyze_single(shared_ptr<Grammer> grammer) {
		switch (grammer->type) {
		case Grammer::call:
			return analyze_call(static_pointer_cast<Calling>(grammer));
		case Grammer::cond:
			return analyze_cond(static_pointer_cast<Condition>(grammer));
		case Grammer::def:
			return analyze_def(static_pointer_cast<Defination>(grammer));
		case Grammer::lamb:
			return analyze_lambda(static_pointer_cast<Lambda>(grammer));
		case Grammer::num:
			return analyze_cons(static_pointer_cast<ConstNumber>(grammer));
		case Grammer::var:
			return analyze_var(static_pointer_cast<Variable>(grammer));
		default:
			throw ParseErr("�﷨�����޷�ʶ����﷨�ṹ��");
		}
	}

	Evaluator analyze_call(shared_ptr<Calling> call) {
		vector<Evaluator> params;
		for (const auto& param : call->params) {
			params.push_back(analyze_single(param));
		}

		return [callable = analyze_single(call->callable), params = move(params)](shared_ptr<Enviroment> env) -> shared_ptr<Value> {
			auto function = callable(env);
			if (function->type != Value::callable) {
				throw RuntimeErr("����ʱ������ͼ����һ�����ɵ��õĶ���");
			} else {
				vector<shared_ptr<Value>> paramList;
				for (const auto& param : params) {
					paramList.push_back(param(env));
				}
				return static_pointer_cast<Callable>(function)->invoke(paramList);
			}
		};
	}
	Evaluator analyze_def(shared_ptr<Defination> def) {
		return[value = analyze_single(def->value), name = def->name](shared_ptr<Enviroment> env)->shared_ptr<Value> {
			if (env->insert(name, value(env))) {
				return Value::Nil();
			} else {
				throw RuntimeErr("����ʱ���󣺱����ض��塣");
			}
		};
	}

	Evaluator analyze_lambda(shared_ptr<Lambda> lambda) {
		// ������������ÿһ������ֵ
		vector<Evaluator> body;
		for (const auto& step : lambda->body) {
			body.push_back(analyze_single(step));
		}
		// ����Callable����Ĺ��캯��
		// lambda�����һ����䣬�����Ǻ�������
		// lambda���ִ�еĽ��������һ��Callable����

		// �˴���Callable�������䱻����λ�õ���ֵ������
		return [args = lambda->args, body = move(body)](shared_ptr<Enviroment> outer) -> shared_ptr<Value> {
			// �˴�Ϊ�������壬��һ����Ԫ�飺�βΡ������塢��ֵ����
			return make_shared<Callable>([&args, &body, outer](vector<shared_ptr<Value>>& params) -> shared_ptr<Value> {
				if (args.size() != params.size()) {
					throw RuntimeErr("����ʱ�����β���ʵ����Ŀ��ƥ��");
				} else {
					using Params = remove_reference_t<decltype(params)>;
					// ���ú���ʱ����չ��ֵ������ѹ��ջ֡��������ʵ�����βΰ�
					auto env = make_shared<Enviroment>(outer);
					for (Params::size_type i = 0; i < params.size(); ++i) {
						env->insert(args[i], params[i]);
					}
					// �����ĺ������ù��̣���������ֵȡ�������һ��������ֵ���
					auto result = Value::Nil();
					for (const auto& step : body) {
						result = step(env);
					}
					return result;
				}
			});
		};
	}
	Evaluator analyze_cond(shared_ptr<Condition> cond) {
		return [
			condition = analyze_single(cond->condition),
			onTrue = analyze_single(cond->onTrue),
			onFalse = analyze_single(cond->onFalse)
		] (shared_ptr<Enviroment> env) -> shared_ptr<Value> {
			auto cond = condition(env);
			if (cond->type != Value::boolean) {
				throw RuntimeErr("����ʱ����ֻ�в����ͱ���������Ϊ������");
			} else if (static_pointer_cast<Boolean>(cond)->val == true) {
				return onTrue(env);
			} else {
				return onFalse(env);
			}
		};
	}
	Evaluator analyze_cons(shared_ptr<ConstNumber> cons) {
		return [number = cons->val](shared_ptr<Enviroment> env) -> shared_ptr<Value> {
			return make_shared<Number>(number);
		};
	}
	Evaluator analyze_var(shared_ptr<Variable> var) {
		return[name = var->name](shared_ptr<Enviroment> env)->shared_ptr<Value> {
			auto val = env->get(name);
			if (val != nullptr) {
				return val;
			} else {
				throw RuntimeErr("����ʱ���󣺱���δ���塣");
			}
		};
	}
}

shared_ptr<Callable> analyze(std::shared_ptr<Model> model) {
	vector<Evaluator> defs;
	for (const auto& def : model->defs) {
		defs.push_back(analyze_def(def));
	}
	
	return make_shared<Callable>([defs = move(defs)](vector<shared_ptr<Value>>& params) -> shared_ptr<Value> {
		// �������������ֵ��������������ջ��
		auto env = make_shared<Enviroment>(Enviroment::built_in());
		// ��ģ���ڵĶ�����ӽ���ֵ����
		for (const auto& def : defs) {
			def(env);
		}
		// ����������
		auto main = env->get("main");
		if (main == nullptr || main->type != Value::callable) {
			throw RuntimeErr("����ʱ�����Ҳ���������");
		} else {
			return static_pointer_cast<Callable>(main)->invoke(params);
		}
	});
}
