#include "BaseFile.h"

std::map<int, std::string> map_code_name
{
	{ 1,"ukdb09.contract" },
	{ 2,"ukdb09.currency" },
	{ 3,"ukdb09.dailyclear" },
	{ 4,"ukdb09.market" },
	{ 5,"ukdb09.secumaster" },
	{ 6,"ukdb09.tssyscalender" },
	{ 7,"ukdb09.ukey" },
	{ 8,"ukdb09.uktype" },
	{ 9,"ukdb09.calendar" },
	{ 10,"ukdb09.component" },
};

std::map<std::string, int> CONTRACT
{
	{ "contract_id", MY_INT },
	{ "contract_code",MY_CHAR },
	{ "chinese_name", MY_CHAR },
	{ "english_name", MY_CHAR },
	{ "multiplier_desc", MY_CHAR },
	{ "price_unit_desc", MY_CHAR },
	{ "tick_size_desc", MY_CHAR },
	{ "max_fluctuation_limit_desc", MY_CHAR },
	{ "contract_month_desc", MY_CHAR },
	{ "trading_time_desc", MY_CHAR },
	{ "last_trading_date_desc", MY_CHAR },
	{ "delivery_date_desc", MY_CHAR },
	{ "delivery_grade_desc", MY_CHAR },
	{ "delivery_points_desc", MY_CHAR },
	{ "min_trading_margin_desc", MY_CHAR },
	{ "trading_fee_desc", MY_CHAR },
	{ "delivery_methods_desc", MY_CHAR },
	{ "list_date",MY_INT },
	{ "delist_date", MY_INT },
	{ "currency_id", MY_INT },
	{ "contract_desc", MY_CHAR },
};

std::map<std::string, int> CURRENCY_
{
	{ "currency_id", MY_INT },
	{ "brief_code", MY_CHAR },
	{ "chinese_name", MY_CHAR },
	{ "english_name", MY_CHAR },
};

std::map<std::string, int> DAILYCLEAR
{
	{ "ukey", MY_INT64 },
	{ "marke_code",MY_CHAR },
	{ "trading_day",MY_INT },
	{ "upper_limit", MY_INT64 },
	{ "lower_limit", MY_INT64 },
	{ "pre_settlement", MY_INT64 },
	{ "pre_close", MY_INT64 },
	{ "pre_interest", MY_INT64 },
	{ "open", MY_INT64 },
	{ "high", MY_INT64 },
	{ "low", MY_INT64 },
	{ "close", MY_INT64 },
	{ "settlement", MY_INT64 },
	{ "volume", MY_INT64 },
	{ "amt", MY_INT64 },
	{ "interest", MY_INT64 },
	{ "state",MY_INT },
};

std::map<std::string, int> MARKET
{
	{ "market_id", MY_INT },
	{ "currency_id", MY_INT },
	{ "time_zone", MY_INT },
	{ "brief_code", MY_CHAR },
	{ "trading_time", MY_CHAR },
	{ "chinese_name", MY_CHAR },
	{ "english_name", MY_CHAR },
};

std::map<std::string, int> SECUMASTER
{
	{ "ukey",MY_INT64 },
	{ "market_id",MY_INT },
	{ "major_type", MY_INT },
	{ "minor_type", MY_INT },
	{ "market_code",MY_CHAR },
	{ "market_abbr", MY_CHAR },
	{ "chinese_name", MY_CHAR },
	{ "english_name", MY_CHAR },
	{ "list_date", MY_INT },
	{ "delist_date", MY_INT },
	{ "currency_id", MY_INT },
	{ "jy_code", MY_INT },
	{ "wind_code", MY_CHAR },
	{ "input_code", MY_CHAR },
	{ "trading_time", MY_CHAR },
	{ "trading_day", MY_INT },
	{ "pre_trading_day", MY_INT },
	{ "upper_limit",MY_INT64 },
	{ "lower_limit", MY_INT64 },
	{ "pre_close", MY_INT64 },
	{ "pre_settlement", MY_INT64 },
	{ "pre_interest", MY_INT64 },
	{ "pre_volume", MY_INT64 },
	{ "total_share", MY_INT64 },
	{ "float_share", MY_INT64 },
	{ "associate_code", MY_INT64 },
	{ "exercise_price", MY_INT64 },
	{ "contract_id", MY_INT },
	{ "min_order_size", MY_INT },
	{ "max_order_size", MY_INT },
	{ "lot_size", MY_INT },
	{ "multiplier", MY_INT },
	{ "tick_size", MY_INT },
	{ "last_delivery_date", MY_INT },
	{ "min_trading_margin", MY_INT },
	{ "share_arrive", MY_INT },
	{ "money_arrive", MY_INT },
	{ "share_avail", MY_INT },
	{ "money_avail", MY_INT },
	{ "state", MY_INT },
	{ "board", MY_INT },
	{ "last_update",MY_CHAR },
};

std::map<std::string, int> TSSYSCALENDER
{
	{ "trday", MY_CHAR },
	{ "holiday", MY_CHAR },
	{ "comm", MY_CHAR },
	{ "weekno", MY_INT },
	{ "dayofweek", MY_INT },
	{ "daystat", MY_CHAR },
};

std::map<std::string, int> UKEY
{
	{ "ukey",MY_INT64 },
	{ "market_id",MY_INT },
	{ "major_type", MY_INT },
	{ "minor_type", MY_INT },
	{ "market_code", MY_CHAR },
	{ "market_abbr", MY_CHAR },
	{ "chinese_name", MY_CHAR },
	{ "english_name", MY_CHAR },
	{ "list_date", MY_INT },
	{ "delist_date", MY_INT },
	{ "currency_id", MY_INT },
	{ "jy_code", MY_INT },
	{ "wind_code", MY_CHAR },
	{ "input_code", MY_CHAR },
	{ "last_update", MY_CHAR },
};

std::map<std::string, int> UKTYPE
{
	{ "major_type", MY_INT },
	{ "minor_type", MY_INT },
	{ "chinese_name", MY_CHAR },
	{ "english_name", MY_CHAR },
};

std::map<std::string, int> CALENDAR
{
	{ "market_id", MY_INT },
	{ "date", MY_CHAR },
	{ "holiday", MY_CHAR },
	{ "weeknum", MY_CHAR },
	{ "comm", MY_CHAR },
	{ "daystat", MY_INT },
};

std::map<std::string, int> COMPONENT
{

	{ "component_id", MY_INT64 },
	{ "update_date", MY_INT },
	{ "ukey", MY_INT64 },
	{ "stock_amount", MY_INT },
	{ "cash_substitute_sign", MY_INT },
	{ "cash_substitute_proportion", MY_DOUBLE },
	{ "fixed_substitute_money", MY_DOUBLE },
};