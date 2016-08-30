/**
 * @file DBConnectUnit.h
 * @brief 定义连接单元
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */

#ifndef QGAME_DBCONNECTUNIT_H
#define QGAME_DBCONNECTUNIT_H

#include <pthread.h>
#include <mysql/mysql.h>
#include <string>
#include <sstream>
#include <memory>

#include "UtilTool.h"
#include "Timestamp.h"

namespace QGame
{

class DBConnectUnit
{
	public:
		DBConnectUnit(DBConnectInfo& info) 
			:	connectInfo_(info), 
				expiredTime_(5),
				handleId_(0),
				mysql_(NULL),
				connStatus_(DB_CONN_STATUS::DB_CONN_INVALID),
				threadId_(0),
				timestamp_()
		{
		}

		~DBConnectUnit();

		/**
		 * @brief 关闭数据库连接
		 */
		void closeDb();

		/**
		 * @brief 连接到数据库中
		 *
		 * @return	成功返回true 
		 */
		bool connectDb();

		/**
		 * @brief 初始化每次使用该连接的信息
		 */
		void initConnect();

		/**
		 * @brief 重置每次使用该连接的信息
		 */
		void resetConnect();

		/**
		 * @brief 每次使用连接单元操作的时间
		 *
		 * @return 时间（秒）
		 */
		int64_t elapseTime();

		uint64_t handleId() const { return handleId_; }

		DB_CONN_STATUS connStatus() const { return connStatus_; }

		pthread_t threadId() const { return threadId_; }

	public:
		//-------------------------------
		//数据库操作------------------>>>
		//返回值为0, 表示成功；否则，失败。
		int32_t querySql(const char* sql, uint32_t sqlLen);
		
		int32_t dbInsert(const char* tableName, const DbCol* dbCol);

		int32_t dbDelete(const char* tableName, const DbCol* dbCol);

		int32_t dbUpdate(const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere);

		int32_t dbSelect(const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere, const DbCol* dbColOrder, unsigned char** data, uint32_t* rows);

		
		int32_t selectSql(const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere, const DbCol* dbColOrder = NULL, uint32_t limit = 0, uint32_t offset = 0);

		int32_t getDataFromMYSQL_ROW(MYSQL_ROW row, unsigned long* lengths, const DbCol* dbCol, unsigned char* data);

		//数据库操作------------------<<<
		//-------------------------------
		//
	private:
		std::ostringstream& appendValue(std::ostringstream& osSql, const DbCol* dbCol);

		/**
		 * @brief sql语句where a = b
		 */
		std::ostringstream& appendWhere(std::ostringstream& osSql, const DbCol* dbColWhere);
	
		/**
		 * @brief 计算数组的size_ 的和
		 */
		uint32_t calColSize(const DbCol* dbCol);

	public:

		static uint32_t	countHandleId_;
		static const std::string cmdRollback;
		static const std::string cmdCommit;
		static const std::string cmdTransaction;
		static const std::string cmdNoTransaction;
	private:
		using MysqlPtr = std::shared_ptr<MYSQL>;
		DBConnectInfo	connectInfo_;
		////过期时长（秒）
		const int64_t	expiredTime_;
		//连接的编号
		uint32_t		handleId_;		
		MysqlPtr		mysql_;
		DB_CONN_STATUS	connStatus_;
		//使用该连接的线程
		pthread_t		threadId_;  
		//计算使用一次连接的操作时间
		Timestamp		timestamp_; 
};  //class DBConnectUnit
}//namespace QGame
#endif

