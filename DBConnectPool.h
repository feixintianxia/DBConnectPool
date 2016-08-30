/**
 * @file DBConnectPool.h
 * @brief	数据库连接池定义
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */

#ifndef QGAME_DBCONNECTPOOL_H
#define QGAME_DBCONNECTPOOL_H

#include <cstdint>
#include <string>
#include <memory>
#include <map>

#include "noncopyable.h"
#include "UtilTool.h"
#include "Mutex.h"

namespace QGame
{

class DBConnectUnit;

class DBConnectPool
{
	public:
		using UnitPtr = std::shared_ptr<DBConnectUnit>;
		using MapPool = std::map<uint32_t/*handleId*/, UnitPtr>;
		using POOLITER = MapPool::iterator;

		DBConnectPool(DBConnectInfo& info, uint32_t poolSize = 50) 
				  :	connectInfo_(info), 
					maxPoolSize_(poolSize)
		{
		}

		~DBConnectPool()
		{
		}

		/**
		 * @brief 从连接池获得一个有效连接
		 */
		uint32_t getUintFromPool();

		/**
		 * @brief 从连接池获取相应的连接
		 *
		 */
		UnitPtr getUnitPtrByHandleId(uint32_t  handleId);

		/**
		 * @brief 把连接还给连接池 
		 */
		void releaseUintToPool(uint32_t handleId);

		/**
		 * @brief 插入操作
		 *
		 * @param handleId	某个连接
		 * @param tableName 表名
		 * @param dbCol		要插入的数据 
		 *
		 * @return			返回零，表示成功；否则，失败。
		 */
		int32_t dbInsert(uint32_t handleId, const char* tableName, const DbCol* dbCol);
		/**
		 * @brief 删除操作
		 *
		 * @param handleId  某个连接
		 * @param tableName 表名
		 * @param dbColWhere条件
		 *
		 * @return			返回零，表示成功；否则，失败。
		 */
		int32_t dbDelete(uint32_t handleId, const char* tableName, const DbCol* dbColWhere);

		/**
		 * @brief 更新操作
		 *
		 * @param handleId   某个连接
		 * @param tableName  表名
		 * @param dbCol		 新数据
		 * @param dbColwhere 条件
		 *
		 * @return			 返回零，表示成功；否则，失败。
		 */
		int32_t dbUpdate(uint32_t handleId, const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere);

		/**
		 * @brief 选择操作
		 *
		 * @param handleId	 某个连接
		 * @param tableName  表名
		 * @param dbCol		 要选择的列 
		 * @param dbColWhere 条件
		 * @param dbColOrder 是否排序
		 * @param data		 获得的数据
		 * @param rows		 返回结果的行数
		 *
		 * @return			 返回零，表示成功；否则，失败。
		 */
		 int32_t dbSelect(uint32_t handleId, const char* tableName, const DbCol* dbCol, const DbCol* dbColWhere, const DbCol* dbColOrder, unsigned char** data, uint32_t* rows); 

	public:
		/**
		 * @brief 提交操作
		 *
		 * @param handleId 某个连接
		 *
		 * @return		   返回零，表示成功；否则，失败。
		 */
		int32_t commit(uint32_t handleId);

		/**
		 * @brief 回退操作
		 *
		 * @param handleId 某个连接
		 *
		 * @return		   返回零，表示成功；否则，失败。
		 */
		int32_t rollback(uint32_t handleId);
		

		/**
		 * @brief 设置事务处理 
		 *
		 * @param handleId		某个连接
		 *
		 * @return				返回零，表示成功；否则，失败。
		 */
		int32_t setTransaction(uint32_t handleId);

	private:

		MutexLock		mutex_;
		//默认每个链接的信息都是相同
		DBConnectInfo	connectInfo_;
		MapPool			mapPool_;
		//连接池的最大连接数
		const uint32_t	maxPoolSize_;

};//class DBConnectPool
}//namespace QGame
#endif
