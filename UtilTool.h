/**
 * @file UtilTool.h
 * @brief 提供通用函数和结构类型
 * @author Li Hai, 690347560@qq.com
 * @version 1.0.0
 * @date 2016-08-27
 */

#ifndef QGAME_UTILTOOL_H
#define QGAME_UTILTOOL_H

#include <cstdio>
namespace QGame
{

//数据库的数据类型枚举
enum DB_DATA
{
	DB_DATA_BOOL,
	DB_DATA_UCHAR,
	DB_DATA_CHAR,
	DB_DATA_USHORT,
	DB_DATA_SHORT,
	DB_DATA_UINT,
	DB_DATA_INT,
	DB_DATA_ULONG,
	DB_DATA_LONG,
	DB_DATA_STR,
	DB_DATA_BIN,

	DB_DATA_MAX,
};

//数据库连接状态
enum DB_CONN_STATUS
{
	DB_CONN_USING,		//连接有效而且正被使用
	DB_CONN_VALID,		//连接有效而且正等待被使用
	DB_CONN_INVALID,	//连接失效
	
	DB_CONN_MAX,
};
//数据库的连接信息
//mysql -u user_ -p passwd_ -h host_ - p port_ dbName_ 
struct DBConnectInfo
{
	std::string		user_;
	std::string		passwd_;
	std::string		host_;
	int				port_;
	std::string     dbName_;
};

//数据库表的列定义
typedef struct
{
	const char*		name_;
	DB_DATA			type_;
	uint32_t		size_;
	unsigned char*  data_;
}DbCol;
}//namesapce QGame


//用来测试, 可自定义其它类型
typedef struct
{
	uint64_t		userId_;
	char			name_[32];
	unsigned char	data_;
}__attribute__((packed)) TestData;

//简单的日志
#define LOG_ERROR(x) \
	std::cerr << __FILE__ << "\t" << __LINE__ << "\t" << x << " error " << std::endl;

#define LOG_INFO(x) \
	std::cout << x << std::endl;
#endif
