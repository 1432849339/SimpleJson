#ifndef _CONNECTION_POOL_H
#define _CONNECTION_POOL_H

#include "BaseFile.h"

class ConnPool
{
public:
protected:
private:
	int					curSize;			//当前已建立的数据库连接数量
	int					maxSize;			//连接池中定义的最大数据库连接数
	string				username;			//用户名
	string				password;			//密码
	string				url;				//数据库名
	list<Connection*>	connList;			//连接池的容器队列
	mutex				lock;				//线程锁
	static ConnPool		*connPool;
	Driver*				driver;

	Connection * CreateConnection();
	//创建一个连接
	void InitConnection(int iInitialSize);		 //初始化数据库连接池
	void DestoryConnection(Connection *conn);	//销毁数据库连接对象
	void DestoryConnPool();						//销毁数据库连接池
	ConnPool(string url, string user, string password, int maxSize); //构造方法
public:
	~ConnPool();
	Connection*GetConnection();				   //获得数据库连接
	void ReleaseConnection(Connection *conn);  //将数据库连接放回到连接池的容器中
	static ConnPool *GetInstance(std::string _url, std::string _user, std::string password, int conn_count);			   //获取数据库连接池对象
};

#endif