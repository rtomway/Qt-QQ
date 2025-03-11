#include "MainWindow.h"
#include <QFile>
#define MARGIN 5//四个角的长度

MainWindow::MainWindow(QWidget*parent)
	:QMainWindow(parent)
	,m_loginWidget(new LoginWidget(this))
	,m_mainWidget(new MainWidget(this))
	,m_stackedWidget(new QStackedWidget(this))
    ,m_expandAnimation(new QPropertyAnimation(this, "geometry"))
{
	init();
	setFixedSize(m_loginWidget->size());
	setCentralWidget(m_stackedWidget);

    //子窗口圆角父类背景需透明
    this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowFlag(Qt::FramelessWindowHint);

	//显示需要刷新背景颜色 否则跟随父窗口背景色
	m_loginWidget->setAutoFillBackground(true);
    qDebug() << "mainWindow:" << this->size();
}

void MainWindow::init()
{
    setMaximumSize(m_loginWidget->size());
	resize(m_loginWidget->size());
	m_stackedWidget->addWidget(m_loginWidget);
	m_stackedWidget->addWidget(m_mainWidget);
	m_stackedWidget->setCurrentWidget(m_loginWidget);
	connect(m_loginWidget, &LoginWidget::Loginsuccess, this, &MainWindow::showMainWidget);
	connect(m_mainWidget, &MainWidget::quitsuccess, this, &MainWindow::showLoginWidget);

    connect(m_mainWidget, &MainWidget::hideWidget, this, [=]
        {
            this->showMinimized();
        });
    connect(m_mainWidget, &MainWidget::expandWidget, this, [=]
        {
            m_mainWidget->resize(1080, 680);
            if (this->size() == this->screen()->size())
            {
                this->setGeometry(288,107,m_mainWidget->width(),m_mainWidget->height());
            }
            else
            {
                this->setGeometry(this->screen()->geometry());
            }
        });
    connect(m_mainWidget, &MainWidget::exitWidget, [=]
        {
            hide();
        });
}

void MainWindow::showLoginWidget()
{
    setMaximumSize(m_loginWidget->size());
	this->hide();
	move(608,207);
    resize(m_loginWidget->size());
	m_stackedWidget->setCurrentWidget(m_loginWidget);
	m_loginWidget->setAutoFillBackground(true);
	this->show();
}

void MainWindow::showMainWidget()
{
    m_mainWidget->resize(1080, 680);
    setMaximumSize(this->screen()->size());
	this->hide();
	move(288, 107);
	resize(m_mainWidget->size());
	m_stackedWidget->setCurrentWidget(m_mainWidget);
	m_mainWidget->setAutoFillBackground(true);
	this->show();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
	//可拖动
	/*QMouseEvent* mev = dynamic_cast<QMouseEvent*>(event);
	if (event->type() == QEvent::MouseButtonPress)
	{
		m_pressPos = mev->pos();
	}
	else if (event->type() == QEvent::MouseMove && mev->buttons() & Qt::MouseButton::LeftButton)
	{
		QWidget* w = dynamic_cast<QWidget*>(watched);
		if (w)
		{
			w->move(mev->globalPos() - m_pressPos);
		}
	}*/
	return false;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
	this->setFocus();
	if (Qt::LeftButton == event->button() && 0 == (Qt::WindowMaximized & this->windowState()))
	{
		QPoint temp = event->globalPos();
		pLast = temp;
		m_curPos = countFlag(event->pos(), countRow(event->pos()));
		event->ignore();
	}
	m_bPressed = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	QApplication::restoreOverrideCursor();//恢复鼠标指针性状
	event->ignore();
	m_bPressed = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (this->isMaximized())
        return;

    int poss = countFlag(event->pos(), countRow(event->pos()));
    if (!event->buttons())
        setCursorType(poss);

    if ((event->buttons() & Qt::LeftButton) && m_bPressed)//是否左击
    {
        QPoint ptemp = event->globalPos();
        ptemp = ptemp - pLast;
        if (m_curPos == 22)//移动窗口
        {
            ptemp = ptemp + pos();
            move(ptemp);
        }
        else
        {
            QRect wid = geometry();

            int minWidth = this->minimumWidth();
            int minHeight = this->minimumHeight();

            switch (m_curPos)//改变窗口的大小
            {
            case 11:
            {
                QPoint pos = wid.topLeft();

                if (wid.width() > minWidth || ptemp.x() < 0)
                    pos.rx() = pos.rx() + ptemp.x();
                if (wid.height() > minHeight || ptemp.y() < 0)
                    pos.ry() = pos.ry() + ptemp.y();

                wid.setTopLeft(pos);
                break;//左上角
            }
            case 13:
            {
                QPoint pos = wid.topRight();

                if (wid.width() > minWidth || ptemp.x() > 0)
                    pos.rx() = pos.rx() + ptemp.x();
                if (wid.height() > minHeight || ptemp.y() < 0)
                    pos.ry() = pos.ry() + ptemp.y();

                wid.setTopRight(pos);
                break;//右上角
            }
            case 31:
            {
                QPoint pos = wid.bottomLeft();

                if (wid.width() > minWidth || ptemp.x() < 0)
                    pos.rx() = pos.rx() + ptemp.x();
                if (wid.height() > minHeight || ptemp.y() > 0)
                    pos.ry() = pos.ry() + ptemp.y();

                wid.setBottomLeft(pos);
                break;//左下角
            }
            case 33:
            {
                QPoint pos = wid.bottomRight();

                if (wid.width() > minWidth || ptemp.x() > 0)
                    pos.rx() = pos.rx() + ptemp.x();
                if (wid.height() > minHeight || ptemp.y() > 0)
                    pos.ry() = pos.ry() + ptemp.y();

                wid.setBottomRight(pos);
                break;//右下角
            }
            case 12:
            {
                int topY = wid.top();
                if (wid.height() > minHeight || ptemp.y() < 0)
                    topY = topY + ptemp.y();

                wid.setTop(topY);
                break;//中上角
            }
            case 21:
            {
                int leftX = wid.left();

                if (wid.width() > minWidth || ptemp.x() < 0)
                    leftX = leftX + ptemp.x();

                wid.setLeft(leftX);
                break;//中左角
            }
            case 23:
            {
                int rightX = wid.right();

                if (wid.width() > minWidth || ptemp.x() > 0)
                    rightX = rightX + ptemp.x();

                wid.setRight(rightX);
                break;//中右角
            }
            case 32:
            {
                int botY = wid.bottom();
                if (wid.height() > minHeight || ptemp.y() > 0)
                    botY = botY + ptemp.y();

                wid.setBottom(botY);
                break;//中下角
            }
            }
            setGeometry(wid);
        }
        pLast = event->globalPos();//更新位置
    }
    event->ignore();
}

void MainWindow::showEvent(QShowEvent* event)
{
    this->setAttribute(Qt::WA_Mapped);//解决不能及时刷新的bug
    QMainWindow::showEvent(event);
}

int MainWindow::countFlag(QPoint p, int row)
{
    if (p.y() < MARGIN)
        return 10 + row;
    else if (p.y() > this->height() - MARGIN)
        return 30 + row;
    else
        return 20 + row;

}

void MainWindow::setCursorType(int flag)
{
    Qt::CursorShape cursor;
    switch (flag)
    {
    case 11:
    case 33:
        cursor = Qt::SizeFDiagCursor; break;
    case 13:
    case 31:
        cursor = Qt::SizeBDiagCursor; break;
    case 21:
    case 23:
        cursor = Qt::SizeHorCursor; break;
    case 12:
    case 32:
        cursor = Qt::SizeVerCursor; break;
    case 22:
        cursor = Qt::ArrowCursor; break;
    default:
        //  QApplication::restoreOverrideCursor();//恢复鼠标指针性状
        break;

    }
    setCursor(cursor);
}

int MainWindow::countRow(QPoint p)
{
    return (p.x() < MARGIN) ? 1 : (p.x() > (this->width() - MARGIN) ? 3 : 2);
}
