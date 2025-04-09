#ifndef DATABASEQUERY
#define DATABASEQUERY

#include <QObject>
#include <QJsonObject>
#include <QSqlQuery>
#include <memory>

class DataBaseQuery:public QObject
{
	Q_OBJECT
public:
	DataBaseQuery(QObject* parent = nullptr);
    // 执行查询并返回结果
    QJsonObject executeQuery(const QString& queryStr, const QVariantList& bindValues, std::shared_ptr<QSqlQuery>queryPtr =nullptr);
    // 执行插入、更新等操作
    bool executeNonQuery(const QString& queryStr, const QVariantList& bindValues, std::shared_ptr<QSqlQuery>queryPtr=nullptr);
    //事务执行
    bool executeTransaction(const std::function<bool(std::shared_ptr<QSqlQuery>)>& transactionCallback);
};

#endif // !DATABASEQUERY
