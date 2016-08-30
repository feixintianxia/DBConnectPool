/**
 * @file example.cpp
 * @brief  数据库连接池的一个测试用列
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-29
 */

#include <pthread.h>
#include <signal.h>
#include <string>
#include <atomic>
#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>

#include "UtilTool.h"
#include "DBConnectPool.h"
#include "Timestamp.h"

#define TABLENAME "DBConnectPool"

using namespace QGame;
bool bExit = false;

std::atomic_uint insertId(0);
std::atomic_uint deleteId(0);
std::atomic_uint updateId(0);
std::atomic_uint selectId(0);

void signal_ctrl_c(int sig)
{
	bExit = true;
}

void* dbInsert(void * arg)
{
	pthread_detach(pthread_self());
	
	QGame::DBConnectPool& dbPool(*(static_cast<QGame::DBConnectPool*>(arg)));
	uint32_t handleId = dbPool.getUintFromPool();

	TestData testData;
	memset(&testData, 0, sizeof(testData));
	testData.userId_ = ++insertId;
	snprintf(testData.name_, sizeof(testData.name_), "UserId = %llu", testData.userId_);
	testData.data_ = '1';

	const DbCol dbCol[] = 
	{
		{"USERID",	QGame::DB_DATA::DB_DATA_ULONG,	8,	(unsigned char*)(&(testData.userId_))},
		{"NAME",	QGame::DB_DATA::DB_DATA_STR,	32,	(unsigned char*)(testData.name_)},
		{"DATA",	QGame::DB_DATA::DB_DATA_UCHAR,	1,	(unsigned char*)(&(testData.data_))},
		{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
	};

	int32_t ret = dbPool.dbInsert(handleId, TABLENAME, dbCol);
	if (0 == ret)
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t插入\t成功。";
		LOG_INFO(os.str());
	}
	else
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t插入\t失败。";
		LOG_INFO(os.str());
	}

	//防止busy-loop
	usleep(100); 
	dbPool.releaseUintToPool(handleId);
	return NULL;
}

void* dbDelete(void * arg)
{
	pthread_detach(pthread_self());
	
	QGame::DBConnectPool& dbPool(*(static_cast<QGame::DBConnectPool*>(arg)));
	uint32_t handleId = dbPool.getUintFromPool();

	TestData testData;
	memset(&testData, 0, sizeof(testData));
	testData.userId_ = ++deleteId;

	const DbCol dbColWhere[] = 
	{
		{"USERID",	QGame::DB_DATA::DB_DATA_ULONG,	8,	(unsigned char*)(&(testData.userId_))},
		{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
	};

	int32_t ret = dbPool.dbDelete(handleId, TABLENAME, dbColWhere);
	if (0 == ret)
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t删除\t成功。";
		LOG_INFO(os.str());
	}
	else
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t删除\t失败。";
		LOG_INFO(os.str());
	}

	//防止busy-loop
	usleep(100); 
	dbPool.releaseUintToPool(handleId);
	return NULL;
}

void* dbUpdate(void * arg)
{
	pthread_detach(pthread_self());
	
	QGame::DBConnectPool& dbPool(*(static_cast<QGame::DBConnectPool*>(arg)));
	uint32_t handleId = dbPool.getUintFromPool();

	TestData testData;
	memset(&testData, 0, sizeof(testData));
	testData.userId_ = ++updateId;
	//跟新数据
	testData.data_ = '2';

	const DbCol dbCol[] = 
	{
		{"DATA",	QGame::DB_DATA::DB_DATA_UCHAR,	1,	(unsigned char*)(&(testData.data_))},
		{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
	};

	const DbCol dbColWhere[] = 
	{
		{"USERID",	QGame::DB_DATA::DB_DATA_ULONG,	8,	(unsigned char*)(&(testData.userId_))},
		{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
	};

	int32_t ret = dbPool.dbUpdate(handleId, TABLENAME, dbCol, dbColWhere);
	if (0 == ret)
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t跟新\t成功。";
		LOG_INFO(os.str());
	}
	else
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t跟新\t失败。";
		LOG_INFO(os.str());
	}

	//防止busy-loop
	usleep(100); 
	dbPool.releaseUintToPool(handleId);
	return NULL;
}

void* dbSelect(void * arg)
{
	pthread_detach(pthread_self());
	
	QGame::DBConnectPool& dbPool(*(static_cast<QGame::DBConnectPool*>(arg)));
	uint32_t handleId = dbPool.getUintFromPool();

	TestData testData;
	memset(&testData, 0, sizeof(testData));
	testData.userId_ = ++selectId;

	const DbCol dbCol[] = 
	{
		{"USERID",	QGame::DB_DATA::DB_DATA_ULONG,	8,	NULL},
		{"NAME",	QGame::DB_DATA::DB_DATA_STR,	32,	NULL},
		{"DATA",	QGame::DB_DATA::DB_DATA_UCHAR,	1,	NULL},
		{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
	};

	const DbCol dbColWhere[] = 
	{
		{"USERID",	QGame::DB_DATA::DB_DATA_ULONG,	8,	(unsigned char*)(&(testData.userId_))},
		{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
	};

	TestData * data = NULL;
	uint32_t rows = 0;
	int32_t ret = dbPool.dbSelect(handleId, TABLENAME, dbCol, dbColWhere, NULL, (unsigned char**)(&data), &rows);
	if (0 == ret)
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t选择\t成功。";
		LOG_INFO(os.str());
		os.clear();
		for (uint32_t index = 0; index < rows; ++index)
		{
			os << "USERID\t=\t" << testData.userId_ << '\n';
			os << "NAME\t=\t" << testData.name_ << '\n';
			os << "DATA\t=\t" << testData.data_;
		}//for
		LOG_INFO(os.str());
	}
	else
	{
		std::ostringstream os;
		os << "USERID\t=\t" << testData.userId_ << "\t选择\t失败。";
		LOG_INFO(os.str());
	}

	//防止busy-loop
	usleep(100); 
	dbPool.releaseUintToPool(handleId);
	return NULL;
}

//测试插入操作的性能
void* dbBeachInsertTest(void * arg)
{
	pthread_detach(pthread_self());
	
	QGame::DBConnectPool& dbPool(*(static_cast<QGame::DBConnectPool*>(arg)));
	Timestamp timestamp;
	timestamp.reset();
	std::cout << insertId << std::endl;
	while(!bExit)
	{
		uint32_t handleId = dbPool.getUintFromPool();

		TestData testData;
		memset(&testData, 0, sizeof(testData));
		testData.userId_ = ++insertId;
		snprintf(testData.name_, sizeof(testData.name_), "UserId = %llu", testData.userId_);
		testData.data_ = '1';

		const DbCol dbCol[] = 
		{
			{"USERID",	QGame::DB_DATA::DB_DATA_ULONG,	8,	(unsigned char*)(&(testData.userId_))},
			{"NAME",	QGame::DB_DATA::DB_DATA_STR,	32,	(unsigned char*)(testData.name_)},
			{"DATA",	QGame::DB_DATA::DB_DATA_UCHAR,	1,	(unsigned char*)(&(testData.data_))},
			{NULL,		QGame::DB_DATA::DB_DATA_MAX,	0,	NULL}
		};

		dbPool.dbInsert(handleId, TABLENAME, dbCol);

		//防止busy-loop
		//	usleep(100); 
		dbPool.releaseUintToPool(handleId);
		std::cout << insertId << std::endl;
		if (insertId % 100)
		{
			int64_t elapse = timestamp.elapseTime();
			std::cout << "每秒插入\t" << 100 / elapse << "\t条记录" << std::endl;
			timestamp.reset();
		}
	}
	return NULL;
}

int main()
{
	QGame::DBConnectInfo connectInfo;
	connectInfo.user_ = "root";
	connectInfo.passwd_ = "lihai";
	connectInfo.host_ = "localhost";
	connectInfo.port_ = 3306;
	connectInfo.dbName_ = "DBConnectPool";

	QGame::DBConnectPool dbPool(connectInfo);
	signal(SIGINT, signal_ctrl_c);

	pthread_t pidInsert;
	pthread_t pidDelete;
	pthread_t pidUpdate;
	pthread_t pidSelect;
	while(!bExit)
	{
		pthread_create(&pidInsert, NULL, dbInsert, (void*)&dbPool);
		pthread_create(&pidDelete, NULL, dbDelete, (void*)&dbPool);
		pthread_create(&pidUpdate, NULL, dbUpdate, (void*)&dbPool);
		pthread_create(&pidSelect, NULL, dbSelect, (void*)&dbPool);
		//防止busy-loop
		usleep(100); 
	}
	//测试插入操作的性能
//	pthread_t pidBeachInsertTest;
//	pthread_create(&pidBeachInsertTest, NULL, dbBeachInsertTest, (void*)&dbPool);
	return 0;
}
