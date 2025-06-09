#include "CreateId.h"

#include "DataBaseQuery.h"
#include <QRandomGenerator>
#include <QJsonArray>

QString CreateId::getRandomID(int length)
{
	QString user_id;
	QRandomGenerator randomID = QRandomGenerator::securelySeeded(); // 手动初始化种子;
	while (user_id.size() < length)
	{
		int ram = randomID.bounded(0, 9);
		user_id.append(QString::number(ram));
	}
	return user_id;
}

QString CreateId::generateUserID(Id type)
{
	QString id;
	QString queryStr;
	if (type == User)
	{
		queryStr="select user_id from user where user_id=?";
	}
	else if(type == Group)
	{
		queryStr = "select group_id from `group` where group_id=?";
	}
	//注册唯一id
	while (true) {
		//服务器随机生成10位数用户id
		id = getRandomID(10);
		//先查询生成id是否已存在
		DataBaseQuery query;
		QVariantList bindvalues;
		bindvalues.append(id);
		auto allQueryObj = query.executeQuery(queryStr, bindvalues);
		//错误返回
		if (allQueryObj.contains("error")) {
			qDebug() << "Error executing query:" << allQueryObj["error"].toString();
			return QString();
		}
		//未找到，id唯一跳出
		QJsonArray userArray = allQueryObj["data"].toArray();
		if (userArray.isEmpty())
		{
			return id;
		}
	}
}
