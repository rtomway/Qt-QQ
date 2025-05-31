#include "ChatWidget.h"
#include <QBoxLayout>

ChatWidget::ChatWidget(QWidget* parent)
	:QWidget(parent)
	, m_stackedChatWidget(new QStackedWidget(this))
	, m_friendChatPage(new FriendChatPage(this))
	, m_groupChatPage(new GroupChatPage(this))
{
	init();
	initStackedWidget();
}

void ChatWidget::init()
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);  // 确保不会有额外的边距
	layout->setSpacing(0);
	layout->addWidget(m_stackedChatWidget);
	setLayout(layout);
}

void ChatWidget::initStackedWidget()
{
	m_stackedChatWidget->addWidget(m_friendChatPage);
	m_stackedChatWidget->addWidget(m_groupChatPage);
}

//会话界面加载
void ChatWidget::loadChatPage(ChatType type, const QString& id)
{
	switch (type)
	{
	case ChatType::User:
		m_stackedChatWidget->setCurrentWidget(m_friendChatPage);
		m_friendChatPage->setChatWidget(id);
		m_currentPage = m_friendChatPage;
		break;
	case ChatType::Group:
		m_stackedChatWidget->setCurrentWidget(m_groupChatPage);
		m_groupChatPage->setChatWidget(id);
		m_currentPage = m_groupChatPage;
		break;
	default:
		break;
	}
}

//判断当前加载界面id是否对应
bool ChatWidget::isCurrentChat(ChatType type, const QString& id)
{
	bool isCurrentChat = false;
	switch (type)
	{
	case ChatType::User:
		isCurrentChat = m_friendChatPage->isCurrentChat(id);
		break;
	case ChatType::Group:
		isCurrentChat = m_groupChatPage->isCurrentChat(id);
		break;
	default:
		isCurrentChat = false;
		break;
	}
	return isCurrentChat;
}

//判断当前界面m_current
bool ChatWidget::isStackedCurrentChat(ChatType type, const QString& id)
{
	bool isStackedCurrentChat = false;
	switch (type)
	{
	case ChatType::User:
		if (m_currentPage == m_friendChatPage && m_friendChatPage->isCurrentChat(id))
		{
			isStackedCurrentChat = true;
		}
		break;
	case ChatType::Group:
		if (m_currentPage == m_groupChatPage && m_groupChatPage->isCurrentChat(id))
		{
			isStackedCurrentChat = true;
		}
		break;
	default:
		break;
	}
	return isStackedCurrentChat;
}

//当前界面接收消息
void ChatWidget::updateReceiveMessage(const ChatMessage& chatMessage)
{
	auto& send_id = chatMessage.sendId;
	auto& messageType = chatMessage.messageType;
	auto& messageData = chatMessage.data;
	switch (chatMessage.chatType)
	{
	case ChatType::User:
		m_friendChatPage->updateReceiveMessage(send_id, messageData, messageType);
		break;
	case ChatType::Group:
		m_groupChatPage->updateReceiveMessage(send_id, messageData, messageType);
		break;
	default:

		break;
	}
}

//清除会话界面
void ChatWidget::clearChatWidget()
{
	m_friendChatPage->clearChatPage();
	m_groupChatPage->clearChatPage();
}

