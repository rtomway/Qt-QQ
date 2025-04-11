#include "GroupChatPage.h"
#include "ui_ChatPage.h"
#include <QVariant>

#include "GroupManager.h"
#include "ChatRecordManager.h"
#include "MessageSender.h"
#include "AvatarManager.h"
#include "GlobalTypes.h"
#include "ImageUtil.h"

GroupChatPage::GroupChatPage(QWidget* parent)
	:ChatPage(parent)
{
	init();
}
GroupChatPage::~GroupChatPage()
{
	delete ui;
}
void GroupChatPage::init()
{
	connect(ui->sendBtn, &QPushButton::clicked, this, [=]
		{
			AvatarManager::instance()->getAvatar(m_oneself->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
				{
					auto user_id = m_oneself->getFriendId();
					auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100), 66);
					sendTextMessageToServer(user_id, headPix);
				});
		});
}
//设置会话界面
void GroupChatPage::setChatWidget(const QString& id)
{
	if (!m_group)
	{
		m_group = GroupManager::instance()->findGroup(id);
	}
	if (m_group->getGroupId() != id)
	{
		m_group = GroupManager::instance()->findGroup(id);
	}
	m_chat = ChatRecordManager::instance()->getGroupChat(id);
	loadChatMessage(*m_chat);
	//界面信息
	auto groupname = QString("%1(%2)").arg(m_group->getGroupName()).arg(m_group->count());
	ui->nameLab->setText(groupname);
}
//保存信息
void GroupChatPage::storeMessage(const QString& sender_id, const QString& receiver_id, const std::shared_ptr<MessageRecord>& message)
{
	ChatRecordManager::instance()->addMessageToGroupChat(receiver_id,message);
}
//判断当前会话
bool GroupChatPage::isCurrentChat(const QString& id, ChatType type) const
{
	if (id == m_group->getGroupId() && type == ChatType::Group)
		return true;
	else
		return false;
}
//消息发送
void GroupChatPage::sendImageMessageToServer(const QString& id, const QPixmap& pixmap)
{

}
void GroupChatPage::sendTextMessageToServer(const QString& user_id, const QPixmap& headPix)
{
	//文字消息
	QString msg = ui->messageTextEdit->toPlainText();
	if (msg.isEmpty())
		return;
	//消息显示至聊天框
	createTextMessageBubble(headPix, msg, MessageBubble::BubbleTextRight, user_id);
	//将消息加入至聊天记录中
	updateChatMessage(user_id, m_group->getGroupId(), QVariant::fromValue(msg));
	//清空输入
	ui->messageTextEdit->clear();
	//发送给服务器通过服务器转发
	QVariantMap groupMessageMap;
	groupMessageMap["message"] = msg;
	groupMessageMap["user_id"] = m_oneself->getFriendId();
	groupMessageMap["group_id"] = m_group->getGroupId();
	groupMessageMap["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
	MessageSender::instance()->sendMessage("groupTextCommunication", groupMessageMap);
}
//消息气泡
void GroupChatPage::createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	auto& member = m_group->getMember(user_id);
	auto memberName = member.member_name;
	auto memberRole = member.member_role;
	MessageBubble* bubble = new MessageBubble(avatar, pixmap, bubbleType, memberName, memberRole);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}
void GroupChatPage::createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	auto& member = m_group->getMember(user_id);
	auto memberName = member.member_name;
	auto memberRole = member.member_role;
	qDebug() << "member:" << memberName << memberRole;
	MessageBubble* bubble = new MessageBubble(avatar, message, bubbleType, memberName, memberRole);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}


