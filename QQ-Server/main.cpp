#include <QCoreApplication>
#include <QRunnable>
#include <QThreadPool>
#include <iostream>
#include "SSqlConnectionPool.h"
#include "Server.h"

// Worker class
class Worker : public QObject, public QRunnable
{
public:
    static void worker1()
    {
        qDebug() << "worker1:";
        auto db = SSqlConnectionPool::instance()->openConnection();
        if (!db.isOpen()) {
            qWarning() << "Failed to open connection in worker1";
            return;
        }

        QSqlQuery query(db);
        if (!query.exec("SELECT * FROM user"))
        {
            qWarning() << query.lastError().text();
        }

        processQuery(query);
        SSqlConnectionPool::instance()->closeConnection(db);
    }

    static void worker2()
    {
        qDebug() << "worker2:";
        auto db = SSqlConnectionPool::instance()->openConnection();
        if (!db.isOpen()) {
            qWarning() << "Failed to open connection in worker2";
            return;
        }

        QSqlQuery query(db);
        if (!query.exec("SELECT * FROM user"))
        {
            qWarning() << query.lastError().text();
        }

        processQuery(query);
        SSqlConnectionPool::instance()->closeConnection(db);
    }

    static void worker3()
    {
        qDebug() << "worker3:";
       // auto db = SSqlConnectionPool::instance()->openConnection();
        SConnectionWrap wrap;
        auto db = wrap.openConnection();
        if (!db.isOpen()) {
            qWarning() << "Failed to open connection in worker3";
            return;
        }

        QSqlQuery query(db);
        if (!query.exec("SELECT * FROM user"))
        {
            qWarning() << query.lastError().text();
        }

        processQuery(query);
    }

private:
    static void processQuery(QSqlQuery& query)
    {
        auto count = query.record().count();
        while (query.next())
        {
            std::cout << __FUNCTION__ << "#######";
            for (int i = 0; i < count; ++i)
            {
                std::cout << query.value(i).toString().toStdString();
            }
            std::cout << std::endl;
        }
    }
};

// Main function
int main(int argc, char* argv[])
{ 
    QCoreApplication a(argc, argv);

    //线程不超过连接数 保证线程绑定连接
    auto maxCount = SSqlConnectionPool::instance()->getMaxConnnectionCount();
    QThreadPool::globalInstance()->setMaxThreadCount(maxCount);

    SSqlConnectionPool::instance()->setPort(3306);
    SSqlConnectionPool::instance()->setHostName("localhost");
    SSqlConnectionPool::instance()->setDatabaseName("qq");
    SSqlConnectionPool::instance()->setUserName("xu");
    SSqlConnectionPool::instance()->setPassword("200600xx");

   // QThreadPool::globalInstance()->start(&Worker::worker3);
   
    Server server;
    
    return a.exec();
}



