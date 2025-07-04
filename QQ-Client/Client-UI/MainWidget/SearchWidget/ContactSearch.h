#ifndef CONTACTSEARCH_H_
#define CONTACTSEARCH_H_

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <memory>

#include "Friend.h"

class ContactSearch :public QWidget
{
	Q_OBJECT
public:
	ContactSearch(QWidget* parent = nullptr);
private:
	void init();
public:
	void searchText(const QString& text);
private:
	void searchFriend(const QString& text);
	void searchGroup(const QString& text);
private:
	void addSearchItem(const QJsonObject& obj, const QPixmap& pixmap);
	void clearFriendList();
private:
	QPushButton* m_friendBtn{};
	QPushButton* m_groupBtn{};
	QListWidget* m_searchList{};
	bool m_isFriendBtn{ true };
};

#endif // !CONTACTSEARCH_H_
