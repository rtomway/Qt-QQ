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
	void onBinaryMessageReceived(const QByteArray& data);
	QString findUserName(QWebSocket* client);
	QString generateUserID();
private:
	QWebSocketServer* m_server{};
	//客户端
	QHash<QString, QWebSocket*>m_clients{};
	//消息处理函数表
	QHash<QString, void(Server::*)(const QJsonObject&, const QByteArray&)>requestHash{};
private:
	//各种消息处理函数
	void handle_login(const QJsonObject& paramsObject,const QByteArray& data = QByteArray());
	void handle_register(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	QString getRandomID(int length);
	void handle_communication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_searchGroup(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_addGroup(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_resultOfAddFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_queryUserDetail(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	QStringList getFriendId(const QString& user_id);
	void handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
};

#endif // !SERVER_H_
