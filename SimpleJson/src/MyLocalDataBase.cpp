#include "MyLocalDataBase.h"

#define  STRNCPY(DES,SOUCE)  strncpy(DES,SOUCE,sizeof(DES)-1)
extern std::map<int, std::string> map_code_name;
extern ConnPool *connpool;

contract_local_sql::contract_local_sql()
{
	char				Sql[10240]{ 0 };
	vector<contract*>	local_data;
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	sprintf(Sql, "SELECT * FROM %s;", map_code_name[1].c_str());
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		contract* temp_data = new contract{ 0 };
		temp_data->contract_id = rs->getInt("contract_id");
		STRNCPY(temp_data->contract_code, rs->getString("contract_code").c_str());
		STRNCPY(temp_data->chinese_name, rs->getString("chinese_name").c_str());
		STRNCPY(temp_data->english_name, rs->getString("english_name").c_str());
		STRNCPY(temp_data->multiplier_desc, rs->getString("multiplier_desc").c_str());
		STRNCPY(temp_data->price_unit_desc, rs->getString("price_unit_desc").c_str());
		STRNCPY(temp_data->tick_size_desc, rs->getString("tick_size_desc").c_str());
		STRNCPY(temp_data->max_fluctuation_limit_desc, rs->getString("max_fluctuation_limit_desc").c_str());
		STRNCPY(temp_data->contract_month_desc, rs->getString("contract_month_desc").c_str());
		STRNCPY(temp_data->trading_time_desc, rs->getString("trading_time_desc").c_str());
		STRNCPY(temp_data->last_trading_date_desc, rs->getString("last_trading_date_desc").c_str());
		STRNCPY(temp_data->delivery_date_desc, rs->getString("delivery_date_desc").c_str());
		STRNCPY(temp_data->delivery_grade_desc, rs->getString("delivery_grade_desc").c_str());
		STRNCPY(temp_data->delivery_points_desc, rs->getString("delivery_points_desc").c_str());
		STRNCPY(temp_data->min_trading_margin_desc, rs->getString("min_trading_margin_desc").c_str());
		STRNCPY(temp_data->trading_fee_desc, rs->getString("trading_fee_desc").c_str());
		STRNCPY(temp_data->delivery_methods_desc, rs->getString("delivery_methods_desc").c_str());
		temp_data->list_date = rs->getInt("list_date");
		temp_data->delist_date = rs->getInt("delist_date");
		temp_data->currency_id = rs->getInt("currency_id");
		STRNCPY(temp_data->contract_desc, rs->getString("contract_desc").c_str());
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	for (auto &it : local_data)
	{
		shared_ptr<Contract> p = make_shared<Contract>(*it);
		_index[(*it).contract_id] = p;
		delete it;
	}
}
contract_local_sql::~contract_local_sql()
{
	_index.clear();
}
bool contract_local_sql::is_empty()
{
	return _index.empty();
}
void contract_local_sql::show_data()
{
}
void contract_local_sql::clear()
{
	_index.clear();
}
bool contract_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Contract>>& result)
{
	if (ukey != 0)
	{
		auto id = _index.find(static_cast<int>(ukey));
		if (id != _index.end())
		{
			result.emplace_back(id->second);
			return true;
		}
	}
	else
	{
		for (auto &it : _index)
		{
			result.emplace_back(it.second);
		}
		return true;
	}
	return false;
}

currency_local_sql::currency_local_sql()
{
	vector<currency*>   local_data;
	char				Sql[10240]{ 0 };
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	sprintf(Sql, "SELECT * FROM %s;", map_code_name[2].c_str());
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		currency* temp_data = new currency{ 0 };
		temp_data->currency_id = rs->getInt("currency_id");
		STRNCPY(temp_data->brief_code, rs->getString("brief_code").c_str());
		STRNCPY(temp_data->chinese_name, rs->getString("chinese_name").c_str());
		STRNCPY(temp_data->english_name, rs->getString("english_name").c_str());
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	for (auto &it : local_data)
	{
		shared_ptr<Currency> p = make_shared<Currency>(*it);
		_index[it->currency_id] = p;
		delete it;
	}
}
currency_local_sql::~currency_local_sql()
{
	_index.clear();
}
bool currency_local_sql::is_empty()
{
	return _index.empty();
}
void currency_local_sql::show_data()
{
}
void currency_local_sql::clear()
{
	_index.clear();
}
bool currency_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Currency>>& result)
{
	if (ukey != 0)
	{
		auto id = _index.find(static_cast<int>(ukey));
		if (id != _index.end())
		{
			result.emplace_back(id->second);
			return true;
		}
	}
	else
	{
		for (auto &it : _index)
		{
			result.emplace_back(it.second);
		}
		return true;
	}
	return false;
}

dailyclear_local_sql::dailyclear_local_sql()
{
	GetTrday ttt;
	date_end = ttt();
	date_start = ttt - DATE_DISTANCE;
	vector<dailyclear*>  local_data;
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT * FROM %s a INNER JOIN (SELECT ukey,MAX(`trading_day`) trading_day FROM %s  GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`trading_day`=b.`trading_day`;", map_code_name[3].c_str(), map_code_name[3].c_str());
	auto TransToLocal = [&]() {
		
		Connection*			con = nullptr;
		Statement*			state = nullptr;
		sql::ResultSet*		rs = nullptr;
		// 从连接池中获取mysql连接
		con = connpool->GetConnection();
		state = con->createStatement();
		rs = state->executeQuery(Sql);
		while (rs->next())
		{
			dailyclear* temp_data = new dailyclear{ 0 };
			temp_data->ukey = rs->getInt64("ukey");
			STRNCPY(temp_data->marke_code, rs->getString("marke_code").c_str());
			temp_data->ukey = rs->getInt("trading_day");
			temp_data->upper_limit = rs->getInt64("upper_limit");
			temp_data->lower_limit = rs->getInt64("lower_limit");
			temp_data->pre_settlement = rs->getInt64("pre_settlement");
			temp_data->pre_close = rs->getInt64("pre_close");
			temp_data->pre_interest = rs->getInt64("pre_interest");
			temp_data->open = rs->getInt64("open");
			temp_data->high = rs->getInt64("high");
			temp_data->low = rs->getInt64("low");
			temp_data->close = rs->getInt64("close");
			temp_data->settlement = rs->getInt64("settlement");
			temp_data->volume = rs->getInt64("volume");
			temp_data->amt = rs->getInt64("amt");
			temp_data->interest = rs->getInt64("interest");
			temp_data->state = rs->getInt("state");
			local_data.emplace_back(temp_data);
		}
		rs->close();
		connpool->ReleaseConnection(con);
	};
	TransToLocal();

	memset(Sql, 0, 10240);
	sprintf(Sql, "SELECT * FROM %s WHERE trading_day>=%d AND trading_day<=%d;", map_code_name[3].c_str(), date_start, date_end);
	TransToLocal();
	for (auto &it : local_data)
	{
		shared_ptr<Dailyclear> p = make_shared<Dailyclear>(*it);
		_index[it->ukey][it->trading_day] = p;
		delete it;
	}
}
dailyclear_local_sql::~dailyclear_local_sql()
{
	_index.clear();
}
bool dailyclear_local_sql::is_empty()
{
	return _index.empty();
}
void dailyclear_local_sql::show_data()
{
}
void dailyclear_local_sql::clear()
{
	_index.clear();
}
bool dailyclear_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Dailyclear>>& result)
{
	if (ukey != 0)
	{
		auto is_ukey = _index.find(ukey);
		if (is_ukey != _index.end())
		{
			if (date == 0)
			{
				auto max_date = (is_ukey->second).rbegin();
				result.emplace_back(max_date->second);
				return true;
			}
			else
			{
				auto is_date = is_ukey->second.find(date);
				if (is_date != is_ukey->second.end())
				{
					result.emplace_back(is_date->second);
					return true;
				}
			}
		}
	}
	else
	{
		for (auto &uk : _index)
		{
			if (date == 0)
			{
				auto max_date = uk.second.rbegin();
				result.emplace_back(max_date->second);
			}
			else
			{
				auto is_date = uk.second.find(date);
				if (is_date != uk.second.end())
				{
					result.emplace_back(is_date->second);
				}
			}
		}
		return true;
	}
	return false;
}

Market_local_sql::Market_local_sql()
{
	vector<market*>  local_data;
	char Sql[10240]{ 0 };
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	sprintf(Sql, "SELECT * FROM %s;", map_code_name[4].c_str());
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		market* temp_data = new market{ 0 };
		temp_data->market_id = rs->getInt("market_id");
		temp_data->currency_id = rs->getInt("currency_id");
		temp_data->time_zone = rs->getInt("time_zone");
		STRNCPY(temp_data->brief_code, rs->getString("brief_code").c_str());
		STRNCPY(temp_data->trading_time, rs->getString("trading_time").c_str());
		STRNCPY(temp_data->chinese_name, rs->getString("chinese_name").c_str());
		STRNCPY(temp_data->english_name, rs->getString("english_name").c_str());
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	for (auto &it : local_data)
	{
		shared_ptr<Market> p = make_shared<Market>(*it);
		_index[it->market_id] = p;
		delete it;
	}
}
Market_local_sql::~Market_local_sql()
{
	_index.clear();
}
bool Market_local_sql::is_empty()
{
	return _index.empty();
}
void Market_local_sql::show_data()
{
}
void Market_local_sql::clear()
{
	_index.clear();
}
bool Market_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Market>>& reulst)
{
	if (market_id != 0)
	{
		auto id = _index.find(market_id);
		if (id != _index.end())
		{
			reulst.emplace_back(id->second);
			return true;
		}
	}
	else
	{
		for (auto &id : _index)
		{
			reulst.emplace_back(id.second);
		}
		return true;
	}
	return false;
}

secumatre_local_sql::secumatre_local_sql()
{
	GetTrday ttt;
	date_end = ttt();
	date_start = ttt - DATE_DISTANCE;
	vector<secumaster*>  local_data;
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT * ,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s a INNER JOIN (SELECT ukey,MAX(`trading_day`) trading_day FROM %s GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`trading_day`=b.`trading_day`;", map_code_name[5].c_str(), map_code_name[5].c_str());
	auto TransToLocal = [&]() {
		
		Connection*			con = nullptr;
		Statement*			state = nullptr;
		sql::ResultSet*		rs = nullptr;
		// 从连接池中获取mysql连接
		con = connpool->GetConnection();
		state = con->createStatement();
		rs = state->executeQuery(Sql);
		while (rs->next())
		{
			secumaster* temp_secumaster = new secumaster{ 0 };
			temp_secumaster->ukey = rs->getInt64("ukey");
			temp_secumaster->market_id = rs->getInt("market_id");
			temp_secumaster->major_type = rs->getInt("major_type");
			temp_secumaster->minor_type = rs->getInt("minor_type");
			STRNCPY(temp_secumaster->market_code, rs->getString("market_code").c_str());
			STRNCPY(temp_secumaster->market_abbr, rs->getString("market_abbr").c_str());
			STRNCPY(temp_secumaster->chinese_name, rs->getString("chinese_name").c_str());
			STRNCPY(temp_secumaster->english_name, rs->getString("english_name").c_str());
			temp_secumaster->list_date = rs->getInt("list_date");
			temp_secumaster->delist_date = rs->getInt("delist_date");
			temp_secumaster->currency_id = rs->getInt("currency_id");
			temp_secumaster->jy_code = rs->getInt("jy_code");
			STRNCPY(temp_secumaster->wind_code, rs->getString("wind_code").c_str());
			STRNCPY(temp_secumaster->input_code, rs->getString("input_code").c_str());
			STRNCPY(temp_secumaster->trading_time, rs->getString("trading_time").c_str());
			temp_secumaster->trading_day = rs->getInt("trading_day");
			temp_secumaster->pre_trading_day = rs->getInt("pre_trading_day");
			temp_secumaster->upper_limit = rs->getInt64("upper_limit");
			temp_secumaster->lower_limit = rs->getInt64("lower_limit");
			temp_secumaster->pre_close = rs->getInt64("pre_close");
			temp_secumaster->pre_settlement = rs->getInt64("pre_settlement");
			temp_secumaster->pre_interest = rs->getInt64("pre_interest");
			temp_secumaster->pre_volume = rs->getInt64("pre_volume");
			temp_secumaster->total_share = rs->getInt64("total_share");
			temp_secumaster->float_share = rs->getInt64("float_share");
			temp_secumaster->associate_code = rs->getInt64("associate_code");
			temp_secumaster->exercise_price = rs->getInt64("exercise_price");
			temp_secumaster->contract_id = rs->getInt("contract_id");
			temp_secumaster->min_order_size = rs->getInt("min_order_size");
			temp_secumaster->max_order_size = rs->getInt("max_order_size");
			temp_secumaster->lot_size = rs->getInt("lot_size");
			temp_secumaster->multiplier = rs->getInt("multiplier");
			temp_secumaster->tick_size = rs->getInt("tick_size");
			temp_secumaster->last_delivery_date = rs->getInt("last_delivery_date");
			temp_secumaster->min_trading_margin = rs->getInt("min_trading_margin");
			temp_secumaster->share_arrive = rs->getInt("share_arrive");
			temp_secumaster->money_arrive = rs->getInt("money_arrive");
			temp_secumaster->share_avail = rs->getInt("share_avail");
			temp_secumaster->money_avail = rs->getInt("money_avail");
			temp_secumaster->state = rs->getInt("state");
			temp_secumaster->board = rs->getInt("board");
			strcpy(temp_secumaster->last_update, rs->getString("lastupdate").c_str());
			local_data.emplace_back(temp_secumaster);
		}
		rs->close();
		connpool->ReleaseConnection(con);
	};
	TransToLocal();
	memset(Sql, 0, 10240);
	sprintf(Sql, "SELECT *,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s WHERE trading_day>=%d AND trading_day<=%d;", map_code_name[5].c_str(), date_start, date_end);
	TransToLocal();
	for (auto &it : local_data)
	{
		shared_ptr<Secumaster> p = make_shared<Secumaster>(*it);
		_index[(*it).market_id][(*it).ukey][(*it).trading_day] = p;
		delete it;
	}
}
secumatre_local_sql::~secumatre_local_sql()
{
	_index.clear();
}
bool secumatre_local_sql::is_empty()
{
	return _index.empty();
}
void secumatre_local_sql::show_data()
{
}
void secumatre_local_sql::clear()
{
	_index.clear();
}

bool secumatre_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Secumaster>>& result)
{
	if (ukey != 0)
	{
		if (market_id != 0)
		{
			auto is_marketid = _index.find(market_id);
			if (is_marketid != _index.end())
			{
				auto is_uk = is_marketid->second.find(ukey);
				if (is_uk != is_marketid->second.end())
				{
					if (date == 0)
					{
						auto max_data = is_uk->second.rbegin();
						result.emplace_back(max_data->second);
						return true;
					}
					else
					{
						auto is_date = is_uk->second.find(date);
						if (is_date != is_uk->second.end())
						{
							result.emplace_back(is_date->second);
							return true;
						}
					}
				}
			}
		}
		else
		{
			for (auto &is_marketid : _index)
			{
				auto is_uk = is_marketid.second.find(ukey);
				if (is_uk != is_marketid.second.end())
				{
					if (date == 0)
					{
						auto max_data = is_uk->second.rbegin();
						result.emplace_back(max_data->second);
						return true;
					}
					else
					{
						auto is_date = is_uk->second.find(date);
						if (is_date != is_uk->second.end())
						{
							result.emplace_back(is_date->second);
							return true;
						}
					}
				}
			}
		}
	}
	else
	{
		if (market_id != 0)
		{
			auto is_marketid = _index.find(market_id);
			if (is_marketid != _index.end())
			{
				for (auto &it : is_marketid->second)
				{
					if (date == 0)
					{
						auto max_data = it.second.rbegin();
						result.emplace_back(max_data->second);
					}
					else
					{
						auto is_date = it.second.find(date);
						if (is_date != it.second.end())
						{
							result.emplace_back(is_date->second);
						}
					}
				}
				return true;
			}
		}
		else
		{
			for (auto &is_marketid : _index)
			{
				for (auto &it : is_marketid.second)
				{
					if (date == 0)
					{
						auto max_data = it.second.rbegin();
						result.emplace_back(max_data->second);
					}
					else
					{
						auto is_date = it.second.find(date);
						if (is_date != it.second.end())
						{
							result.emplace_back(is_date->second);
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}


tssyscalender_local_sql::tssyscalender_local_sql()
{
	vector<tssyscalender*>  local_data;
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT * FROM %s;", map_code_name[6].c_str());
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		tssyscalender* temp_data = new tssyscalender;
		memset(temp_data, '\0', static_cast<int>(sizeof(temp_data)));
		STRNCPY(temp_data->trday, rs->getString("trday").c_str());
		STRNCPY(temp_data->holiday, rs->getString("holiday").c_str());
		STRNCPY(temp_data->comm, rs->getString("comm").c_str());
		temp_data->weekno = rs->getInt("weekno");
		temp_data->dayofweek = rs->getInt("dayofweek");
		STRNCPY(temp_data->daystat, rs->getString("daystat").c_str());
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	for (auto &it : local_data)
	{
		shared_ptr<Tssyscalender> p = make_shared<Tssyscalender>(*it);
		_index[atoi(it->trday)] = p;
		delete it;
	}
}
tssyscalender_local_sql::~tssyscalender_local_sql()
{
	_index.clear();
}
bool tssyscalender_local_sql::is_empty()
{
	return _index.empty();
}
void tssyscalender_local_sql::show_data()
{
}
void tssyscalender_local_sql::clear()
{
	_index.clear();
}
bool tssyscalender_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Tssyscalender>>& result)
{
	if (date == 0)
	{
		result.emplace_back(_index.rbegin()->second);
		return true;
	}
	else
	{
		auto data = _index.find(date);
		if (data != _index.end())
		{
			result.emplace_back(data->second);
			return true;
		}
	}
	return false;
}

ukey_local_sql::ukey_local_sql()
{
	vector<ukey*>  local_data;
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT *,DATE_FORMAT(last_update,'%%Y%%m%%d%%H%%I%%S') AS lastupdate FROM %s;", map_code_name[7].c_str());
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		ukey* temp_data = new ukey{ 0 };
		temp_data->_ukey = rs->getInt64("ukey");
		temp_data->market_id = rs->getInt("market_id");
		temp_data->major_type = rs->getInt("major_type");
		temp_data->minor_type = rs->getInt("minor_type");
		STRNCPY(temp_data->market_code, rs->getString("market_code").c_str());
		STRNCPY(temp_data->market_abbr, rs->getString("market_abbr").c_str());
		STRNCPY(temp_data->chinese_name, rs->getString("chinese_name").c_str());
		STRNCPY(temp_data->english_name, rs->getString("english_name").c_str());
		temp_data->list_date = rs->getInt("list_date");
		temp_data->delist_date = rs->getInt("delist_date");
		temp_data->currency_id = rs->getInt("currency_id");
		temp_data->jy_code = rs->getInt("jy_code");
		STRNCPY(temp_data->wind_code, rs->getString("wind_code").c_str());
		STRNCPY(temp_data->input_code, rs->getString("input_code").c_str());
		strcpy(temp_data->last_update, rs->getString("lastupdate").c_str());
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	for (auto &it : local_data)
	{
		shared_ptr<Ukey> p = make_shared<Ukey>(*it);
		_index[it->market_id][it->_ukey] = p;
		delete it;
	}
}
ukey_local_sql::~ukey_local_sql()
{
	_index.clear();
}
bool ukey_local_sql::is_empty()
{
	return _index.empty();
}
void ukey_local_sql::show_data()
{
}
void ukey_local_sql::clear()
{
	_index.clear();
}
bool ukey_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Ukey>>& result)
{
	if (ukey != 0)
	{
		if (market_id != 0)
		{
			auto id = _index.find(market_id);
			if (id != _index.end())
			{
				auto uk = (id->second).find(ukey);
				if (uk != id->second.end())
				{
					result.emplace_back(uk->second);
					return true;
				}
			}
		}
		else
		{
			for (auto &id : _index)
			{
				auto uk = (id.second).find(ukey);
				if (uk != id.second.end())
				{
					result.emplace_back(uk->second);
					return true;
				}
			}
		}
	}
	else
	{
		if (market_id != 0)
		{
			auto id = _index.find(market_id);
			if (id != _index.end())
			{
				for (auto &it : id->second)
				{
					result.emplace_back(it.second);
				}
				return true;
			}
		}
		else
		{
			for (auto &id:_index)
			{
				for (auto &it : id.second)
				{
					result.emplace_back(it.second);
				}
			}
			return true;
		}
	}
	return false;
}

uktype_local_sql::uktype_local_sql()
{
	vector<uktype*>  local_data;
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT * FROM %s;", map_code_name[8].c_str());
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		uktype* temp_data = new uktype{ 0 };
		temp_data->major_type = rs->getInt("major_type");
		temp_data->minor_type = rs->getInt("minor_type");
		STRNCPY(temp_data->chinese_name, rs->getString("chinese_name").c_str());
		STRNCPY(temp_data->english_name, rs->getString("english_name").c_str());
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	int i = 0;
	for (auto &it : local_data)
	{
		shared_ptr<Uktype> p = make_shared<Uktype>(*it);
		_index[++i] = p;
		delete it;
	}
}
uktype_local_sql::~uktype_local_sql()
{
	_index.clear();
}
bool uktype_local_sql::is_empty()
{
	return _index.empty();
}
void uktype_local_sql::show_data()
{
}
void uktype_local_sql::clear()
{
	_index.clear();
}
bool uktype_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Uktype>>& result)
{
	for (auto &it : _index)
	{
		result.emplace_back(it.second);
	}
	return true;
}

calendar_local_sql::calendar_local_sql()
{
	vector<calendar*>  local_data;
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT * FROM %s;", map_code_name[9].c_str());
	Connection*			con = nullptr;
	Statement*			state = nullptr;
	sql::ResultSet*		rs = nullptr;
	// 从连接池中获取mysql连接
	con = connpool->GetConnection();
	state = con->createStatement();
	rs = state->executeQuery(Sql);
	while (rs->next())
	{
		calendar* temp_data = new calendar{ 0 };
		temp_data->market_id = rs->getInt("market_id");
		STRNCPY(temp_data->date, rs->getString("date").c_str());
		STRNCPY(temp_data->holiday, rs->getString("holiday").c_str());
		STRNCPY(temp_data->weeknum, rs->getString("weeknum").c_str());
		STRNCPY(temp_data->comm, rs->getString("comm").c_str());
		temp_data->daystat = rs->getInt("daystat");
		local_data.emplace_back(temp_data);
	}
	rs->close();
	connpool->ReleaseConnection(con);
	for (auto &it : local_data)
	{
		shared_ptr<Calendar> p = make_shared<Calendar>(*it);
		_index[it->market_id][atoi(it->date)] = p;
		delete it;
	}
}
calendar_local_sql::~calendar_local_sql()
{
	_index.clear();
}
bool calendar_local_sql::is_empty()
{
	return _index.empty();
}
void calendar_local_sql::show_data()
{
}
void calendar_local_sql::clear()
{
	_index.clear();
}
bool calendar_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Calendar>>& result)
{
	if (market_id != 0)
	{
		auto id = _index.find(market_id);
		if (id != _index.end())
		{
			if (date == 0)
			{
				auto max_data = id->second.rbegin();
				result.emplace_back(max_data->second);
				return true;
			}
			else
			{
				auto is_date = id->second.find(date);
				if (is_date != id->second.end())
				{
					result.emplace_back(is_date->second);
					return true;
				}
			}
		}
	}
	else
	{
		for (auto &id:_index)
		{
			if (date == 0)
			{
				auto max_data = id.second.rbegin();
				result.emplace_back(max_data->second);
			}
			else
			{
				auto is_date = id.second.find(date);
				if (is_date != id.second.end())
				{
					result.emplace_back(is_date->second);
				}
			}
		}
		return true;
	}
	return false;
}

component_local_sql::component_local_sql()
{
	GetTrday ttt;
	int date_end = ttt();
	int date_start = ttt - DATE_DISTANCE;
	vector<component*>  local_data;
	char Sql[10240]{ 0 };
	sprintf(Sql, "SELECT * FROM %s a INNER JOIN (SELECT ukey,MAX(`update_date`) update_date FROM %s  GROUP BY ukey) b ON a.`ukey`=b.`ukey` AND a.`update_date`=b.`update_date`;", map_code_name[10].c_str(), map_code_name[10].c_str());
	auto TransToLocal = [&]() {
		
		Connection*			con = nullptr;
		Statement*			state = nullptr;
		sql::ResultSet*		rs = nullptr;
		// 从连接池中获取mysql连接
		con = connpool->GetConnection();
		state = con->createStatement();
		rs = state->executeQuery(Sql);
		while (rs->next())
		{
			component* temp_data = new component{ 0 };
			temp_data->component_id = rs->getInt64("component_id");
			temp_data->update_date = rs->getInt("update_date");
			temp_data->ukey = rs->getInt64("ukey");
			temp_data->stock_amount = rs->getInt("stock_amount");
			temp_data->cash_substitute_sign = rs->getInt("cash_substitute_sign");
			temp_data->cash_substitute_proportion = rs->getDouble("cash_substitute_proportion");
			temp_data->fixed_substitute_money = rs->getDouble("fixed_substitute_money");
			local_data.emplace_back(temp_data);
		}
		rs->close();
		connpool->ReleaseConnection(con);
	};
	TransToLocal();

	memset(Sql, '\0', 10240);
	sprintf(Sql, "SELECT * FROM %s WHERE update_date>=%d AND update_date<=%d;", map_code_name[10].c_str(), date_start, date_end);
	TransToLocal();
	
	for (auto &it : local_data)
	{
		shared_ptr<Component> p = make_shared<Component>(*it);
		_index[it->ukey][it->update_date] = p;
		delete it;
	}
}
component_local_sql::~component_local_sql()
{
	_index.clear();
}
bool component_local_sql::is_empty()
{
	return _index.empty();
}
void component_local_sql::show_data()
{
}
void component_local_sql::clear()
{
	_index.clear();
}
bool component_local_sql::find(int market_id, int64_t ukey, int date, vector<shared_ptr<Component>>& result)
{
	if (ukey != 0)
	{
		auto is_ukey = _index.find(ukey);
		if (is_ukey != _index.end())
		{
			if (date == 0)
			{
				auto max_date = (is_ukey->second).rbegin();
				result.emplace_back(max_date->second);
				return true;
			}
			else
			{
				auto is_date = is_ukey->second.find(date);
				if (is_date != is_ukey->second.end())
				{
					result.emplace_back(is_date->second);
					return true;
				}
			}
		}
	}
	else
	{
		for (auto &uk : _index)
		{
			if (date == 0)
			{
				auto max_date = uk.second.rbegin();
				result.emplace_back(max_date->second);
			}
			else
			{
				auto is_date = uk.second.find(date);
				if (is_date != uk.second.end())
				{
					result.emplace_back(is_date->second);
				}
			}
		}
		return true;
	}
	return false;
}
