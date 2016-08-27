/**
 * @file Mutex.h
 * @brief  封装 pthread_mutex_t
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */
#ifndef QGAME_MUTEX_H
#define QGAME_MUTEX_H

#include <cassert>
#include <pthread.h>
#include "noncopyable.h"

namespace  QGame
{

class MutexLock : private noncopyable
{
	public:
		MutexLock()
		{
			assert(0 == pthread_mutex_init(&mutex_, NULL));
		}
		~MutexLock()
		{
			assert(0 == pthread_mutex_destory(&mutex_));
		}

		void lock()
		{
			assert(0 == pthread_mutex_lock(&mutex_));
		}

		void unlock()
		{
			assert(0 == pthread_mutex_unlock(&mutex_));
		}
	private:
		pthread_mutex_t mutex_;

}; //class MutexLock

class MutexLockGuard : private noncopyable
{

	public:
		explicit MutexLockGuard(MutexLock& mutex)
					: mutex_(mutex)
		{
			mutex_.lock();
		}

		~MutexLockGuard()
		{
			mutex_.unlock();
		}
	private:
		MutexLock& mutex_;
};//class MutexLockGuard
					   
}//namespace QGame
#endif

