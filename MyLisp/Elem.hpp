#pragma once

#include <deque>
#include <string>
#include <memory>

enum class ElemType { atom, list };

struct Elem {
	virtual ElemType type() const = 0;
	virtual ~Elem() = default;
};

using ElemPtr = std::shared_ptr<Elem>;

struct Atom final : Elem {
	Atom(
		std::string::const_iterator& begin,
		const std::string::const_iterator& end
	);
	ElemType type() const override {
		return ElemType::atom;
	}
	std::string val;
	Atom() = default;
};

struct List final : Elem {
	List(
		std::string::const_iterator& begin,
		const std::string::const_iterator& end
	);
	ElemType type() const override {
		return ElemType::list;
	}
	std::deque<ElemPtr> val;
	List() = default;
};

std::shared_ptr<List> parse_file(const std::string& code);
