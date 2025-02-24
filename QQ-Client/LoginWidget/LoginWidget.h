#ifndef LOGINWIDGET_H_
#define LOGINWIDGET_H_

#include <QWidget>
#include "LineEditwithButton.h"
#include <QRadioButton>
#include <QPushButton>
#include <QPoint>
#include "AngleRoundedWidget.h"
#include <memory>
#include "RegisterPage.h"

class LoginWidget :public AngleRoundedWidget
{
	Q_OBJECT 
public:
	LoginWidget( QWidget* parent = nullptr);
	void init();
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
signals:
	void editfinish();
	void Loginsuccess();
private:
	LineEditwithButton* m_account{};
	LineEditwithButton* m_password{};
	QRadioButton* m_attentionRBtn{};
	QPushButton* m_loginBtn{};
	QPushButton* m_setBtn{};
	QPushButton* m_exitBtn{};
	QPushButton* m_scanBtn{};
	QPushButton* m_moreBtn{};
	QPoint m_pressPos{};
	std::unique_ptr<RegisterPage> m_registerPage{};
};





#endif // !LOGINWIDGET_H_
