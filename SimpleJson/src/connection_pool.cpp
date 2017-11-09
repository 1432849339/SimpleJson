#include "connection_pool.h"

ConnPool *ConnPool::connPool = NULL;
// Console logger with color
auto console = spdlog::stdout_color_mt("console");

Connection * ConnPool::CreateConnection()
{
	Connection *conn;
	try
	{
		conn = driver->connect(this->url, this->username, this->password); //建立连接
		return conn;
	}
	catch (sql::SQLException& e)
	{
		console->error("创建连接失败");
		return NULL;
	}
	catch (...)
	{
		console->error("运行时失败");
		return NULL;
	}
}

void ConnPool::InitConnection(int iInitialSize)
{
	Connection*		conn;
	lock.lock();
	for (int i = 0; i < iInitialSize; i++) {
		conn = this->CreateConnection();
		if (conn) {
			connList.push_back(conn);
			++(this->curSize);
		}
		else {
			console->error("创建CONNECTION出错");
		}
	}
	lock.unlock();
}

void ConnPool::DestoryConnection(Connection * conn)
{
	if (conn) {
		try {
			conn->close();
		}
		catch (sql::SQLException&e) {
			console->error(e.what());
		}
		delete conn;
	}
}

void ConnPool::DestoryConnPool()
{
	lock.lock();
	for (auto it : connList)
	{
		this->DestoryConnection(it);
	}
	curSize = 0;
	connList.clear(); //清空连接池中的连接
	lock.unlock();
}

ConnPool::ConnPool(string url, string user, string password, int maxSize)
{
	this->maxSize = maxSize;
	this->url = url;
	this->username = user;
	this->password = password;
	try
	{
		this->driver = sql::mysql::get_mysql_driver_instance();
	}
	catch (sql::SQLException& e)
	{
		console->error("驱动连接出错");
	}
	catch (...)
	{
		console->error("运行出错");
	}
	this->InitConnection(maxSize / 2);
}

ConnPool::~ConnPool()
{
	this->DestoryConnPool();
}

Connection * ConnPool::GetConnection()
{
	Connection*con;
	lock.lock();

	if (connList.size() > 0) {   //连接池容器中还有连接
		con = connList.front(); //得到第一个连接
		connList.pop_front();   //移除第一个连接
		if (con->isClosed()) {   //如果连接已经被关闭，删除后重新建立一个
			delete con;
			con = this->CreateConnection();
		}
		//如果连接为空，则创建连接出错
		if (con == NULL) {
			--curSize;
		}
		lock.unlock();
		return con;
	}
	else {
		if (curSize < maxSize) { //还可以创建新的连接
			con = this->CreateConnection();
			if (con) {
				++curSize;
				lock.unlock();
				return con;
			}
			else {
				lock.unlock();
				return NULL;
			}
		}
		else { //建立的连接数已经达到maxSize
			lock.unlock();
			return NULL;
		}
	}
}

void ConnPool::ReleaseConnection(Connection* conn)
{
	if (conn) {
		lock.lock();
		connList.push_back(conn);
		lock.unlock();
	}
}

ConnPool * ConnPool::GetInstance(std::string _url,std::string _user,std::string password,int conn_count)
{
	if (connPool == nullptr)
	{
		connPool = new ConnPool(_url, _user, password, conn_count);
	}
	console->info("创建了", conn_count, "个连接!!!");
	return connPool;
}
