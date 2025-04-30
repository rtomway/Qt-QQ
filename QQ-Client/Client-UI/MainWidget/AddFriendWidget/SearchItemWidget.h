#ifndef SEARCHITEMWIDGET
#define SEARCHITEMWIDGET

#include <QWidget>
#include "GlobalTypes.h"

namespace Ui { class SearchItemWidget; }

class SearchItemWidget :public QWidget
{
	Q_OBJECT
public:
	SearchItemWidget(ChatType type,QWidget* parent = nullptr);
	~SearchItemWidget();
private:
	void init();
public:
	void setUser(const QJsonObject& obj);
	QJsonObject getUser();
	void setPixmap(const QPixmap& pixmap);
	void setGroup(const QJsonObject& obj);
	QJsonObject getGroup();
private:
	Ui::SearchItemWidget* ui{};
private:
	QString m_searchName{};
	QString m_search_id{};
	QPixmap m_searchPix{};
	bool m_isAdded{true};
	ChatType m_type;
};

#endif // !SEARCHITEMWIDGET