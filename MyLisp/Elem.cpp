#include "Elem.hpp"
#include <cctype>
#include <climits>

using namespace std;

namespace {
	using Iter = std::string::const_iterator;

	inline void skip_blanks(Iter& begin, const Iter& end) {
		while (begin != end && isspace(*begin)) {
			++begin;
		}
	}

	inline void skip_comments(Iter& begin, const Iter& end) {
		if (begin != end && *begin == ';') {
			while (begin != end && *begin != '\n') {
				++begin;
			}
		}
	}
	inline bool is_bc(char ch) {
		return ch == ';' || isspace(ch);
	}

	inline void skip_bc(Iter& begin, const Iter& end) {
		while (begin != end && is_bc(*begin)) {
			skip_blanks(begin, end);
			skip_comments(begin, end);
		}
	}

	inline bool is_l(char ch) {
		return ch == '(' || ch == '[' || ch == '{';
	}

	inline bool is_r(char ch) {
		return ch == ')' || ch == ']' || ch == '}';
	}

	inline bool is_lr(char ch) {
		return is_l(ch) || is_r(ch);
	}

	inline bool match(char l, char r) {
		return
			(l == '(' && r == ')') ||
			(l == '[' && r == ']') ||
			(l == '{' && r == '}');
	}

	ElemPtr parse(Iter& begin, const Iter& end) {
		skip_bc(begin, end);
		if (begin == end) {
			throw exception("there's no code");
		} else if (!is_l(*begin)) {
			return make_shared<Atom>(begin, end);
		} else {
			return make_shared<List>(begin, end);
		}
	}
}

Atom::Atom(
	std::string::const_iterator& begin,
	const std::string::const_iterator& end
) {
	Iter iter = begin;
	while (iter != end && !is_lr(*iter) && !is_bc(*iter)) {
		++iter;
	}
	val = string(begin, iter);
	begin = iter;
}

List::List(
	std::string::const_iterator& begin,
	const std::string::const_iterator& end
) {
	char l = *(begin++);
	while (begin != end && !is_r(*begin)) {
		val.push_back(parse(begin, end));
	}
	if (begin == end || !match(l, *(begin++))) {
		throw exception("barcket not match");
	}
}

std::shared_ptr<List> parse_file(const string& code) {
	auto result = make_shared<List>();
	Iter begin = code.begin();
	const Iter end = code.end();
	while (begin != end) {
		skip_bc(begin, end);
		if (is_l(*begin)) {
			auto list = make_shared<List>(begin, end);
			result->val.push_back(list);
		} else {
			throw exception("not a list");
		}
	}
	return result;
}
