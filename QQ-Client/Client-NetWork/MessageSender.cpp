#include "MessageSender.h"

MessageSender::MessageSender(QObject* parent)
{
	
}


//Web发送文本消息
void MessageSender::sendWebTextMessage(const QString& message)
{
	emit sendWebTextMessage_client(message);
}

//Web发送二进制数据
void MessageSender::sendWebBinaryData(const QByteArray& data)
{
	emit sendWebBinaryData_client(data);
}

//发送http请求
void MessageSender::sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack)
{
	emit sendHttpRequest_client(type, data, Content_type, callBack);
}



