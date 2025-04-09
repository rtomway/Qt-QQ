#include "HttpWorker.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

HttpWorker::HttpWorker(QObject* parent)
    : QObject(parent),
    m_networkManager(new QNetworkAccessManager(this))
{

}

void HttpWorker::sendRequest(const QString& type, const QByteArray& data, const QString& Content_type)
{
    QUrl url(m_baseUrl + type);
    //请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, Content_type);
     m_reply = m_networkManager->post(request, data);
	qDebug() << "发送http请求";
	connect(m_reply, &QNetworkReply::finished, [this]() {
		if (m_reply->error() == QNetworkReply::NoError) {
			// 检查 HTTP 状态码
			int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			if (statusCode == 204) {
				// 无响应体，但请求成功
				qDebug() << "请求成功，但没有返回任何内容（204 No Content）";
				return;
			}
			QByteArray responseData = m_reply->readAll();
			QByteArray contentType = m_reply->rawHeader("Content-Type");
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
			qDebug() << "HTTP Error:" << m_reply->errorString();
			// 你可以根据不同的错误码做更细致的处理
			switch (m_reply->error()) {
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
				qDebug() << "Other error:" << m_reply->errorString();
			}
		}
		m_reply->deleteLater();
		});
}

