#ifndef  REGISTERPAGE_H_
#define REGISTERPAGE_H_

#include <QWidget>
#include <QJsonObject>

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
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
signals:
	void registerSuccess(const QJsonObject&obj);
};



#endif // ! REGISTERPAGE_H_
