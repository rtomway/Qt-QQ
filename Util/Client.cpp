#include "Client.h"
#include <QJsonDocument>
#include <QJsonObject>


Client::Client(QObject* parent)
    :QObject(parent), m_client(new QWebSocket()), m_isConnected(false)
{
    //客户端接受信号
    connect(m_client, &QWebSocket::textMessageReceived, this, &Client::onTextMessageReceived);
    connect(m_client, &QWebSocket::errorOccurred, this, &Client::onErrorOccurred);
    connect(m_client, &QWebSocket::connected, this, &Client::onConnected);
    connect(m_client, &QWebSocket::disconnected, this, &Client::onDisconnected);
    initRequestHash();
}

Client::~Client()
{
    delete m_client;
}

void Client::initRequestHash()
{
    requestHash["communication"] = &Client::handle_communication;
    requestHash["addFriend"] = &Client::handle_addFriend;
    requestHash["resultOfAddFriend"] = &Client::handle_resultOfAddFriend;
}

Client* Client::instance()
{
    static Client* ins;
    if (!ins)
    {
        ins = new Client();
    }
    return ins;
}

Client* Client::connectToServer(const QString& url)
{
    if (!m_isConnected)
    {
        m_client->open(url);
        m_isConnected = true;
    }
    return this;
}

//发送Json数据
Client* Client::sendMessage(const QString& type,const QVariantMap&params)
{
    if (m_isConnected)
    {
        QJsonObject jsonData;
        jsonData["type"] = type;
        //QVariantMap转为QJsonObject
        QJsonObject paramsObject;
        for (auto it=params.begin();it!=params.end();it++)
        {
            //将不同类型数据转为Json
            paramsObject[it.key()] =QJsonValue::fromVariant(it.value());
        }
        jsonData["params"] = paramsObject;
        //发送Json数据
        QJsonDocument doc(jsonData);
        QString message = QString(doc.toJson(QJsonDocument::Compact));
        m_client->sendTextMessage(message);
    }
    else
    {
        qWarning() << "数据发送失败 连接已断开......";
    }
    return this;
}

Client* Client::ReciveMessage(std::function<void(const QString&)> callback)
{
    m_messageCallback = callback;
    return this;
}

Client* Client::Error(std::function<void(const QString&)> callback)
{
    m_errorCallback = callback;
    return this;
}

Client* Client::Connected(std::function<void()> callback)
{
    m_connectedCallback = callback;
    return this;
}

Client* Client::DisconnectFromServer(std::function<void()> callback)
{
    m_disconnectedCallback = callback;
    return this;
}

void Client::onTextMessageReceived(const QString& message)
{
    qDebug() << "接受到服务端的消息:";
    //判断是其他客户端通信还是服务器回复响应
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        auto type = obj["type"].toString();
        auto paramsObject = obj["params"].toObject();
        qDebug() << type<< requestHash.contains(type);

        if (requestHash.contains(type))
        {
            auto handle = requestHash[type];
            (this->*handle)(paramsObject);
        }
        else
        {
            qDebug() << "——————————————————————————";
            if (m_messageCallback) {
                m_messageCallback(message);
            }
        }
      
        //auto client_id = paramsObject["user_id"].toString();

       
    }
}

void Client::onErrorOccurred(QAbstractSocket::SocketError error)
{
    if (m_errorCallback) {
        m_errorCallback(m_client->errorString());
   }
}

void Client::onConnected()
{
    if (m_connectedCallback)
    {
        m_connectedCallback();
    }
}

void Client::onDisconnected()
{
    m_isConnected = false;
    if (m_disconnectedCallback) {
        m_disconnectedCallback();
    }
}

void Client::disconnect()
{
    if (m_isConnected) {
        m_client->close();
        m_isConnected = false;
    }
}

void Client::handle_communication(const QJsonObject& paramsObject)
{
    qDebug() << "communication";
    auto adverse_id = paramsObject["user_id"].toString();
    auto adverse_message = paramsObject["message"].toString();
    auto time = paramsObject["time"].toString();
    emit communication(paramsObject);
}
void Client::handle_addFriend(const QJsonObject& paramsObject)
{
    auto adverse_id = paramsObject["user_id"].toString();
    auto adverse_message = paramsObject["message"].toString();
    auto time = paramsObject["time"].toString();
    emit addFriend(paramsObject);
}
void Client::handle_resultOfAddFriend(const QJsonObject& paramsObject)
{
    if (paramsObject["result"].toBool())
    {
        emit agreeAddFriend(paramsObject);
    }
    else
    {
        emit rejectAddFriend(paramsObject);
    }
}