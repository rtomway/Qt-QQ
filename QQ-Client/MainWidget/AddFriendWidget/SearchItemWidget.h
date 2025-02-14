#ifndef SEARCHITEMWIDGET
#define SEARCHITEMWIDGET

#include <QWidget>

namespace Ui { class SearchItemWidget; }
class SearchItemWidget :public QWidget
{
	Q_OBJECT
public:
	SearchItemWidget(QWidget* parent = nullptr);
	~SearchItemWidget();
	void init();
	void setUser(const QJsonObject&obj);
	QJsonObject getUser();
	void setGroup(const QJsonObject& obj);
private:
	Ui::SearchItemWidget* ui{};
private:
	QString m_userName{};
	QString m_user_id{};
	QPixmap m_userHead{};
	QString m_groupName{};
	QString m_group_id{};
	QPixmap m_groupHead{};
};

#endif // !SEARCHITEMWIDGET