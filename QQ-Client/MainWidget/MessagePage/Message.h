#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <QDateTime>

class Message
{
public:
	Message(QString sender, QString receiver, QString message, QDateTime time);
	void readMessage();
	QString getSenderId();
	QString getMessage();
private:
	QString m_sender_id;
	QString m_receiver_id;
	QString m_message;
	QDateTime m_time;
	bool m_isRead;

};

#endif // !MESSAGE_H_
