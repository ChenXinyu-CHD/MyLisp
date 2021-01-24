#include "Enviroment.hpp"
#include "Value.hpp"

using namespace std;

namespace {
	// 求值器，即解释代码语义的函数
	// analyze的真正作用是将语法树转换为求值器
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
			throw ParseErr("语法错误：无法识别的语法结构。");
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
				throw RuntimeErr("运行时错误：试图调用一个不可调用的对象");
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
				throw RuntimeErr("运行时错误：变量重定义。");
			}
		};
	}

	Evaluator analyze_lambda(shared_ptr<Lambda> lambda) {
		// 分析函数体中每一步的求值
		vector<Evaluator> body;
		for (const auto& step : lambda->body) {
			body.push_back(analyze_single(step));
		}
		// 生成Callable对象的构造函数
		// lambda语句是一个语句，并不是函数本体
		// lambda语句执行的结果是生成一个Callable对象

		// 此处，Callable对象与其被定义位置的求值环境绑定
		return [args = lambda->args, body = move(body)](shared_ptr<Enviroment> outer) -> shared_ptr<Value> {
			// 此处为函数本体，是一个三元组：形参、函数体、求值环境
			return make_shared<Callable>([&args, &body, outer](vector<shared_ptr<Value>>& params) -> shared_ptr<Value> {
				if (args.size() != params.size()) {
					throw RuntimeErr("运行时错误：形参与实参数目不匹配");
				} else {
					using Params = remove_reference_t<decltype(params)>;
					// 调用函数时，扩展求值环境（压入栈帧），并将实参与形参绑定
					auto env = make_shared<Enviroment>(outer);
					for (Params::size_type i = 0; i < params.size(); ++i) {
						env->insert(args[i], params[i]);
					}
					// 真正的函数调用过程，函数返回值取决于最后一个语句的求值结果
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
				throw RuntimeErr("运行时错误：只有布尔型变量才能作为条件。");
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
				throw RuntimeErr("运行时错误：变量未定义。");
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
		// 构造最基础的求值环境（创建运行栈）
		auto env = make_shared<Enviroment>(Enviroment::built_in());
		// 将模块内的定义添加进求值环境
		for (const auto& def : defs) {
			def(env);
		}
		// 调用主函数
		auto main = env->get("main");
		if (main == nullptr || main->type != Value::callable) {
			throw RuntimeErr("运行时错误：找不到主函数");
		} else {
			return static_pointer_cast<Callable>(main)->invoke(params);
		}
	});
}
