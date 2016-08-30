/**
 * @file Timestamp.cpp
 * @brief 
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */

#include <sys/time.h>
#include "Timestamp.h"

using namespace QGame;

int64_t Timestamp::currentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t nowTime = static_cast<int64_t>(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
	return nowTime; 
}
