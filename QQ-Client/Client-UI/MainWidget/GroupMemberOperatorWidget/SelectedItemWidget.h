#ifndef SELECTEDITEMWIDGET_H_
#define SELECTEDITEMWIDGET_H_

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>


class SelectedItemWidget :public QWidget
{
	Q_OBJECT
public:
	SelectedItemWidget(const QString& user_id, const QString& username, QWidget* parent = nullptr);
	const QString& getUserId()const;
	void setChecked(bool isCheck);
	bool isChecked();
	void setEnabled(bool status);
private:
	void init();
private:
	QHBoxLayout* m_layout{};
	QCheckBox* m_selected{};
	QLabel* m_headLab{};
	QLabel* m_nameLab{};
private:
	QString m_userId{};
	QString m_userName{};
signals:
	void checked(bool isChecked);
};

#endif // !SELECTEDITEMWIDGET_H_
