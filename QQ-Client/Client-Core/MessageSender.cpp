#include "MessageSender.h"
#include "MessageSender.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include "Client.h"

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
//HttpRequest文本消息
void MessageSender::sendHttpRequest(const QString& type, const QJsonObject& paramsObj)
{
	QUrl url(m_baseUrl + type);
	QByteArray data = QJsonDocument(paramsObj).toJson();
	//请求
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	QNetworkReply* reply = m_networkManager->post(request, data);
	qDebug() << "发送http请求";
	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QByteArray responseData = reply->readAll();
			QByteArray contentType = reply->rawHeader("Content-Type");
			qDebug() << "接收http回复";
			if (contentType.contains("application/json")) {
				emit httpTextResponseReceived(responseData);
			}
			else {
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
//HttpRequest文件
void MessageSender::sendFile(const QString& path, const QString& filePath, const QVariantMap& params)
{
	QFile* file = new QFile(filePath);
	if (!file->open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open file:" << filePath;
		delete file;
		return;
	}

	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();

	sendFile(path, file->readAll(), fileName, params);
	file->deleteLater();
}
//HttpRequest文件
void MessageSender::sendFile(const QString& path, const QByteArray& fileData, const QString& fileName, const QVariantMap& params)
{
	QUrl url(m_baseUrl + path);

	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart filePart;
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
		QVariant(QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileName)));
	filePart.setBody(fileData);
	multiPart->append(filePart);

	for (auto it = params.begin(); it != params.end(); ++it) {
		QHttpPart textPart;
		textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant(QString("form-data; name=\"%1\"").arg(it.key())));
		textPart.setBody(it.value().toString().toUtf8());
		multiPart->append(textPart);
	}

	QNetworkRequest request(url);
	QNetworkReply* reply = m_networkManager->post(request, multiPart);
	multiPart->setParent(reply); // 避免内存泄漏

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QByteArray responseData = reply->readAll();
			QByteArray contentType = reply->rawHeader("Content-Type");
			if (contentType == "text") {
				emit httpTextResponseReceived(responseData);
			}
			else {
				emit httpDataResponseReceived(responseData);
			}

		}
		else {
			qDebug() << "HTTP File Upload Error:" << reply->errorString();
		}
		reply->deleteLater();
		});
}

MessageSender::MessageSender()
{
	m_networkManager = new QNetworkAccessManager(this);
}
