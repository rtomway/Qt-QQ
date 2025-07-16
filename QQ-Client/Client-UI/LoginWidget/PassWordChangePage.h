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
private:
	Ui::PassWordChangePage* ui{};
};


#endif // !PASSWORDCHANGEWIDGET_H_
