#include "Server.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "SSqlConnectionPool.h"
#include "SResultCode.h"
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QDir>
#include <QBuffer>

Server::Server(QObject* parent)
	:QObject(parent)
{
	//设定服务器监听端口
	if (!m_server)
	{
		m_server = new QWebSocketServer("QQ-Server", QWebSocketServer::SslMode::NonSecureMode, this);
		connect(m_server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
	}
	m_server->listen(QHostAddress::Any, 8888);
	initRequestHash();
}

Server::~Server()
{
	m_server->close();
	qDeleteAll(m_clients);
}

//初始化消息处理映射表
void Server::initRequestHash()
{
	requestHash["login"] = &Server::handle_login;
	requestHash["register"] = &Server::handle_register;
	requestHash["communication"] = &Server::handle_communication;
	requestHash["searchUser"] = &Server::handle_searchUser;
	requestHash["searchGroup"] = &Server::handle_searchGroup;
	requestHash["addFriend"] = &Server::handle_addFriend;
	requestHash["addGroup"] = &Server::handle_addGroup;
	requestHash["resultOfAddFriend"] = &Server::handle_resultOfAddFriend;
	requestHash["queryUserDetail"] = &Server::handle_queryUserDetail;
	requestHash["updateUserMessage"] = &Server::handle_updateUserMessage;
	requestHash["updateUserAvatar"] = &Server::handle_updateUserAvatar;
}
//通信连接
void Server::onNewConnection()
{
	while (m_server->hasPendingConnections())
	{
		//请求连接的客户端
		auto client = m_server->nextPendingConnection();
		qDebug() << client;
		connect(client, &QWebSocket::disconnected, this, &Server::onDisConnection);
		connect(client, &QWebSocket::textMessageReceived, this, &Server::onTextMessageReceived);
		connect(client, &QWebSocket::binaryMessageReceived, this, &Server::onBinaryMessageReceived);
	}
}
void Server::onDisConnection()
{
	auto client = dynamic_cast<QWebSocket*>(sender());
	if (client)
	{
		auto username = findUserName(client);
		m_clients.remove(username);
		qDebug() << "客户端" << username << "断开";
	}
}
//接受消息
void Server::onTextMessageReceived(const QString& data)
{
	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject())
	{
		QJsonObject obj = doc.object();
		QString client_id;
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		if (paramsObject["user_id"].toString().isEmpty())
		{
			//注册时没有user_id服务器产生
			client_id = generateUserID();
			paramsObject["user_id"] = client_id;
		}
		else
		{
			client_id = paramsObject["user_id"].toString();
		}
		QByteArray data;
		m_clients[client_id] = dynamic_cast<QWebSocket*>(sender());
		qDebug() << "客户端发来消息:" << m_clients[client_id] << "type:" << type << requestHash.contains(type);
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
}
void Server::onBinaryMessageReceived(const QByteArray& message)
{
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
	qDebug() << "客户端发来消息:" << "type:" << type << requestHash.contains(type);
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
//判断用户是否已存在
QString Server::findUserName(QWebSocket* client)
{
	for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (it.value() == client)
		{
			return it.key();
		}
	}
	return QString("user not exist!");
}
//用户id生成
QString Server::getRandomID(int length)
{
	QString user_id;
	QRandomGenerator randomID = QRandomGenerator::securelySeeded(); // 手动初始化种子;
	while (user_id.size() < length)
	{
		int ram = randomID.bounded(0, 9);
		user_id.append(QString::number(ram));
	}
	qDebug() << "RandomID:" << user_id;
	return user_id;
}
QString Server::generateUserID()
{
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	QString user_id;
	//注册唯一id
	while (true) {
		//服务器随机生成10位数用户id
		user_id = getRandomID(10);
		qDebug() << "RandomID后的:" << user_id;
		//先查询生成id是否已存在
		query.prepare("select user_id from user where user_id=? ");
		query.addBindValue(user_id);
		if (!query.exec())
		{
			qDebug() << query.lastError();
			return nullptr;
		}
		//未找到，id唯一跳出
		if (!query.next())
		{
			break;
		}
	}
	return user_id;
}
//查询好友id
QStringList Server::getFriendId(const QString& user_id)
{
	QStringList friendIdList;
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());

	query.prepare("select friend_id from friendship where user_id=?");
	query.addBindValue(user_id);
	if (query.exec())
	{
		while (query.next())
		{
			auto friend_id = query.value(0).toString();
			qDebug() << "好友id:" << friend_id;
			friendIdList.append(friend_id);
		}
	}
	else
	{
		qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
		qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
		return QStringList();
	}
	return friendIdList;
}
//保存图片
bool Server::saveImage(const QString& user_id, const QImage& image)
{
	//保存目录
	QString avatarFolder = QStandardPaths::writableLocation
	(QStandardPaths::AppDataLocation)+"/avatars";
	// 如果目录不存在，则创建
	QDir dir;
	if (!dir.exists(avatarFolder)) {
		dir.mkpath(avatarFolder);
	}
	qDebug() << "avatarFolder:" << avatarFolder;
	auto avatar_Path = avatarFolder + "/" + user_id + ".png";
	if (!image.save(avatar_Path))
	{
		qWarning() << "Failed to save avatar for user:" << user_id;
		return false;
	}
	return true;
}
//加载图片
QByteArray Server::loadImage(const QString& user_id)
{
	// 确定保存目录
	QString avatarFolder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/avatars";
	QDir().mkpath(avatarFolder);  // 确保目录存在
	auto avatarPath = avatarFolder + "/" + user_id + ".png";
	qDebug() << "Attempting to load image from:" << avatarPath;
	// 加载图片
	QFile file(avatarPath);
	if (!file.exists()) {
		qDebug() << "File does not exist:" << avatarPath;

		QImage image(":/picture/Resource/Picture/qq.png");
		// 创建一个 QByteArray 来保存图像数据
		QByteArray byteArray;

		// 将 QImage 保存到 QByteArray，指定格式为 PNG（你也可以选择其他格式）
		QBuffer buffer(&byteArray);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG"); // 这里可以更换为 "JPG", "BMP" 等格式
		return byteArray;
	}

	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Could not open the file:" << avatarPath << "Error:" << file.errorString();
		return QByteArray();
	}

	if (file.size() == 0) {
		qDebug() << "File is empty:" << avatarPath;
		return QByteArray();
	}

	QByteArray imageData = file.readAll();  // 读取图片数据

	if (imageData.isEmpty()) {
		qDebug() << "Failed to read image data from:" << avatarPath;
	}
	else {
		qDebug() << "Successfully loaded image from:" << avatarPath << "Size:" << imageData.size();
	}

	return imageData;
}
//所有数据包
QByteArray Server::allBinaryPacket(const QByteArray& packet)
{
	QByteArray allData;
	qint32 totalSize = sizeof(qint32) + packet.size(); // 计算总大小
	qDebug() << "Server sending total size:" << totalSize;

	QDataStream stream(&allData, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_6_5);
	stream.setByteOrder(QDataStream::BigEndian); // 确保服务器和客户端一致
	stream << totalSize; // 先写入总大小
	stream.writeRawData(packet.constData(), packet.size()); // 再写入用户数据
	return allData;
}
//用户个人信息
QVariantMap Server::getUserMessage(const QString& user_id)
{
	//查询用户信息
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	query.prepare("select * from user where user_id=?");
	query.addBindValue(user_id);
	qDebug() << "-------------" << query.exec();
	if (query.exec())
	{
		QVariantMap obj;
		while (query.next())
		{
			for (size_t i = 0; i < 8; i++)
			{
				qDebug() << query.value(i).toString();
			}
			obj["user_id"] = query.value(1).toString();
			obj["username"] = query.value(2).toString();
			obj["gender"] = query.value(3).toInt();
			obj["age"] = query.value(4).toInt();
			obj["phone_number"] = query.value(5).toString();
			obj["email"] = query.value(6).toString();
			obj["avatar_path"] = query.value(8).toString();
			obj["birthday"] = query.value(9).toDate().toString("MM-dd");
			obj["signature"] = query.value(10).toString();
		}
		return obj;
	}
	else
	{
		qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
		qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
		return QVariantMap();
	}
}
//打包二进制数据
QByteArray Server::binaryPacket(const QString& type, const QVariantMap& params, const QByteArray& data)
{

	// 1️⃣ 构造 JSON 头部（metadata）
	QJsonObject jsonData;
	jsonData["type"] = type;

	QJsonObject paramsObject;
	for (auto it = params.begin(); it != params.end(); ++it) {
		paramsObject[it.key()] = QJsonValue::fromVariant(it.value());
	}
	jsonData["params"] = paramsObject;
	qDebug() << "params:" << jsonData;
	QJsonDocument doc(jsonData);
	QByteArray headerData = doc.toJson(QJsonDocument::Compact);

	// 2️⃣ 组合数据包（头部长度 + 头部 JSON + 二进制数据）
	QByteArray packet;
	QDataStream stream(&packet, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_6_5);
	stream.setByteOrder(QDataStream::BigEndian);

	// 先写入 JSON 头部大小
	stream << qint32(headerData.size());

	// 再写入 JSON 头部
	stream.writeRawData(headerData.constData(), headerData.size());

	// 最后写入二进制数据
	stream.writeRawData(data.constData(), data.size());

	return packet;
}

//消息请求处理函数
//登录
void Server::handle_login(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	auto password = paramsObject["password"].toString();
	//获取数据库连接
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	//查询数据验证
	QJsonObject obj; //存放返回客户端的所有信息
	QJsonArray jarray;
	auto client = m_clients[user_id];
	//查询登录用户
	query.prepare(QString("select f.Fgrouping,u.* from user u join friendship f on u.user_id=f.user_id where u.user_id=?"));
	query.addBindValue(user_id);
	if (query.exec())
	{
		while (query.next())
		{
			auto count = query.record().count();
			qDebug() << "count:" << count;
			for (int i = 0; i < count; i++)
			{
				qDebug() << query.record().fieldName(i);
				qDebug() << i << query.value(i).toString();
			}
			qDebug() << "password:" << query.value(8).toString();
			if (password == query.value(8).toString())
			{
				qDebug() << "登陆成功";
				QJsonObject jsonData;
				jsonData["grouping"] = query.value(0).toString();
				jsonData["user_id"] = query.value(2).toString();
				jsonData["username"] = query.value(3).toString();
				jsonData["gender"] = query.value(4).toInt();
				jsonData["age"] = query.value(5).toInt();
				jsonData["phone_number"] = query.value(6).toString();
				jsonData["email"] = query.value(7).toString();
				jsonData["avatar_path"] = query.value(9).toString();
				jsonData["birthday"] = query.value(10).toDate().toString("MM-dd");
				jsonData["signature"] = query.value(11).toString();
				obj["loginUser"] = jsonData;
			}
			else
			{
				client->sendTextMessage(SResult::failure(SResultCode::UserLoginError));
				return;
			}
		}
	}
	else
	{
		qDebug() << query.lastError();
		return;
	}
	//查询好友
	query.prepare(
		QString("select f.friend_id,f.Fgrouping,user.* from friendship f \
			join user ON f.friend_id = user.user_id \
	where f.user_id=? and f.friend_id!=f.user_id"));
	query.addBindValue(user_id);
	if (query.exec())
	{
		while (query.next())
		{
			auto count = query.record().count();
			qDebug() << "count:" << count;
			for (int i = 0; i < count; i++)
			{
				qDebug() << query.record().fieldName(i);
				qDebug() << i << query.value(i).toString();
			}
			QJsonObject jsonData;
			jsonData["user_id"] = query.value(0).toString();
			jsonData["grouping"] = query.value(1).toString();
			jsonData["username"] = query.value(4).toString();
			jsonData["gender"] = query.value(5).toInt();
			jsonData["age"] = query.value(6).toInt();
			jsonData["phone_number"] = query.value(7).toString();
			jsonData["email"] = query.value(8).toString();
			jsonData["avatar_path"] = query.value(10).toString();
			jsonData["birthday"] = query.value(11).toDate().toString("MM-dd");
			jsonData["signature"] = query.value(12).toString();
			jarray.append(jsonData);
			qDebug() << "好友列表：" << jsonData;
		}
		obj["friendArray"] = jarray;
		client->sendTextMessage(SResult::success(obj));
	}
	else
	{
		qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
		qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
		return;
	}
}
//注册
void Server::handle_register(const QJsonObject& paramsObject, const QByteArray& data)
{
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	QString user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id];
	QString username = paramsObject["username"].toString();
	QString password = paramsObject["password"].toString();
	//用户表
	query.prepare("insert into user (user_id,username,password)values(?,?,?)");
	query.addBindValue(user_id);
	query.addBindValue(username);
	query.addBindValue(password);
	if (!query.exec())
	{
		qDebug() << query.lastError();
		return;
	}

	//好友表 自己和自己是好友
	query.prepare("insert into friendship (user_id,friend_id,create_time,Fgrouping)values(?,?,?,?)");
	query.addBindValue(user_id);
	auto friend_id = user_id;
	query.addBindValue(friend_id);
	query.addBindValue(QDateTime::currentDateTime());
	query.addBindValue("好友");
	if (!query.exec())
	{
		qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
		qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
		return;
	}

	//注册成功后返回该用户账号与密码
	client->sendTextMessage(SResult::success(paramsObject));

}
//通信转发
void Server::handle_communication(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	QJsonObject jsondate;
	jsondate["type"] = "communication";
	jsondate["params"] = paramsObject;
	auto receive_id = paramsObject["to"].toString();
	auto client = m_clients[receive_id];

	QJsonDocument doc(jsondate);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	client->sendTextMessage(message);
}
//搜索用户
void Server::handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << paramsObject;
	auto user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id];
	auto search_id = paramsObject["search_id"].toString();
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	auto user = "%" + search_id + "%";
	query.prepare("select user_id,username from user where user_id like ?");
	query.addBindValue(user);
	QByteArray userData;

	if (query.exec())
	{
		if (query.next())
		{
			qDebug() << "用户存在";
			do {
				QVariantMap userMap;
				auto user_id = query.value(0).toString();
				auto username = query.value(1).toString();
				userMap["user_id"] = user_id;
				userMap["username"] = username;
				QByteArray image = loadImage(user_id);
				userMap["size"] = image.size();
				auto userPacket = binaryPacket("searchUser", userMap, image);
				qint32 packetSize = userPacket.size();
				QByteArray packetSizeBytes;
				QDataStream sizeStream(&packetSizeBytes, QIODevice::WriteOnly);
				sizeStream.setVersion(QDataStream::Qt_6_5);
				sizeStream.setByteOrder(QDataStream::BigEndian);
				sizeStream << packetSize;

				userData.append(packetSizeBytes);  // 先加上包长度
				userData.append(userPacket);       // 再加上包数据
			} while (query.next());

			auto allData = allBinaryPacket(userData);
			client->sendBinaryMessage(allData);
		}
		else
		{
			qDebug() << "用户不存在";
			client->sendTextMessage(SResult::failure(SResultCode::UserUnExists));
		}
	}
	else
	{
		qDebug() << query.lastError();
	}

}
//搜索群组
void Server::handle_searchGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << paramsObject;
	auto user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id];
	auto search_id = paramsObject["search_id"].toString();

	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	auto group = "%" + search_id + "%";
	query.prepare("select group_id,group_name,group_avatar from `group` where group_id like ?");
	query.addBindValue(group);
	if (query.exec())
	{
		if (query.next())
		{
			qDebug() << "群聊存在";
			QJsonArray allData;
			do {
				auto count = query.record().count();
				qDebug() << "count:" << count;
				QJsonObject userData;
				for (size_t i = 0; i < count; i++)
				{
					qDebug() << query.record().fieldName(i);
					qDebug() << query.value(i).toString();
					userData[query.record().fieldName(i)] = query.value(i).toString();
				}
				allData.append(userData);
			} while (query.next());
			QJsonObject jsonResponse;
			//jsonResponse[""] = allData;
			jsonResponse["search_data"] = allData;
			client->sendTextMessage(SResult::success(jsonResponse));
		}
		else
		{
			qDebug() << "群聊不存在";
			client->sendTextMessage(SResult::failure(SResultCode::GroupUnExists));
		}
	}
	else
	{
		qDebug() << query.lastError();
	}

}
//添加好友
void Server::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "添加好友";
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	//保存分组信息
	m_sendGrouping= paramsObject["grouping"].toString();
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage["user_id"] = send_id;
	senderMessage["message"] = paramsObject["message"].toString();
	senderMessage["username"] = paramsObject["username"].toString();
	senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	senderMessage["addFriend"] = "请求加为好友";
	auto imageData = loadImage(send_id);
	senderMessage["size"] = imageData.size();
	//包装数据包
	auto userPacket = binaryPacket("addFriend", senderMessage, imageData);
	qint32 packetSize = userPacket.size();
	QByteArray packetSizeBytes;
	QDataStream sizeStream(&packetSizeBytes, QIODevice::WriteOnly);
	sizeStream.setVersion(QDataStream::Qt_6_5);
	sizeStream.setByteOrder(QDataStream::BigEndian);
	sizeStream << packetSize;
	QByteArray userData;
	userData.append(packetSizeBytes);  // 先加上包长度
	userData.append(userPacket);  // 再加上包数据
	auto allData = allBinaryPacket(userData);
	//发送数据
	auto client = m_clients[receive_id];
	client->sendBinaryMessage(allData);
	qDebug() << "发送了申请信息";
}
//添加群组
void Server::handle_addGroup(const QJsonObject& paramsObject, const QByteArray& data)
{

}
//好友添加结果
void Server::handle_resultOfAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "好友添加结果";
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	auto result = paramsObject["result"].toBool();
	if (result)
	{
		auto client_receive = m_clients[receive_id];
		auto client_send = m_clients[send_id];
		//保存分组信息
		m_receiveGrouping= paramsObject["grouping"].toString();
		//好友列表新增
		SConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare("insert into friendship (user_id,friend_id,Fgrouping)values(?,?,?),(?,?,?)");
		query.addBindValue(send_id);
		query.addBindValue(receive_id);
		query.addBindValue(m_receiveGrouping);
		query.addBindValue(receive_id);
		query.addBindValue(send_id);
		query.addBindValue(m_sendGrouping);
		if (!query.exec())
		{
			qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
			qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
			return;
		}
		//通知两边客户端
		QVariantMap receiveMap = getUserMessage(receive_id);
		receiveMap["grouping"] = m_sendGrouping;
		QVariantMap sendMap = getUserMessage(send_id);
		sendMap["grouping"] = m_receiveGrouping;
		auto reveiveImage = loadImage(receive_id);
		auto sendImage = loadImage(send_id);
		receiveMap["size"] = reveiveImage.size();
		sendMap["size"] = sendImage.size();
		//打包
		auto receivePacket = binaryPacket("newFriend", receiveMap, reveiveImage);
		QByteArray receiveData;
		qint32 receivepacketSize = receivePacket.size();
		QByteArray receivepacketSizeBytes;
		QDataStream receivesizeStream(&receivepacketSizeBytes, QIODevice::WriteOnly);
		receivesizeStream.setVersion(QDataStream::Qt_6_5);
		receivesizeStream.setByteOrder(QDataStream::BigEndian);
		receivesizeStream << receivepacketSize;
		receiveData.append(receivepacketSizeBytes);  // 先加上包长度
		receiveData.append(receivePacket);       // 再加上包数据

		auto sendPacket = binaryPacket("newFriend", sendMap, sendImage);
		QByteArray sendData;
		qint32 sendpacketSize = sendPacket.size();
		QByteArray sendpacketSizeBytes;
		QDataStream sendsizeStream(&sendpacketSizeBytes, QIODevice::WriteOnly);
		sendsizeStream.setVersion(QDataStream::Qt_6_5);
		sendsizeStream.setByteOrder(QDataStream::BigEndian);
		sendsizeStream << sendpacketSize;
		sendData.append(sendpacketSizeBytes);  // 先加上包长度
		sendData.append(sendPacket);       // 再加上包数据
		auto receiveAllData = allBinaryPacket(receiveData);
		auto sendAllData = allBinaryPacket(sendData);
		//发送
		client_receive->sendBinaryMessage(sendAllData);
		client_send->sendBinaryMessage(receiveAllData);
	}
	else
	{
		//数据包装入信息
		QVariantMap senderMessage;
		senderMessage["user_id"] = send_id;
		senderMessage["username"] = paramsObject["username"].toString();
		senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
		senderMessage["addFriend"] = "拒绝了你的好友申请";
		auto imageData = loadImage(send_id);
		senderMessage["size"] = imageData.size();
		//包装数据包
		auto userPacket = binaryPacket("rejectAddFriend", senderMessage, imageData);
		qint32 packetSize = userPacket.size();
		QByteArray packetSizeBytes;
		QDataStream sizeStream(&packetSizeBytes, QIODevice::WriteOnly);
		sizeStream.setVersion(QDataStream::Qt_6_5);
		sizeStream.setByteOrder(QDataStream::BigEndian);
		sizeStream << packetSize;
		QByteArray userData;
		userData.append(packetSizeBytes);  // 先加上包长度
		userData.append(userPacket);  // 再加上包数据
		auto allData = allBinaryPacket(userData);
		//发送数据
		auto client = m_clients[receive_id];
		client->sendBinaryMessage(allData);
	}

}
//查询用户信息
void Server::handle_queryUserDetail(const QJsonObject& paramsObject, const QByteArray& data)
{
	//客户端
	auto user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id]; qDebug() << "&&&&&&&&&" << paramsObject;
	//查询用户信息
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	auto query_id = paramsObject["query_id"].toString();
	query.prepare("select * from user where user_id=?");
	query.addBindValue(query_id);
	qDebug() << "-------------" << query.exec();
	if (query.exec())
	{
		QJsonObject obj;
		while (query.next())
		{
			for (size_t i = 0; i < 8; i++)
			{
				qDebug() << query.value(i).toString();
			}
			obj["user_id"] = query.value(1).toString();
			obj["username"] = query.value(2).toString();
			obj["gender"] = query.value(3).toInt();
			obj["age"] = query.value(4).toInt();
			obj["phone_number"] = query.value(5).toString();
			obj["email"] = query.value(6).toString();
			obj["avatar"] = query.value(8).toString();
		}
		client->sendTextMessage(SResult::success(obj));
	}
	else
	{
		qDebug() << query.lastError();
	}

}
//更新用户信息
void Server::handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data)
{
	//客户端
	auto user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id];

	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());

	auto username = paramsObject["username"].toString();
	auto gender = paramsObject["gender"].toInt();
	auto age = paramsObject["age"].toInt();
	auto phone_number = paramsObject["phone_number"].toString();
	auto email = paramsObject["email"].toString();
	auto birthday = QDate::fromString(paramsObject["birthday"].toString(), "yyyy-MM-dd");
	auto signature = paramsObject["signature"].toString();

	phone_number = phone_number.isEmpty() ? QVariant(QVariant::String).toString() : phone_number;
	email = email.isEmpty() ? QVariant(QVariant::String).toString() : email;
	signature = signature.isEmpty() ? QVariant(QVariant::String).toString() : signature;

	QJsonObject jsondata;
	QString dataObj;

	query.prepare("UPDATE user SET username=?,\
		gender = ?, age =?, phone_number = ? , \
		email = ? , birthday = ? , signature = ? \
		WHERE user_id = ? ");
	query.addBindValue(username);
	query.addBindValue(gender);
	query.addBindValue(age);
	query.addBindValue(phone_number);
	query.addBindValue(email);
	query.addBindValue(birthday);
	query.addBindValue(signature);
	query.addBindValue(user_id);
	if (!query.exec())
	{
		qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
		qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
		return;
	}
	else
	{
		jsondata["type"] = "updateUserMessage";
		jsondata["params"] = paramsObject;
		QJsonDocument doc(jsondata);
		dataObj = QString(doc.toJson(QJsonDocument::Compact));
	}
	QStringList friendIdList = getFriendId(user_id);
	for (auto friend_id : friendIdList)
	{
		if (m_clients.contains(friend_id))
		{
			m_clients[friend_id]->sendTextMessage(dataObj);
		}
	}

}
//更新用户头像
void Server::handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	//更新数据库
	auto user_id = paramsObject["user_id"].toString();
	auto avatarPath = user_id + ".png";
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	query.prepare("update user set avatar_path=? where user_id=?");
	query.addBindValue(avatarPath);
	query.addBindValue(user_id);
	if (!query.exec())
	{
		qDebug() << "SQL Query: " << query.lastQuery();  // 输出执行的 SQL 语句
		qDebug() << "SQL Error: " << query.lastError().text();  // 获取错误信息
		return;
	}
	//将头像存储到服务器中
	QImage image;
	if (!image.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}
	saveImage(user_id, image);

	//转发头像信息
	QByteArray userData;
	auto userPacket = binaryPacket("updateUserAvatar", paramsObject.toVariantMap(), data);
	qint32 packetSize = userPacket.size();
	QByteArray packetSizeBytes;
	QDataStream sizeStream(&packetSizeBytes, QIODevice::WriteOnly);
	sizeStream.setVersion(QDataStream::Qt_6_5);
	sizeStream.setByteOrder(QDataStream::BigEndian);
	sizeStream << packetSize;

	userData.append(packetSizeBytes);  // 先加上包长度
	userData.append(userPacket);       // 再加上包数据
	auto allData= allBinaryPacket(userData);

	QStringList friendIdList = getFriendId(user_id);
	for (const auto& friend_id : friendIdList)
	{
		if (m_clients.contains(friend_id))
		{
			m_clients[friend_id]->sendBinaryMessage(allData);
		}
	}
}

