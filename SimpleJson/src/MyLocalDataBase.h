#ifndef __MYLOCALDATABASE_H__
#define __MYLOCALDATABASE_H__

#include "BaseFile.h"
#include "connection_pool.h"

class contract_local_sql
{
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

#endif


