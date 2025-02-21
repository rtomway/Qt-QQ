#ifndef SERVER_H_
#define  SERVER_H_

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QHash>

class Server :public QObject
{
	Q_OBJECT
public:
	explicit Server(QObject* parent = nullptr);
	~Server();
	void initRequestHash();
protected:
	void onNewConnection();
	void onDisConnection();
	void onTextMessageReceived(const QString& data);
	QString findUserName(QWebSocket* client);
private:
	QWebSocketServer* m_server{};
	//客户端
	QHash<QString, QWebSocket*>m_clients{};
	//消息处理函数表
	QHash<QString, void(Server::*)(const QJsonObject&)>requestHash{};
private:
	//各种消息处理函数
	void handle_login(const QJsonObject& paramsObject);
	void handle_register(const QJsonObject& paramsObject);
	void handle_communication(const QJsonObject& paramsObject);
	void handle_searchUser(const QJsonObject& paramsObject);
	void handle_searchGroup(const QJsonObject& paramsObject);
	void handle_addFriend(const QJsonObject& paramsObject);
	void handle_addGroup(const QJsonObject& paramsObject);
	void handle_resultOfAddFriend(const QJsonObject& paramsObject);
	void handle_queryUserDetail(const QJsonObject& paramsObject);
};

#endif // !SERVER_H_
