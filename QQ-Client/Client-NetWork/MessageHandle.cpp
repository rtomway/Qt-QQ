#include "MessageHandle.h"
#include <QJsonDocument>
#include <QJsonArray>

#include "PacketCreate.h"
#include "Network_logger.h"


MessageHandle::MessageHandle()
	: QObject(nullptr)
	, m_consumerThread(new QThread(this))
{
	initRequestHash();
}
// 注册请求类型和对应的处理函数
template <typename T>
void MessageHandle::registerHandle(const QString& key, T& instance, void (T::* handler)(const QJsonObject&, const QByteArray&))
{
	requestHash[key] = [&instance, handler](const QJsonObject& obj, const QByteArray& data)
		{
			(instance.*handler)(obj, data);
		};
}

//注册表
void MessageHandle::initRequestHash()
{
	//登录界面
	registerHandle("loginValidationSuccess", m_loginHandle, &Client_LoginHandle::handle_loginValidationSuccess);
	registerHandle("loginSuccess", m_loginHandle, &Client_LoginHandle::handle_loginSuccess);
	registerHandle("loadFriendList", m_loginHandle, &Client_LoginHandle::handle_loadFriendList);
	registerHandle("loadGroupList", m_loginHandle, &Client_LoginHandle::handle_loadGroupList);
	registerHandle("loadGroupMember", m_loginHandle, &Client_LoginHandle::handle_loadGroupMember);
	registerHandle("loadGroupMemberAvatar", m_loginHandle, &Client_LoginHandle::handle_loadGroupMemberAvatar);
	registerHandle("loadGroupAvatars", m_loginHandle, &Client_LoginHandle::handle_loadGroupAvatars);
	registerHandle("loadFriendAvatars", m_loginHandle, &Client_LoginHandle::handle_loadFriendAvatars);
	registerHandle("registerSuccess", m_loginHandle, &Client_LoginHandle::handle_registerSuccess);
	//好友处理
	registerHandle("addFriend", m_friendHandle, &Client_FriendHandle::handle_addFriend);
	registerHandle("friendDeleted", m_friendHandle, &Client_FriendHandle::handle_friendDeleted);
	registerHandle("newFriend", m_friendHandle, &Client_FriendHandle::handle_newFriend);
	registerHandle("rejectAddFriend", m_friendHandle, &Client_FriendHandle::handle_rejectAddFriend);
	registerHandle("textCommunication", m_friendHandle, &Client_FriendHandle::handle_textCommunication);
	registerHandle("pictureCommunication", m_friendHandle, &Client_FriendHandle::handle_pictureCommunication);
	//用户
	registerHandle("searchUser", m_userHandle, &Client_UserHandle::handle_searchUser);
	registerHandle("searchGroup", m_userHandle, &Client_UserHandle::handle_searchGroup);
	registerHandle("updateUserMessage", m_userHandle, &Client_UserHandle::handle_updateUserMessage);
	registerHandle("updateUserAvatar", m_userHandle, &Client_UserHandle::handle_updateUserAvatar);
	//群组
	registerHandle("createGroupSuccess", m_groupHandle, &Client_GroupHandle::handle_createGroupSuccess);
	registerHandle("groupInviteSuccess", m_groupHandle, &Client_GroupHandle::handle_groupInviteSuccess);
	registerHandle("addGroup", m_groupHandle, &Client_GroupHandle::handle_addGroup);
	registerHandle("rejectAddGroup", m_groupHandle, &Client_GroupHandle::handle_rejectAddGroup);
	registerHandle("groupAddFailed", m_groupHandle, &Client_GroupHandle::handle_groupAddFailed);
	registerHandle("groupTextCommunication", m_groupHandle, &Client_GroupHandle::handle_groupTextCommunication);
	registerHandle("groupPictureCommunication", m_groupHandle, &Client_GroupHandle::handle_groupPictureCommunication);
	registerHandle("newGroupMember", m_groupHandle, &Client_GroupHandle::handle_newGroupMember);
	registerHandle("newGroup", m_groupHandle, &Client_GroupHandle::handle_newGroup);
	registerHandle("groupInvite", m_groupHandle, &Client_GroupHandle::handle_groupInvite);
	registerHandle("groupMemberDeleted", m_groupHandle, &Client_GroupHandle::handle_groupMemberDeleted);
	registerHandle("groupMemberExitGroup", m_groupHandle, &Client_GroupHandle::handle_groupMemberExitGroup);
	registerHandle("disbandGroup", m_groupHandle, &Client_GroupHandle::handle_disbandGroup);
	registerHandle("beRemovedGroup", m_groupHandle, &Client_GroupHandle::handle_beRemovedGroup);
	registerHandle("batch_groupMemberDeleted", m_groupHandle, &Client_GroupHandle::handle_batch_groupMemberDeleted);
	registerHandle("updateGroupAvatar", m_groupHandle, &Client_GroupHandle::handle_updateGroupAvatar);
}

//文本消息处理
void MessageHandle::handle_textMessage(const QString& message)
{
	auto messageDoc = QJsonDocument::fromJson(message.toUtf8());
	if (messageDoc.isObject())
	{
		QJsonObject obj = messageDoc.object();
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		QByteArray data;
		if (requestHash.contains(type))
		{
			auto& handle = requestHash[type];
			handle(paramsObject, data);
		}
	}
}

//二进制数据处理
void MessageHandle::handle_binaryData(const QByteArray& data)
{
	auto parsePacketList = PacketCreate::parseDataPackets(data);
	for (auto& parsePacket : parsePacketList)
	{
		// 根据类型给处理函数处理
		if (requestHash.contains(parsePacket.type))
		{
			auto& handle = requestHash[parsePacket.type];
			handle(parsePacket.params, parsePacket.data);
		}
		else
		{
			Network_Logger::debug("unknow type :" + parsePacket.type);
		}
	}
}

//设置消息来源
void MessageHandle::setMessageSrc(MessageQueue* messageQueue)
{
	m_messageQueue = messageQueue;
	startConsumerThread();
}

//启动线程
void MessageHandle::startConsumerThread()
{
	if (m_running) return;

	m_running = true;
	// 将消费者对象移到新线程
	this->moveToThread(&m_consumerThread);
	// 连接线程启动信号
	connect(&m_consumerThread, &QThread::started, this, &MessageHandle::consumerLoop);
	m_consumerThread.start();
}

//停止线程
void MessageHandle::stopConsumerThread()
{
	m_running = false;
	m_consumerThread.quit();
	m_consumerThread.wait();
}

//消息处理
void MessageHandle::consumerLoop()
{
	while (m_running)
	{
		if (!m_messageQueue)
		{
			QThread::msleep(10);
			continue;
		}

		auto msg = m_messageQueue->pop(100); // 带超时的pop
		if (msg.data.isNull())
		{
			continue;
		}

		// 处理消息 (注意: 现在在消费者线程上下文)
		switch (msg.type)
		{
		case MessageQueue::Text:
			handle_textMessage(QString::fromUtf8(msg.data));
			break;
		case MessageQueue::Binary:
			handle_binaryData(msg.data);
			break;
		}
	}
}

