#ifndef SEARCHWIDGET_H_
#define SEARCHWIDGET_H_

#include <QWidget>
#include <QListWidget>


enum search_type
{
	User,
	Grouop
};

namespace Ui { class SearchWidget; }

class SearchWidget : public QWidget
{
	Q_OBJECT
public:
	SearchWidget(QWidget* parent = nullptr);
	~SearchWidget();
	void init();

private:
	search_type m_searchType{ search_type::User };
	QListWidget* m_userList{};
	QListWidget* m_groupList{};
	int m_currentPage = 1;
	QString m_searchText;
	bool m_isLoading{ false };
	int m_loadCount = 0;
private:
	void addListWidgetItem(QListWidget* list, const QJsonObject& obj, const QPixmap& pixmap);
	void sendSearchRequest();
	void onListScroll(QScrollBar* scrollBar, int scrollValue);
private:
	Ui::SearchWidget* ui{};

};

#endif // !SEARCHWIDGET_H_
