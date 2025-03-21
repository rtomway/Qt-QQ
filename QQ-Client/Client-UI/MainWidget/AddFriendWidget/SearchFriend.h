#ifndef SEARCHFRIEND_H_
#define SEARCHFRIEND_H_

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class SearchFriend :public QWidget
{
	Q_OBJECT
public:
	SearchFriend(QWidget* parent = nullptr);
private:
	void init();
public:
	void addSearchItem(const QJsonObject& obj, const QPixmap& pixmap);
	void clearFriendList();
private:
	QPushButton* m_friendBtn{};
	QPushButton* m_groupBtn{};
	QListWidget* m_searchList{};
};

#endif // !SEARCHFRIEND_H_
