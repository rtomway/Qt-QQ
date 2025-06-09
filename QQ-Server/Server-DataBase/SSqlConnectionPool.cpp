#include "SSqlConnectionPool.h"

#include<QThread>

SSqlConnectionPool::SSqlConnectionPool()
	:m_databaseType("QMYSQL")
	,m_maxConnectionCount(14)
	,m_waitInterval(100)
	,m_waitTime(1000)
{

}

//单例
SSqlConnectionPool* SSqlConnectionPool::instance()
{
	static SSqlConnectionPool ins;
	return &ins;
}

//析构
SSqlConnectionPool::~SSqlConnectionPool()
{
	//销毁所有连接
	QMutexLocker locker(&m_mutex);
	for (const auto& con_name : m_useConnections)
	{
		QSqlDatabase::removeDatabase(con_name);
	}
	for (const auto& con_name : m_unuseConnections)
	{
		QSqlDatabase::removeDatabase(con_name);
	}
}

//获取连接
QSqlDatabase SSqlConnectionPool::openConnection()
{
	QMutexLocker locker(&m_mutex); //QMutexLocker辅助管理互斥锁m_mutex加锁解锁操作  

	//当前总连接数
	int count = m_useConnections.count() + m_unuseConnections.count();
	for (size_t i = 0; i < m_waitTime && m_unuseConnections.isEmpty() && count == m_maxConnectionCount; i+=m_waitInterval)
	{
		//等待其他连接关闭  等待其他线程唤醒信号 超时则退出
		m_waitCondition.wait(&m_mutex, m_waitInterval);
		//重新计算总连接数
		count = m_useConnections.count() + m_unuseConnections.count();
	}
	//判断是否创建新的连接
	QString con_name;
	if (m_unuseConnections.count() > 0)     //存在连接未使用
		con_name = m_unuseConnections.dequeue();
	else if (count < m_maxConnectionCount)   //连接池未满可创建新连接
		con_name = QString("connection-%1").arg(count);
	else
	{
		qWarning() << "all connection used!";
		return QSqlDatabase();
	}

	qDebug() << "连接名：" << con_name;
	auto db = CreateConnection(con_name);
	if (db.isOpen())
	{
		m_useConnections.enqueue(con_name);   //连接成功加入使用队列
	}
	return db;
}

//关闭连接
void SSqlConnectionPool::closeConnection(QSqlDatabase db)
{
	QMutexLocker<QMutex> locker(&m_mutex);
	 
	QString con_name = db.connectionName();
	if (m_useConnections.contains(con_name))
	{
		m_useConnections.removeOne(con_name);    //将连接放入未使用连接队列中
		m_unuseConnections.enqueue(con_name);
		//连接空出 唤醒等待中的进程(随机)
		m_waitCondition.wakeOne();
	}
	qDebug() << db.isOpen();
	qDebug() << "all:" << m_useConnections.count() + m_unuseConnections.count();
	qDebug() << "used:" << m_useConnections.count();
	qDebug() << "no use:" << m_unuseConnections.count();
}

//创建新连接
QSqlDatabase SSqlConnectionPool::CreateConnection(const QString& con_name)
{
	if (QSqlDatabase::contains(con_name))
	{
		qWarning() << "connection" << con_name << "has exit";
		auto db= QSqlDatabase::database(con_name);
		qDebug() << "------------------" << db.databaseName() << con_name;
		return db;
	}
	 
	//创建数据库连接 初始化信息
	QSqlDatabase db=QSqlDatabase::addDatabase(m_databaseType,con_name);
	db.setPort(m_port);
	db.setHostName(m_hostName);
	db.setDatabaseName(m_databaseName);
	db.setUserName(m_userName);
	db.setPassword(m_password);

	if (!db.open())
	{
		qWarning() << "creatConnection error:"<<db.lastError().text();
	}

	return db;
}

//销毁连接
void SSqlConnectionPool::releaseConnection(QSqlDatabase db)
{
	QString con_name = db.connectionName();
	QSqlDatabase::removeDatabase(con_name);
}

//开启连接的封装
SConnectionWrap::SConnectionWrap()
{

}

SConnectionWrap::~SConnectionWrap()
{
	auto pool = SSqlConnectionPool::instance();
	pool->closeConnection(m_db);
}

QSqlDatabase SConnectionWrap::openConnection()
{
	m_db = SSqlConnectionPool::instance()->openConnection();
	return m_db;
}

