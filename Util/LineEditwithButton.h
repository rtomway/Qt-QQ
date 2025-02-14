#ifndef LINEEDITWITHBUTTON
#define LINEEDITWITHBUTTON

#include <QLineEdit>
#include <QToolButton>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include "RoundMenu.h"

class LineEditwithButton :public QWidget
{
	Q_OBJECT
public:
	LineEditwithButton(QWidget* parent = nullptr);
	void init();
	void addBtn(QPushButton* toolBtn);
	void initComboboxMenu();
private:
	QLineEdit* m_lineEdit{};
	QPushButton* m_clearBtn{};
	QPushButton* m_arrowBtn{};
	QLabel* m_preLab{};
	QLabel* m_lastLab{};
	QHBoxLayout* mlayout{};
	QFrame* mframe{};
	QMenu* m_comboboxMenu{};
public:
	void setPlaceholderText(QString text);
	void setText(QString text);
	void setEchoMode();
	void setPreLab(QString text);
	void setLastLab(QString text);
	void setEditEnable(bool edit);
	void setEditPosition(Qt::Alignment);
	void setComboBox();
	void setclearBtn();
	void setWidth(int width);
	void addMenuItem(QString item);
	QString getLineEdit();
	bool m_isCombobox{false};
	bool m_isclear{ false };
signals:
	void clicked();
public slots:
	void editfinished();
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
};



#endif // !LINEEDITWITHBUTTON
