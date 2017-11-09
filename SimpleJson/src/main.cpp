#include <iostream>
#include "json_stream.hpp"
#include "connection_pool.h"

using namespace  std;

ConnPool *connpool = nullptr;
int main()
{
	JsonStream("abc");
	//初始化连接池
	connpool = ConnPool::GetInstance("172.24.54.1","ukdb","ukdb",50);
	Connection*		con;
	Statement*		state;
	ResultSet*		result;

	// 从连接池中获取mysql连接
	con = connpool->GetConnection();

	state = con->createStatement();
	state->execute("use ukdb2017");

	// 查询
	result = state->executeQuery("select * from contract;");
	// 输出查询
	while (result->next()) {
		int id = result->getInt("contractid");
		string name = result->getString("contractchname");
		cout << id << " : " << name << endl;
	}
	delete state;
	connpool->ReleaseConnection(con);
    return 0;
}