﻿#include "DataBaseQuery.h"
#include <QJsonArray>

#include "SSqlConnectionPool.h"


DataBaseQuery::DataBaseQuery(QObject* parent)
    :QObject(parent)
{

}
//查询信息
QJsonObject DataBaseQuery::executeQuery(const QString& queryStr, const QVariantList& bindValues)
{
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	query.prepare(queryStr);
	// 绑定查询参数
	for (const auto& value : bindValues) {
		query.addBindValue(value);
	}
    QJsonObject allQueryObj;
    QJsonArray queryArray;
    if (query.exec()) {
        // 查询结果处理
        while (query.next()) {
            QJsonObject queryObj;
            int count = query.record().count();
            for (int i = 0; i < count; ++i) {
                auto fieldName = query.record().fieldName(i);
                if (fieldName == "birthday")
                {
                    queryObj[fieldName] = query.value(i).toDate().toString("MM-dd");
                }
                else if (fieldName == "age" || fieldName == "gender")
                {
                    queryObj[fieldName] = query.value(i).toInt();
                }
                else
                {
                    queryObj[fieldName] = query.value(i).toString();
                }
            }
            queryArray.append(queryObj);
        }
        allQueryObj["data"] = queryArray;
    }
    else {
        qDebug() << query.lastError();
        allQueryObj["error"] = query.lastError().text();
    }
    return allQueryObj;
}
//更新插入信息
bool DataBaseQuery::executeNonQuery(const QString& queryStr, const QVariantList& bindValues)
{
    SConnectionWrap wrap;
    QSqlQuery query(wrap.openConnection());
    query.prepare(queryStr);
    // 绑定查询参数
    for (const auto& value : bindValues) {
        query.addBindValue(value);
        qDebug() << "绑定查询参数" << value;
    }
    //操作结果
    if (query.exec()){
        return true;
    }else
    {
        qDebug() << query.lastError();
        return false;
    }
}
//事务
bool DataBaseQuery::executeTransaction(const std::function<bool(QSqlQuery&)>& transactionCallback)
{
    SConnectionWrap wrap;
    QSqlQuery query(wrap.openConnection());

    // 启动事务
    if (!query.exec("START TRANSACTION")) {
        qDebug() << "Failed to start transaction:" << query.lastError();
        return false;
    }

    bool result = false;

    // 执行传入的回调函数，传递当前的查询对象
    result = transactionCallback(query);

    // 判断事务执行结果
    if (result) {
        // 提交事务
        if (!query.exec("COMMIT")) {
            qDebug() << "Failed to commit transaction:" << query.lastError();
            query.exec("ROLLBACK");
            result = false;
        }
    }
    else {
        // 回滚事务
        query.exec("ROLLBACK");
    }

    return result;
}