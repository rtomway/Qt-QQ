#ifndef MESSAGEPAGE_H_
#define MESSAGEPAGE_H_

#include <QWidget>
#include <QMenu>
#include <QPixmap>
namespace Ui { class MessagePage; }

class MessagePage :public QWidget
{
	Q_OBJECT
public:
	MessagePage(QWidget* parent = nullptr);
	~MessagePage();
	void init();             
	void setUser(const QJsonObject& obj);
	QString getCurrentID();
	void updateReciveMessage(const QString&message);
private:
	void updateWidget();
private:
	QString m_username{};
	QPixmap m_headPix{};
	QString m_user_id{};
	QString m_currentID{};
private:
	Ui::MessagePage* ui{};
};





#endif // !MESSAGEPAGE_H_
