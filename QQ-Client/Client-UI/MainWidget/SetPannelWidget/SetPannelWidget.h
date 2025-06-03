#ifndef SETPANNELWIDGET_H_
#define SETPANNELWIDGET_H_

#include <QListWidget>

class SetPannelWidget :public QWidget
{
	Q_OBJECT
public:
	SetPannelWidget(QWidget* parent = nullptr);
	void init();
	void addItemWidget(QWidget* widget, int height = 0);
	void addSpaceItem(int height = 10);
	void setItemWidgetHidden(QWidget* widget, bool isHidden);
	void clearListWidget();
	QListWidgetItem* getItemByWidget(QWidget* widget) const;
private:
	QListWidget* m_listWidget{};
	QString m_pannelId;
};

#endif // !SETPANNELWIDGET_H_