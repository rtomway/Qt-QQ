#ifndef SETPANNELWIDGET_H_
#define SETPANNELWIDGET_H_

#include <QListWidget>

class SetPannelWidget :public QWidget
{
	Q_OBJECT
public:
	SetPannelWidget(QWidget* parent = nullptr);
	void init();
	void setId(const QString& id);
	void addItemWidget(QWidget* widget,int height);
	void addSpaceItem(int height=10);
	void clearListWidget();
private:
	QListWidget* m_listWidget{};
	QString m_pannelId;
};

#endif // !SETPANNELWIDGET_H_