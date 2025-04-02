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

bool ChatWidget::isCurrentChat(ChatType type, const QString& id)
{
	return m_currentPage->isCurrentChat(id, type);
}

void ChatWidget::updateReceiveMessage(const QString& user_id, const QString& message)
{
	if (m_currentPage)
		m_currentPage->updateReciveMessage(user_id, message);
}

void ChatWidget::updateReceiveMessage(const QString& user_id, const QPixmap& pixmap)
{
	if (m_currentPage)
		m_currentPage->updateReciveMessage(user_id, pixmap);
}

void ChatWidget::clearChatWidget()
{
	if (m_currentPage)
		m_currentPage->clearMessageWidget();
}

void ChatWidget::updateChatMessage(ChatType type, const QString& sender_id, const QString& receiver_id, const QVariant& msg)
{
	if (type == ChatType::User)
	{
		m_friendChatPage->updateChatMessage(sender_id, receiver_id, msg);
	}
	else if (type == ChatType::Group)
	{
		m_groupChatPage->updateChatMessage(sender_id, receiver_id, msg);
	}
}
