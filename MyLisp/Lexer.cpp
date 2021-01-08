#include "Token.hpp"

#include <sstream>
#include <stack>

using namespace std;

Lexer::Lexer(string code):
	in(code), token(Token::End()) {
	this->next();
}

Token Lexer::now() const {
	return token;
}

namespace {
	bool is_com(char ch) {
		return ch == ';';
	}

	void skip_useless(istream& in) {
		while (in) {
			if (is_com(in.peek())) {
				while (in && in.get() != '\n')
					continue;
			} else if (isspace(in.peek())){
				while (in && isspace(in.peek()))
					in.get();
			} else {
				break;
			}
		}
	}

	bool is_lb(char ch) {
		return ch == '[' || ch == '(' || ch == '{';
	}

	bool is_rb(char ch) {
		return ch == ']' || ch == ')' || ch == '}';
	}

	bool match(char l, char r) {
		return	(l == '(' && r == ')') ||
				(l == '[' && r == ']') ||
				(l == '{' && r == '}');
	}
}

Token Lexer::next() {
	skip_useless(in);
	if (in) {
		auto ch = in.peek();
		if (is_lb(ch)) {
			brackets.push(ch);
			in.get();
			token = Token::Lb();
		} else if (is_rb(ch)) {
			if (brackets.empty() || !match(brackets.top(), ch)) {
				throw LexErr("Brackets do not match");
			} else {
				brackets.pop();
				in.get();
				token = Token::Rb();
			}
		} else if (isdigit(ch)) {
			int val = 0;
			in >> val;
			token = Token::Num(val);
		} else {
			string str;
			while (
				!isspace(in.peek()) &&
				!is_lb(in.peek()) &&
				!is_rb(in.peek()) &&
				!is_com(in.peek())
			) {
				str += in.get();
			}
			token = Token::Id(str);
		}
	} else {
		token = Token::End();
	}

	return token;
}

/*
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
			throw runtime_error("there's no code");
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
		skip_bc(begin, end);
	}
	if (begin == end || !match(l, *(begin++))) {
		throw runtime_error("barcket not match");
	}
}

std::shared_ptr<List> parse_file(const string& code) {
	auto result = make_shared<List>();
	Iter begin = code.begin();
	const Iter end = code.end();
	while (begin != end) {
		skip_bc(begin, end);
		if (begin != end) {
			if (is_l(*begin)) {
				auto list = make_shared<List>(begin, end);
				result->val.push_back(list);
			} else {
				throw runtime_error("not a list");
			}
		}
	}
	return result;
}
*/