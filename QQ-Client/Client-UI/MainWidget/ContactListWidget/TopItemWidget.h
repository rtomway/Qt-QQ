#ifndef TOPITEMWIDGET_H_
#define TOPITEMWIDGET_H_

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QTreeWidgetItem>

#include "RotationLabel.h"

class TopItemWidget :public QWidget
{
	Q_OBJECT
public:
	TopItemWidget(QWidget* parent = nullptr);
	~TopItemWidget();
private:
	void init();
public:
	void setCount(const int& x);
	void setName(const QString& name);
	QString getName()const;
	void setAgale();
private:
	QBoxLayout* m_hLayout{};
	RotationLabel* m_headLab{};
	QLabel* m_nameLab{};
	QLabel* m_countLab{};
	QTreeWidgetItem* m_item{};
};

#endif // !TOPITEMWIDGET_H_
