#include "QMySqlDatabase.h"
#include <assert.h>

class QMySqlDatabasePrivate{
public:
    QMySqlDatabasePrivate(QMySqlDatabase *d):
        q(d),
        port(-1)
    {

    }

    QMySqlDatabase *q;
    QString dbname;
    QString uname;
    QString pword;
    QString hname;
    int port;
};

QMySqlDatabase::QMySqlDatabase(void)
{
	mysql = NULL;
	num = 0;
	error="";
	dataSet = NULL;
    d = new QMySqlDatabasePrivate(this);
}

QMySqlDatabase::~QMySqlDatabase(void)
{
    delete d;
}

void QMySqlDatabase::setDatabaseName(const QString& name)
{
    d->dbname = name;
}

void QMySqlDatabase::setUserName(const QString& name)
{
    d->uname = name;
}

void QMySqlDatabase::setPassword(const QString& password)
{
    d->pword = password;
}

void QMySqlDatabase::setHostName(const QString& host)
{
    d->hname = host;
}

void QMySqlDatabase::setPort(int p)
{
    d->port = p;
}

bool QMySqlDatabase::open()
{
    return (0 == connect(d->hname.toStdString(), d->dbname.toStdString(), d->uname.toStdString(), d->pword.toStdString(), d->port));
}

// 设置数据库连接参数
unsigned int QMySqlDatabase::connect(std::string host,std::string database, std::string user, std::string password,unsigned int port/*=3306*/)
{
	if (connected())
		return 0;

	MYSQL *con;
	if(mysql == NULL)
	{
		mysql = ::mysql_init(NULL);

		if (mysql == NULL)
		{
			error = "初始化mysql错误";
			return 1;
		}
	}

	my_bool my_true = true;
	mysql_options(mysql, MYSQL_OPT_RECONNECT, &my_true);
	con = ::mysql_real_connect(mysql,host.c_str(),user.c_str(),password.c_str(),NULL,port,NULL,0);
	if(con == NULL)
	{
		error = ::mysql_error(mysql);
		return ::mysql_errno(mysql);
	}

	// mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "gb2312");
    ::mysql_set_character_set(mysql,"utf8");

	unsigned int re;
	re = ::mysql_select_db(mysql,database.c_str());
	if(re != 0)
	{
		error += ::mysql_error(mysql);
	}

	return re;
}

//查询数据库
unsigned int QMySqlDatabase::exec(std::string sql)
{
	if(dataSet != NULL )
		mysql_free_result(dataSet);

	unsigned int re;
	if( mysql == NULL) return 1;
	assert(!sql.empty());
	re = ::mysql_query(mysql,sql.c_str());
	if(re == 0)
	{
		dataSet = ::mysql_store_result(mysql);
		num = ::mysql_affected_rows(mysql);
		record.clear();
		records.clear();
		fields.clear();
	}
	else
	{
		re = ::mysql_errno(mysql);
		error = ::mysql_error(mysql);
	}

	return re;
}

/*获取查询得到的所有结果*/
std::vector<FieldsValue> QMySqlDatabase::fieldValues()
{
	MYSQL_ROW row;
	unsigned int i;
	FieldsValue tmp;
	assert(mysql != NULL);
	if(records.size() > 0) return records;
	if(dataSet != NULL)
	{
		fieldsName();
		while(row = mysql_fetch_row(dataSet))
		{
			if(row != NULL)
			{
				for(i=0;i<fields.size();i++)
				{
					if ((char *)row[i] != NULL)
					{
						tmp[fields[i]] = (char *)row[i];
					}
					else
					{
						tmp[fields[i]] = "";
					}
				}

				records.push_back(tmp);
			}
		}
	}

	return records;
}

//返回错误信息
std::string QMySqlDatabase::errorCode()
{
	return error;
}

//返回查询后的列值
std::vector<std::string> QMySqlDatabase::fieldsName()
{
	/*
	field = mysql_fetch_fields(dataSet);
	然后通过field[i].name访问在此有错误，不知道为什么，可能是mysql的bug
	*/
	MYSQL_FIELD *field;
	assert(mysql != NULL);
	if(fields.size()>0) return fields;
	while(field = mysql_fetch_field(dataSet))
	{
		fields.push_back(field->name);
	}

	return fields;
}

//向数据库中插入一条数据
unsigned int QMySqlDatabase::insert(std::string table, FieldsValue *data)
{
	FieldsValue::const_iterator iter;
	std::string sql;
	int flag=0;
	assert(mysql != NULL);
	assert(!table.empty());
	assert(data != NULL);
	for(iter = data->begin();iter!= data->end();iter++)
	{
		if(flag == 0)
		{
			sql = "insert into ";
			sql += table;
			sql += " set ";
			sql += iter->first;
			sql += "='";
			sql += iter->second;
			sql += "'";
			flag++;
		}
		else
		{
			sql += ",";
			sql += iter->first;
			sql += "='";
			sql += iter->second;
			sql += "'";
		}
	}

	return exec(sql);
}

//返回最后一个自动增量的值
unsigned long QMySqlDatabase::getlastinsertid()
{
	return (unsigned long)::mysql_insert_id(mysql);
}

//返回一条sql语句影响的行数
unsigned long QMySqlDatabase::numRowsAffected()
{
	return (unsigned long)num;
}

//根据条件修改一条数据
unsigned int QMySqlDatabase::update(std::string table, FieldsValue *data, std::string condition)
{
	FieldsValue::const_iterator iter;
	std::string sql;
	int flag=0;
	assert(mysql != NULL);
	assert(!table.empty());
	assert(data != NULL);
	for(iter = data->begin();iter!= data->end();iter++)
	{
		if(flag == 0)
		{
			sql  = "update ";
			sql += table;
			sql += " set ";
			sql += iter->first;
			sql += "='";
			sql += iter->second;
			sql += "'";
			flag++;
		}
		else
		{
			sql += ",";
			sql += iter->first;
			sql += "='";
			sql += iter->second;
			sql += "'";
		}
	}

	if(!condition.empty())
	{
		sql += " where ";
		sql += condition;
	}

	return exec(sql);
}

//根据条件删除数据
unsigned int QMySqlDatabase::remove(std::string table, std::string condition)
{
	std::string sql;
	assert(mysql != NULL);
	assert(!table.empty());
	sql = "delete from ";
	sql += table;
	if(!condition.empty())
	{
		sql += " where ";
		sql += condition;
	}

	return exec(sql);
}

bool QMySqlDatabase::connected()
{
	if (mysql == NULL || mysql->host==NULL)
		return false;

    if (mysql_ping(mysql)==0)
    {
        return true;
    }
    else{
        return false;
    }
}

void QMySqlDatabase::disconnected()
{
	if(dataSet != NULL)
		::mysql_free_result(dataSet);
	fields.clear();
	error = "";
	record.clear();
	records.clear();
	::mysql_close(mysql);
	mysql = NULL;
}

//////////////////////////////////////////////////////////////////////////

QMySqlQuery::QMySqlQuery(QMySqlDatabase* pMysqlDriver)
{
	m_pMysqlDriver = pMysqlDriver;
}

QMySqlQuery::QMySqlQuery()
{
    m_pMysqlDriver = QMySqlDatabase::instance();
}

QMySqlQuery::~QMySqlQuery(void)
{
}

bool QMySqlQuery::exec(QString sql)
{
    return this->exec(sql.toStdString());
}

bool QMySqlQuery::exec(std::string sql)
{
	if (m_pMysqlDriver != NULL)
	{	
		if (m_pMysqlDriver->exec(sql)==0)
		{
			records = m_pMysqlDriver->fieldValues();
			index = 0;
            return true;
            // if (records.size() > 0)
            // 	return true;
            // else
            // 	return false;
		}
		else
			return false;
	}

	return false;
}

QVariant QMySqlQuery::value(const QString& fieldName)
{
    std::string result;
    this->stringValue(result, fieldName.toStdString());
    return QVariant::fromValue(QString::fromStdString(result));
}

bool QMySqlQuery::stringValue(std::string &str, std::string fieldName)
{
	str = records[index][fieldName];
	return true;
}

bool QMySqlQuery::intValue(long &lVal, std::string fieldName)
{
	const char* pszVal = records[index][fieldName].c_str();
	lVal = atoi(pszVal);
	return true;
}

bool QMySqlQuery::floatValue(double &lVal, std::string fieldName)
{
	const char* pszVal = records[index][fieldName].c_str();
	lVal = atof(pszVal);
	return true;
}

long QMySqlQuery::numRowsAffected()
{
	if (m_pMysqlDriver != NULL)
	{
		return m_pMysqlDriver->numRowsAffected();
	}

	return 0;
}

bool QMySqlQuery::next()
{
	if (m_pMysqlDriver == NULL) return false;

	if (++index >= (int)m_pMysqlDriver->numRowsAffected()) return false;
	
	return true;
}

bool QMySqlQuery::previous()
{
	if (m_pMysqlDriver == NULL) return false;

	if (--index < 0) return false;

	return true;
}

bool QMySqlQuery::first()
{
	if (m_pMysqlDriver == NULL) return false;
	index= 0;
	if (index >= (int)m_pMysqlDriver->numRowsAffected()) return false;

	return true;
}

bool QMySqlQuery::last()
{
	if (m_pMysqlDriver == NULL) return false;
	index= m_pMysqlDriver->numRowsAffected() - 1;
	if (index >= (int)m_pMysqlDriver->numRowsAffected()) return false;
	if (index < 0) return false;

	return true;
}

bool QMySqlQuery::eof()
{
	if (m_pMysqlDriver == NULL) return false;
	if (index == m_pMysqlDriver->numRowsAffected()) return true;
	return false;
}

bool QMySqlQuery::bof()
{
	if (m_pMysqlDriver == NULL) return false;
	if (index == 0 && m_pMysqlDriver->numRowsAffected()>0) return true;
	return false;
}

unsigned long QMySqlQuery::lastInsertId()
{
    if (m_pMysqlDriver == NULL) return 0;
    return m_pMysqlDriver->getlastinsertid();
}
