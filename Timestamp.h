/**
 * @file Timestamp.h
 * @brief 用于计算数据库操作的时间
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */
#ifndef QGAME_TIMESTAMP_H
#define QGAME_TIMESTAMP_H

#include <cstdint>
#include <iostream>
#include "noncopyable.h"
namespace QGame
{

class Timestamp : private noncopyable
{
	public:
		Timestamp()
			: microSecondsSinceEpoch_(0)
		{
		}
		explicit Timestamp(int64_t microSecondsSinceEpoch)
			: microSecondsSinceEpoch_(microSecondsSinceEpoch)
		{
		}

		void reset()
		{
			microSecondsSinceEpoch_ = currentTime();
		}
		/**
		 * @brief 
		 *
		 * @return  返回相隔时间（以秒为单位）
		 */
		int64_t elapseTime()
		{
			int64_t diff = currentTime() - microSecondsSinceEpoch_;
			return diff / kMicroSecondsPerSecond;
		}
	public:
		static const int64_t kMicroSecondsPerSecond = 1000 * 1000;
		static int64_t currentTime();
	private:
		int64_t microSecondsSinceEpoch_;
};
}//namespace QGame
#endif
