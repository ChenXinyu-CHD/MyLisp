 #pragma once

#include "Value.hpp"
#include <map>
#include <string>

class Enviroment final {
private:
	std::map<std::string, ValPtr> env;
	std::shared_ptr<Enviroment> upper;
	Enviroment() = default;
public:
	Enviroment(std::shared_ptr<Enviroment> upper) :
		upper(upper) { /* Nothing to do; */ }
	bool has(const std::string& name);
	ValPtr get(const std::string& name);
	// 返回值表示是否插入成功
	// 插入时不考虑upper中是否有name项
	// 但若env中有name项插入会失败
	bool insert(const std::string& name, ValPtr val);

	static std::shared_ptr<Enviroment> get_root();
};
