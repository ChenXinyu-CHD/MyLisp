#include "Token.hpp"
#include "Grammer.hpp"
#include "Value.hpp"

#include <iostream>
#include <fstream>

using namespace std;

int main() {
	try {
		ifstream file("test.mlsp");
		Lexer lexer(file);
		auto grammer = parse(lexer);
		auto runnable = analyze(grammer);
		vector<shared_ptr<Value>> params;
		runnable->invoke(params);
	} catch (exception e) {
		cerr << e.what() << endl;
	}

	return 0;
}
