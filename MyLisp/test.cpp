#include "Elem.hpp"
#include "Grammer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>

using namespace std;

int main() {
	ifstream file("test.mlsp");
	ostringstream str;
	str << file.rdbuf();
	auto c = parse_file(str.str());
	auto g = analyze(c->val.front());
	auto r = g->eval(Enviroment::get_root());
	auto n = reinterpret_pointer_cast<Number>(r);
	cout << n->val << endl;

	return 0;
}