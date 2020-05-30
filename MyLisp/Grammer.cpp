#include "Grammer.hpp"
#include <functional>
#include <stdexcept>
#include <sstream>
#include <map>

using namespace std;

namespace {
	GramPtr makeDefination(shared_ptr<List> list);
	GramPtr makeAssignment(shared_ptr<List> list);
	GramPtr makeLambda(shared_ptr<List> list);
	GramPtr makeCondition(shared_ptr<List> list);
	GramPtr makeApplication(shared_ptr<List> list);
	GramPtr makeElement(shared_ptr<Atom> atom);				// 常量或变量

	using Constructor = function<GramPtr(shared_ptr<List>)>;
	map<string, Constructor> constructors {
		{ "define", makeDefination },
		{ "set", makeAssignment },
		{ "lambda", makeLambda },
		{ "if", makeCondition }
	};

	GramPtr makeDefination(shared_ptr<List> list) {
		auto first = list->val.front();
		list->val.pop_front();
		if (first->type() == ElemType::list) {	// 说明它是一个函数定义
			// 取出函数名
			auto args = reinterpret_pointer_cast<List>(first);
			if (args->val.empty()) {
				throw runtime_error("lack of function name in defination");
			} else {
				first = args->val.front();
				args->val.pop_front();
			}
			// 其余部分封装为一个lambda表达式的形式作为函数体
			list->val.push_front(args);	// 放入参数表

			if (first->type() != ElemType::atom) {
				throw runtime_error("function name is illegal");
			} else {
				auto name = reinterpret_pointer_cast<Atom>(first);
				auto result = make_shared<Defination>();
				result->name = name->val;
				result->value = makeLambda(list);
				return result;
			}
		} else { // 此处是一个变量定义
			if (list->val.size() != 1) {
				throw runtime_error("变量定义格式错误");
			} else {
				auto name = reinterpret_pointer_cast<Atom>(first);
				auto result = make_shared<Defination>();
				result->name = name->val;
				result->value = analyze(list->val.front());
				return result;
			}
		}
	}
	GramPtr makeAssignment(shared_ptr<List> list) {
		if (list->val.size() != 2) {
			throw runtime_error("Wrong format in assignment");
		} else {
			auto result = make_shared<Assignment>();
			result->name = analyze(list->val.front());
			result->value = analyze(list->val.back());
			return result;
		}
	}
	GramPtr makeLambda(shared_ptr<List> list) {
		if (list->val.size() < 2) {
			throw runtime_error("Wrong format in lambda");
		} else {
			auto args = list->val.front();
			list->val.pop_front();

			if (args->type() != ElemType::list) {
				throw runtime_error("Wrong format in arguments list");
			} else {
				auto result = make_shared<Lambda>();
				auto l = reinterpret_pointer_cast<List>(args);
				for (auto elem : l->val) {
					if (elem->type() == ElemType::atom) {
						auto arg = reinterpret_pointer_cast<Atom>(elem);
						result->args.push_back(arg->val);
					} else {
						throw runtime_error("argument must be a string");
					}
				}
				for (auto elem : list->val) {
					result->body.push_back(analyze(elem));
				}
				return result;
			}
		}
	}
	GramPtr makeCondition(shared_ptr<List> list) {
		if (list->val.size() < 2 || list->val.size() > 3) {
			throw runtime_error("wrong format in condition");
		} else {
			auto result = make_shared<Condition>();
			result->condition = analyze(list->val.front());
			list->val.pop_front();
			result->onTrue = analyze(list->val.front());
			list->val.pop_front();
			if (!list->val.empty()) {
				result->onFalse = analyze(list->val.front());
			}
			return result;
		}
	}
	GramPtr makeApplication(shared_ptr<List> list) {
		if (list->val.empty()) {
			throw runtime_error("lack of the name of function in application");
		} else {
			auto result = make_shared<Application>();
			result->callable = analyze(list->val.front());
			list->val.pop_front();
			for (auto elem : list->val) {
				result->args.push_back(analyze(elem));
			}
			return result;
		}
	}
	GramPtr makeElement(shared_ptr<Atom> atom) {
		string& str = atom->val;
		if (isdigit(str[0])) {	// 此时应该是数字
			istringstream sin(str);
			auto result = make_shared<Constant>();
			auto value = make_shared<Number>();
			value->val = stod(str);
			result->val = value;
			return result;
		} else {
			static map<string, ValPtr> constant{
				{"nil", make_shared<Nil>()},
				{"true", make_shared<Boolean>(true)},
				{"false", make_shared<Boolean>(false)}
			};
			if (constant.count(str) != 0) {
				auto result = make_shared<Constant>();
				result->val = constant[str];
				return result;
			} else {
				auto result = make_shared<Variable>();
				result->name = str;
				return result;
			}
		}
	}
}

GramPtr analyze(ElemPtr elem) {
	if (elem->type() == ElemType::atom) {
		auto atom = reinterpret_pointer_cast<Atom>(elem);
		return makeElement(atom);
	} else {
		auto list = reinterpret_pointer_cast<List>(elem);
		auto attr = list->val.front();
		if (attr->type() == ElemType::atom) {
			auto atom = reinterpret_pointer_cast<Atom>(attr);
			if (constructors.count(atom->val)) {
				list->val.pop_front();
				return constructors[atom->val](list);
			} else {
				return makeApplication(list);
			}
		} else {
			return makeApplication(list);
		}
	}
}
