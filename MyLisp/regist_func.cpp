#include "regist_func.hpp"
#include <memory>
#include <stdexcept>

using namespace std;

namespace {
	using Args = std::vector<ValPtr>;
	using Num = Number::Type;
	using Operator = function<Num(Num, Num)>;
	namespace oper {
		Num add(Num n1, Num n2) { return n1 + n2; }
		Num sub(Num n1, Num n2) { return n1 - n2; }
		Num mul(Num n1, Num n2) { return n1 * n2; }
		Num div(Num n1, Num n2) { return n1 / n2; }
	}
	Callable::Function make_operator(Operator op) {
		auto oper = [op](Args args)->ValPtr {
			auto arg1 = args.front();
			auto arg2 = args.back();
			if (
				arg1->type() == ValueType::number &&
				arg2->type() == ValueType::number
			) {
				auto result = make_shared<Number>();
				auto n1 = reinterpret_pointer_cast<Number>(arg1);
				auto n2 = reinterpret_pointer_cast<Number>(arg2);
				result->val = op(n1->val, n2->val);
				return result;
			} else {
				throw runtime_error("运算类型不匹配");
			}
		};
		return oper;
	}
	using Compare = function<bool(Num, Num)>;
	namespace comp {
		bool eq(Num n1, Num n2) { return n1 == n2; }
		bool ls(Num n1, Num n2) { return n1 < n2; }
		bool gt(Num n1, Num n2) { return n1 > n2; }
	}
	Callable::Function make_Compare(Compare comp) {
		auto compare = [comp](Args args)->ValPtr {
			auto arg1 = args.front();
			auto arg2 = args.back();
			if (
				arg1->type() == ValueType::number &&
				arg2->type() == ValueType::number
			) {
				auto result = make_shared<Boolean>();
				auto n1 = reinterpret_pointer_cast<Number>(arg1);
				auto n2 = reinterpret_pointer_cast<Number>(arg2);
				result->val = comp(n1->val, n2->val);
				return result;
			} else {
				throw runtime_error("运算类型不匹配");
			}
		};
		return compare;
	}
}

void install_std_func() {
	regist("+", make_operator(oper::add), 2);
	regist("-", make_operator(oper::sub), 2);
	regist("*", make_operator(oper::mul), 2);
	regist("/", make_operator(oper::div), 2);

	regist("=", make_Compare(comp::eq), 2);
	regist("<", make_Compare(comp::ls), 2);
	regist(">", make_Compare(comp::gt), 2);
}
