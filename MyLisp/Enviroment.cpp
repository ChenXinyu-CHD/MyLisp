#include "Enviroment.hpp"

bool Enviroment::has(const std::string& name) {
	if (env.count(name) != 0) {
		return true;
	} else if (upper != nullptr){
		return upper->has(name);
	} else {
		return false;
	}
}

ValPtr Enviroment::get(const std::string& name) {
	if (env.count(name) != 0) {
		return env[name];
	} else if (upper != nullptr) {
		return upper->get(name);
	} else {
		return ValPtr();
	}
}

bool Enviroment::insert(const std::string& name, ValPtr val) {
	if (env.count(name) == 0) {
		env[name] = val;
		return true;
	} else {
		return false;
	}
}

std::shared_ptr<Enviroment> Enviroment::get_root() {
	static auto root = std::shared_ptr<Enviroment>();
	return root;
}
