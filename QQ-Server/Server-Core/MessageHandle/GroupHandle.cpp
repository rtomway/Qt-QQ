#include "GroupHandle.h"
#include <QDate>
#include <QJSonDocument>

#include "CreateId.h"
#include "DataBaseQuery.h"
#include "ConnectionManager.h"


//群组搜索
void GroupHandle::handle_searchGroup(const QJsonObject& paramsObject, const QByteArray& data)
{

}
//添加群组
void GroupHandle::handle_createGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto groupOwerId = paramsObject["user_id"].toString();
	auto groupId = CreateId::generateUserID(CreateId::Id::Group);
	auto groupName= paramsObject["group_name"].toString();
	auto groupAvatar = groupId + ".png";

	DataBaseQuery query;
	auto result = query.executeTransaction([&](QSqlQuery& sqlquery)->bool
		{
			QString queryStr_1 = "insert into `group` (group_id,group_name,ower_id,create_time,group_avatar)values(?,?,?,?,?)";
			QVariantList bindvalues_1;
			bindvalues_1.append(groupId);
			bindvalues_1.append(groupName);
			bindvalues_1.append(groupOwerId);
			bindvalues_1.append(QDate::currentDate());
			bindvalues_1.append(groupAvatar);
			auto insertResult_1 = query.executeNonQuery(queryStr_1, bindvalues_1);
			if (!insertResult_1) {
				qDebug() << "insertResult_1失败";
				return false;  // 查询失败，回滚事务
			}
			QString queryStr_2 = "insert into groupMembers (group_id,user_id,role)values(?,?,?)";
			QVariantList bindvalues_2;
			bindvalues_2.append(groupId);
			bindvalues_2.append(groupOwerId);
			bindvalues_2.append("ower");
			auto insertResult_2 = query.executeNonQuery(queryStr_2, bindvalues_2);
			if (!insertResult_2) {
				qDebug() << "insertResult_2失败";
				return false;  // 查询失败，回滚事务
			}
			//所有操作都成功
			return true;
		});

	if (result)
	{
		auto groupObj = paramsObject;
		groupObj["groupId"] = groupId;
		QJsonObject allObj;
		allObj["type"] = "createGroupSuccess";
		allObj["params"] = groupObj;

		QJsonDocument doc(allObj);
		QString message = doc.toJson(QJsonDocument::Compact);
		ConnectionManager::instance()->sendTextMessage(groupOwerId, message);
	}
}
