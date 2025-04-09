#ifndef REGISTERHANDLE_H_
#define REGISTERHANDLE_H_

#include <QJsonObject>
#include <QByteArray>
#include <QHttpServerResponder>

class RegisterHandle
{
public:
	static void handle_register(const QJsonObject&paramsObj,const QByteArray& data, QHttpServerResponder& responder);
};

#endif // !REGISTERHANDLE_H_