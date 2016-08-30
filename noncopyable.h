/**
 * @file Noncopyable.h
 * @brief 
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */
#ifndef QGAME_NONCOPYABLE_H
#define QGAME_NONCOPYABLE_H

namespace QGame
{
class noncopyable
{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:
		noncopyable(const noncopyable&);
		noncopyable& operator=(const noncopyable&);
};
}//namespace QGame
#endif
