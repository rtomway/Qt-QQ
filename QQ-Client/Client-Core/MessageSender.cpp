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
//设置客户端
void MessageSender::setClient(Client* client)
{
	m_client = client;
}
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
//HttpRequest
void MessageSender::sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type)
{
	QUrl url(m_baseUrl + type);
	//请求
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, Content_type);
	QNetworkReply* reply = m_networkManager->post(request, data);
	qDebug() << "发送http请求";
	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			// 检查 HTTP 状态码
			int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			if (statusCode == 204) {
				// 无响应体，但请求成功
				qDebug() << "请求成功，但没有返回任何内容（204 No Content）";
				return;
			}
			QByteArray responseData = reply->readAll();
			QByteArray contentType = reply->rawHeader("Content-Type");
			qDebug() << "接收http回复";
			if (contentType.contains("application/json")) {
				qDebug() << "接收http文本回复";
				emit httpTextResponseReceived(responseData);
			}
			else {
				qDebug() << "接收http数据回复";
				emit httpDataResponseReceived(responseData);
			}
		}
		else {
			// 错误处理
			qDebug() << "HTTP Error:" << reply->errorString();
			// 你可以根据不同的错误码做更细致的处理
			switch (reply->error()) {
			case QNetworkReply::TimeoutError:
				qDebug() << "The request timed out.";
				break;
			case QNetworkReply::HostNotFoundError:
				qDebug() << "The host was not found.";
				break;
			case QNetworkReply::ConnectionRefusedError:
				qDebug() << "The connection was refused.";
				break;
				// 其他错误处理
			default:
				qDebug() << "Other error:" << reply->errorString();
			}
		}
		reply->deleteLater();
		});
}

MessageSender::MessageSender()
{
	m_networkManager = new QNetworkAccessManager(this);
}
