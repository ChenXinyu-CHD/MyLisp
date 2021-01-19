#include "Token.hpp"
#include "Grammer.hpp"

#include <iostream>
#include <fstream>

using namespace std;

void print(shared_ptr<Grammer> grammer) {
	switch (grammer->type) {
	case Grammer::call: {
		auto call = reinterpret_pointer_cast<Calling>(grammer);
		cout << "Call:" << endl;
		print(call->callable);
		cout << "args:" << endl;
		for (auto& arg : call->args) {
			print(arg);
		}
		break;
	}
	case Grammer::def: {
		auto def = reinterpret_pointer_cast<Defination>(grammer);
		cout << "Define:" << def->name << endl;
		cout << "val:";
		print(def->value);
		break;
	}
	case Grammer::lamb: {
		auto lamb = reinterpret_pointer_cast<Lambda>(grammer);
		cout << "Lambda:" << endl;
		for (auto& arg : lamb->args)
			cout << arg << "\t";
		cout << "body:" << endl;
		for (auto& b : lamb->body)
			print(b);
		break;
	}
	case Grammer::cond: {
		auto cond = reinterpret_pointer_cast<Condition>(grammer);
		cout << "Condition:" << endl;
		print(cond->condition);
		cout << "onTrue:" << endl;
		print(cond->onTrue);
		cout << "onFalse:" << endl;
		print(cond->onFalse);
		break;
	}
	case Grammer::num: {
		auto cons = reinterpret_pointer_cast<Number>(grammer);
		cout << "Constant:" << cons->val << endl;
		break;
	}
	case Grammer::var: {
		auto var = reinterpret_pointer_cast<Variable>(grammer);
		cout << "Variable:" << var->name << endl;
		break;
	}
	}
}

int main() {
	ifstream file("test.mlsp");
	Lexer lexer(file);
	for (auto& def : parse(lexer)) {
		print(def);
	}
	return 0;
}
