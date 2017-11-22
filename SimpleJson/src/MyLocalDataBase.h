#ifndef __MYLOCALDATABASE_H__
#define __MYLOCALDATABASE_H__

#include "BaseFile.h"
#include "connection_pool.h"

class contract_local_sql
{
public:
	typedef LocalData<contract> Contract;
public:
	contract_local_sql();
	~contract_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Contract>>& result);
private:
	map<int64_t, shared_ptr<Contract>>		_index;
};

class currency_local_sql
{
public:
	typedef LocalData<currency> Currency;
public:
	currency_local_sql();
	~currency_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Currency>>& result);
private:
	map<int, shared_ptr<Currency>>	_index;
};

class dailyclear_local_sql
{
public:
	typedef LocalData<dailyclear> Dailyclear;
public:
	dailyclear_local_sql();
	~dailyclear_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Dailyclear>>& result);
private:
	int date_start;
	int date_end;
	map<int64_t, map<int, shared_ptr<Dailyclear>>> _index;
};

class Market_local_sql
{
public:
	typedef LocalData<market> Market;
public:
	Market_local_sql();
	~Market_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Market>>& reulst);
private:
	map<int, shared_ptr<Market>>   _index;
};

class secumatre_local_sql
{
public:
	typedef LocalData<secumaster> Secumaster;
public:
	secumatre_local_sql();
	~secumatre_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Secumaster>>& result);
private:
	int		date_start;
	int		date_end;
	map<int, map<int64_t, map<int, shared_ptr<Secumaster>>>>  _index;
};

class tssyscalender_local_sql
{
public:
	typedef LocalData<tssyscalender> Tssyscalender;
public:
	tssyscalender_local_sql();
	~tssyscalender_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Tssyscalender>>& result);
private:
	map<int, shared_ptr<Tssyscalender>>   _index;
};

class ukey_local_sql
{
public:
	typedef LocalData<ukey> Ukey;
public:
	ukey_local_sql();
	~ukey_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Ukey>>& result);
private:
	map<int, map<int64_t, shared_ptr<Ukey>>>  _index;
};

class uktype_local_sql
{
public:
	typedef LocalData<uktype> Uktype;
public:
	uktype_local_sql();
	~uktype_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Uktype>>& result);
private:
	map<int, shared_ptr<Uktype>>	_index;
};

class calendar_local_sql
{
public:
	typedef LocalData<calendar> Calendar;
public:
	calendar_local_sql();
	~calendar_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Calendar>>& result);
private:
	map<int, map<int, shared_ptr<Calendar>>>		_index;
};

class component_local_sql
{
public:
	typedef LocalData<component> Component;
public:
	component_local_sql();
	~component_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Component>>& result);
private:
	map<int64_t, map<int, shared_ptr<Component>>>		_index;
};

class etf_component_local_sql
{
public:
	typedef LocalData<etf_component> Etf_component;
public:
	etf_component_local_sql();
	~etf_component_local_sql();
	bool is_empty();
	void show_data();
	void clear();
	bool find(int market_id, int64_t ukey, int date, vector<shared_ptr<Etf_component>>& result);
protected:
private:
	map<int64_t, map<int, map<int64_t, shared_ptr<Etf_component>>>>		_index;
};

#endif
