#include "Enviroment.hpp"

#include <iostream>

using namespace std;

bool Enviroment::has(const std::string& name) {
	if (env.count(name) != 0) {
		return true;
	} else if (upper != nullptr){
		return upper->has(name);
	} else {
		return false;
	}
}

std::shared_ptr<Value> Enviroment::get(const std::string& name) {
	if (env.count(name) != 0) {
		return env[name];
	} else if (upper != nullptr) {
		return upper->get(name);
	} else {
		return nullptr;
	}
}

bool Enviroment::insert(const std::string& name, std::shared_ptr<Value> val) {
	if (env.count(name) == 0) {
		env[name] = val;
		return true;
	} else {
		return false;
	}
}

namespace {
	using ValPtr = std::shared_ptr<Value>;
	using Params = std::vector<ValPtr>;
	using Num = Token::Number;
	using Operator = function<Num(Num, Num)>;
	namespace oper {
		Num add(Num n1, Num n2) { return n1 + n2; }
		Num sub(Num n1, Num n2) { return n1 - n2; }
		Num mul(Num n1, Num n2) { return n1 * n2; }
		Num div(Num n1, Num n2) { return n1 / n2; }
	}
	shared_ptr<Callable> make_operator(Operator op) {
		return make_shared<Callable>([op](Params params)->ValPtr {
			if (params.size() != 2) {
				throw RuntimeErr("运行时错误：函数形参与实参数目不匹配");
			} else {
				const auto& param1 = params.front();
				const auto& param2 = params.back();
				if (
					param1->type == Value::number &&
					param2->type == Value::number
					) {
					auto n1 = static_pointer_cast<Number>(param1)->val;
					auto n2 = static_pointer_cast<Number>(param2)->val;
					return make_shared<Number>(op(n1, n2));
				} else {
					throw runtime_error("运算类型不匹配");
				}
			}
		});
	}
	using Compare = function<bool(Num, Num)>;
	namespace comp {
		bool eq(Num n1, Num n2) { return n1 == n2; }
		bool ls(Num n1, Num n2) { return n1 < n2; }
		bool gt(Num n1, Num n2) { return n1 > n2; }
	}
	shared_ptr<Callable> make_compare(Compare comp) {
		return make_shared<Callable>([comp](Params params)->ValPtr {
			if (params.size() != 2) {
				throw RuntimeErr("运行时错误：函数形参与实参数目不匹配");
			} else {
				const auto& param1 = params.front();
				const auto& param2 = params.back();
				if (
					param1->type == Value::number &&
					param2->type == Value::number
				) {
					auto n1 = static_pointer_cast<Number>(param1)->val;
					auto n2 = static_pointer_cast<Number>(param2)->val;
					if (comp(n1, n2)) {
						return Boolean::True();
					} else {
						return Boolean::False();
					}
				} else {
					throw runtime_error("运算类型不匹配");
				}
			}
		});
	}
	shared_ptr<Value> print(Params params) {
		for (const auto& param : params) {
			switch (param->type) {
			case Value::boolean:
				cout << "boolean: " << static_pointer_cast<Boolean>(param)->val << endl;
				break;
			case Value::number:
				cout << "number: " << static_pointer_cast<Number>(param)->val << endl;
				break;
			case Value::nil:
				cout << "nil" << endl;
				break;
			case Value::callable:
				cout << "callable" << endl;
				break;
			}
		}
		return Value::Nil();
	}
}

shared_ptr<Enviroment> Enviroment::built_in() {
	static const auto init = [](map<string, shared_ptr<Value>>& std) {
		std = map<string, shared_ptr<Value>> {
			{ "+", make_operator(oper::add) },
			{ "-", make_operator(oper::sub) },
			{ "*", make_operator(oper::mul) },
			{ "/", make_operator(oper::div) },

			{ "=", make_compare(comp::eq) },
			{ "<", make_compare(comp::ls) },
			{ ">", make_compare(comp::gt) },

			{ "nil", Value::Nil() },
			{ "true", Boolean::True() },
			{ "false", Boolean::False() },
			{ "print", make_shared<Callable>(print) }
		};
		return true;
	};
	static auto built_in = std::make_shared<Enviroment>(nullptr);
	static auto _ = init(built_in->env);
	return built_in;
}
