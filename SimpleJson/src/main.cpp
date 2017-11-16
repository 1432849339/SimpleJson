#include <iostream>
#include "json_stream.hpp"
#include "connection_pool.h"
#include "UKServer.h"

DataBase   local;
ConnPool *connpool = nullptr;
ison::base::Context ctx;
ison::base::Socket *g_socket;

int Get_Date()
{
	struct tm * pstm = NULL;
	time_t tt = time(NULL);
	pstm = localtime(&tt);
	while (pstm->tm_wday == 0 || pstm->tm_wday == 6)
	{
		tt -= 24 * 60 * 60;
		pstm = localtime(&tt);
	}
	int year = pstm->tm_year + 1900;
	int mon = pstm->tm_mon + 1;
	int day = pstm->tm_mday;
	return year * 10000 + mon * 100 + day;
}

int GetDate()
{
	struct tm * pstm = NULL;
	time_t tt = time(NULL);
	pstm = localtime(&tt);
	//周日
	if (pstm->tm_wday == 0)
	{
		tt += 24 * 60 * 60;
	}
	//周六
	else if (pstm->tm_wday == 6)
	{
		tt += 2 * 24 * 60 * 60;
	}
	//周五晚上
	else if (pstm->tm_wday == 5)
	{
		if (pstm->tm_hour > 16 || (pstm->tm_hour == 16 && pstm->tm_min > 30))
			tt += 3 * 24 * 60 * 60;
	}
	else
	{
		if (pstm->tm_hour > 16 || (pstm->tm_hour == 16 && pstm->tm_min > 30))
			tt += 24 * 60 * 60;
	}
	pstm = localtime(&tt);
	int year = pstm->tm_year + 1900;
	int mon = pstm->tm_mon + 1;
	int day = pstm->tm_mday;
	return year * 10000 + mon * 100 + day;
}


int main(int argc,char **argv)
{
	ison::base::iblog_init(argv[0], "log");
	ison::base::iblog_v(1002);
	ison::base::iblog_stderrthreshold(1);
	ison::base::iblog_logbufsecs(0);
	LOG(WARNING) << "日期:" << GetDate() << std::endl;
	connpool = ConnPool::GetInstance("172.24.54.1", "ukdb", "ukdb", 10);

	thread t(Update_local_data);
	const char* g_query_bind = NULL;//req-rep监听请求的端口
	const char* g_query_con = NULL;//连接tgw的端口
	g_query_bind = "tcp://*:9112";
	g_query_con = "tcp://172.24.10.35:8888";
	ison::base::Stage Service(ctx);
	int ret = 0;
	if (g_query_bind && g_query_con)
	{
		ret = Service.Bind(g_query_bind);//直连监听
		if (ret)
		{
			LOG(ERROR) << "Service Bind Error!";
			return -1;
		}
		ret = Service.Connect("con2tgw", g_query_con);//con2tgw:链接名,tgw连接
		if (ret)
		{
			LOG(ERROR) << "Service Connect Error!";
			return -1;
		}
		//ison::base::ActorPtr query_service(new UKService(argv[3]));//actor name
		ison::base::ActorPtr query_service(new UKService("GetUkeyTable"));
		Service.AddActor(query_service);
		Service.Start();
		LOG(WARNING) << "service Startting...";
		Service.Join();		//connect to Service and join service queue
	}
	if (t.joinable())
	{
		t.join();
	}
	return 0;
}