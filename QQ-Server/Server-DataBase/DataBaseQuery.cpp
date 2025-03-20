#include "DataBaseQuery.h"
#include "SSqlConnectionPool.h"
#include "QJsonArray"

DataBaseQuery::DataBaseQuery(QObject* parent)
    :QObject(parent)
{
}

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

bool DataBaseQuery::executeNonQuery(const QString& queryStr, const QVariantList& bindValues)
{
    SConnectionWrap wrap;
    QSqlQuery query(wrap.openConnection());
    query.prepare(queryStr);
    // 绑定查询参数
    for (const auto& value : bindValues) {
        query.addBindValue(value);
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
