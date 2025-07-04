#include "GroupService.h"
#include <QJsonDocument>
#include <QJsonArray>

#include "EventBus.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "LoginUserManager.h"
#include "ChatRecordManager.h"
#include "ChatRecordMessage.h"


constexpr int MIN_GROUPMEMBER_COUNT_FOR_LOADING = 50;  // 低于此人数时加载群信息
constexpr int BATCHSIZE_LOADAVATAR = 10;       //一次申请加载的数量

GroupService::GroupService(GroupRespository* groupRespository,QObject* parent)
	:QObject(parent)
	,m_groupRespository(groupRespository)
{

}

GroupService::~GroupService()
{

}

void GroupService::initAllGroupRequest()
{
	auto& user_id = LoginUserManager::instance()->getLoginUserID();
	loadGroupInfoFromServer(user_id, "loadGroupList");
}

void GroupService::initAllGroup(const QJsonObject& obj)
{
	auto groupListArray = obj["groupList"].toArray();
	QStringList localGroup_IdList;
	QStringList needLoadGroup_IdList;
	for (auto groupValue : groupListArray)
	{
		//加载群聊数据
		auto groupObj = groupValue.toObject();
		auto myGroup = QSharedPointer<Group>::create();
		myGroup->setGroup(groupObj);
		m_groupRespository->addGroup(myGroup);

		//加载客户端自身在群组中的角色
		QJsonObject loginUserObj;
		auto& loginUser = LoginUserManager::instance()->getLoginUser();
		loginUserObj["user_id"] = loginUser->getFriendId();
		loginUserObj["username"] = loginUser->getFriendName();
		loginUserObj["group_role"] = groupObj["group_role"];
		myGroup->loadGroupMember(loginUserObj);

		//头像是否申请的分组
		auto& group_id = myGroup->getGroupId();
		if (!AvatarManager::instance()->hasLocalAvatar(group_id, ChatType::Group))
		{
			needLoadGroup_IdList.append(group_id);
		}
		else
		{
			localGroup_IdList.append(group_id);
		}

		//向服务器申请群成员信息加载
		if (myGroup->count() <= MIN_GROUPMEMBER_COUNT_FOR_LOADING)
		{
			loadGroupInfoFromServer(group_id, "loadGroupMember");
		}
	}

	//加载已有头像群组列表
	emit g_loadLocalAvatarGroup(localGroup_IdList);
	//分批向服务器发送头像请求
	if (!needLoadGroup_IdList.isEmpty())
	{
		loadGroupAvatarFromServer(needLoadGroup_IdList, "loadGroupAvatars");
	}
}

void GroupService::loadGroupMember(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto group = m_groupRespository->findGroup(group_id);
	auto groupMemberArray = obj["groupMemberArray"].toArray();
	group->batchLoadGroupMember(groupMemberArray);

	auto groupMemberIdList = group->getGroupMembersIdList();
	QStringList loadAvatarFromServer_idList;
	for (auto& member_id : groupMemberIdList)
	{
		//头像是否申请的分组
		if (!AvatarManager::instance()->hasLocalAvatar(member_id, ChatType::User))
		{
			loadAvatarFromServer_idList.append(member_id);
		}
	}
	if (!loadAvatarFromServer_idList.isEmpty())
	{
		loadGroupAvatarFromServer(loadAvatarFromServer_idList, "loadGroupMemberAvatar");
	}
}

void GroupService::createGroupSuccess(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto group_name = obj["group_name"].toString();
	auto myGroup = QSharedPointer<Group>::create();
	myGroup->setGroup(obj);
	m_groupRespository->addGroup(myGroup);

	//群主
	QJsonObject groupMemberObj;
	groupMemberObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
	groupMemberObj["username"] = LoginUserManager::instance()->getLoginUserName();
	groupMemberObj["group_role"] = "owner";
	myGroup->loadGroupMember(groupMemberObj);

	emit g_createGroupSuccess(group_id);
}

void GroupService::newGroupMember(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto user_id = obj["user_id"].toString();
	auto group = m_groupRespository->findGroup(group_id);
	group->addMember(obj);

	emit g_newGroupMember(group_id, user_id);
	emit g_updateGroupProfile(group_id);
}

void GroupService::newGroup(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto myGroup = QSharedPointer<Group>::create();
	myGroup->setGroup(obj);
	m_groupRespository->addGroup(myGroup);

	//加载客户端自身在群组中的角色
	QJsonObject loginUserObj;
	auto& loginUser = LoginUserManager::instance()->getLoginUser();
	loginUserObj["user_id"] = loginUser->getFriendId();
	loginUserObj["username"] = loginUser->getFriendName();
	loginUserObj["group_role"] = "member";
	myGroup->loadGroupMember(loginUserObj);

	//通知内部各个控件
	emit g_newGroup(group_id);
}

void GroupService::groupTextCommunication(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto user_id = obj["user_id"].toString();

	//确保成员信息存在
	this->ensureGroupMemberLoad(group_id, user_id, [=]()
		{
			emit g_groupTextCommunication(obj);
		});
}

void GroupService::groupPictureCommunication(const QJsonObject& obj, const QPixmap& pixmap)
{
	auto group_id = obj["group_id"].toString();
	auto user_id = obj["user_id"].toString();

	this->ensureGroupMemberLoad(group_id, user_id, [=]()
		{
			emit g_groupPictureCommunication(obj, pixmap);
		});
}

void GroupService::exitGroup(const QString& group_id, const QString& user_id)
{
	emit g_exitGroup(group_id, user_id);
}

void GroupService::removeGroup(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto user_id = obj["user_id"].toString();

	emit g_exitGroup(group_id, user_id);
}

void GroupService::groupMemberDeleted(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto user_id = obj["user_id"].toString();
	auto group = m_groupRespository-> findGroup(group_id);
	group->removeMember(user_id);

	emit g_updateGroupProfile(group_id);
}

void GroupService::disbandGroup(const QString& group_id)
{
	emit g_exitGroup(group_id, LoginUserManager::instance()->getLoginUserID());
}

void GroupService::batch_groupMemberDeleted(const QJsonObject& obj)
{
	auto group_id = obj["group_id"].toString();
	auto group = m_groupRespository->findGroup(group_id);
	auto deleteMemberArray = obj["deleteMemberList"].toArray();
	for (auto deleteMember_idValue : deleteMemberArray)
	{
		auto deleteMember_id = deleteMember_idValue.toString();
		group->removeMember(deleteMember_id);
	}

	emit g_updateGroupProfile(group_id);
}

void GroupService::loadGroupInfoFromServer(const QString& id, const QString& requestType)
{
	QJsonObject loadListObj;
	loadListObj["id"] = id;
	QJsonDocument loadListDoc(loadListObj);
	QByteArray loadListData = loadListDoc.toJson(QJsonDocument::Compact);
	NetWorkServiceLocator::instance()->sendHttpRequest(requestType, loadListData, "application/json");
}

void GroupService::loadGroupAvatarFromServer(const QStringList& group_idList, const QString& requestType)
{
	int totalSize = group_idList.size();
	// 按批次处理 group_idList
	for (int i = 0; i < totalSize; i += BATCHSIZE_LOADAVATAR)
	{
		QJsonArray loadGroupAvatarIdArray;
		// 获取当前批次的 10 个 group_id
		QStringList currentBatch = group_idList.mid(i, BATCHSIZE_LOADAVATAR);
		// 处理当前批次的 group_id
		for (const auto& groupId : currentBatch)
		{
			loadGroupAvatarIdArray.append(groupId);
		}

		QJsonObject loadGroupAvatarIdObj;
		loadGroupAvatarIdObj["applicateAvatar_idList"] = loadGroupAvatarIdArray;
		QJsonDocument loadGroupAvatarIdDoc(loadGroupAvatarIdObj);
		auto data = loadGroupAvatarIdDoc.toJson(QJsonDocument::Compact);
		NetWorkServiceLocator::instance()->sendHttpRequest(requestType, data, "application/json");
	}
}

void GroupService::ensureGroupMemberLoad(const QString& group_id, const QString& user_id, std::function<void()> callback) const
{
	auto group = m_groupRespository->findGroup(group_id);
	if (!group)
	{
		qWarning() << "群组不存在:" << group_id;
		return;
	}

	auto member = group->getMember(user_id);
	if (!member.member_id.isEmpty()) {
		callback();
		return;
	}

	// 构建请求体
	QJsonObject memberObj;
	memberObj["member_id"] = user_id;
	memberObj["group_id"] = group_id;
	QJsonDocument doc(memberObj);
	QByteArray data = doc.toJson(QJsonDocument::Compact);
	// 发送加载请求
	NetWorkServiceLocator::instance()->sendHttpRequest("groupMemberLoad", data, "application/json",
		[=](const QJsonObject& params, const QByteArray& avatarData)
		{
			group->loadGroupMember(params);
			ImageUtils::saveAvatarToLocal(avatarData, user_id, ChatType::User, [=]()
				{
						callback();
				});
		});
}
