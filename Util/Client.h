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
	Client* connectToServer(const QString&url);
	Client* sendMessage(const QString& type,const QVariantMap& params = {});
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
	void onErrorOccurred(QAbstractSocket::SocketError error);
	void onConnected();
	void onDisconnected();

private:
	//消息处理函数表
	QHash<QString, void(Client::*)(const QJsonObject&)>requestHash{};
private:
	//各种消息处理函数
	void handle_communication(const QJsonObject& paramsObject);
signals:   //接受到消息发送信号通知UI界面更新同步
	void communication(const QJsonObject& paramsObject);

};


#endif // !CLIENT_H_
