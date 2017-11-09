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
		auto id = _index.find(ukey);
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