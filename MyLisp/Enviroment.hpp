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
	// ����ʧ�ܷ���nullptr
	std::shared_ptr<Value> get(const std::string& name);
	// ����ֵ��ʾ�Ƿ����ɹ�
	// ����ʱ������upper���Ƿ���name�����env����name������ʧ��
	// Ҳ����˵�������һ�б������Ǿֲ�����
	bool insert(const std::string& name, std::shared_ptr<Value> val);

	// ���ú��������������Ļ�����Ҳ������ν�ı�׼��
	static std::shared_ptr<Enviroment> built_in();
};

#endif // !MY_LISP_ENVIROMENT_HPP
