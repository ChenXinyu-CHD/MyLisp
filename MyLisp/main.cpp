#include "Token.hpp"
// #include "Grammer.hpp"
// #include "regist_func.hpp"
#include <iostream>
#include <fstream>
#include <climits>
#include <exception>

using namespace std;

int main() {
	ifstream file("test.mlsp");
	string str(
		istreambuf_iterator<char>{file},
		istreambuf_iterator<char>()
	);
	Lexer lexer(str);

	while (lexer.now().type != Token::END) {
		auto token = lexer.now();
		switch (token.type) {
		case Token::ID:
			cout << "ID:" <<get<string>(token.val) << endl;
			break;
		case Token::NUM:
			cout << "NUM:" << get<int>(token.val) << endl;
			break;
		case Token::LB:
			cout << '(' << endl;
			break;
		case Token::RB:
			cout << ')' << endl;
			break;
		default:
			break;
		}
		lexer.next();
	}

	return 0;
}

/*
int main() {
	install_std_func();
	ifstream file("test.mlsp");
	string str(
		istreambuf_iterator<char>{file},
		istreambuf_iterator<char>()
	);
	auto code = parse_file(str);
	auto global = make_shared<Enviroment>(
		Enviroment::get_root()
	);
	for (auto elem : code->val) {
		auto grammer = analyze(elem);
		if (grammer->type() != GrammerType::defination) {
			throw runtime_error("此处只允许存在定义");
		} else {
			grammer->eval(global);
		}
	}
	auto main_val = global->get("main");
	if (main_val->type() != ValueType::callable) {
		throw runtime_error("缺少主函数");
	} else {
		auto main_func = reinterpret_pointer_cast<Callable>(main_val);
		auto val = main_func->val(vector<ValPtr>{});
		auto num = reinterpret_pointer_cast<Number>(val);
		cout << num->val << endl;
	}

	return 0;
}
*/