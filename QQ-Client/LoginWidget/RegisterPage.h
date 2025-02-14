#ifndef  REGISTERPAGE_H_
#define REGISTERPAGE_H_

#include <QWidget>

namespace Ui { class RegisterPage; }

class RegisterPage:public QWidget
{
	Q_OBJECT
public:
	RegisterPage(QWidget* parent = nullptr);
	~RegisterPage();
	void init();
private:
	Ui::RegisterPage* ui{};
};



#endif // ! REGISTERPAGE_H_
