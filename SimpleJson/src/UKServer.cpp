#include "UKServer.h"

atomic<bool>				is_init_seccuss(false);
atomic<int>					date(0);
extern std::map<int, std::string>	map_code_name;
extern ConnPool *					connpool;
extern DataBase						local;

UKService::UKService(std::string id_) :Actor(id_)
{

}

int UKService::OnStart(ActorContext &)
{
	return 0;
}

int UKService::OnEvent(Event & e)
{
	if (e.type() == kMessage)
	{
		string smess = e.message();
		LOG(INFO) << smess << endl;
		Query   query(smess);
		query.Parse();
		auto result = query.make_choice_queuy();
		bool conn_by_tgw = get<0>(result);
		vector<string>  json;
		json = get<1>(result);
		if (conn_by_tgw == false)
		{
			for (auto &it : json)
			{
				e.Reply(it);
			}
		}
		else
		{
			for (auto &it : json)
			{
				//e.Reply(it);
				Request("con2tgw", "tgw", it);
			}
		}
	}
	return 0;
}


inline Query::Query(string& smess)
{
	err_num = 0;
	m_messge = smess;
	jsonstream.set_messge(m_messge);
}
inline Query::~Query()
{
}
inline string Query::Document2String(rapidjson::Document & doc)
{
	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}
inline tuple<bool, vector<string>> Query::make_choice_queuy()
{
	if (err_num != 0)
	{
		return make_tuple(jsonstream.ConnectByTgw(), GetErrorInfo());
	}
	GetTrday tt;
	if (is_init_seccuss && date == tt())//本地数据库初始化完成
	{
		if (jsonstream.GetReqHead().TableType == 3 || jsonstream.GetReqHead().TableType == 5 || jsonstream.GetReqHead().TableType == 10)//与时间相关的表
		{
			if ((jsonstream.GetReqHead().Date == 0) || ((jsonstream.GetReqHead().Date <= date) && (jsonstream.GetReqHead().Date >= tt - DATE_DISTANCE)))
				//判断查询的时间范围是否在本地数据库的范围内
			{
				date == tt() ? query_by_local() : query_by_sql();
			}
			else
			{
				query_by_sql();
			}
		}
		else//与时间无关的表
		{
			query_by_local();
		}
	}
	else//本地数据未初始化完成,使用sql查询
	{
		query_by_sql();
	}
	if (err_num != 0)
	{
		return make_tuple(jsonstream.ConnectByTgw(), GetErrorInfo());
	}
	return make_tuple(jsonstream.ConnectByTgw(), result_str);
}

bool Query::Parse()
{
	JsonException info = jsonstream.ParaseTgwHead();
	if (std::get<0>(info.GetInfo()) != 0)
	{
		err_num = std::get<0>(info.GetInfo());
		error_str.emplace_back(std::get<1>(info.GetInfo()));
		return false;
	}
	info = jsonstream.ParaseReqHead();
	if (std::get<0>(info.GetInfo()) != 0)
	{
		err_num = std::get<0>(info.GetInfo());
		error_str.emplace_back(std::get<1>(info.GetInfo()));
		return false;
	}
	info = jsonstream.GetRequestField();
	if (std::get<0>(info.GetInfo()) != 0)
	{
		err_num = std::get<0>(info.GetInfo());
		error_str.emplace_back(std::get<1>(info.GetInfo()));
		return false;
	}
	return true;
}
//通过数据库查询
bool Query::query_by_sql()
{
	LOG(INFO) << "query_by_sql";
	char Sql[10240]{ 0 };
	TgwHead& tgw_head = jsonstream.GetTgwHead();
	ReqHead& req_head = jsonstream.GetReqHead();
	std::map<std::string,int32_t>& Parameter = jsonstream.GetParameter();
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	switch (req_head.TableType)
	{
	case 1://contract
		if (req_head.SecurityID == 0)
		{
			sprintf(Sql, "SELECT * FROM %s;", map_code_name[req_head.TableType].c_str());
		}
		else
		{
			sprintf(Sql, "SELECT * FROM %s a WHERE a.contract_id=%ld;",map_code_name[req_head.TableType].c_str(), req_head.SecurityID);
		}
		break;
	case 2://currency
		if (req_head.SecurityID == 0)
		{
			sprintf(Sql, "SELECT * FROM %s;", map_code_name[req_head.TableType].c_str());
		}
		else
		{
			sprintf(Sql, "SELECT * FROM %s a WHERE a.currency_id=%ld;",map_code_name[req_head.TableType].c_str(), req_head.SecurityID);
		}
		break;
	case 3://dailyclear
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		/*if (req_head.SecurityID == 0)
		{
			if (req_head.Date == 0)
			{
				sprintf(Sql, "SELECT * FROM %s a INNER JOIN (SELECT ukey,MAX(`trading_day`) trading_day FROM %s  GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`trading_day`=b.`trading_day`;",map_code_name[req_head.TableType].c_str(), map_code_name[req_head.TableType].c_str());
			}
			else
			{
				sprintf(Sql, "SELECT * FROM %s a WHERE a.trading_day=%d;", map_code_name[req_head.TableType].c_str(), req_head.Date);
			}
		}
		else
		{
			if (req_head.Date == 0)
			{
				sprintf(Sql, "SELECT * FROM %s a WHERE a.ukey=%ld  ORDER BY trading_day DESC LIMIT 1;",map_code_name[req_head.TableType].c_str(), req_head.SecurityID);
			}
			else
			{
				sprintf(Sql, "SELECT * FROM %s a WHERE a.trading_day=%d AND a.ukey=%ld;", map_code_name[req_head.TableType].c_str(), req_head.Date, req_head.SecurityID);
			}
		}*/
		break;
	case 4://market
		if (req_head.MarketID != 0)
		{
			sprintf(Sql, "SELECT *  FROM %s a WHERE a.market_id=%d;", map_code_name[req_head.TableType].c_str(), req_head.MarketID);
		}
		else
		{
			sprintf(Sql, "SELECT *  FROM %s ;", map_code_name[req_head.TableType].c_str());
		}
		break;
	case 5://secumaster
		if (req_head.SecurityID == 0)
		{
			if (req_head.MarketID != 0)
			{
				if (req_head.Date == 0)
				{
					sprintf(Sql, "SELECT *,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a INNER JOIN (SELECT ukey,MAX(`trading_day`) trading_day FROM %s WHERE `market_id`=%d  GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`trading_day`=b.`trading_day`;", map_code_name[req_head.TableType].c_str(), map_code_name[req_head.TableType].c_str(), req_head.MarketID);
				}
				else
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.market_id=%d AND a.trading_day=%d;", map_code_name[req_head.TableType].c_str(), req_head.MarketID, req_head.Date);
				}
			}
			else
			{
				if (req_head.Date == 0)
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a INNER JOIN (SELECT ukey,MAX(`trading_day`) trading_day FROM %s GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`trading_day`=b.`trading_day`;", map_code_name[req_head.TableType].c_str(), map_code_name[req_head.TableType].c_str());
				}
				else
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE  a.trading_day=%d;", map_code_name[req_head.TableType].c_str(), req_head.Date);
				}
			}
		}
		else
		{
			if (req_head.MarketID != 0)
			{
				if (req_head.Date == 0)
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.market_id=%d AND a.ukey=%ld ORDER BY a.trading_day DESC LIMIT 1;", map_code_name[req_head.TableType].c_str(), req_head.MarketID, req_head.SecurityID);
				}
				else
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.market_id=%d AND a.ukey=%ld AND a.trading_day=%d;", map_code_name[req_head.TableType].c_str(), req_head.MarketID, req_head.SecurityID, req_head.Date);
				}
			}
			else
			{
				if (req_head.Date == 0)
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.ukey=%ld ORDER BY a.trading_day DESC LIMIT 1;", map_code_name[req_head.TableType].c_str(),req_head.SecurityID);
				}
				else
				{
					sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.ukey=%ld AND a.trading_day=%d;", map_code_name[req_head.TableType].c_str(), req_head.SecurityID, req_head.Date);
				}
			}
		}
		break;
	case 6://tssyscalender
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		/*if (req_head.Date == 0)
		{
			sprintf(Sql, "SELECT * FROM %s ORDER BY trday DESC LIMIT 1;",map_code_name[req_head.TableType].c_str());
		}
		else
		{
			sprintf(Sql, "SELECT * FROM %s a WHERE a.trday='%d';", map_code_name[req_head.TableType].c_str(), req_head.Date);
		}*/
		break;
	case 7://ukey
		if (req_head.MarketID != 0)
		{
			if (req_head.SecurityID == 0)
			{
				sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.market_id=%d;", map_code_name[req_head.TableType].c_str(), req_head.MarketID);
			}
			else
			{
				sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.ukey=%ld AND a.market_id=%d;",map_code_name[req_head.TableType].c_str(), req_head.SecurityID, req_head.MarketID);
			}
		}
		else
		{
			if (req_head.SecurityID == 0)
			{
				sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s;",map_code_name[req_head.TableType].c_str());
			}
			else
			{
				sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a WHERE a.ukey=%ld;", map_code_name[req_head.TableType].c_str(), req_head.SecurityID);
			}
		}
		break;
	case 8://uktype
		sprintf(Sql, "SELECT * FROM %s;",map_code_name[req_head.TableType].c_str());
		break;
	case 9://calendar  对时间铭感,需要处理
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		//if (req_head.MarketID != 0)
		//{
		//	if (req_head.Date == 0)
		//	{
		//		sprintf(Sql, "SELECT * FROM %s a WHERE a.market_id=%d ORDER BY `date` DESC LIMIT 1;", map_code_name[req_head.TableType].c_str(), req_head.MarketID);
		//	}
		//	else
		//	{
		//		sprintf(Sql, "SELECT * FROM %s a WHERE a.market_id=%d AND `date`='%d';",map_code_name[req_head.TableType].c_str(), req_head.MarketID, req_head.Date);
		//	}
		//}
		//else
		//{
		//	if (req_head.Date == 0)
		//	{
		//		
		//		sprintf(Sql, "SELECT * FROM %s a INNER JOIN(SELECT `market_id`,MAX(`date`)AS `date` FROM %s GROUP  BY `market_id`)b ON a.market_id = b.market_id AND a.`date`=b.date;; ", map_code_name[req_head.TableType].c_str(), map_code_name[req_head.TableType].c_str());
		//		//sprintf(Sql, "SELECT * FROM %s ORDER BY `date` DESC LIMIT 1;",map_code_name[req_head.TableType].c_str());
		//	}
		//	else
		//	{
		//		sprintf(Sql, "SELECT * FROM %s a WHERE  `date`='%d';",map_code_name[req_head.TableType].c_str(), req_head.Date);
		//	}
		//}
		break;
	case 10://component 对时间铭感,需要处理
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		/*if (req_head.SecurityID == 0)
		{
			if (req_head.Date == 0)
			{
				sprintf(Sql, "SELECT * FROM %s a INNER JOIN (SELECT ukey,MAX(`update_date`) update_date FROM %s  GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`update_date`=b.`update_date`;", map_code_name[req_head.TableType].c_str(), map_code_name[req_head.TableType].c_str());
			}
			else
			{
				sprintf(Sql, "SELECT * FROM %s a WHERE a.update_date=%d;", map_code_name[req_head.TableType].c_str(), req_head.Date);
			}
		}
		else
		{
			if (req_head.Date == 0)
			{
				sprintf(Sql, "SELECT * FROM %s a WHERE a.ukey=%ld  ORDER BY update_date DESC LIMIT 1;",map_code_name[req_head.TableType].c_str(), req_head.SecurityID);
			}
			else
			{
				sprintf(Sql, "SELECT * FROM %s a WHERE a.update_date=%d AND a.ukey=%ld;",map_code_name[req_head.TableType].c_str(), req_head.Date, req_head.SecurityID);
			}
		}*/
		break;
	default:
		break;
	}
	LOG(INFO) << Sql;
	rs = state->executeQuery(Sql);
	LOG(INFO) << "Retrieved " << rs->rowsCount() << " row(s)." << std::endl;
	int pCount = rs->rowsCount();
	if (pCount == 0)
	{
		error_str.emplace_back("表中没有数据");
		LOG(ERROR) << "表中没有数据";
		err_num = -10;
		return false;
	}
	std::string tep_str;
	int packpCount = pCount / req_head.PackSize;
	if (pCount%req_head.PackSize != 0)
	{
		packpCount++;
	}
	if (req_head.SerialID == 0)
	{
		for (int i = 1; i <= packpCount; ++i)
		{
			int count = 0;
			if (i < packpCount)
			{
				count = req_head.PackSize;
			}
			else
			{
				count = (pCount%req_head.PackSize == 0 ? req_head.PackSize : pCount%req_head.PackSize);
			}
			rapidjson::Document doc_reply;
			doc_reply.SetObject();
			rapidjson::Value data_reply;
			data_reply.SetObject();
			rapidjson::Document::AllocatorType &allocator = doc_reply.GetAllocator();

			data_reply.AddMember("Seqno", req_head.Seqno, allocator);
			data_reply.AddMember("ErrNo", 0, allocator);
			data_reply.AddMember("ErrInfo", "", allocator);
			if (i == packpCount)
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
			while (count--)
			{
				rs->next();
				rapidjson::Value v_struct;
				v_struct.SetObject();
				//处理uint字段
				for (auto &it : Parameter)
				{
					if (it.second == MY_UINT)
					{
						unsigned int val = (unsigned int)(rs->getUInt(it.first.c_str()));
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理int字段
				for (auto &it : Parameter)
				{
					if (it.second == MY_INT)
					{
						int val = (int)rs->getInt(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理int64_t
				for (auto &it : Parameter)
				{
					if (it.second == MY_INT64)
					{
						int64_t val = rs->getInt64(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理double字段
				for (auto &it : Parameter)
				{
					if (it.second == MY_DOUBLE)
					{
						double val = rs->getDouble(it.first.c_str());
						v_struct.AddMember(it.first.c_str(), val, allocator);
					}
				}
				//处理char
				for (auto &it : Parameter)
				{
					if (it.second == MY_CHAR)
					{
						if (it.first == "last_update")
						{
							string str = rs->getString("lastupdate").c_str();
							rapidjson::Value v(str.c_str(), str.size(), allocator);
							v_struct.AddMember(it.first.c_str(), v, allocator);
						}
						else
						{
							string str = rs->getString(it.first.c_str()).c_str();
							rapidjson::Value v(str.c_str(), str.size(), allocator);
							v_struct.AddMember(it.first.c_str(), v, allocator);
						}
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
				tgw_replay.AddMember("nodeid", tgw_head.NodeId, allocator);
				tgw_replay.AddMember("apptype", tgw_head.AppType, allocator);
				tgw_replay.AddMember("packtype", tgw_head.PackType + 1, allocator);
				tgw_replay.AddMember("conlvl", tgw_head.Conlvl, allocator);
				tgw_replay.AddMember("address", tgw_head.Address, allocator);
				tgw_replay.AddMember("termid", tgw_head.TermId.c_str(), allocator);
				tgw_replay.AddMember("userid", tgw_head.UserId.c_str(), allocator);
				tgw_replay.AddMember("appname", tgw_head.AppName.c_str(), allocator);
				tgw_replay.AddMember("packname", tgw_head.PackName.c_str(), allocator);
				tgw_replay.AddMember("cellid", tgw_head.CellId.c_str(), allocator);
				doc_reply.AddMember("tgw", tgw_replay, allocator);
				doc_reply.AddMember("data", data_reply, allocator);
				result_str.emplace_back(Document2String(doc_reply));
			}
		}
	}
	else
	{
		if (packpCount < req_head.SerialID)
		{
			error_str.emplace_back("请求包id SerialID超出范围");
			LOG(ERROR) << "请求包id SerialID超出范围";
			err_num = -11;
			return false;
		}
		int PackIndex = 0;//包id开始的位置位置
		int index = 0;//记录位置
		rapidjson::Document doc_reply;
		doc_reply.SetObject();
		rapidjson::Value data_reply;
		data_reply.SetObject();
		rapidjson::Document::AllocatorType &allocator = doc_reply.GetAllocator();

		data_reply.AddMember("Seqno", req_head.Seqno, allocator);
		data_reply.AddMember("ErrNo", 0, allocator);
		data_reply.AddMember("ErrInfo", "", allocator);

		if (packpCount == req_head.SerialID)
		{
			data_reply.AddMember("IsLast", "Y", allocator);
			data_reply.AddMember("SerialID", req_head.SerialID, allocator);

			if (pCount % req_head.PackSize == 0)
			{
				data_reply.AddMember("Count", req_head.PackSize, allocator);
			}
			else
			{
				data_reply.AddMember("Count", pCount % req_head.PackSize, allocator);
			}
		}
		else
		{
			data_reply.AddMember("IsLast", "N", allocator);
			data_reply.AddMember("SerialID", req_head.SerialID, allocator);
			data_reply.AddMember("Count", req_head.PackSize, allocator);
		}
		//找到需要包的位置
		PackIndex = (req_head.SerialID - 1)*req_head.PackSize;
		while (index != PackIndex)
		{
			rs->next();
			index++;
		}
		//将需要对的记录打包
		int size = data_reply["Count"].GetInt();
		rapidjson::Value Data;
		Data.SetArray();
		while (rs->next() && size--)
		{
			rapidjson::Value v_struct;
			v_struct.SetObject();

			//处理uint字段
			for (auto &it : Parameter)
			{
				if (it.second == MY_UINT)
				{
					unsigned int val = (unsigned int)(rs->getUInt(it.first.c_str()));
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理int字段
			for (auto &it : Parameter)
			{
				if (it.second == MY_INT)
				{
					int val = (int)rs->getInt(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理int64_t
			for (auto &it : Parameter)
			{
				if (it.second == MY_INT64)
				{
					int64_t val = rs->getInt64(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理double字段
			for (auto &it : Parameter)
			{
				if (it.second == MY_DOUBLE)
				{
					double val = rs->getDouble(it.first.c_str());
					v_struct.AddMember(it.first.c_str(), val, allocator);
				}
			}
			//处理char
			for (auto &it : Parameter)
			{
				if (it.second == MY_CHAR)
				{
					if (it.first == "last_update")
					{
						string str = rs->getString("lastupdate").c_str();
						rapidjson::Value v(str.c_str(), str.size(), allocator);
						v_struct.AddMember(it.first.c_str(), v, allocator);
					}
					else
					{
						string str = rs->getString(it.first.c_str()).c_str();
						rapidjson::Value v(str.c_str(), str.size(), allocator);
						v_struct.AddMember(it.first.c_str(), v, allocator);
					}
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
			tgw_replay.AddMember("nodeid", tgw_head.NodeId, allocator);
			tgw_replay.AddMember("apptype", tgw_head.AppType, allocator);
			tgw_replay.AddMember("packtype", tgw_head.PackType + 1, allocator);
			tgw_replay.AddMember("conlvl", tgw_head.Conlvl, allocator);
			tgw_replay.AddMember("address", tgw_head.Address, allocator);
			tgw_replay.AddMember("termid", tgw_head.TermId.c_str(), allocator);
			tgw_replay.AddMember("userid", tgw_head.UserId.c_str(), allocator);
			tgw_replay.AddMember("appname", tgw_head.AppName.c_str(), allocator);
			tgw_replay.AddMember("packname", tgw_head.PackName.c_str(), allocator);
			tgw_replay.AddMember("cellid", tgw_head.CellId.c_str(), allocator);
			doc_reply.AddMember("tgw", tgw_replay, allocator);
			doc_reply.AddMember("data", data_reply, allocator);
			result_str.emplace_back(Document2String(doc_reply));
		}
	}
	rs->close();
	connpool->ReleaseConnection(con);
	return true;
}
//通过内存数据查询
bool Query::query_by_local()
{
	LOG(INFO) << "query_by_local";
	TgwHead& tgw_head = jsonstream.GetTgwHead();
	ReqHead& req_head = jsonstream.GetReqHead();
	std::map<std::string, int32_t>& Parameter = jsonstream.GetParameter();
	if (err_num != 0)
	{
		return false;
	}
	switch (req_head.TableType)
	{
	case 1:
		[&]() {
			vector<shared_ptr<contract_local_sql::Contract>>			result_1;
			shared_ptr<contract_local_sql> contract_local = local.contract_local;
			contract_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_1);
			package(&tgw_head, &req_head, result_1);
		}();
		break;
	case 2:
		[&]() {
			vector<shared_ptr<currency_local_sql::Currency>>			result_2;
			shared_ptr<currency_local_sql>	currency_local = local.currency_local;
			currency_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_2);
			package(&tgw_head, &req_head, result_2);
		}();
		break;
	case 3:
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		[&]() {
			vector<shared_ptr<dailyclear_local_sql::Dailyclear>>			result_3;
			shared_ptr<dailyclear_local_sql>	dailyclear_local = local.dailyclear_local;
			dailyclear_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_3);
			package(&tgw_head, &req_head, result_3);
		}();
		break;
	case 4:
		[&]() {
			vector<shared_ptr<Market_local_sql::Market>>				result_4;
			shared_ptr<Market_local_sql> market_local = local.market_local;
			market_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_4);
			package(&tgw_head, &req_head, result_4);
		}();
		break;
	case 5:
		[&]() {
			vector<shared_ptr<secumatre_local_sql::Secumaster>>			result_5;
			shared_ptr<secumatre_local_sql> secumatre_local = local.secumatre_local;
			secumatre_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_5);
			package(&tgw_head, &req_head, result_5);
		}();
		break;
	case 6:
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		[&]() {
			vector<shared_ptr<tssyscalender_local_sql::Tssyscalender>>		result_6;
			shared_ptr<tssyscalender_local_sql> tssyscalender_local = local.tssyscalender_local;
			//tssyscalender_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_6);
			package(&tgw_head, &req_head, result_6);
		}();
		break;
	case 7:
		[&]() {
			vector<shared_ptr<ukey_local_sql::Ukey>>				result_7;
			shared_ptr<ukey_local_sql> ukey_local = local.ukey_local;
			ukey_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_7);
			package(&tgw_head, &req_head, result_7);
		}();
		break;
	case 8:
		[&]() {
			vector<shared_ptr<uktype_local_sql::Uktype>>				result_8;
			shared_ptr<uktype_local_sql> uktype_local = local.uktype_local;
			uktype_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_8);
			package(&tgw_head, &req_head, result_8);
		}();
		break;
	case 9:
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		[&]() {
			vector<shared_ptr<calendar_local_sql::Calendar>>			result_9;
			shared_ptr<calendar_local_sql> calendar_local = local.calendar_local;
			//calendar_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_9);
			package(&tgw_head, &req_head, result_9);
		}();
		break;
	case 10:
		error_str.emplace_back("暂时不支持此表的查询");
		LOG(ERROR) << "暂时不支持此表的查询";
		err_num = -10;
		return false;
		[&]() {
			vector < shared_ptr < component_local_sql::Component >> result_10;
			shared_ptr<component_local_sql> component_local = local.component_local;
			//component_local->find(req_head.MarketID, req_head.SecurityID, req_head.Date, result_10);
			package(&tgw_head, &req_head, result_10);
		}();
		break;
	default:
		break;
	}
	return true;
}
bool Query::HasError()
{
	return (err_num != 0);
}
vector<string> Query::GetErrorInfo()
{
	rapidjson::Document doc_reply;
	doc_reply.SetObject();
	auto& allocator = doc_reply.GetAllocator();
	if (!jsonstream.ConnectByTgw())//不包含tgw头,只有请求信息json字符串
	{
		doc_reply.AddMember("Seqno", jsonstream.GetReqHead().Seqno, allocator);
		doc_reply.AddMember("ErrNo", err_num, allocator);
		rapidjson::Value v(error_str[0].c_str(), error_str[0].size(), allocator);
		doc_reply.AddMember("ErrInfo", v, allocator);
		doc_reply.AddMember("IsLast", "Y", allocator);
		doc_reply.AddMember("SerialID", jsonstream.GetReqHead().SecurityID, allocator);
		doc_reply.AddMember("Count", 0, allocator);
		rapidjson::Value Struct;
		Struct.SetArray();
		doc_reply.AddMember("Structs", Struct, allocator);
		error_str[0] = Document2String(doc_reply);
	}
	else
	{
		rapidjson::Value tgw_replay;
		tgw_replay.SetObject();

		tgw_replay.AddMember("nodeid", jsonstream.GetTgwHead().NodeId, allocator);
		tgw_replay.AddMember("apptype", jsonstream.GetTgwHead().AppType, allocator);
		tgw_replay.AddMember("packtype", jsonstream.GetTgwHead().PackType + 1, allocator);
		tgw_replay.AddMember("conlvl", jsonstream.GetTgwHead().Conlvl, allocator);
		tgw_replay.AddMember("address", jsonstream.GetTgwHead().Address, allocator);
		tgw_replay.AddMember("termid", jsonstream.GetTgwHead().TermId.c_str(), allocator);
		tgw_replay.AddMember("userid", jsonstream.GetTgwHead().UserId.c_str(), allocator);
		tgw_replay.AddMember("appname", jsonstream.GetTgwHead().AppName.c_str(), allocator);
		tgw_replay.AddMember("packname", jsonstream.GetTgwHead().PackName.c_str(), allocator);
		tgw_replay.AddMember("cellid", jsonstream.GetTgwHead().CellId.c_str(), allocator);

		rapidjson::Value data_reply;
		data_reply.SetObject();
		data_reply.AddMember("Seqno", jsonstream.GetReqHead().Seqno, allocator);
		data_reply.AddMember("ErrNo", err_num, allocator);
		rapidjson::Value v(error_str[0].c_str(), error_str[0].size(), allocator);
		doc_reply.AddMember("ErrInfo", v, allocator);
		data_reply.AddMember("IsLast", "Y", allocator);
		data_reply.AddMember("SerialID", jsonstream.GetReqHead().SecurityID, allocator);
		data_reply.AddMember("Count", 0, allocator);
		rapidjson::Value Struct;
		Struct.SetArray();
		data_reply.AddMember("Structs", Struct, allocator);
		doc_reply.AddMember("tgw", tgw_replay, allocator);
		doc_reply.AddMember("data", data_reply, allocator);
		error_str[0] = Document2String(doc_reply);
	}
	return error_str;
}

void Update_local_data()
{
	GetTrday tt;
	while (true)
	{
		if (date == tt())//还不是更新的时间,循环睡眠
		{
			sleep(10);
		}
		else//到达更新时间
		{
			is_init_seccuss = false;
			sleep(10);
			LOG(WARNING) << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
			LOG(WARNING) << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
			LOG(WARNING) << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
			LOG(WARNING) << "开始更新 : 日期  " << tt();
			//重新更新,新的数据
			vector<thread>  thread_update;
			thread_update.emplace_back([&]() {local.contract_local.reset(new contract_local_sql); });
			thread_update.emplace_back([&]() {local.currency_local.reset(new currency_local_sql); });
			//thread_update.emplace_back([&]() {local.dailyclear_local.reset(new dailyclear_local_sql); });
			thread_update.emplace_back([&]() {local.market_local.reset(new Market_local_sql); });
			thread_update.emplace_back([&]() {local.secumatre_local.reset(new secumatre_local_sql); });
			//thread_update.emplace_back([&]() {local.tssyscalender_local.reset(new tssyscalender_local_sql); });
			thread_update.emplace_back([&]() {local.ukey_local.reset(new ukey_local_sql); });
			thread_update.emplace_back([&]() {local.uktype_local.reset(new uktype_local_sql); });
			//thread_update.emplace_back([&]() {local.calendar_local.reset(new calendar_local_sql); });
			//thread_update.emplace_back([&]() {local.component_local.reset(new component_local_sql); });
			for (auto &th : thread_update)
			{
				th.join();
			}
			is_init_seccuss = true;
			date = tt();
			LOG(WARNING) << "初始化内存数据库成功" << " $$$$$$$@@@@@@@@ 最新数据日期 : " << date << endl;
			LOG(WARNING) << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
			LOG(WARNING) << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
			LOG(WARNING) << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
		}
	}
}
