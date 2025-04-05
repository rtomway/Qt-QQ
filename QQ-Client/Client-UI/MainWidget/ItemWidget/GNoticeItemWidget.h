#ifndef GNOTICEITEMWIDGET_H_
#define	GNOTICEITEMWIDGET_H_

#include "ItemWidget.h"
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>

enum GroupNoticeType
{
	GroupInvite,
	GroupRequestAdd
};

class GNoticeItemWidget :public ItemWidget
{
public:
	GNoticeItemWidget(QWidget* parent = nullptr);
	void setItemWidget(const QString& group_id)override;
	void setMode(bool isReply);
private:
	void init();
private:
	QJsonObject m_json{};
	QPixmap m_headPix{};
private:
	QLabel* m_noticeMessageLab{};
	QLabel* m_timeLab{};
	QPushButton* m_okBtn{};
	QPushButton* m_cancelBtn{};
	GroupNoticeType m_type;
	bool m_isReply = false;
};

#endif // !GNOTICEITEMWIDGET_H_
