#ifndef SSQLCONNECTIONPOOL_H_
#define SSQLCONNECTIONPOOL_H_

#include <QSqlDataBase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class SSqlConnectionPool
{
	SSqlConnectionPool();
public:
	static SSqlConnectionPool* instance();
	~SSqlConnectionPool();

	QSqlDatabase openConnection();
	void closeConnection(QSqlDatabase db);


	void setHostName(const QString& host) { m_hostName = host; }
	void setPort(quint16 port) { m_port = port; }
	void setDatabaseName(const QString& databaseName) { m_databaseName = databaseName; }
	void setUserName(const QString& username) { m_userName = username; }
	void setPassword(const QString& password) { m_password = password; }
	void setDatabaseType(const QString& type) { m_databaseType = type; }
	void setMaxConnnectionCount(quint32 maxCount) { m_maxConnectionCount = maxCount; }
	quint32 getMaxConnnectionCount() {return m_maxConnectionCount;}
	
private:
	QSqlDatabase CreateConnection(const QString& con_name);
	void  releaseConnection(QSqlDatabase db);

	quint16 m_port{};
	QString m_hostName{};
	QString m_databaseName{};
	QString m_userName{};
	QString m_password{};
	QString m_databaseType{};

	static inline QMutex m_mutex;  //互斥量
	QWaitCondition m_waitCondition; //条件
	quint32 m_waitInterval; //等待时间间隔
	quint32 m_waitTime; //等待次数;
	qint32 m_maxConnectionCount; //最大连接数
	QQueue<QString>m_useConnections; //已使用连接
	QQueue<QString>m_unuseConnections; //未使用连接

};
class SConnectionWrap
{
public:
	SConnectionWrap();
	~SConnectionWrap();
	QSqlDatabase openConnection();
private:
	QSqlDatabase m_db;
};


#endif // !SSQLCONNECTIONPOOL_H_
