#ifndef CLIENT_H_
#define CLIENT_H_

#include <QWebSocket>
#include <QObject>
#include <functional>
#include <QUrl>

class Client :public QObject
{
	Q_OBJECT
public:
	~Client();
	//初始化映射表
	void initRequestHash();
	static Client* instance();
private:
	Client(QObject* parent = nullptr);
public:
	//链式调用
	Client* connectToServer(const QString& url);
	Client* sendMessage(const QString& type, const QVariantMap& params = {});
	Client* sendBinaryMessage(const QString& type, const QVariantMap& params, const QByteArray& data);
	//事件回调
	Client* ReciveMessage(std::function<void(const QString&)>callback);
	Client* Error(std::function<void(const QString&)>callback);
	Client* Connected(std::function<void()>callback);
	Client* DisconnectFromServer(std::function<void()>callback);

	void disconnect();
private:
	QWebSocket* m_client;
	bool m_isConnected;

	//回调函数
	std::function<void(const QString&)> m_messageCallback;
	std::function<void(const QString&)> m_errorCallback;
	std::function<void()> m_connectedCallback;
	std::function<void()> m_disconnectedCallback;

private slots:
	void onTextMessageReceived(const QString& message);
	void onBinaryMessageReceived(const QByteArray& data);
	void onErrorOccurred(QAbstractSocket::SocketError error);
	void onConnected();
	void onDisconnected();

private:
	//消息处理函数表
	QHash<QString, void(Client::*)(const QJsonObject&, const QByteArray&)>requestHash{};
private:
	//各种消息处理函数
	void handle_communication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_newFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_rejectAddFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
signals:   //接受到消息发送信号通知UI界面更新同步
	void communication(const QJsonObject& paramsObject);
	void addFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void newFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void addGroup(const QJsonObject& paramsObject);
	void rejectAddFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void searchUser(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void updateUserMessage(const QJsonObject& paramsObject);
	void updateUserAvatar(const QString& user_id, const QPixmap& pixmap);
};


#endif // !CLIENT_H_
