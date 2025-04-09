#include "GroupChatPage.h"
#include "ui_ChatPage.h"
#include "GroupManager.h"
#include "ChatRecordManager.h"
#include "MessageSender.h"
#include "AvatarManager.h"
#include "GlobalTypes.h"
#include <QVariant>
#include "ImageUtil.h"

GroupChatPage::GroupChatPage(QWidget* parent)
	:ChatPage(parent)
{
	init();
}

GroupChatPage::~GroupChatPage()
{
}
void GroupChatPage::init()
{
	connect(ui->sendBtn, &QPushButton::clicked, this, [=]
		{
			auto user_id = m_oneself->getFriendId();
			auto& avatar = AvatarManager::instance()->getAvatar(m_oneself->getFriendId(), ChatType::User);
			auto headPix = ImageUtils::roundedPixmap(avatar, QSize(100, 100), 66);
			//文字消息
			QString msg = ui->messageTextEdit->toPlainText();
			if (msg.isEmpty())
				return;
			//消息显示至聊天框
			createTextMessageBubble(headPix, msg, MessageBubble::BubbleTextRight, user_id);
			qDebug() << m_group->getGroupId();
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
		});
}

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

void GroupChatPage::storeMessage(const QString& sender_id, const QString& receiver_id, const std::shared_ptr<MessageRecord>& message)
{
	ChatRecordManager::instance()->updateGroupChat(receiver_id)->addMessage(message);
}

bool GroupChatPage::isCurrentChat(const QString& id, ChatType type) const
{
	if (id == m_group->getGroupId() && type == ChatType::Group)
		return true;
	else
		return false;
}

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


