#include "HttpClient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QCoreApplication>

#include "PacketCreate.h"
#include "TokenManager.h"
#include "LoginUserManager.h"
#include "Network_logger.h"

HttpClient::HttpClient(QObject* parent)
	: HttpClientPort(parent),
	m_networkManager(new QNetworkAccessManager(this))
{

}
//发送http请求
void HttpClient::sendRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack)
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

	Network_Logger::debug("[HTTP] Send Http Request:  " + type + "  to  " + m_baseUrl);

	QNetworkReply* reply = m_networkManager->post(request, data);

	connect(reply, &QNetworkReply::finished, [this, reply, callBack]()
		{
			Network_Logger::debug("[HTTP] Revice Http Reponse");
			// 错误处理
			if (reply->error() != QNetworkReply::NoError)
			{
				Network_Logger::error("[HTTP] HTTP Error:" + reply->errorString());
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
				Network_Logger::info("[HTTP] Http Request Success 204  No Content");
				reply->deleteLater();
				return;
			}

			//处理响应
			replyDataHandle(reply, callBack);

			reply->deleteLater();
		});
}

//错误码处理
void HttpClient::replyErrorHandle(QNetworkReply::NetworkError error)
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
void HttpClient::replyDataHandle(QNetworkReply* reply, HttpCallback callBack)
{
	//处理响应
	QByteArray responseData = reply->readAll();
	QByteArray contentType = reply->rawHeader("Content-Type");

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
		Network_Logger::debug("[HTTP] Revice Http  Text message reponse");
		emit httpTextResponse(responseData);
	}
	else
	{
		Network_Logger::debug("[HTTP] Revice Http Binary data reponse");
		emit httpDataResponse(responseData);
	}
}

