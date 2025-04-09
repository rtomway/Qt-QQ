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
    QNetworkReply* reply = m_networkManager->post(request, data);
    qDebug() << "发送http请求";

    connect(m_reply, &QNetworkReply::finished, this, &HttpWorker::onRequestFinished);
}

void HttpWorker::onRequestFinished()
{
    if (m_reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = m_reply->readAll();
        emit responseReceived(responseData);
    }
    else {
        emit errorOccurred(m_reply->errorString());
    }
    m_reply->deleteLater();
}
