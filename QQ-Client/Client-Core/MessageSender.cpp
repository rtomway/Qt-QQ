#include "MessageSender.h"
#include <QJsonObject>
#include <QJsonDocument>

MessageSender* MessageSender::instance()
{
	static MessageSender ins;
	return &ins;
}

void MessageSender::setClient(Client* client)
{
	m_client = client;
}

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

void MessageSender::sendBinaryData(const QByteArray& data)
{
	m_client->getClientSocket()->sendBinaryMessage(data);
}
