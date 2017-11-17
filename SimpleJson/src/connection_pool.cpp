#include "connection_pool.h"

ConnPool *ConnPool::connPool = NULL;


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
		LOG(ERROR)<<"创建连接失败";
		return NULL;
	}
	catch (...)
	{
		LOG(ERROR) << "运行时失败";
		return NULL;
	}
}

void ConnPool::InitConnection(int iInitialSize)
{
	Connection*		conn;
	lock.lock();
	for (int i = 0; i < iInitialSize; i++) 
	{
		conn = this->CreateConnection();
		if (conn) 
		{
			connList.push_back(conn);
			++(this->curSize);
		}
		else 
		{
			LOG(ERROR) << "创建CONNECTION出错";
		}
	}
	lock.unlock();
}

void ConnPool::DestoryConnection(Connection * conn)
{
	if (conn) 
	{
		try 
		{
			conn->close();
		}
		catch (sql::SQLException&e)
		{
			LOG(ERROR) << e.what();
		}
		delete conn;
	}
}

void ConnPool::DestoryConnPool()
{
	//lock.lock();
	for (auto it : connList)
	{
		this->DestoryConnection(it);
	}
	curSize = 0;
	connList.clear(); //清空连接池中的连接
	//lock.unlock();
}

ConnPool::ConnPool(string url, string user, string password, int maxSize)
{
	this->maxSize = maxSize;
	this->url = url;
	this->username = user;
	this->password = password;
	this->curSize = 0;
	try
	{
		this->driver = sql::mysql::get_mysql_driver_instance();
	}
	catch (sql::SQLException& e)
	{
		LOG(ERROR) << "驱动连接出错";
	}
	catch (...)
	{
		LOG(ERROR) << "运行出错";
	}
	this->InitConnection(maxSize / 2);
}

ConnPool::~ConnPool()
{
	this->DestoryConnPool();
}

Connection * ConnPool::GetConnection()
{
	Connection*		con;
	lock.lock();
	if (connList.size() > 0)  //连接池容器中还有连接
	{  
		con = connList.front(); //得到第一个连接
		connList.pop_front();   //移除第一个连接
		--curSize;
		LOG(WARNING) << "##################$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
		while (con == nullptr || con->isClosed())
		{
			con = this->CreateConnection();
			if (con == nullptr || con->isClosed())
			{
				sleep(1);
				LOG(WARNING) << "获取连接中...";
			}
		}
		LOG(WARNING) << "##################$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
		lock.unlock();
		return con;
	}
	else 
	{
		LOG(WARNING) << "##################$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
		con = this->CreateConnection();
		while (con == nullptr || con->isClosed())
		{
			con = this->CreateConnection();
			if (con == nullptr || con->isClosed())
			{
				sleep(1);
				LOG(WARNING) << "获取连接中...";
			}
		}
		LOG(WARNING) << "##################$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
		lock.unlock();	
		return con;
	}
}

void ConnPool::ReleaseConnection(Connection* conn)
{
	if (curSize < maxSize)
	{
		if (conn)
		{
			lock.lock();
			connList.push_back(conn);
			this->curSize++;
			lock.unlock();
		}
	}
	else
	{
		conn->close();
		delete conn;
		conn = nullptr;
		//
	}
}

ConnPool * ConnPool::GetInstance(std::string _url,std::string _user,std::string password,int conn_count)
{
	if (connPool == nullptr)
	{
		connPool = new ConnPool(_url, _user, password, conn_count);
	}
	LOG(INFO) << "创建了"<<conn_count<<"个连接!!!";
	return connPool;
}

void ConnPool::UpdateConnectPool()
{
	using std::chrono::system_clock;
	while (true)
	{
		std::time_t tt = system_clock::to_time_t(system_clock::now());
		struct std::tm * ptm = std::localtime(&tt);
		LOG(WARNING) << "数据库连接池启动@@@@...";
		LOG(WARNING) << "等待3小时,跳动@@@@...";
		ptm->tm_hour += 3;
		//ptm->tm_sec += 3;
		std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));

		lock.lock();
		this->DestoryConnPool();
		Connection*		conn;
		for (auto i = 0; i < this->maxSize; ++i)
		{
			conn = this->CreateConnection();
			if (conn)
			{
				this->connList.push_back(conn);
				++(this->curSize);
			}
			else
			{
				LOG(ERROR) << "创建CONNECTION出错";
			}
		}
		lock.unlock();
		LOG(WARNING) << "数据库连接池刷新@@@当前时间...";
	}
}
