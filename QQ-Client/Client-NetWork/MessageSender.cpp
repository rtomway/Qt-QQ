#include "MessageSender.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include "Client.h"
#include "SConfigFile.h"


MessageSender* MessageSender::instance()
{
	static MessageSender ins;
	return &ins;
}
MessageSender::MessageSender()
	:m_workerThread(new QThread(this))
	, m_httpWorker(new HttpWorker)
	, m_networkManager(new QNetworkAccessManager(this))
{
	//后台线程处理消息
	m_httpWorker->moveToThread(m_workerThread);
	m_workerThread->start();
	//http请求
	connect(this, &MessageSender::sendHttpRequestToThread, m_httpWorker, &HttpWorker::sendRequest);
	//http文本消息响应
	connect(m_httpWorker, &HttpWorker::httpTextResponseReceived, this, &MessageSender::httpTextResponseReceived);
	//http二进制数据响应
	connect(m_httpWorker, &HttpWorker::httpDataResponseReceived, this, &MessageSender::httpDataResponseReceived);
}
//设置当前客户端
void MessageSender::setClient(Client* client)
{
	m_client = client;
}
//关闭连接
void MessageSender::disConnect()
{
	qDebug() << "客户端关闭连接";
	m_client->disconnect();
}
//Web发送文本消息
void MessageSender::sendMessage(const QString& type, const QVariantMap& params)
{
	if (m_client->isConnected())
	{
		QJsonObject jsonData;
		jsonData["type"] = type;
		//QVariantMap转为QJsonObject
		QJsonObject paramsObject;
		for (auto it = params.begin(); it != params.end(); it++)
		{
			//将不同类型数据转为Json
			paramsObject[it.key()] = QJsonValue::fromVariant(it.value());
		}
		jsonData["params"] = paramsObject;
		//token
		SConfigFile config("config.ini");
		jsonData["token"] = config.value("token").toString();
		//发送Json数据
		QJsonDocument doc(jsonData);
		QString message = QString(doc.toJson(QJsonDocument::Compact));
		m_client->getClientSocket()->sendTextMessage(message);
	}
	else
	{
		qWarning() << "数据发送失败 连接已断开......";
	}
}
//Web发送二进制数据
void MessageSender::sendBinaryData(const QByteArray& data)
{
	m_client->getClientSocket()->sendBinaryMessage(data);
}
//发送http请求
void MessageSender::sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type)
{
	emit sendHttpRequestToThread(type, data, Content_type);
}



