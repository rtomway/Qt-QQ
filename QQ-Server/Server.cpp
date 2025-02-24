#include "Server.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "SSqlConnectionPool.h"
#include "SResultCode.h"
#include <QRandomGenerator>

Server::Server(QObject*parent)
	:QObject(parent)
{
	//设定服务器监听端口
	if (!m_server)
	{
		m_server = new QWebSocketServer("QQ-Server",QWebSocketServer::SslMode::NonSecureMode,this);
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
	requestHash["login"] =&Server::handle_login;
	requestHash["register"] = &Server::handle_register;
	requestHash["communication"] = &Server::handle_communication;
	requestHash["searchUser"] = &Server::handle_searchUser;
	requestHash["searchGroup"] = &Server::handle_searchGroup;
	requestHash["addFriend"] = &Server::handle_addFriend;
	requestHash["addGroup"] = &Server::handle_addGroup; 
	requestHash["resultOfAddFriend"] = &Server::handle_resultOfAddFriend;
	requestHash["queryUserDetail"] = &Server::handle_queryUserDetail;
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
		m_clients[client_id] = dynamic_cast<QWebSocket*>(sender());
		qDebug() << "客户端发来消息:" << m_clients[client_id]<<"type:"<<type<< requestHash.contains(type);
		//根据类型给处理函数处理
		if (requestHash.contains(type))
		{
			auto handle=requestHash[type];
			(this->*handle)(paramsObject);
		}
		else
		{

		}

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

QString Server::getRandomID(int length)
{
	QString user_id;
	QRandomGenerator randomID;
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
	//auto username = paramsObject["username"].toString();
	//auto password = paramsObject["password"].toString();
	//注册唯一id
	while (true) {
		//服务器随机生成10位数用户id
		user_id = getRandomID(10);
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

//消息请求处理函数
//登录
void Server::handle_login(const QJsonObject& paramsObject)
{
	auto user_id = paramsObject["user_id"].toString();
	auto password = paramsObject["password"].toString();
	//获取数据库连接
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	//查询数据验证
	query.prepare(QString("select * from user where user_id=?"));
	query.addBindValue(user_id);
	auto client = m_clients[user_id];
	if (query.exec())
	{
		while (query.next())
		{
			auto count = query.record().count();
			qDebug() << "count:" << count;
			for (int i = 0; i < count; i++)
			{
				qDebug() << query.record().fieldName(i);
				qDebug() << query.value(i).toString();
			}
			qDebug() << "password:" << query.value(6).toString();
			if (password == query.value(7).toString())
			{
				qDebug() << "登陆成功";
				QJsonObject jsonData;
				jsonData["user_id"]= query.value(1).toString();
				jsonData["username"] = query.value(2).toString();
				client->sendTextMessage(SResult::success(jsonData));
				
			}
			else
			{
				client->sendTextMessage(SResult::failure(SResultCode::UserLoginError));
			}
		}
	}
	else
	{
		qDebug() << query.lastError();
	}
}
//注册
void Server::handle_register(const QJsonObject& paramsObject)
{
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	QString user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id];
	QString username = paramsObject["username"].toString();
	QString password = paramsObject["password"].toString();
	query.prepare("insert into user (user_id,username,password)values(?,?,?)");
	query.addBindValue(user_id);
	query.addBindValue(username);
	query.addBindValue(password);
	if (!query.exec())
	{
		qDebug() << query.lastError();
		return;
	}
	//注册成功后返回该用户账号与密码
	client->sendTextMessage(SResult::success(paramsObject));
}

//通信转发
void Server::handle_communication(const QJsonObject& paramsObject)
{
	qDebug()<<"发送方:"<< paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	QJsonObject jsondate;
	jsondate["type"] = "communication";
	jsondate["params"] = paramsObject;
	auto receive_id = paramsObject["to"].toString();
	auto client = m_clients[receive_id];

	QJsonDocument doc(jsondate);
	QString data = QString(doc.toJson(QJsonDocument::Compact));
	client->sendTextMessage(data);
}
//搜索用户
void Server::handle_searchUser(const QJsonObject& paramsObject)
{
	qDebug() << paramsObject;
	auto user_id = paramsObject["user_id"].toString();
	auto client = m_clients[user_id];
	auto search_id = paramsObject["search_id"].toString();
	SConnectionWrap wrap;
	QSqlQuery query(wrap.openConnection());
	auto user = "%" + search_id + "%";
	query.prepare("select user_id,username,avatar_path from user where user_id like ?");
	query.addBindValue(user);
	if (query.exec())
	{
		if (query.next())
		{
			qDebug() << "用户存在";
			QJsonArray allData;
				do {
					auto count = query.record().count();
					qDebug() << "count:" << count;
					QJsonObject userData;
					for (int i = 0; i < count; i++)
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
void Server::handle_searchGroup(const QJsonObject& paramsObject)
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
void Server::handle_addFriend(const QJsonObject& paramsObject)
{
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	QJsonObject jsondate;
	jsondate["type"] = "addFriend";
	jsondate["params"] = paramsObject;
	auto receive_id = paramsObject["to"].toString();
	auto receive_message = paramsObject["message"].toString(); 
	auto client = m_clients[receive_id];
	QJsonDocument doc(jsondate);
	QString data = QString(doc.toJson(QJsonDocument::Compact));
	client->sendTextMessage(data);
}
//添加群组
void Server::handle_addGroup(const QJsonObject& paramsObject)
{

}
//拒绝添加
void Server::handle_resultOfAddFriend(const QJsonObject& paramsObject)
{
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	QJsonObject jsondate;
	jsondate["type"] = "resultOfAddFriend";
	jsondate["params"] = paramsObject;
	auto receive_id = paramsObject["to"].toString();
	auto client = m_clients[receive_id];
	QJsonDocument doc(jsondate);
	QString data = QString(doc.toJson(QJsonDocument::Compact));
	client->sendTextMessage(data);
}
//查询用户信息
void Server::handle_queryUserDetail(const QJsonObject& paramsObject)
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
				qDebug()<< query.value(i).toString();
			}
			obj["user_id"] = query.value(1).toString();
			obj["username"] = query.value(2).toString();
			obj["gender"] = query.value(3).toBool();
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
