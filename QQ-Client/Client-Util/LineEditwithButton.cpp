#include "LineEditwithButton.h"
#include <QFile>
#include <QPoint>
#include <QSpacerItem>
#include <QWidgetAction>

LineEditwithButton::LineEditwithButton(QWidget* parent)
	:QWidget(parent)
	,m_lineEdit(new QLineEdit(this))
	,m_clearBtn(new QPushButton(this))
	,m_arrowBtn(new QPushButton(this))
	,mlayout(new QHBoxLayout(this))
	,m_lastLab(new QLabel(" ",this))
	,m_preLab(new QLabel(" ",this))
	,mframe(new QFrame(this))
	,m_comboboxMenu(new QMenu(this))
{
	 init();
	 QFile file(":/stylesheet/Resource/StyleSheet/LineEditwithButton.css");
	 if (file.open(QIODevice::ReadOnly))
	 {
		 this->setStyleSheet(file.readAll());
	 }
	 else
	 {
		 qDebug() << file.fileName() << "打开失败";
	 }
}

void LineEditwithButton::init()
{
	 this->setFixedWidth(250);
	 this->installEventFilter(this);
	 m_lineEdit->installEventFilter(this);
	 mlayout->setSpacing(0);
	 mlayout->setContentsMargins(0, 0, 0, 0);

	 //前label
	 m_preLab->setIndent(10);
	 m_preLab->setObjectName("preLab");
	 mlayout->addWidget(m_preLab);
	 mlayout->addWidget(m_lineEdit);

	 //按钮
	 m_clearBtn->hide();
	 m_arrowBtn->hide();

	 //后label
	 m_lastLab->setIndent(10);
	 m_lastLab->setObjectName("lastLab");
	 mlayout->addWidget(m_lastLab);
	
	 //qframe避免缝隙
	 mframe->setFrameStyle(QFrame::NoFrame); // 设置边框为无
	 mframe->setLayout(mlayout);
	
	 auto vlayout = new QVBoxLayout(this);
	 vlayout->addWidget(mframe);
	 //菜单设置
	 m_comboboxMenu->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
	 m_comboboxMenu->setStyleSheet(QString("QMenu{background-color:white;margin:7px;}"));
	 m_comboboxMenu->setAttribute(Qt::WA_TranslucentBackground);
	 //进入编辑
	connect(this, &LineEditwithButton::clicked, this, [=]
		{
			m_lineEdit->setFocus();
			if(m_isclear)
			m_clearBtn->show();
		});
	//编辑完成
	connect(m_lineEdit, &QLineEdit::editingFinished, this, [=]
		{
			m_lineEdit->clearFocus();
			if (m_isclear)
			m_clearBtn->hide(); 
		});
	//清空编辑框
	connect(m_clearBtn, &QPushButton::clicked, [=]
		{
			m_lineEdit->setText("");
			m_clearBtn->show();
			m_lineEdit->setFocus();
		});
	//菜单弹出按钮
	connect(m_arrowBtn, &QPushButton::clicked, [=]
		{
			const QPoint globalPos = mapToGlobal(rect().bottomLeft());
			const QSize size = rect().size();
			// 设置菜单的位置
			m_comboboxMenu->popup(globalPos+QPoint(0,-10));
		});
	//菜单选择
	connect(m_comboboxMenu, &QMenu::triggered, [=](QAction* action)
		{
			m_lineEdit->setText(action->text());
		});
}

//添加按钮
void LineEditwithButton::addBtn(QPushButton* toolBtn)
{
	toolBtn->setFixedWidth(20);
	mlayout->removeWidget(m_lastLab);
	mlayout->addWidget(toolBtn);
	mlayout->addWidget(m_lastLab);
	toolBtn->installEventFilter(this);
}

//编辑完成
void LineEditwithButton::editfinished()
{
	//发送离开编辑框信号
	QMetaObject::invokeMethod(m_lineEdit, "editingFinished");
}

void LineEditwithButton::setPlaceholderText(QString text)
{
	m_lineEdit->setPlaceholderText(text);
}

//文本设置
void LineEditwithButton::setText(QString text)
{
	m_lineEdit->setText(text);
	
}

//获取文本
QString LineEditwithButton::getLineEditText()
{
	return m_lineEdit->text();
}

//文本隐藏
void LineEditwithButton::setEchoMode()
{
	m_lineEdit->setEchoMode(QLineEdit::Password);
}

//前置lab
void LineEditwithButton::setPreLab(QString text)
{
	m_preLab->setText(text);
}

//后置lab
void LineEditwithButton::setLastLab(QString text)
{
	m_lastLab->setText(text);
}

//可否编辑
void LineEditwithButton::setEditEnable(bool edit)
{
	m_lineEdit->setEnabled(edit);
}

//初始编辑位置
void LineEditwithButton::setEditPosition(Qt::Alignment alignment)
{
	m_lineEdit->setAlignment(alignment);
}

//下拉菜单设置
void LineEditwithButton::setComboBox()
{
	setEditEnable(false);
	m_arrowBtn->show();
	m_arrowBtn->setIcon(QIcon(":/icon/Resource/Icon/down.png"));
	addBtn(m_arrowBtn);
	m_isCombobox = true;
	m_comboboxMenu->setFixedWidth(this->width());
}

//清除按钮
void LineEditwithButton::setclearBtn()
{
	m_clearBtn->setIcon(QIcon(":/icon/Resource/Icon/x.png"));
	addBtn(m_clearBtn);
	m_isclear = true;
}

//宽度
void LineEditwithButton::setWidth(int width)
{
	this->setFixedWidth(width);
}

//菜单项
void LineEditwithButton::addMenuItem(QString item)
{
	m_comboboxMenu->addAction(item);
}

 //下拉菜单获取
QMenu* LineEditwithButton::getMenu() const
{
	return m_comboboxMenu;
}

//控件事件重写
bool LineEditwithButton::eventFilter(QObject* watched, QEvent* event)
{
	//进入编辑状态
	if (event->type() == QEvent::MouseButtonPress)
	{
		if (watched == m_clearBtn)
		{
			m_clearBtn->click();
			return true;
		}
		emit clicked();
		return true;
	}
	//退出编辑
	if (event->type() == QEvent::FocusOut)
	{
		m_clearBtn->hide();
	}
	
	return false;
}
