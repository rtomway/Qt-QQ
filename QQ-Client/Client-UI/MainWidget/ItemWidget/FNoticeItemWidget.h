#ifndef FNOTICEITEMWIDGET_H_
#define	FNOTICEITEMWIDGET_H_

#include "ItemWidget.h"
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include "AddWidget.h"

class FNoticeItemWidget :public ItemWidget
{
public:
	FNoticeItemWidget(QWidget* parent = nullptr);
	void setItemWidget(const QString& user_id)override;
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
	std::unique_ptr<AddWidget> m_addWidget{};
	bool m_isReply = false;
};

#endif // !FNOTICEITEMWIDGET_H_
