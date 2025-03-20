#ifndef REGISTERHANDLE_H_
#define REGISTERHANDLE_H_

#include <QJsonObject>
#include <QByteArray>

class RegisterHandle
{
public:
	static void handle_register(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
};

#endif // !REGISTERHANDLE_H_