/**
 * @file DBConnectPool.cpp
 * @brief 
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-28
 */
#include <unistd.h>
#include "DBConnectPool.h"
#include "DBConnectUnit.h"

using namespace QGame;

//从连接池获得一个有效连接
//先遍历mapPool_, 寻找有效连接，若成功，返回。
//invalidUint 记录mapPool_的一个无效连接
//如果mapPool_既没有可供使用的有效连接，
//也没有无效连接，则申请一个新的连接加入连接池。
uint32_t DBConnectPool::getUintFromPool()
{
	while(true)
	{
		MutexLockGuard lock(mutex_);
	    UnitPtr invalidUnit = NULL;	
		for (auto iter = mapPool_.begin(); iter != mapPool_.end(); )
		{
			UnitPtr iterValue = iter->second;
			if (NULL == iterValue)
			{
				iter = mapPool_.erase(iter);
				continue;
			}

			if (DB_CONN_VALID == iterValue->connStatus())
			{
				iterValue->initConnect();
				return iterValue->handleId();
			}
			if (DB_CONN_INVALID == iterValue->connStatus())
			{
				invalidUnit = iterValue;
			}
			++iter;
		}//for

		if (NULL != invalidUnit)
		{
			if (invalidUnit->connectDb())
			{
				invalidUnit->initConnect();
				return invalidUnit->handleId();
			}
		}

		if (mapPool_.size() < maxPoolSize_)
		{
			UnitPtr newUint = std::make_shared<DBConnectUnit>(connectInfo_);
			if (NULL == newUint)
				continue;

			if (false == newUint->connectDb())
				continue;

			newUint->initConnect();
			mapPool_[newUint->handleId()] = newUint;
			return newUint->handleId();
		}//if

		//防止busy-loop
		usleep(100);

	}//while(true)
}

DBConnectPool::UnitPtr DBConnectPool::getUnitPtrByHandleId(uint32_t  handleId)
{
	UnitPtr unitPtr = NULL;
	{
		MutexLockGuard lock(mutex_);
		POOLITER iter = mapPool_.find(handleId);
		if (iter != mapPool_.end())
		{
			unitPtr = iter->second;
		}
	}
	return unitPtr;
}

//把连接还给连接池
void DBConnectPool::releaseUintToPool(uint32_t handleId)
{
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		unitPtr->resetConnect();
	}
}

int32_t DBConnectPool::dbInsert(uint32_t handleId, const char* tableName, const DbCol* dbCol)
{
	int32_t ret = -1;
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		ret = unitPtr->dbInsert(tableName, dbCol);
	}
	return ret;
}


int32_t DBConnectPool::dbDelete(uint32_t handleId, const char* tableName, const DbCol* dbColWhere)
{
	int32_t ret = -1;
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		ret = unitPtr->dbDelete(tableName, dbColWhere);
	}
	return ret;
}

int32_t DBConnectPool::dbUpdate(uint32_t handleId, const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere)
{
	int32_t ret = -1;
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		ret = unitPtr->dbUpdate(tableName, dbCol, dbColWhere);
	}
	return ret;
}

int32_t DBConnectPool::dbSelect(uint32_t handleId, const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere, const DbCol* dbColOrder, unsigned char** data, uint32_t* rows)
{
	int32_t ret = -1;
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		ret = unitPtr->dbSelect(tableName, dbCol, dbColWhere, dbColOrder, data, rows);
	}
	return ret;
}

int32_t DBConnectPool::rollback(uint32_t handleId)
{
	int32_t ret = -1;
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		ret = unitPtr->querySql(DBConnectUnit::cmdRollback.c_str(), DBConnectUnit::cmdRollback.size());
	}
	return ret;
}

int32_t DBConnectPool::setTransaction(uint32_t handleId)
{
	int32_t ret = -1;
	UnitPtr unitPtr = getUnitPtrByHandleId(handleId);
	if (NULL != unitPtr)
	{
		ret = unitPtr->querySql(DBConnectUnit::cmdTransaction.c_str(), DBConnectUnit::cmdTransaction.size());
	}
	return ret;
}

