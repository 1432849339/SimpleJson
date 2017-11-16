#ifndef  __UKSERVER_H__
#define  __UKSERVER_H__

#include "BaseFile.h"
#include "json_stream.hpp"
#include "MyLocalDataBase.h"
#include "isonbase.h"

class UKService :public Actor
{
public:
	UKService(std::string id_);
	int OnStart(ActorContext&);
	int OnEvent(Event& e);
};

class Query
{
public:
	Query(string& smess);
	~Query();
	bool Parse();
	//通过数据库查询
	bool query_by_sql();
	//通过内存数据查询
	bool query_by_local();
	bool HasError();
	vector<string> GetErrorInfo();
	tuple<bool, vector<string>> make_choice_queuy();
protected:
	string Document2String(rapidjson::Document& doc);
	template<typename T>
	bool package(TgwHead* Tgw_head, ReqHead* Req_head, vector<shared_ptr<T>>& result);
private:
	std::string					m_messge;
	JsonStream					jsonstream;
	vector<string>				error_str;
	vector<string>				result_str;
	int							err_num;
};

template<typename T>
inline bool Query::package(TgwHead * Tgw_head, ReqHead * Req_head, vector<shared_ptr<T>>& result)
{
	int pcount = result.size();
	if (pcount == 0)
	{
		err_num = -12;
		LOG(ERROR) << "表中没有记录";
		error_str.emplace_back("表中没有记录");
		return false;
	}
	int PackCount = pcount / Req_head->PackSize;
	if (pcount % Req_head->PackSize != 0)
	{
		PackCount++;
	}
	if (Req_head->SerialID == 0)
	{
		for (int i = 1; i <= PackCount; ++i)
		{
			int count = 0;
			if (i < PackCount)
			{
				count = Req_head->PackSize;
			}
			else
			{
				count = (pcount % Req_head->PackSize == 0 ? Req_head->PackSize : pcount % Req_head->PackSize);
			}
			rapidjson::Document doc_reply;
			doc_reply.SetObject();
			rapidjson::Value data_reply;
			data_reply.SetObject();
			rapidjson::Document::AllocatorType &allocator = doc_reply.GetAllocator();

			data_reply.AddMember("Seqno", Req_head->Seqno, allocator);
			data_reply.AddMember("ErrNo", 0, allocator);
			data_reply.AddMember("ErrInfo", "", allocator);
			if (i == PackCount)
			{
				data_reply.AddMember("IsLast", "Y", allocator);
			}
			else
			{
				data_reply.AddMember("IsLast", "N", allocator);
			}
			data_reply.AddMember("SerialID", i, allocator);
			data_reply.AddMember("Count", count, allocator);
			rapidjson::Value Data;
			Data.SetArray();
			int _start = (i - 1)*Req_head->PackSize;
			int _end = _start + count;
			for (; _start != _end; ++_start)
			{
				rapidjson::Value v_struct;
				v_struct.SetObject();
				//处理uint字段
				for (auto &it : jsonstream.GetParameter())
				{
					if (it.second == MY_UINT)
					{
						result[_start]->Init();
						unsigned int val = result[_start]->GetUInt(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理int字段
				for (auto &it : jsonstream.GetParameter())
				{
					if (it.second == MY_INT)
					{
						result[_start]->Init();
						int val = result[_start]->GetInt(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理int64_t
				for (auto &it : jsonstream.GetParameter())
				{
					if (it.second == MY_INT64)
					{
						result[_start]->Init();
						int64_t val = result[_start]->GetInt64_t(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理double字段
				for (auto &it : jsonstream.GetParameter())
				{
					if (it.second == MY_DOUBLE)
					{
						result[_start]->Init();
						double val = result[_start]->GetDouble(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理char
				for (auto &it : jsonstream.GetParameter())
				{
					if (it.second == MY_CHAR)
					{
						result[_start]->Init();
						string str = result[_start]->GetString(it.first.c_str()).c_str();
						rapidjson::Value v(str.c_str(), str.size(), allocator);
						v_struct.AddMember(it.first.c_str(), v, allocator);
					}
				}
				Data.PushBack(v_struct, allocator);
			}
			data_reply.AddMember("Structs", Data, allocator);
			if (!jsonstream.ConnectByTgw())//直连
			{
				doc_reply.AddMember("Seqno", data_reply["Seqno"].GetInt(), allocator);
				doc_reply.AddMember("ErrNo", data_reply["ErrNo"].GetInt64(), allocator);
				doc_reply.AddMember("ErrInfo", data_reply["ErrInfo"].GetString(), allocator);
				doc_reply.AddMember("IsLast", data_reply["IsLast"].GetString(), allocator);
				doc_reply.AddMember("SerialID", data_reply["SerialID"].GetInt(), allocator);
				doc_reply.AddMember("Count", data_reply["Count"].GetInt(), allocator);
				doc_reply.AddMember("Structs", data_reply["Structs"], allocator);
				result_str.emplace_back(Document2String(doc_reply));
			}
			else//tgw
			{
				rapidjson::Value tgw_replay;
				tgw_replay.SetObject();
				tgw_replay.AddMember("nodeid", Tgw_head->NodeId, allocator);
				tgw_replay.AddMember("apptype", Tgw_head->AppType, allocator);
				tgw_replay.AddMember("packtype", Tgw_head->PackType + 1, allocator);
				tgw_replay.AddMember("conlvl", Tgw_head->Conlvl, allocator);
				tgw_replay.AddMember("address", Tgw_head->Address, allocator);

				tgw_replay.AddMember("termid", Tgw_head->TermId.c_str(), allocator);
				tgw_replay.AddMember("userid", Tgw_head->UserId.c_str(), allocator);
				tgw_replay.AddMember("appname", Tgw_head->AppName.c_str(), allocator);
				tgw_replay.AddMember("packname", Tgw_head->PackName.c_str(), allocator);
				tgw_replay.AddMember("cellid", Tgw_head->CellId.c_str(), allocator);
				doc_reply.AddMember("tgw", tgw_replay, allocator);
				doc_reply.AddMember("data", data_reply, allocator);
				result_str.emplace_back(Document2String(doc_reply));
			}
		}
	}
	else
	{
		if (PackCount < Req_head->SerialID)
		{
			err_num = -2 * 13;
			LOG(ERROR) << "请求包id:SerialID 超出范围";
			error_str.emplace_back("请求包id:SerialID 超出范围");
			return false;
		}
		rapidjson::Document doc_reply;
		doc_reply.SetObject();
		rapidjson::Value data_reply;
		data_reply.SetObject();
		rapidjson::Document::AllocatorType &allocator = doc_reply.GetAllocator();

		data_reply.AddMember("Seqno", Req_head->Seqno, allocator);
		data_reply.AddMember("ErrNo", 0, allocator);
		data_reply.AddMember("ErrInfo", "", allocator);
		if (PackCount == Req_head->SerialID)
		{
			data_reply.AddMember("IsLast", "Y", allocator);
			data_reply.AddMember("SerialID", Req_head->SerialID, allocator);
			if (pcount % Req_head->PackSize == 0)
			{
				data_reply.AddMember("Count", Req_head->PackSize, allocator);
			}
			else
			{
				data_reply.AddMember("Count", pcount % Req_head->PackSize, allocator);
			}
		}
		else
		{
			data_reply.AddMember("IsLast", "N", allocator);
			data_reply.AddMember("SerialID", Req_head->SerialID, allocator);
			data_reply.AddMember("Count", Req_head->PackSize, allocator);
		}

		int start_PackIndex = (Req_head->SerialID - 1)*Req_head->PackSize;
		int end_PackIndex = start_PackIndex + data_reply["Count"].GetInt();
		rapidjson::Value Data;
		Data.SetArray();
		for (int i = start_PackIndex; i < end_PackIndex; i++)
		{
			rapidjson::Value v_struct;
			v_struct.SetObject();

			//处理uint字段
			for (auto &it : jsonstream.GetParameter())
			{
				if (it.second == MY_UINT)
				{
					result[i]->Init();
					unsigned int val = result[i]->GetUInt(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理int字段
			for (auto &it : jsonstream.GetParameter())
			{
				if (it.second == MY_INT)
				{
					result[i]->Init();
					int val = result[i]->GetInt(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理int64_t
			for (auto &it : jsonstream.GetParameter())
			{
				if (it.second == MY_INT64)
				{
					result[i]->Init();
					int64_t val = result[i]->GetInt64_t(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理double字段
			for (auto &it : jsonstream.GetParameter())
			{
				if (it.second == MY_DOUBLE)
				{
					result[i]->Init();
					double val = result[i]->GetDouble(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理char
			for (auto &it : jsonstream.GetParameter())
			{
				if (it.second == MY_CHAR)
				{
					result[i]->Init();
					string str = result[i]->GetString(it.first.c_str()).c_str();
					//rapidjson::Value v(str.c_str(), allocator);
					rapidjson::Value v(str.c_str(), str.size(), allocator);
					v_struct.AddMember(it.first.c_str(), v, allocator);
				}
			}
			Data.PushBack(v_struct, allocator);
		}
		data_reply.AddMember("Structs", Data, allocator);

		if (!jsonstream.ConnectByTgw())//直连
		{
			doc_reply.AddMember("Seqno", data_reply["Seqno"].GetInt(), allocator);
			doc_reply.AddMember("ErrNo", data_reply["ErrNo"].GetInt64(), allocator);
			doc_reply.AddMember("ErrInfo", data_reply["ErrInfo"].GetString(), allocator);
			doc_reply.AddMember("IsLast", data_reply["IsLast"].GetString(), allocator);
			doc_reply.AddMember("SerialID", data_reply["SerialID"].GetInt(), allocator);
			doc_reply.AddMember("Count", data_reply["Count"].GetInt(), allocator);
			doc_reply.AddMember("Structs", data_reply["Structs"], allocator);
			result_str.emplace_back(Document2String(doc_reply));
		}
		else//tgw
		{
			rapidjson::Value tgw_replay;
			tgw_replay.SetObject();
			tgw_replay.AddMember("nodeid", Tgw_head->NodeId, allocator);
			tgw_replay.AddMember("apptype", Tgw_head->AppType, allocator);
			tgw_replay.AddMember("packtype", Tgw_head->PackType + 1, allocator);
			tgw_replay.AddMember("conlvl", Tgw_head->Conlvl, allocator);
			tgw_replay.AddMember("address", Tgw_head->Address, allocator);

			tgw_replay.AddMember("termid", Tgw_head->TermId.c_str(), allocator);
			tgw_replay.AddMember("userid", Tgw_head->UserId.c_str(), allocator);
			tgw_replay.AddMember("appname", Tgw_head->AppName.c_str(), allocator);
			tgw_replay.AddMember("packname", Tgw_head->PackName.c_str(), allocator);
			tgw_replay.AddMember("cellid", Tgw_head->CellId.c_str(), allocator);
			doc_reply.AddMember("tgw", tgw_replay, allocator);
			doc_reply.AddMember("data", data_reply, allocator);
			result_str.emplace_back(Document2String(doc_reply));
		}
	}
	return true;
}

struct DataBase
{
	shared_ptr<contract_local_sql>		contract_local;
	shared_ptr<currency_local_sql>		currency_local;
	shared_ptr<dailyclear_local_sql>	dailyclear_local;
	shared_ptr<Market_local_sql>		market_local;
	shared_ptr<secumatre_local_sql>     secumatre_local;
	shared_ptr<tssyscalender_local_sql> tssyscalender_local;
	shared_ptr<ukey_local_sql>			ukey_local;
	shared_ptr<uktype_local_sql>		uktype_local;
	shared_ptr<calendar_local_sql>		calendar_local;
	shared_ptr<component_local_sql>		component_local;
}; 

void Update_local_data();

#endif

