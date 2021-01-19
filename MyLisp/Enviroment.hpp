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
	// ����ֵ��ʾ�Ƿ����ɹ�
	// ����ʱ������upper���Ƿ���name��
	// ����env����name������ʧ��
	bool insert(const std::string& name, ValPtr val);

	static std::shared_ptr<Enviroment> get_root();
};
