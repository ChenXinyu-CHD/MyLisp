#ifndef MY_LISP_ENVIROMENT_HPP
#define MY_LISP_ENVIROMENT_HPP

#include "Value.hpp"
#include "Grammer.hpp"

#include <map>
#include <string>

class Enviroment final {
private:
	std::map<std::string, std::shared_ptr<Value>> env;
	std::shared_ptr<Enviroment> upper;
	Enviroment() = delete;
public:
	Enviroment(std::shared_ptr<Enviroment> upper) :
		upper(upper) { /* Nothing to do; */ }
	bool has(const std::string& name);
	// 查找失败返回nullptr
	std::shared_ptr<Value> get(const std::string& name);
	// 返回值表示是否插入成功
	// 插入时不考虑upper中是否有name项，但若env中有name项插入会失败
	// 也就是说，插入的一切变量都是局部变量
	bool insert(const std::string& name, std::shared_ptr<Value> val);

	// 内置函数、常量所属的环境，也就是所谓的标准库
	static std::shared_ptr<Enviroment> built_in();
};

#endif // !MY_LISP_ENVIROMENT_HPP
