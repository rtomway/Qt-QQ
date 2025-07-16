#include "RegisterHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "CreateId.h"
#include "GlobalTypes.h"
#include "LoginDBUtils.h"
#include "FriendDBUtils.h"

//注册
void RegisterHandle::handle_register(const QJsonObject& paramsObj,const QByteArray& data, QHttpServerResponder& responder)
{
	RegisterMessage registerMessage;
	registerMessage.user_id = CreateId::generateUserID(CreateId::Id::User);
	registerMessage.username = paramsObj["username"].toString();
	registerMessage.password = paramsObj["password"].toString();
	registerMessage.confidential = paramsObj["confidential"].toString();
	//注册信息插入
	DataBaseQuery query;
	auto registerResult = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!LoginDBUtils::insertUser(registerMessage, query, queryPtr))
			{
				return false;
			}
			//好友表插入
			MyFriend myFriend(registerMessage.user_id, registerMessage.user_id, "我的好友");
			if (!FriendDBUtils::insertFriend(myFriend, query, queryPtr))
			{
				return false;
			}
		});
	//注册成功后返回该用户账号与密码
	if (!registerResult)
		return;
	QJsonObject resgisterObj;
	resgisterObj["type"] = "registerSuccess";
	QJsonObject params;
	params["user_id"] = registerMessage.user_id;
	params["password"] = registerMessage.password;
	resgisterObj["params"] = params;
	QJsonDocument doc(resgisterObj);
	//http响应
	responder.write(doc);
}
