/**
 * @file DBConnectUnit.cpp
 * @brief 
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-28
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include "DBConnectUnit.h"

using namespace QGame;

uint32_t DBConnectUnit::countHandleId_ = 0;
const std::string DBConnectUnit::cmdRollback = "ROLLBACK";
const std::string DBConnectUnit::cmdCommit = "COMMIT";
const std::string DBConnectUnit::cmdTransaction = "SET AUTOCOMMIT = 0";
const std::string DBConnectUnit::cmdNoTransaction = "SET AUTOCOMMIT = 1";

DBConnectUnit::~DBConnectUnit()
{
	closeDb();
}

void DBConnectUnit::closeDb()
{
	if (mysql_)
	{
		mysql_close(mysql_.get());
		mysql_.reset();
	}
}

bool DBConnectUnit::connectDb()
{
	MYSQL* mysql = mysql_init(NULL);
	if (NULL == mysql)
	{
		LOG_ERROR("mysql_init");
		return false;
	}
	mysql_.reset(mysql);
	mysql_options(mysql_.get(), MYSQL_OPT_READ_TIMEOUT, (const char*)&expiredTime_);
	mysql_options(mysql_.get(), MYSQL_OPT_WRITE_TIMEOUT, (const char*)&expiredTime_);

	int reconnect = 1;
	mysql_options(mysql_.get(), MYSQL_OPT_RECONNECT, (const char*)&reconnect);

	mysql = mysql_real_connect(mysql_.get(), connectInfo_.host_.c_str(), 
			connectInfo_.user_.c_str(), connectInfo_.passwd_.c_str(), 
			connectInfo_.dbName_.c_str(), connectInfo_.port_, 0, 0);

	if (!mysql)
	{
		LOG_ERROR("mysql_real_connect");
		closeDb();
		return false;
	}

	handleId_ = ++countHandleId_;
	connStatus_ = DB_CONN_VALID;
	std::cout << "连接单元: " << handleId_ << "\t连接数据库成功" << std::endl; 
	return true;
}


void DBConnectUnit::initConnect()
{
	threadId_ = pthread_self();
	connStatus_ = DB_CONN_USING;	

	//重置时间
	timestamp_.reset();
	std::cout << "线程: " << threadId_ << "\t开始使用连接单元: " << handleId_ << std::endl;
}

void DBConnectUnit::resetConnect()
{
	std::cout << "线程: " << threadId_ << "\t结束使用连接单元: " << handleId_ << std::endl;
	threadId_ = 0;
	connStatus_ = DB_CONN_VALID;	

}


int64_t DBConnectUnit::elapseTime()
{
	return timestamp_.elapseTime();
}

std::ostringstream& DBConnectUnit::appendValue(std::ostringstream& osSql, const DbCol* dbCol)
{
	assert(dbCol);
	switch(dbCol->type_)
	{
		case DB_DATA_UCHAR:
			{
				uint8_t data = *(uint8_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_CHAR:
			{
				int8_t data = *(int8_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_USHORT:
			{
				uint16_t data = *(uint16_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_SHORT:
			{
				int16_t data = *(int16_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_UINT:
			{
				uint32_t data = *(uint32_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_INT:
			{
				int32_t data = *(int32_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_ULONG:
			{
				uint64_t data = *(uint64_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_LONG:
			{
				int64_t data = *(int64_t*)dbCol->data_;
				osSql << data;
			}
			break;
		case DB_DATA_STR:
		case DB_DATA_BIN:
			{
				uint32_t len = 2 * dbCol->size_ + 1;
				char* tmpData = new char[len];
				memset(tmpData, 0x00, len);
				mysql_real_escape_string(mysql_.get(), tmpData, (const char*)dbCol->data_, dbCol->size_);
				osSql << "\'" << tmpData << "\'";
			}
			break;
		default:
			break;
	}//scase witch
	return osSql;
}

std::ostringstream& DBConnectUnit::appendWhere(std::ostringstream& osSql, const DbCol* dbColWhere)
{
	if (NULL == dbColWhere)
		return osSql;

	osSql << " WHERE ";
	const DbCol* unitCol = dbColWhere;
	bool bFirst = true;
	while(NULL != unitCol->name_)
	{
		if (0 == strlen(unitCol->name_))
			continue;
		if (false == bFirst)
		{
			osSql << ", ";
		}
		else
		{
			bFirst = false;
		}

		osSql << unitCol->name_ << " = ";
		appendValue(osSql, unitCol);
		++unitCol;
	}//while
	return osSql;
}

uint32_t DBConnectUnit::calColSize(const DbCol* dbCol)
{
	if (NULL == dbCol)
	{
		return 0;
	}
	uint32_t sizeSum = 0;
	while(NULL != dbCol->name_)
	{
		sizeSum += dbCol->size_;
	}
	return sizeSum;
}

//数据库操作
int32_t DBConnectUnit::querySql(const char* sql, uint32_t sqlLen)
{
	if (NULL == sql || 0 == sqlLen || NULL == mysql_)
	{
		LOG_ERROR("querySql");
		return -1;
	}

	int32_t result = 0;
	result = mysql_real_query(mysql_.get(), sql, sqlLen);
	if (0 != result)
	{
		LOG_ERROR("mysql_real_query");
		std::cerr << mysql_error(mysql_.get()) << std::endl;
	}
	return result;
}

int32_t DBConnectUnit::dbInsert(const char* tableName, const DbCol* dbCol)
{
	if (NULL == tableName || NULL == dbCol || NULL == mysql_)
	{
		LOG_ERROR("dbDelete");
		return -1;
	}
	int32_t result = 0;

	const DbCol* unitCol = NULL;
	std::ostringstream osSql;	
	osSql << "INSERT INTO " << tableName << "(";
	unitCol = dbCol;
	bool bFirst = true;
	while(NULL != unitCol->name_)
	{
		uint32_t len = ::strlen(unitCol->name_);
		if (0 == len)
			continue;
		if (false == bFirst)
		{
			osSql << ", ";
		}
		else
		{
			bFirst = false;
		}
		osSql << unitCol->name_;
		++unitCol;
	}//while
	
	osSql << ") VALUES(";
	unitCol = dbCol;
	bFirst = true;
	while(NULL != unitCol->name_)
	{
		if (0 == strlen(unitCol->name_))
			continue;
		if (false == bFirst)
		{
			osSql << ", ";
		}
		else
		{
			bFirst = false;
		}

		appendValue(osSql, unitCol);
		++unitCol;
	}//while

	osSql << ")";
	result = querySql(osSql.str().c_str(), osSql.str().size());
	return result;
}

int32_t DBConnectUnit::dbDelete(const char* tableName, const DbCol* dbColWhere)
{
	if (NULL == tableName || NULL == mysql_)
	{
		LOG_ERROR("dbDelete");
		return -1;
	}
	int result = 0;
	std::ostringstream osSql;	
	osSql << "DELETE FROM " << tableName;
	
	appendWhere(osSql, dbColWhere);
	result = querySql(osSql.str().c_str(), osSql.str().size());
	return result;
}

int32_t DBConnectUnit::dbUpdate(const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere)
{
	if (NULL == tableName || NULL == dbCol || NULL == mysql_)	
	{
		LOG_ERROR("dbUpdate");
		return -1;
	}

	int32_t result = 0;

	const DbCol* unitCol = NULL;
	bool bFirst = true;
	std::ostringstream osSql;	
	osSql << "UPDATE " << tableName << " SET ";
	unitCol = dbCol;
	while(NULL != unitCol->name_)
	{
		uint32_t len = ::strlen(unitCol->name_);
		if (0 == len)
			continue;
		if (false == bFirst)
		{
			osSql << ", ";
		}
		else
		{
			bFirst = false;
		}
		osSql << unitCol->name_ << " = ";
		appendValue(osSql, unitCol);
		osSql << " ";
		++unitCol;
	}//while
	
	appendWhere(osSql, dbColWhere);
	result = querySql(osSql.str().c_str(), osSql.str().size());
	return result;
}

int32_t DBConnectUnit::dbSelect(const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere, const DbCol* dbColOrder, unsigned char** data, uint32_t * rows)
{
	if (NULL == tableName || NULL == dbCol || NULL == mysql_)
	{
		LOG_ERROR("dbSelect");
		return -1;
	}
	int32_t result = 0;	
	result = selectSql(tableName, dbCol, dbColWhere);
	if (0 != result)
	{
		LOG_ERROR("selectSql");
		return -1;
	}
	uint32_t rowCounts = 0;
	MYSQL_RES *  mysqlRes = NULL;
	mysqlRes = mysql_store_result(mysql_.get());
	if (NULL == mysqlRes)
	{
		LOG_ERROR("mysql_store_result");
		return -1;
	}
		
	rowCounts = static_cast<uint32_t>(mysql_num_rows(mysqlRes));
	if (0 == rowCounts)
	{
		mysql_free_result(mysqlRes);
		return 0;
	}

	uint32_t sizeSum = calColSize(dbCol);
	if (0 == sizeSum)
	{
		LOG_ERROR("calColSize");
		return -1;
	}

	*data = new unsigned char [rowCounts * sizeSum];
	memset(*data, 0, rowCounts * sizeSum);
	unsigned char* pData = *data;
	MYSQL_ROW      mysqlRow;
	
	while((mysqlRow = mysql_fetch_row(mysqlRes)))
	{
		unsigned long* lengths = mysql_fetch_lengths(mysqlRes);
		uint32_t offset = getDataFromMYSQL_ROW(mysqlRow, lengths, dbCol, pData);
		pData += offset;
	}
	*rows = rowCounts; 
	mysql_free_result(mysqlRes);
	return result;
}


int32_t DBConnectUnit::selectSql(const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere, const DbCol* dbColOrder, uint32_t limit, uint32_t offset)
{
	//由上一层调用函数判断参数有效
	int result = 0;	
	const DbCol* unitCol = NULL;
	bool bFirst = true;
	std::ostringstream osSql;
	unitCol = dbCol;
	
	osSql << "SELECT ";
	while(NULL != unitCol->name_)
	{
		uint32_t len = ::strlen(unitCol->name_);
		if (0 == len)
			continue;
		if (false == bFirst)
		{
			osSql << ", ";
		}
		else
		{
			bFirst = false;
		}
		osSql << unitCol->name_;
		++unitCol;
	}//while

	osSql << " From " << tableName;
	appendWhere(osSql, dbColWhere);
	
	if (limit > 0)
	{
		osSql << "LIMIT " << limit;
		if (offset > 0)
		{
			osSql << "OFFSET " << offset;
		}
	}

	result = querySql(osSql.str().c_str(), osSql.str().size());
	return result;
}

int32_t getDataFromMYSQL_ROW(MYSQL_ROW row, unsigned long* lengths, const DbCol* dbCol, unsigned char* data)
{
	assert(lengths && dbCol && data);

	uint32_t index  = 0;
	uint32_t offset = 0;	
	unsigned char* pData = NULL;
	while(NULL != dbCol->name_)
	{
		pData = data + offset;
		switch (dbCol->type_)
		{
			case DB_DATA_UCHAR:
				{
					*(unsigned char*)pData = (row[index] ? *(unsigned char*)(row[index]) : 0);
				}
				break;
			case DB_DATA_CHAR:
				{
					*(char*)pData = (row[index] ? *(char*)(row[index]) : 0);
				}
				break;
			case DB_DATA_USHORT:
				{
					*(unsigned short*)pData = (row[index] ? *(unsigned short*)(row[index]) : 0);
				}
				break;
			case DB_DATA_SHORT:
				{
					*(short*)pData = (row[index] ? *(short*)(row[index]) : 0);
				}
				break;
			case DB_DATA_UINT:
				{
					*(unsigned int*)pData = (row[index] ? *(unsigned int*)(row[index]) : 0);
				}
				break;
			case DB_DATA_INT:
				{
					*(int*)pData = (row[index] ? *(int*)(row[index]) : 0);
				}
				break;
			case DB_DATA_ULONG:
				{
					*(unsigned long*)pData = (row[index] ? *(unsigned long*)(row[index]) : 0);
				}
				break;
			case DB_DATA_LONG:
				{
					*(long*)pData = (row[index] ? *(long*)(row[index]) : 0);
				}
				break;
			case DB_DATA_STR:
			case DB_DATA_BIN:
				{
					memset(pData, 0, dbCol->size_);
					uint32_t minLen = (lengths[index] < dbCol->size_? lengths[index] : dbCol->size_);
					memcpy(pData, row[index], minLen);
				}
				break;
			default:
				break;
		}//switch (dbCol->type_)

		++index;
		offset += dbCol->size_;
		++dbCol;
	}//while(NULL != dbCol->name_)
	return offset;
}

