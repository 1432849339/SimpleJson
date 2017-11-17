#ifndef JSON_STREAM_H__
#define JSON_STREAM_H__

#include "BaseFile.h"

class JsonException{
public:
	JsonException()
	{
		error_num = 0;
		error_str.clear();
	}

	JsonException(int num, std::string err_info)
	{
		error_num = num;
		error_str = err_info;
	}

	void set_exception_info(int num, std::string err_info)
	{
		error_num = num;
		error_str = err_info;
	}

	std::tuple<int, std::string> GetInfo()
	{
		return std::make_tuple(error_num, error_str);
	}

private:
	int				error_num;
	std::string		error_str;
};

class JsonStream
{
public:

	JsonStream() = default;
	JsonStream(std::string messege) :m_messege(messege)
	{
		is_tgw_conn = false;
	}
	void set_messge(std::string message)
	{
		m_messege = message;
		is_tgw_conn = false;
	}
	virtual JsonException ParaseTgwHead()
	{
		m_document.Parse<0>(m_messege.c_str());
		if (m_document.HasParseError())
		{
			LOG(ERROR) << m_document.GetParseError();
			return JsonException(-1, m_document.GetParseError());
		}
		if (!m_document.IsObject())
		{
			LOG(ERROR) << "请求信息不是json对象";
			return JsonException(-2, "请求信息不是json对象");
		}
		if (!m_document.HasMember("tgw"))//后端请求:直连
		{
			LOG(INFO) << "后端直连";
			return JsonException(0, "后端直连");
		}
		else
		{
			if (!m_document["tgw"].IsObject())
			{
				LOG(ERROR) << "tgw头不是json对象";
				return JsonException(-3, "tgw头不是json对象");
			}
			else
			{
				is_tgw_conn = true;
				rapidjson::Value tgw_head;
				tgw_head.SetObject();
				tgw_head = m_document["tgw"];
				const char * tgw_head_int[] = { "nodeid", "apptype", "packtype", "conlvl", "address" };
				const char * tgw_head_char[] = { "termid", "userid", "appname", "packname", "cellid" };
				for (int i = 0; i < sizeof(tgw_head_int) / sizeof(tgw_head_int[0]); i++)
				{
					if (!tgw_head.HasMember(tgw_head_int[i]) || !tgw_head[tgw_head_int[i]].IsInt())
					{
						std::string temp = std::string("tgw头中缺少字段:") + tgw_head_int[i];
						LOG(ERROR) << temp;
						return JsonException(-4, temp);
					}
				}
				for (int i = 0; i < sizeof(tgw_head_char) / sizeof(tgw_head_char[0]); i++)
				{
					if (!tgw_head.HasMember(tgw_head_char[i]) || !tgw_head[tgw_head_char[i]].IsString())
					{
						std::string temp = std::string("tgw头中缺少字段:") + tgw_head_char[i];
						LOG(ERROR) << temp;
						return JsonException(-4, temp);
					}
				}
				m_tgwhead.NodeId = tgw_head[tgw_head_int[0]].GetInt();
				m_tgwhead.AppType = tgw_head[tgw_head_int[1]].GetInt();
				m_tgwhead.PackType = tgw_head[tgw_head_int[2]].GetInt();
				m_tgwhead.Conlvl = tgw_head[tgw_head_int[3]].GetInt();
				m_tgwhead.Address = tgw_head[tgw_head_int[4]].GetInt();
				m_tgwhead.TermId = tgw_head[tgw_head_char[0]].GetString();
				m_tgwhead.UserId = tgw_head[tgw_head_char[1]].GetString();
				m_tgwhead.AppName = tgw_head[tgw_head_char[2]].GetString();
				m_tgwhead.PackName = tgw_head[tgw_head_char[3]].GetString();
				m_tgwhead.CellId = tgw_head[tgw_head_char[4]].GetString();
			}
		}
		return JsonException(0, "");
	}
	virtual JsonException ParaseReqHead()
	{
		if (m_document.HasParseError())
		{
			LOG(ERROR) << m_document.GetParseError();
			return JsonException(-1, m_document.GetParseError());
		}
		if (!m_document.IsObject())
		{
			LOG(ERROR) << "请求信息不是json对象";
			return JsonException(-2, "请求信息不是json对象");
		}
		else
		{
			rapidjson::Value req_head;
			req_head.SetObject();
			if (m_document.HasMember("tgw"))
			{
				if (!m_document.HasMember("data"))
				{
					LOG(ERROR) << "请求信息没有data字段";
					return JsonException(-5, "请求信息没有data字段");
				}
				req_head = m_document["data"];
			}
			else
			{
				rapidjson::Document::AllocatorType &allocator = m_document.GetAllocator();
				req_head.AddMember("Seqno", m_document["Seqno"],allocator);
				req_head.AddMember("SecurityID", m_document["SecurityID"], allocator);
				req_head.AddMember("TableType", m_document["TableType"], allocator);
				req_head.AddMember("MarketID", m_document["MarketID"], allocator);
				req_head.AddMember("Date", m_document["Date"], allocator);
				req_head.AddMember("SerialID", m_document["SerialID"], allocator);
				req_head.AddMember("PackSize", m_document["PackSize"], allocator);
				req_head.AddMember("Field", m_document["Field"], allocator);
			}
			const char * req_tgwint[] = { "Seqno", "TableType", "MarketID", "Date", "SerialID", "PackSize" };
			for (int i = 0; i < sizeof(req_tgwint) / sizeof(req_tgwint[0]); i++)
			{
				if (!req_head.HasMember(req_tgwint[i]) || !req_head[req_tgwint[i]].IsInt())
				{
					std::string temp = std::string("没有字段: ") + req_tgwint[i];
					LOG(ERROR) << temp;
					return JsonException(-7, temp);
				}
			}
			if (!req_head.HasMember("SecurityID") || !req_head["SecurityID"].IsInt64())
			{
				LOG(ERROR) << "没有字段: SecurityID";
				return JsonException(-7, "没有字段: SecurityID");
			}
			if (!req_head.HasMember("Field") || !req_head["Field"].IsString())
			{
				LOG(ERROR) << "没有字段: Field";
				return JsonException(-7, "没有字段: Field");
			}
			m_reqhead.Seqno = req_head["Seqno"].GetInt();
			m_reqhead.SecurityID = req_head["SecurityID"].GetInt64(); 
			m_reqhead.TableType = req_head["TableType"].GetInt();
			m_reqhead.MarketID = req_head["MarketID"].GetInt();
			m_reqhead.Date = req_head["Date"].GetInt();
			m_reqhead.SerialID = req_head["SerialID"].GetInt();
			m_reqhead.PackSize = req_head["PackSize"].GetInt();
			m_reqhead.Field = req_head["Field"].GetString();
		}
		return JsonException(0, "");
	}
	virtual JsonException GetRequestField()
	{
		extern std::map<std::string, int> CONTRACT;
		extern std::map<std::string, int> CURRENCY_;
		extern std::map<std::string, int> DAILYCLEAR;
		extern std::map<std::string, int> MARKET;
		extern std::map<std::string, int> SECUMASTER;
		extern std::map<std::string, int> TSSYSCALENDER;
		extern std::map<std::string, int> UKEY;
		extern std::map<std::string, int> UKTYPE;
		extern std::map<std::string, int> CALENDAR;
		extern std::map<std::string, int> COMPONENT;
		if (m_reqhead.Field == "*")
		{
			switch (m_reqhead.TableType)
			{
			case 1:
				m_parameter.insert(CONTRACT.begin(), CONTRACT.end());
				break;
			case 2:
				m_parameter.insert(CURRENCY_.begin(), CURRENCY_.end());
				break;
			case 3:
				m_parameter.insert(DAILYCLEAR.begin(), DAILYCLEAR.end());
				break;
			case 4:
				m_parameter.insert(MARKET.begin(), MARKET.end());
				break;
			case 5:
				m_parameter.insert(SECUMASTER.begin(), SECUMASTER.end());
				break;
			case 6:
				m_parameter.insert(TSSYSCALENDER.begin(), TSSYSCALENDER.end());
				break;
			case 7:
				m_parameter.insert(UKEY.begin(), UKEY.end());
				break;
			case 8:
				m_parameter.insert(UKTYPE.begin(), UKTYPE.end());
				break;
			case 9:
				m_parameter.insert(CALENDAR.begin(), CALENDAR.end());
				break;
			case 10:
				m_parameter.insert(COMPONENT.begin(), COMPONENT.end());
				break;
			default:
				LOG(ERROR) << "TableType 超出范围";
				return JsonException(-8, "TableType 超出范围");
				break;
			}
		}
		else
		{
			size_t last = 0;
			size_t index = m_reqhead.Field.find_first_of(',', last);
			JsonException ret;
			while (index != std::string::npos)
			{
				std::string Field = m_reqhead.Field.substr(last, index - last);
				ret = GetType(Field);
				if (std::get<0>(ret.GetInfo()))
				{
					return ret;
				}
				last = index + 1;
				index = m_reqhead.Field.find_first_of(',', last);
			}
			if (index - last > 0)
			{
				std::string Field = m_reqhead.Field.substr(last);
				ret = GetType(Field);
				if (std::get<0>(ret.GetInfo()))
				{
					return ret;
				}
			}
		}
		return JsonException(0, "");
	}
	TgwHead& GetTgwHead()
	{
		return m_tgwhead;
	}
	ReqHead& GetReqHead()
	{
		return m_reqhead;
	}
	std::map<std::string, int32_t>& GetParameter()
	{
		return m_parameter;
	}
	bool ConnectByTgw()
	{
		return is_tgw_conn;
	}
protected:
	JsonException GetType(std::string& lable)
	{
		extern std::map<std::string, int> CONTRACT;
		extern std::map<std::string, int> CURRENCY_;
		extern std::map<std::string, int> DAILYCLEAR;
		extern std::map<std::string, int> MARKET;
		extern std::map<std::string, int> SECUMASTER;
		extern std::map<std::string, int> TSSYSCALENDER;
		extern std::map<std::string, int> UKEY;
		extern std::map<std::string, int> UKTYPE;
		extern std::map<std::string, int> CALENDAR;
		extern std::map<std::string, int> COMPONENT;

		auto func = [&](std::map<std::string, int>& table)->JsonException {
			auto index = table.find(lable);
			if (index != table.end())
			{
				m_parameter.emplace(index->first, index->second);
				return JsonException(0,"");
			}
			else
			{
				std::string temp = std::string("没有字段:") + lable;
				LOG(ERROR) << temp;
				return JsonException(-9, temp);
			}
		};
		JsonException ret;
		switch (m_reqhead.TableType)
		{
		case 1://"ukdb2017.contract";
			ret = func(CONTRACT);
			break;
		case 2://"ukdb2017.currency";
			ret = func(CURRENCY_);
			break;
		case 3://"ukdb2017.dailyclear";
			ret = func(DAILYCLEAR);
			break;
		case 4://"ukdb2017.market";
			ret = func(MARKET);
			break;
		case 5://"ukdb2017.secumaster";
			ret = func(SECUMASTER);
			break;
		case 6://"ukdb2017.tssyscalender";
			ret = func(TSSYSCALENDER);
			break;
		case 7://"ukdb2017.ukey";
			ret = func(UKEY);
			break;
		case 8://"ukdb2017.uktype";
			ret = func(UKTYPE);
			break;
		case 9://"ukdb2017.cal";
			ret = func(CALENDAR);
			break;
		case 10://"ukdb2017.com";
			ret = func(COMPONENT);
			break;
		default:
			std::string temp = "TableType 超出范围";
			LOG(ERROR) << temp;
			return JsonException(-8, temp);
			break;
		}
		return ret;
	}
private:
	bool								is_tgw_conn;
	std::string							m_messege;
	rapidjson::Document					m_document;
	TgwHead								m_tgwhead;
	ReqHead								m_reqhead;
	std::map<std::string, int32_t>		m_parameter;
};
#endif