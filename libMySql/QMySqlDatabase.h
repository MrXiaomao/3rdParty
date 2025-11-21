#ifndef QMYSQLDATABASE_H
#define QMYSQLDATABASE_H

#include <QString>
#include <QVariant>

#include <string>
#include <map>
#include <vector>
#include "mysql.h"

using namespace std;
typedef std::map<std::string, std::string> FieldsValue;

class QMySqlDatabasePrivate;
class QMySqlDatabase
{
protected:
	//代表一个到数据库的连接
	MYSQL *mysql;
	//操作的结果集
	MYSQL_RES *dataSet;
	//返回查询得到的结果数
	my_ulonglong num;
	//错误提示信息
	std::string error;
	//查询语句返回一条结果
	FieldsValue record;
	//查询结果集
	std::vector<FieldsValue> records;
	//查询表结构的字段名称
	std::vector<std::string> fields;

public:
    static QMySqlDatabase* instance(){
        static QMySqlDatabase db;
        return &db;
    }

    QMySqlDatabase(void);
    virtual ~QMySqlDatabase(void);

    void setDatabaseName(const QString& name);
    void setUserName(const QString& name);
    void setPassword(const QString& password);
    void setHostName(const QString& host);
    void setPort(int p);
    bool open();

	// 连接驱动
	unsigned int connect(std::string host, std::string database, std::string user, std::string password, unsigned int port=3306);
	//执行sql语句
	unsigned int exec(std::string sql);
	//返回查询得到的结果
	std::vector<FieldsValue> fieldValues();
	//返回错误信息
	std::string errorCode();
	//返回查询表结构字段名称
	std::vector<std::string> fieldsName();
	//向数据库中插入一条数据
	unsigned int insert(std::string table, FieldsValue *data);
	//返回最后一个自动增量的值
    unsigned long getlastinsertid();
	//返回一条sql语句影响的行数
	unsigned long numRowsAffected();
	//根据条件修改一条数据
	unsigned int update(std::string table, FieldsValue *data, std::string condition);
	//根据条件删除数据
	unsigned int remove(std::string table, std::string condition);

	//判断数据库是否连接
	bool connected();
	//断开数据库连接
	void disconnected();

private:
    friend class QMySqlDatabasePrivate;
    QMySqlDatabasePrivate *d;
};

class QMySqlQuery
{
public:
    QMySqlQuery(QMySqlDatabase* pMysqlDriver);
    QMySqlQuery();
    virtual ~QMySqlQuery(void);
	//执行sql语句
    bool exec(std::string sql);
    bool exec(QString sql);

    QVariant value(const QString& fieldName);
	//取字符串值
    bool stringValue(std::string &str, std::string fieldName);
	//取整型值
	bool intValue(long &lVal, std::string fieldName);
	//取浮点型值
	bool floatValue(double &lVal, std::string fieldName);
	//返回一条sql语句影响的行数
	long numRowsAffected();
	//记录下移
	bool next();
	//记录上移
	bool previous();
	//记录开头
	bool first();
	//记录结尾
	bool last();
	//是否到记录尾
	bool eof();
	//是否到记录头
	bool bof();
    //最后插入id
    unsigned long lastInsertId();

private:
	//当前值在记录集中的问题
	int index;
	//记录集
	std::vector<FieldsValue> records;
	//驱动
    QMySqlDatabase	*m_pMysqlDriver;
};

#endif
