#pragma once

#include "Enviroment.hpp"
#include "Value.hpp"

inline bool regist(std::string name, ValPtr val) {
	static auto env = Enviroment::get_root();
	return env->insert(name, val);
}

inline bool regist(std::string name, Callable::Function func, size_t argNum) {
	auto callable = std::make_shared<Callable>();
	callable->argsSize = argNum;
	callable->val = func;
	return regist(name, callable);
}

void install_std_func();