
#ifndef GROUPHANDLE_H_
#define GROUPHANDLE_H_

#include <QJsonObject>
#include <QByteArray>

class GroupHandle
{
public:
	static void handle_searchGroup(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_createGroup(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_groupTextCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
};

#endif // !GROUPHANDLE_H_