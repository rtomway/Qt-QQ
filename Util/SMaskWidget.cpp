#include "SMaskWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QKeyEvent>

SMaskWidget::SMaskWidget(QWidget* parent)
	:QWidget(parent)
	,m_pos(PopPosition::MiddleWidget)
{
	setStyleSheet(R"(
	 background-color:rgba(194,195,201,0.4);
		)");
	setAttribute(Qt::WA_StyledBackground);

}

SMaskWidget* SMaskWidget::instance()
{
	static SMaskWidget* ins;
	if (!ins)
	{
		ins = new SMaskWidget;
	}
	return ins;
}

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

void SMaskWidget::popUp(QWidget* dialog)
{
	
	addDialog(dialog);
	
	if (dialog && m_dialogs.contains(dialog))
	{
		/*if (m_currentPopUp)
			m_currentPopUp->setParent(nullptr);*/
		m_currentPopUp = dialog; 
		auto child = this->findChildren<QWidget*>();
		
		dialog->setFocus();
		this->show();
		dialog->show();
	}
}
void SMaskWidget::setPopPostion(PopPosition position)
{
	m_pos = position;
}
void SMaskWidget::setPopGeometry(const QRect& rect)
{
	m_currentPopUp->setGeometry(rect);
}
QSize SMaskWidget::getMainWidgetSize()
{
	return m_MainWidget->size();
}
bool SMaskWidget::eventFilter(QObject* object, QEvent* ev)
{
	//判断当前是否为mainwindow对象
	if (object == m_MainWidget)
	{
		if (ev->type() == QEvent::Resize)
		{
			this->setGeometry(m_MainWidget->rect());   //setGeometry 控制位置大小

			//this->resize(m_MainWidget->size());
		}
	}
	else
		//弹出框
	{
		if (ev->type() == QEvent::Hide)
		{
			this->hide();
		}
		else if(ev->type()==QEvent::Resize)
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
			/*switch (m_pos)
			{
			case SMaskWidget::LeftWidget:
				m_currentPopUp->move(0, 0);
				break;
			case SMaskWidget::MiddleWidget:
				m_currentPopUp->move((this->width() - m_currentPopUp->width()) / 2, 0);
				break;
			case SMaskWidget::RightWidget:
				m_currentPopUp->move(this->width() - m_currentPopUp->width(), 0);
				break;
			default:
				break;
			}*/
			/*int x = (m_MainWidget->width() - m_currentPopUp->width()) / 2;
			int y = (m_MainWidget->height() - m_currentPopUp->height()) / 2;
			m_currentPopUp->setGeometry(x, y, 480, 540);*/
		}
	}
	return false;
}

void SMaskWidget::resizeEvent(QResizeEvent* ev)
{
	onResize();
}

void SMaskWidget::onResize()
{
	if (m_currentPopUp)
	{
		/*m_currentPopUp->setFixedHeight(this->height());
		m_currentPopUp->setFixedWidth(this->width() / 4);*/
		m_currentPopUp->setFixedHeight(580);
		m_currentPopUp->setFixedWidth(540);
	}
}


