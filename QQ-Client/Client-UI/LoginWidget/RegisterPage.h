#ifndef  REGISTERPAGE_H_
#define REGISTERPAGE_H_

#include <QWidget>
#include <QJsonObject>

namespace Ui { class RegisterPage; }

class RegisterPage :public QWidget
{
	Q_OBJECT
public:
	RegisterPage(QWidget* parent = nullptr);
	~RegisterPage();
private:
	void init();
	void onRegisterUser();
private:
	Ui::RegisterPage* ui{};
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
};



#endif // ! REGISTERPAGE_H_
