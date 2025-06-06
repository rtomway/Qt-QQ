#include "HttpWorker.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QCoreApplication>

#include "PacketCreate.h"
#include "TokenManager.h"
#include "LoginUserManager.h"

HttpWorker::HttpWorker(QObject* parent)
	: QObject(parent),
	m_networkManager(new QNetworkAccessManager(this))
{

}
//发送http请求
void HttpWorker::sendRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack)
{
	QUrl url(m_baseUrl + type);
	//请求
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, Content_type);

	// 添加 Authorization token 头
	QString token = TokenManager::getToken();
	if (!token.isEmpty()) {
		QString authHeader = "Bearer " + token;
		request.setRawHeader("Authorization", authHeader.toUtf8());
	}
	auto& user_id = LoginUserManager::instance()->getLoginUserID();
	request.setRawHeader("user_id", user_id.toUtf8());

	qDebug() << "------------------发送http请求-----------------";
	qDebug() << url;
	QNetworkReply* reply = m_networkManager->post(request, data);
	
	connect(reply, &QNetworkReply::finished, [this, reply, callBack]()
		{
			qDebug() << "------------------接收http回复-----------------";
			// 错误处理
			if (reply->error() != QNetworkReply::NoError)
			{
				qDebug() << "HTTP Error:" << reply->errorString();
				// 你可以根据不同的错误码做更细致的处理
				replyErrorHandle(reply->error());
				reply->deleteLater();
				return;
			}

			// 检查 HTTP 状态码
			int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			if (statusCode == 204)
			{
				// 无响应体，但请求成功
				qDebug() << "请求成功，但没有返回任何内容（204 No Content）";
				reply->deleteLater();
				return;
			}

			//处理响应
			replyDataHandle(reply, callBack);

			reply->deleteLater();
		});
}

//错误码处理
void HttpWorker::replyErrorHandle(QNetworkReply::NetworkError error)
{
	switch (error)
	{
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
		qDebug() << "Other error:";
	}
}

//响应数据处理
void HttpWorker::replyDataHandle(QNetworkReply* reply, HttpCallback callBack)
{
	//处理响应
	QByteArray responseData = reply->readAll();
	QByteArray contentType = reply->rawHeader("Content-Type");
	qDebug() << "http响应头:" << contentType;

	//调用回调
	if (callBack)
	{
		//解析返回数据包
		auto parsePacketList = PacketCreate::parseDataPackets(responseData);
		//回调,主线程进行，GuI操作
		QMetaObject::invokeMethod(QCoreApplication::instance(), [callBack, parsePacketList]()
			{
				callBack(parsePacketList.first().params, parsePacketList.first().data);
			});
		return;
	}

	//信号传出处理
	if (contentType.contains("application/json"))
	{
		qDebug() << "接收http文本回复";
		emit httpTextResponseReceived(responseData);
	}
	else
	{
		qDebug() << "接收http数据回复";
		emit httpDataResponseReceived(responseData);
	}
}

