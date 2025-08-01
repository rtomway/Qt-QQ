﻿#ifndef USERHANDLE_H_
#define USERHANDLE_H_

#include <QJsonObject>
#include <QByteArray>
#include <QHttpServerResponder>

class UserHandle
{
public:
	static void handle_searchUser(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_queryUser(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_searchGroup(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_updateUserAvatar(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_updateUserMessage(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_passwordChange(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
};

#endif // !USERHANDLE_H_