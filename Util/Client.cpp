#include "Client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <Qpixmap>


Client::Client(QObject* parent)
    :QObject(parent), m_client(new QWebSocket()), m_isConnected(false)
{
    //客户端接受信号
    connect(m_client, &QWebSocket::textMessageReceived, this, &Client::onTextMessageReceived);
    connect(m_client, &QWebSocket::binaryMessageReceived, this, &Client::onBinaryMessageReceived);
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
    requestHash["updateUserMessage"] = &Client::handle_updateUserMessage;
    requestHash["updateUserAvatar"] = &Client::handle_updateUserAvatar;
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
//发送二进制数据
Client* Client::sendBinaryMessage(const QString& type,const QVariantMap& params,const QByteArray& data)
{
    // 1️⃣ 构造 JSON 头部（metadata）
    QJsonObject jsonData;
    jsonData["type"] = type;

    QJsonObject paramsObject;
    for (auto it = params.begin(); it != params.end(); ++it) {
        paramsObject[it.key()] = QJsonValue::fromVariant(it.value());
    }
    jsonData["params"] = paramsObject;

    QJsonDocument doc(jsonData);
    QByteArray headerData = doc.toJson(QJsonDocument::Compact);

    // 2️⃣ 组合数据包（头部长度 + 头部 JSON + 二进制数据）
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_5);

    // 先写入 JSON 头部大小
    stream << qint32(headerData.size());

    // 再写入 JSON 头部
    stream.writeRawData(headerData.constData(), headerData.size());

    // 最后写入二进制数据
    stream.writeRawData(data.constData(), data.size());

    // 3️⃣ 发送数据包
    m_client->sendBinaryMessage(packet);

    qDebug() << "发送二进制数据：" << type << "，头部大小：" << headerData.size() << "，数据大小：" << data.size();
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
        QByteArray data;
        if (requestHash.contains(type))
        {
            auto handle = requestHash[type];
            (this->*handle)(paramsObject, data);
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
void Client::onBinaryMessageReceived(const QByteArray& message)
{
    qDebug() << "接受到服务端的消息:二进制";
    QDataStream stream(message);
    stream.setVersion(QDataStream::Qt_6_5);

    // 读取 JSON 头部的大小
    qint32 headerSize;
    stream >> headerSize;  // 从数据流中读取头部大小

    // 读取头部数据（即 JSON）
    QByteArray headerData(headerSize, Qt::Uninitialized);
    stream.readRawData(headerData.data(), headerSize);  // 读取头部的内容

    // 将头部数据转换为 QJsonObject
    QJsonDocument doc = QJsonDocument::fromJson(headerData);
    QJsonObject json = doc.object();

    // 获取 'type' 和 'params'
    QString type = json["type"].toString();
    QJsonObject paramsObject = json["params"].toObject();
    // 读取二进制数据（即文件、图片等）
    qint32 dataSize = paramsObject["size"].toInt();  // 获取二进制数据的大小
    QByteArray data(dataSize, Qt::Uninitialized);
    stream.readRawData(data.data(), dataSize);  // 读取二进制数据
    //根据类型给处理函数处理
    if (requestHash.contains(type))
    {
        auto handle = requestHash[type];
        (this->*handle)(paramsObject, data);
    }
    else
    {

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

void Client::handle_communication(const QJsonObject& paramsObject, const QByteArray& data)
{
    qDebug() << "communication";
    auto adverse_id = paramsObject["user_id"].toString();
    auto adverse_message = paramsObject["message"].toString();
    auto time = paramsObject["time"].toString();
    emit communication(paramsObject);
}
void Client::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
    auto adverse_id = paramsObject["user_id"].toString();
    auto adverse_message = paramsObject["message"].toString();
    auto time = paramsObject["time"].toString();
    emit addFriend(paramsObject);
}
void Client::handle_resultOfAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
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
void Client::handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data)
{
    emit updateUserMessage(paramsObject);
}
void Client::handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
    auto user_id = paramsObject["user_id"].toString();
    qDebug() << "----------C好友头像更新-----------";
    // 1️⃣ 把 QByteArray 转换成 QPixmap
    QPixmap avatar;
    if (!avatar.loadFromData(data))  // 从二进制数据加载图片
    {
        qWarning() << "Failed to load avatar for user:" << user_id;
        return;
    }
   
    emit updateUserAvatar(user_id, avatar);
}