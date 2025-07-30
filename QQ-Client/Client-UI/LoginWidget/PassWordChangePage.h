#ifndef PASSWORDCHANGEPAGE_H_
#define PASSWORDCHANGEPAGE_H_

#include <QWidget>

namespace Ui { class PassWordChangePage; }


class PassWordChangePage :public QWidget
{
	Q_OBJECT
public:
	PassWordChangePage(QWidget* parent = nullptr);
	~PassWordChangePage();
private:
	void init();
	void onPasswordVisable();
	void onChangePassword();
private:
	Ui::PassWordChangePage* ui{};
	bool m_passwordIsVisable = false;
};


#endif // !PASSWORDCHANGEWIDGET_H_
