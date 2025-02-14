#pragma once
#include "MessagePage.h"
#include "ui_MessagePage.h"
#include <QFile>
#include "MessageListItem.h"
#include "MessageBubble.h"
#include <QPainter>
#include <QPainterPath>
#include "ImageUtil.h"
#include "Client.h"
#include <QJsonObject>
#include <QJsonArray>
#include"SConfigFile.h"
#include "User.h"

MessagePage::MessagePage(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::MessagePage)
{
	ui->setupUi(this);
	init();
	this->setObjectName("MessagePage");
	QFile file(":/stylesheet/Resource/StyleSheet/MessagePage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
	//QPalette palette = this->palette();
	//QColor backgroundColor = palette.color(QPalette::Window);

	//qDebug() << "Background color:" << backgroundColor;
	//// 获取RGB值
	//int red = backgroundColor.red();
	//int green = backgroundColor.green();
	//int blue = backgroundColor.blue();

	//qDebug() << "Background color in RGB:" << red << green << blue;
}

MessagePage::~MessagePage()
{
	delete ui;
}

void MessagePage::init()
{
	m_headPix= ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(100, 100), 66);
	QPixmap pixmap = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(100,100), 66);
	MessageBubble* b = new MessageBubble(pixmap, "hahaha");
	ui->messageListWidget->addItem(b);
	ui->messageListWidget->setItemWidget(b, b);

	MessageBubble* a = new MessageBubble(pixmap, "hahaha",MessageBubble::BubbleLeft);
	ui->messageListWidget->addItem(a);
	ui->messageListWidget->setItemWidget(a, a);

	//设置输入字体大小
	QFont font = ui->messageTextEdit->font();
	font.setPointSize(13); 
	ui->messageTextEdit->setFont(font);
	//发送文字消息
	connect(ui->sendBtn, &QPushButton::clicked, [=]
		{
			QString msg = ui->messageTextEdit->toPlainText();
			if (msg.isEmpty())
				return;
			MessageBubble* message = new MessageBubble(pixmap,msg,MessageBubble::BubbleRight);
			ui->messageListWidget->addItem(message);
			ui->messageListWidget->setItemWidget(message,message);
			ui->messageListWidget->scrollToBottom();
			ui->messageTextEdit->clear();
			//发送给服务器通过服务器转发
			auto config = new SConfigFile("config.ini");
			
			QVariantMap messageMap;
			messageMap["message"] = msg;
			//messageMap["user_id"]= config->value("user_id");
			messageMap["user_id"] = User::getInstance().getUserId();
			messageMap["to"] = m_user_id;
			qDebug()<< "当前用户id" << config->value("user_id");
			Client::instance()->sendMessage("communication", messageMap);
		});
	
	ui->messageListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
}

void MessagePage::setUser(const QJsonObject& obj)
{
	m_username = obj["username"].toString();
	m_user_id = obj["user_id"].toString();
	m_currentID = m_user_id;
	//信息更新到界面
	updateWidget();
	
	QJsonArray messageArray = obj["message"].toArray();
	for (const QJsonValue& jsonvalue : messageArray)
	{
		updateReciveMessage(jsonvalue.toString());
	}

}

QString MessagePage::getCurrentID()
{
	return m_currentID;
}

void MessagePage::updateWidget()
{
	ui->nameLab->setText(m_username);

}


void MessagePage::updateReciveMessage(const QString& Recivemessage)
{
	MessageBubble* message = new MessageBubble(m_headPix,Recivemessage, MessageBubble::BubbleLeft);
	ui->messageListWidget->addItem(message);
	ui->messageListWidget->setItemWidget(message, message);
	ui->messageListWidget->scrollToBottom();
}




