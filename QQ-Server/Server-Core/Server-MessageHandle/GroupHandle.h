
#ifndef GROUPHANDLE_H_
#define GROUPHANDLE_H_

#include <QJsonObject>
#include <QByteArray>
#include <QHttpServerResponder>

class GroupHandle
{
public:
	static void handle_searchGroup(const QJsonObject&paramsObj,const QByteArray& data, QHttpServerResponder& responder);
	static void handle_createGroup(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_groupTextCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_groupInviteSuccess(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
};

#endif // !GROUPHANDLE_H_