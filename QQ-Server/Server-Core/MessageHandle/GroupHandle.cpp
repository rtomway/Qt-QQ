#include "GroupHandle.h"
#include <QDate>

#include "CreateId.h"
#include "DataBaseQuery.h"


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
	QString queryStr = "insert into `group` (group_id,group_name,ower_id,create_time,group_avatar)values(?,?,?,?,?)";
	QVariantList bindvalues;
	bindvalues.append(groupId);
	bindvalues.append(groupName);
	bindvalues.append(groupOwerId);
	bindvalues.append(QDate::currentDate());
	bindvalues.append(groupAvatar);
	query.executeNonQuery(queryStr, bindvalues);
}
