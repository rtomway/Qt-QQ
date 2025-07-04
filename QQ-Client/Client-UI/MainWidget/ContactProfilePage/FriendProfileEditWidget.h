#ifndef	FRIENDPROFILEDITWIDGET_H_
#define FRIENDPROFILEDITWIDGET_H_

#include <QWidget>
#include <QJsonObject>
#include <memory>
#include <QCalendarWidget>
#include <QWidgetAction>

#include "LineEditwithButton.h"
#include "AngleRoundedWidget.h"

namespace Ui { class FriendProfileEditWidget; }

class FriendProfileEditWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	FriendProfileEditWidget(QWidget* parent = nullptr);
	void init();
private:
	void updateAvatar();
	void updateMessage();
private:
	QLabel* m_editDetail{};
	QPushButton* m_exitBtn{};
	QLabel* m_headLab{};
	LineEditwithButton* m_nickNameEdit{};
	LineEditwithButton* m_signaltureEdit{};
	LineEditwithButton* m_genderEdit{};
	LineEditwithButton* m_birthdayEdit{};
	LineEditwithButton* m_ageEdit{};
	LineEditwithButton* m_countryEdit{};
	LineEditwithButton* m_provinceEdit{};
	LineEditwithButton* m_areaEdit{};
private:
	QPixmap m_headPix{};
	QString m_avatarOldPath{};
	QString m_avatarNewPath{};
	bool m_avatarIsChange{ false };
	QJsonObject m_json{};
	QString m_userId{};
	std::unique_ptr<QWidgetAction>m_calendarAction{};
	std::unique_ptr<QCalendarWidget>m_calendarWidget{};
public:
	void setUser(const QJsonObject& obj);
	const QJsonObject& getUser()const;
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
};

#endif // !FRIENDPROFILEDITWIDGET_H_