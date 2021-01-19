#ifndef MY_LISP_SYMBOLS_HPP
#define MY_LISP_SYMBOLS_HPP

#include "Grammer.hpp"

#include <map>
#include <memory>

struct Symbol;
class SymbolTable;

struct Symbol final {
	std::shared_ptr<Grammer> grammer;
	std::weak_ptr<SymbolTable> owner;
};

class SymbolTable final {
private:
	std::map<std::string, std::shared_ptr<Symbol>> table;
	std::shared_ptr<SymbolTable> upper;
public:
	SymbolTable(std::shared_ptr<SymbolTable> upper = nullptr) : upper(upper) {}
	std::shared_ptr<Grammer> search(const std::string& name);
	void store(const std::string& name, std::shared_ptr<Grammer> grammer);
};

#endif // !MY_LISP_SYMBOLS_HPP

