#include "SMaskWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QKeyEvent>

SMaskWidget::SMaskWidget(QWidget* parent)
	:QWidget(parent)
	, m_pos(PopPosition::MiddleWidget)
{
	setStyleSheet(R"(
	 background-color:rgba(194,195,201,0.4);
		)");
	setAttribute(Qt::WA_StyledBackground);

}

//单例
SMaskWidget* SMaskWidget::instance()
{
	static SMaskWidget ins;
	return &ins;
}

//主窗口设置
void SMaskWidget::setMainWidget(QWidget* widget)
{
	if (!widget)
	{
		qWarning() << "widget不能为空";
		return;
	}
	if (m_MainWidget)
	{
		qWarning() << "m_MainWidget已存在,will be changed";
	}
	m_MainWidget = widget;
	this->setParent(m_MainWidget);
	this->hide();
	m_MainWidget->installEventFilter(this);
}

//获取主窗口大小
QSize SMaskWidget::getMainWidgetSize()
{
	return m_MainWidget->size();
}

//子窗口添加
void SMaskWidget::addDialog(QWidget* dialog)
{
	if (!dialog)
	{
		qWarning() << "dialog不能为空";
		return;
	}
	auto it = m_dialogs.find(dialog);
	if (it != m_dialogs.end())
	{
		qWarning() << "dialog is exists";
		return;
	}
	m_dialogs.insert(dialog);
	dialog->setParent(this);
	dialog->installEventFilter(this);
}

//弹出窗口
void SMaskWidget::popUp(QWidget* dialog)
{
	this->raise();  // 强制置顶
	dialog->raise(); // 弹出窗口也要置顶
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint); // 添加置顶标志

	addDialog(dialog);
	if (dialog && m_dialogs.contains(dialog))
	{
		m_currentPopUp = dialog;
		auto child = this->findChildren<QWidget*>();

		dialog->setFocus();
		this->show();
		dialog->show();
	}
}

//弹出位置
void SMaskWidget::setPopPostion(PopPosition position)
{
	m_pos = position;
}

//弹出位置
void SMaskWidget::setPopGeometry(const QRect& rect)
{
	m_currentPopUp->setGeometry(rect);
}

//重写
bool SMaskWidget::eventFilter(QObject* object, QEvent* ev)
{
	//判断当前是否为mainwindow对象
	if (object == m_MainWidget)
	{
		if (ev->type() == QEvent::Resize)
		{
			this->setGeometry(m_MainWidget->rect());   //setGeometry 控制位置大小
		}
	}
	else
		//弹出框
	{
		if (ev->type() == QEvent::Hide)
		{
			this->hide();
		}
		else if (ev->type() == QEvent::Resize)
		{
			onResize();
		}
		else if (ev->type() == QEvent::KeyRelease)
		{
			auto kev = static_cast<QKeyEvent*>(ev);
			if (kev->key() == Qt::Key_Escape)
			{
				this->hide();
			}
		}
		else if (ev->type() == QEvent::Show)
		{
		}
	}
	return false;
}
void SMaskWidget::mousePressEvent(QMouseEvent* event)
{
	event->accept();
}

//放缩重写
void SMaskWidget::resizeEvent(QResizeEvent* ev)
{
	onResize();
}
void SMaskWidget::onResize()
{
	/*if (m_currentPopUp)
	{
		m_currentPopUp->setFixedHeight(580);
		m_currentPopUp->setFixedWidth(540);
	}*/
}


