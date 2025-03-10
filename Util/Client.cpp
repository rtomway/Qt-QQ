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
	requestHash["searchUser"] = &Client::handle_searchUser;
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
Client* Client::sendMessage(const QString& type, const QVariantMap& params)
{
	if (m_isConnected)
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
		m_client->sendTextMessage(message);
	}
	else
	{
		qWarning() << "数据发送失败 连接已断开......";
	}
	return this;
}
//发送二进制数据
Client* Client::sendBinaryMessage(const QString& type, const QVariantMap& params, const QByteArray& data)
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
//接受文本信息
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
		qDebug() << type << requestHash.contains(type);
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
//接受二进制数据
void Client::onBinaryMessageReceived(const QByteArray& message)
{
	qDebug() << message;
	QDataStream stream(message);
	stream.setByteOrder(QDataStream::BigEndian);
	stream.setVersion(QDataStream::Qt_6_5);

	// 1️⃣ 读取 `allData` 的总大小（4 字节）
	if (message.size() < sizeof(qint32)) {
		qDebug() << "Incomplete packet: waiting for more data...";
		return;
	}
	qint32 totalSize;
	stream >> totalSize;

	qDebug() << "Total data size:" << totalSize;

	// 2️⃣ 开始循环解析多个数据包
	while (!stream.atEnd()) {
		if (message.size() - stream.device()->pos() < sizeof(qint32)) {
			qDebug() << "Incomplete packet header size!";
			return;
		}

		// 读取当前数据包的大小
		qint32 packetSize;
		stream >> packetSize;

		if (message.size() - stream.device()->pos() < packetSize) {
			qDebug() << "Incomplete full packet!";
			return;
		}

		// 读取 JSON 头部大小
		qint32 headerSize;
		stream >> headerSize;

		// 读取 JSON 头部
		QByteArray headerData(headerSize, Qt::Uninitialized);
		stream.readRawData(headerData.data(), headerSize);

		// 解析 JSON
		QJsonDocument jsonDoc = QJsonDocument::fromJson(headerData);
		if (jsonDoc.isNull()) {
			qDebug() << "Invalid JSON data!";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();
		QString type = jsonObj["type"].toString();
		QJsonObject params = jsonObj["params"].toObject();

		qDebug() << "Received packet type:" << type;
		qDebug() << "Params:" << params;

		// 读取二进制数据（图片）
		int imageSize = params["size"].toInt();
		QByteArray imageData(imageSize, Qt::Uninitialized);
		stream.readRawData(imageData.data(), imageSize);


		// 根据类型给处理函数处理
		if (requestHash.contains(type)) {
			auto handle = requestHash[type];
			(this->*handle)(params, imageData);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << type;
		}
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
	qDebug() << "接受到申请信息";
	auto adverse_id = paramsObject["user_id"].toString();
	auto adverse_message = paramsObject["message"].toString();
	auto time = paramsObject["time"].toString();
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	else if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	emit addFriend(paramsObject, pixmap);
}
void Client::handle_resultOfAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	if (paramsObject["result"].toBool())
	{
		emit agreeAddFriend(paramsObject);
	}
	else
	{
		emit rejectAddFriend(paramsObject, pixmap);
	}
}
void Client::handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar";
		avatar = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	emit searchUser(paramsObject, avatar);
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