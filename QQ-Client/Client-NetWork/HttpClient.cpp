#include "HttpClient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QCoreApplication>
#include <QJsonDocument>

#include "PacketCreate.h"
#include "TokenManager.h"
#include "LoginUserManager.h"

HttpClient::HttpClient(QObject* parent)
	: HttpClientPort(parent)
	, m_networkManager(new QNetworkAccessManager(this))
{

}
//http请求
void HttpClient::get(const QString& type, const QUrlQuery& params, const QMap<QString, QString>& headers, HttpCallback callback)
{
	// 构造URL
	QUrl url(m_baseUrl + type);
	if (!params.isEmpty())
	{
		url.setQuery(params);
	}

	QNetworkRequest request(url);

	// 添加所有请求头
	this->setHeaders(request, headers);

	// 发送GET请求
	QNetworkReply* reply = m_networkManager->get(request);
	//响应回复
	connect(reply, &QNetworkReply::finished, [this, reply, callback]()
		{
			this->handleReply(reply, callback);
		});
}
void HttpClient::post(const QString& type, const QByteArray& data, const QMap<QString, QString>& headers, HttpCallback callback)
{
	//url
	QUrl url(m_baseUrl + type);
	//请求
	QNetworkRequest request(url);
	//请求头
	this->setHeaders(request, headers);
	//post提交
	QNetworkReply* reply = m_networkManager->post(request, data);
	//响应回复
	connect(reply, &QNetworkReply::finished, [this, reply, callback]()
		{
			handleReply(reply, callback);
		});
}

void HttpClient::get(const QString& type, const QUrlQuery& params, HttpCallback callback)
{
	this->get(type, params, QMap<QString, QString>(), callback);
}
void HttpClient::post(const QString& type, const QByteArray& data, HttpCallback callback)
{
	this->post(type, data, QMap<QString, QString>(), callback);
}

//请求头的添加
void HttpClient::setHeaders(QNetworkRequest& request, const QMap<QString, QString>& headers)
{
	//添加请求头
	for (auto it = headers.constBegin(); it != headers.constEnd(); ++it)
	{
		request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
	}
	//默认application/json
	if (!headers.contains("Content-Type"))
	{
		request.setRawHeader("Content-Type", "application/json");
	}
	//token
	QString token = TokenManager::getToken();
	if (!token.isEmpty())
	{
		QString authHeader = "Bearer " + token;
		request.setRawHeader("Authorization", authHeader.toUtf8());
	}
	//user_id
	auto& user_id = LoginUserManager::instance()->getLoginUserID();
	request.setRawHeader("user_id", user_id.toUtf8());
}

//响应处理
void HttpClient::handleReply(QNetworkReply* reply, HttpCallback callback)
{
	// 错误处理
	if (reply->error() != QNetworkReply::NoError)
	{
		replyErrorHandle(reply->error());
		reply->deleteLater();
		return;
	}

	// 检查 HTTP 状态码
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 204)
	{
		// 无响应体，但请求成功
		reply->deleteLater();
		return;
	}

	//处理响应
	replyDataHandle(reply, callback);
	reply->deleteLater();
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
		//返回json信息
		if (contentType.contains("application/json"))
		{
			//回调,主线程进行，GuI操作
			QMetaObject::invokeMethod(QCoreApplication::instance(), [callBack, responseData]()
				{
					callBack(QJsonDocument::fromJson(responseData).object(), QByteArray());
				});
			return;
		}
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
		emit httpTextResponse(responseData);
	}
	else
	{
		emit httpDataResponse(responseData);
	}
}

