#ifndef __BASEFILE_H__
#define __BASEFILE_H__

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/exception.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <pthread.h>
#include <list>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <chrono>
#include <string>
#include <tuple>
#include <map>
#include <functional>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <exception>
#include "rapidjson/filestream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "iblog.h"
#include "isonbase.h"
#include "context.h"
#include "actor.h"
#include "stage.h"
#include "event.h"
#include "ibprotocol.h"
#include "socket.h"
#include "message.h"

#define  DEBUG
#define  _CRT_SECURE_NO_WARNINGS
#define  UPDATE_TIME_HOURS		14
#define  UPDATE_TIME_MIN		55
#define  DATE_DISTANCE			1
#define  STRNCPY(DES,SOUCE)  strncpy(DES,SOUCE,sizeof(DES)-1)

using namespace std;
using namespace sql;
using namespace chrono;
using namespace ison::base;

struct TgwHead
{
	int NodeId;
	std::string TermId;
	std::string UserId;
	int AppType;
	int PackType;
	std::string AppName;
	std::string PackName;
	std::string CellId;
	int Conlvl;
	int Address;
};

struct ReqHead
{
	int				Seqno;		//客户指令编号(不能重复)
	int64_t			SecurityID; //请求某证券的信息0代表所有
	int				TableType;  //表ID
	int				MarketID;	//市场ID
	int				Date;		//日期传0 表示全量最新或者对日期不敏感
	int				SerialID;	//请求第几个包
	int				PackSize;	//打包条数限制
	std::string		Field;		//请求的字段	字段2, 字段3   全部*
};

enum TYPE
{
	MY_UINT = 1,
	MY_INT,
	MY_INT64,
	MY_CHAR,
	MY_DOUBLE,
};

struct contract
{
	int		contract_id;
	char	contract_code[32];
	char	chinese_name[128];
	char	english_name[128];
	char	multiplier_desc[128];
	char	price_unit_desc[32];
	char	tick_size_desc[32];
	char	max_fluctuation_limit_desc[256];
	char	contract_month_desc[256];
	char	trading_time_desc[256];
	char	last_trading_date_desc[128];
	char	delivery_date_desc[128];
	char	delivery_grade_desc[512];
	char	delivery_points_desc[64];
	char	min_trading_margin_desc[512];
	char	trading_fee_desc[64];
	char	delivery_methods_desc[64];
	int		list_date;
	int		delist_date;
	int		currency_id;
	char	contract_desc[1024];
};

struct currency
{
	int		 currency_id;
	char	 brief_code[12];
	char	 chinese_name[128];
	char	 english_name[128];
};

struct dailyclear
{
	int64_t			ukey;
	char			marke_code[32];
	int				trading_day;
	int64_t			upper_limit;
	int64_t			lower_limit;
	int64_t			pre_settlement;
	int64_t			pre_close;
	int64_t			pre_interest;
	int64_t			open;
	int64_t			high;
	int64_t			low;
	int64_t			close;
	int64_t			settlement;
	int64_t			volume;
	int64_t			amt;
	int64_t			interest;
	int				state;
};

struct market
{
	int		market_id;
	int		currency_id;
	int		time_zone;
	char	brief_code[12];
	char	trading_time[64];
	char	chinese_name[128];
	char	english_name[128];
};

struct secumaster
{
	int64_t		ukey;
	int			market_id;
	int			major_type;
	int			minor_type;
	char		market_code[32];
	char		market_abbr[32];
	char		chinese_name[128];
	char		english_name[128];
	int			list_date;
	int			delist_date;
	int			currency_id;
	int			jy_code;
	char		wind_code[32];
	char		input_code[64];
	char		trading_time[64];
	int			trading_day;
	int			pre_trading_day;
	int64_t		upper_limit;
	int64_t		lower_limit;
	int64_t		pre_close;
	int64_t		pre_settlement;
	int64_t		pre_interest;
	int64_t		pre_volume;
	int64_t		total_share;
	int64_t		float_share;
	int64_t		associate_code;
	int64_t		exercise_price;
	int			contract_id;
	int			min_order_size;
	int			max_order_size;
	int			lot_size;
	int			multiplier;
	int			tick_size;
	int			last_delivery_date;
	int			min_trading_margin;
	int			share_arrive;
	int			money_arrive;
	int			share_avail;
	int			money_avail;
	int			state;
	int			board;
	char		last_update[32];
};

struct tssyscalender
{
	char	trday[12];
	char	holiday[4];
	char	comm[128];
	int		weekno;
	int		dayofweek;
	char	daystat[4];
};

struct ukey
{
	int64_t		_ukey;
	int			market_id;
	int			major_type;
	int			minor_type;
	char		market_code[32];
	char		market_abbr[32];
	char		chinese_name[128];
	char		english_name[128];
	int			list_date;
	int			delist_date;
	int			currency_id;
	int			jy_code;
	char		wind_code[32];
	char		input_code[64];
	char		last_update[32];
};

struct uktype
{
	int			major_type;
	int			minor_type;
	char		chinese_name[128];
	char		english_name[128];
};

struct calendar
{
	int			market_id;
	char		date[12];
	char		holiday[4];
	char		weeknum[4];
	char		comm[128];
	int			daystat;
};

struct  component
{
	int64_t			component_id;
	int				update_date;
	int64_t			ukey;
	int				stock_amount;
	int				cash_substitute_sign;
	double			cash_substitute_proportion;
	double			fixed_substitute_money;
};

class Contract
{
public:
	Contract() = default;

	Contract(contract& data)
	{
		_data = data;
	}

	Contract(Contract& data)
	{
		_data = data._data;
	}

	~Contract()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Contract& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetInt("contract_id") << "\t" << GetString("chinese_name") << "\t" << GetString("english_name") << "\t" << GetInt("list_date") << endl;
	}

	void Init()
	{
		ptr["contract_id"] = (void*)&_data.contract_id;
		ptr["contract_code"] = (void*)&_data.contract_code;
		ptr["chinese_name"] = (void*)&_data.chinese_name;
		ptr["english_name"] = (void*)&_data.english_name;
		ptr["multiplier_desc"] = (void*)&_data.multiplier_desc;
		ptr["price_unit_desc"] = (void*)&_data.price_unit_desc;
		ptr["tick_size_desc"] = (void*)&_data.tick_size_desc;
		ptr["max_fluctuation_limit_desc"] = (void*)&_data.max_fluctuation_limit_desc;
		ptr["contract_month_desc"] = (void*)&_data.contract_month_desc;
		ptr["trading_time_desc"] = (void*)&_data.trading_time_desc;
		ptr["last_trading_date_desc"] = (void*)&_data.last_trading_date_desc;
		ptr["delivery_date_desc"] = (void*)&_data.delivery_date_desc;
		ptr["delivery_grade_desc"] = (void*)&_data.delivery_grade_desc;
		ptr["delivery_points_desc"] = (void*)&_data.delivery_points_desc;
		ptr["min_trading_margin_desc"] = (void*)&_data.min_trading_margin_desc;
		ptr["trading_fee_desc"] = (void*)&_data.trading_fee_desc;
		ptr["delivery_methods_desc"] = (void*)&_data.delivery_methods_desc;
		ptr["list_date"] = (void*)&_data.list_date;
		ptr["delist_date"] = (void*)&_data.delist_date;
		ptr["currency_id"] = (void*)&_data.currency_id;
		ptr["contract_desc"] = (void*)&_data.contract_desc;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>	ptr;
	contract			_data;
};

class Currency
{
public:
	Currency() = default;

	Currency(currency& data)
	{
		_data = data;
	}

	Currency(Currency& data)
	{
		_data = data._data;
	}

	~Currency()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Currency& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("currency_id") << "\t" << GetString("chinese_name") << endl;
	}

	void Init()
	{
		ptr["currency_id"] = (void*)&_data.currency_id;
		ptr["brief_code"] = (void*)&_data.brief_code;
		ptr["chinese_name"] = (void*)&_data.chinese_name;
		ptr["english_name"] = (void*)&_data.english_name;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>	ptr;
	currency			_data;
};

class Dailyclear
{
public:
	Dailyclear() = default;

	Dailyclear(dailyclear& data)
	{
		_data = data;
	}

	Dailyclear(Dailyclear& data)
	{
		_data = data._data;
	}

	~Dailyclear()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Dailyclear& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("ukcode") << "\t" << GetInt("markecode") << endl;
	}

	void Init()
	{
		ptr["ukey"] = (void*)&_data.ukey;
		ptr["marke_code"] = (void*)&_data.marke_code;
		ptr["trading_day"] = (void*)&_data.trading_day;
		ptr["upper_limit"] = (void*)&_data.upper_limit;
		ptr["lower_limit"] = (void*)&_data.lower_limit;
		ptr["pre_settlement"] = (void*)&_data.pre_settlement;
		ptr["pre_close"] = (void*)&_data.pre_close;
		ptr["pre_interest"] = (void*)&_data.pre_interest;
		ptr["open"] = (void*)&_data.open;
		ptr["high"] = (void*)&_data.high;
		ptr["low"] = (void*)&_data.low;
		ptr["close"] = (void*)&_data.close;
		ptr["settlement"] = (void*)&_data.settlement;
		ptr["volume"] = (void*)&_data.volume;
		ptr["amt"] = (void*)&_data.amt;
		ptr["interest"] = (void*)&_data.interest;
		ptr["state"] = (void*)&_data.state;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}

private:
	map<string, void*>	ptr;
	dailyclear			_data;
};

class Market
{
public:
	Market() = default;

	Market(market& data)
	{
		_data = data;
	}

	Market(Market& data)
	{
		_data = data._data;
	}

	~Market()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Market& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("marketid") << "\t" << GetString("marketchname") << endl;
	}

	void Init()
	{
		ptr["market_id"] = (void*)&_data.market_id;
		ptr["currency_id"] = (void*)&_data.currency_id;
		ptr["time_zone"] = (void*)&_data.time_zone;
		ptr["brief_code"] = (void*)&_data.brief_code;
		ptr["trading_time"] = (void*)&_data.trading_time;
		ptr["chinese_name"] = (void*)&_data.chinese_name;
		ptr["english_name"] = (void*)&_data.english_name;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>	ptr;
	market			_data;
};

class Secumaster
{
public:
	Secumaster() = default;

	Secumaster(secumaster& data)
	{
		_data = data;
	}

	Secumaster(Secumaster& data)
	{
		_data = data._data;
	}

	~Secumaster()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Secumaster& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("ukcode") << "\t" << GetInt("marketid") << "\t" << GetString("chname") << endl;
	}

	void Init()
	{
		ptr["ukey"] = (void*)&_data.ukey;
		ptr["market_id"] = (void*)&_data.market_id;
		ptr["major_type"] = (void*)&_data.major_type;
		ptr["minor_type"] = (void*)&_data.minor_type;
		ptr["market_code"] = (void*)&_data.market_code;
		ptr["market_abbr"] = (void*)&_data.market_abbr;
		ptr["chinese_name"] = (void*)&_data.chinese_name;
		ptr["english_name"] = (void*)&_data.english_name;
		ptr["list_date"] = (void*)&_data.list_date;
		ptr["delist_date"] = (void*)&_data.delist_date;
		ptr["currency_id"] = (void*)&_data.currency_id;
		ptr["jy_code"] = (void*)&_data.jy_code;
		ptr["wind_code"] = (void*)&_data.wind_code;
		ptr["input_code"] = (void*)&_data.input_code;
		ptr["trading_time"] = (void*)&_data.trading_time;
		ptr["trading_day"] = (void*)&_data.trading_day;
		ptr["pre_trading_day"] = (void*)&_data.pre_trading_day;;
		ptr["upper_limit"] = (void*)&_data.upper_limit;
		ptr["lower_limit"] = (void*)&_data.lower_limit;
		ptr["pre_close"] = (void*)&_data.pre_close;
		ptr["pre_settlement"] = (void*)&_data.pre_settlement;
		ptr["pre_interest"] = (void*)&_data.pre_interest;
		ptr["pre_volume"] = (void*)&_data.pre_volume;
		ptr["total_share"] = (void*)&_data.total_share;
		ptr["float_share"] = (void*)&_data.float_share;
		ptr["associate_code"] = (void*)&_data.associate_code;
		ptr["exercise_price"] = (void*)&_data.exercise_price;
		ptr["contract_id"] = (void*)&_data.contract_id;
		ptr["min_order_size"] = (void*)&_data.min_order_size;
		ptr["max_order_size"] = (void*)&_data.max_order_size;
		ptr["lot_size"] = (void*)&_data.lot_size;
		ptr["multiplier"] = (void*)&_data.multiplier;
		ptr["tick_size"] = (void*)&_data.tick_size;
		ptr["last_delivery_date"] = (void*)&_data.last_delivery_date;
		ptr["min_trading_margin"] = (void*)&_data.min_trading_margin;
		ptr["share_arrive"] = (void*)&_data.share_arrive;
		ptr["money_arrive"] = (void*)&_data.money_arrive;
		ptr["share_avail"] = (void*)&_data.share_avail;
		ptr["money_avail"] = (void*)&_data.money_avail;
		ptr["state"] = (void*)&_data.state;
		ptr["board"] = (void*)&_data.board;
		ptr["last_update"] = (void*)&_data.last_update;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>	ptr;
	secumaster			_data;
};

class Tssyscalender
{
public:
	Tssyscalender() = default;

	Tssyscalender(tssyscalender& data)
	{
		_data = data;
	}

	Tssyscalender(Tssyscalender& data)
	{
		_data = data._data;
	}

	~Tssyscalender()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Tssyscalender& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetString("trday") << "\t" << GetString("holiday") << "\t" << GetString("comm") << "\t"
			<< GetInt("weekno") << "\t" << GetUInt("dayofweek") << "\t" << GetString("daystat") << endl;
	}

	void Init()
	{
		ptr["trday"] = (void*)&_data.trday;
		ptr["holiday"] = (void*)&_data.holiday;//需要处理
		ptr["comm"] = (void*)&_data.comm;
		ptr["weekno"] = (void*)&_data.weekno;
		ptr["dayofweek"] = (void*)&_data.dayofweek;
		ptr["daystat"] = (void*)&_data.daystat;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>		ptr;
	tssyscalender			_data;
};

class Ukey
{
public:
	Ukey() = default;

	Ukey(ukey& data)
	{
		_data = data;
	}

	Ukey(Ukey& data)
	{
		_data = data._data;
	}

	~Ukey()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Ukey& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("ukcode") << "\t" << GetInt("ukext") << "\t" << GetString("marketcode") << endl;
	}

	void Init()
	{
		ptr["ukey"] = (void*)&_data._ukey;
		ptr["market_id"] = (void*)&_data.market_id;
		ptr["major_type"] = (void*)&_data.major_type;
		ptr["minor_type"] = (void*)&_data.minor_type;
		ptr["market_code"] = (void*)&_data.market_code;
		ptr["market_abbr"] = (void*)&_data.market_abbr;
		ptr["chinese_name"] = (void*)&_data.chinese_name;

		ptr["english_name"] = (void*)&_data.english_name;
		ptr["list_date"] = (void*)&_data.list_date;
		ptr["delist_date"] = (void*)&_data.delist_date;
		ptr["currency_id"] = (void*)&_data.currency_id;
		ptr["jy_code"] = (void*)&_data.jy_code;
		ptr["wind_code"] = (void*)&_data.wind_code;
		ptr["input_code"] = (void*)&_data.input_code;
		ptr["last_update"] = (void*)&_data.last_update;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>		ptr;
	ukey					_data;
};

class Uktype
{
public:
	Uktype() = default;

	Uktype(uktype& data)
	{
		_data = data;
	}

	Uktype(Uktype& data)
	{
		_data = data._data;
	}

	~Uktype()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Uktype& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("majortype") << "\t" << GetInt("minortype") << "\t" << GetString("typechname") << endl;
	}

	void Init()
	{
		ptr["major_type"] = (void*)&_data.major_type;
		ptr["minor_type"] = (void*)&_data.minor_type;
		ptr["chinese_name"] = (void*)&_data.chinese_name;
		ptr["english_name"] = (void*)&_data.english_name;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>		ptr;
	uktype					_data;
};

class Calendar
{
public:
	Calendar() = default;

	Calendar(calendar& data)
	{
		_data = data;
	}

	Calendar(Calendar& data)
	{
		_data = data._data;
	}

	~Calendar()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Calendar& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("market_id") << "\t" << GetInt("daystat") << "\t" << GetString("comm") << endl;
	}

	void Init()
	{
		ptr["market_id"] = (void*)&_data.market_id;
		ptr["date"] = (void*)&_data.date;
		ptr["holiday"] = (void*)&_data.holiday;
		ptr["weeknum"] = (void*)&_data.weeknum;
		ptr["comm"] = (void*)&_data.comm;
		ptr["daystat"] = (void*)&_data.daystat;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>			ptr;
	calendar					_data;
};

class Component
{
public:
	Component() = default;

	Component(component& data)
	{
		_data = data;
	}

	Component(Component& data)
	{
		_data = data._data;
	}

	~Component()
	{
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void operator=(Component& data)
	{
		_data = data._data;
		if (!ptr.empty())
		{
			ptr.clear();
		}
	}

	void show()
	{
		cout << GetUInt("update_date") << "\t" << GetDouble("component_id") << "\t" << GetDouble("ukey") << endl;
	}

	void Init()
	{
		ptr["component_id"] = (void*)&_data.component_id;
		ptr["update_date"] = (void*)&_data.update_date;
		ptr["ukey"] = (void*)&_data.ukey;
		ptr["stock_amount"] = (void*)&_data.stock_amount;
		ptr["cash_substitute_sign"] = (void*)&_data.cash_substitute_sign;
		ptr["cash_substitute_proportion"] = (void*)&_data.cash_substitute_proportion;
		ptr["fixed_substitute_money"] = (void*)&_data.fixed_substitute_money;
	}
	double GetDouble(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(double*)(it->second);
		}
		return -1.0;
	}
	int GetInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int*)(it->second);
		}
		return -1;
	}
	unsigned int GetUInt(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(unsigned int*)(it->second);
		}
		return -1;
	}
	string GetString(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return string((char*)(it->second));
		}
		return string("");
	}
	int64_t GetInt64_t(string lable)
	{
		auto it = ptr.find(lable);
		if (it != ptr.end())
		{
			return *(int64_t*)(it->second);
		}
		return -1;
	}
private:
	map<string, void*>			ptr;
	component					_data;
};

struct GetTrday
{
public:
	GetTrday()
	{
		pstm = nullptr;
	}

	int operator()()
	{
		tt = time(NULL);
		pstm = localtime(&tt);
		if (pstm->tm_wday == 6)//星期6
		{
			tt -= 1 * 24 * 60 * 60;
		}
		else if (pstm->tm_wday == 0)//星期天
		{
			tt -= 2 * 24 * 60 * 60;
		}
		else if (pstm->tm_wday == 1)//周一
		{
			if (pstm->tm_hour < UPDATE_TIME_HOURS)
			{
				tt -= 3 * 24 * 60 * 60;
			}
			else if (pstm->tm_hour == UPDATE_TIME_HOURS && pstm->tm_min < UPDATE_TIME_MIN)
			{
				tt -= 3 * 24 * 60 * 60;
			}
		}
		else//周二到周五
		{
			if (pstm->tm_hour < UPDATE_TIME_HOURS)//判断是否,到达更新时间,没到的交易日为上一天
			{
				tt -= 1 * 24 * 60 * 60;
			}
			else if (pstm->tm_hour == UPDATE_TIME_HOURS && pstm->tm_min < UPDATE_TIME_MIN)
			{
				tt -= 1 * 24 * 60 * 60;
			}
		}
		pstm = localtime(&tt);
		int year = pstm->tm_year + 1900;
		int mon = pstm->tm_mon + 1;
		int day = pstm->tm_mday;
		return year * 10000 + mon * 100 + day;
	}
	int operator-(int day)
	{
		(*this)();
		tt -= day * 24 * 60 * 60;
		pstm = localtime(&tt);
		int year = pstm->tm_year + 1900;
		int mon = pstm->tm_mon + 1;
		day = pstm->tm_mday;
		return year * 10000 + mon * 100 + day;
	}
	int operator+(int day)
	{
		(*this)();
		tt += day * 24 * 60 * 60;
		pstm = localtime(&tt);
		int year = pstm->tm_year + 1900;
		int mon = pstm->tm_mon + 1;
		day = pstm->tm_mday;
		return year * 10000 + mon * 100 + day;
	}

	int64_t GetSecond()
	{
		return time(nullptr);
	}

	int64_t GetMsTime(int ymd, int hmsu)
	{
		struct tm timeinfo = { 0 };
		timeinfo.tm_year = ymd / 10000 - 1900;
		timeinfo.tm_mon = (ymd % 10000) / 100 - 1;
		timeinfo.tm_mday = ymd % 100;
		timeinfo.tm_hour = hmsu / 10000;
		timeinfo.tm_min = (hmsu % 10000) / 100;
		timeinfo.tm_sec = ((hmsu % 10000) % 100);
		return mktime(&timeinfo);
	}
private:
	struct tm*	pstm = nullptr;
	time_t		tt;
};

#endif // !__BASEFILE_H__
