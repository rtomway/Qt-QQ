#ifndef MESSAGEHANDLE_H_
#define MESSAGEHANDLE_H_

#include <QByteArray>
#include <QJsonObject>
#include <QObject>

#include "Client_MessageHandle/Client_LoginHandle.h"
#include "Client_MessageHandle/Client_FriendHandle.h"
#include "Client_MessageHandle/Client_GroupHandle.h"
#include "Client_MessageHandle/Client_UserHandle.h"

class MessageHandle :public QObject
{
	Q_OBJECT
public:
	MessageHandle(QObject* parent = nullptr);
	//消息处理接口
	void handle_textMessage(const QJsonDocument& messageDoc);
	void handle_binaryMessage(const QByteArray& message);
	//消息注册
	template <typename T>
	void registerHandle(const QString& key, T& instance, void (T::* handler)(const QJsonObject&, const QByteArray&));
private:
	Client_LoginHandle m_loginHandle{};
	Client_FriendHandle m_friendHandle{};
	Client_GroupHandle m_groupHandle{};
	Client_UserHandle m_userHandle{};
	//消息处理函数表
	QHash<QString, std::function<void(const QJsonObject&, const QByteArray&)>> requestHash{};
private:
	void initRequestHash();
};

#endif // !MESSAGEHANDLE_H_
