#include "MainWindow.h"
#include <QFile>
#define MARGIN 3//四个角的长度

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
	, m_loginWidget(new LoginWidget(this))
	, m_mainWidget(new MainWidget(this))
	, m_stackedWidget(new QStackedWidget(this))
	, m_expandAnimation(new QPropertyAnimation(this, "geometry"))
{
	init();
	setFixedSize(m_loginWidget->size());
	setCentralWidget(m_stackedWidget);
	setMouseTracking(true);
	enableMouseTracking(this);  // 递归启用子控件鼠标跟踪

	//子窗口圆角父类背景需透明
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowFlag(Qt::FramelessWindowHint);
	//显示需要刷新背景颜色 否则跟随父窗口背景色
	m_loginWidget->setAutoFillBackground(true);
}

void MainWindow::init()
{
	setMaximumSize(m_loginWidget->size());
	resize(m_loginWidget->size());
	m_stackedWidget->addWidget(m_loginWidget);
	m_stackedWidget->addWidget(m_mainWidget);
	m_stackedWidget->setCurrentWidget(m_loginWidget);
	//登录成功
	connect(m_loginWidget, &LoginWidget::Loginsuccess, this, &MainWindow::showMainWidget);
	//退出
	connect(m_mainWidget, &MainWidget::quitSuccess, this, &MainWindow::showLoginWidget);
	//隐藏
	connect(m_mainWidget, &MainWidget::hideWidget, this, [=]
		{
			this->showMinimized();
		});
	//放缩
	connect(m_mainWidget, &MainWidget::expandWidget, this, [=]
		{
			m_mainWidget->resize(1080, 680);
			if (this->size() == this->screen()->size())
			{
				this->setGeometry(288, 107, m_mainWidget->width(), m_mainWidget->height());
			}
			else
			{
				this->setGeometry(this->screen()->geometry());
			}
		});
	//注销
	connect(m_mainWidget, &MainWidget::exitWidget, [=]
		{
			hide();
		});
}

//登录窗口
void MainWindow::showLoginWidget()
{
	// 1. 创建全屏透明覆盖层
	QWidget* cover = new QWidget(nullptr, Qt::Tool | Qt::FramelessWindowHint);
	cover->setAttribute(Qt::WA_TranslucentBackground);
	cover->setWindowOpacity(0.0);
	cover->showFullScreen();
	// 2. 在后台完成所有操作
	QTimer::singleShot(0, [=] {
		hide();
		setMaximumSize(m_loginWidget->size());
		resize(m_loginWidget->size());
		move(608, 207);
		m_stackedWidget->setCurrentWidget(m_loginWidget);
		m_mainWidget->setAutoFillBackground(true);
		// 3. 添加20ms延迟确保系统完成渲染
		QTimer::singleShot(15, [=] {
			show();
			cover->deleteLater();
			});
		});
}

//主窗口
void MainWindow::showMainWidget()
{
	// 1. 创建全屏透明覆盖层
	QWidget* cover = new QWidget(nullptr, Qt::Tool | Qt::FramelessWindowHint);
	cover->setAttribute(Qt::WA_TranslucentBackground);
	cover->setAttribute(Qt::WA_TransparentForMouseEvents);
	cover->setWindowOpacity(0.0);
	cover->showFullScreen();
	// 2. 在后台完成所有操作
	QTimer::singleShot(0, [=] {
		hide();
		setMaximumSize(this->screen()->size());
		resize(1080, 680);
		move(288, 107);
		m_stackedWidget->setCurrentWidget(m_mainWidget);
		m_mainWidget->setAutoFillBackground(true);
		// 3. 添加20ms延迟确保系统完成渲染
	/*	QTimer::singleShot(18, [=] {
			show();
			cover->deleteLater();
			});*/
		show();
		cover->deleteLater();
		});
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
		QPoint temp = event->globalPosition().toPoint();
		pLast = temp;
		m_curPos = countFlag(event->pos(), countRow(event->pos()));
		event->ignore();
	}
	m_bPressed = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	QApplication::restoreOverrideCursor(); // 恢复鼠标指针形状
	event->ignore();
	m_bPressed = false;
}

void MainWindow::showEvent(QShowEvent* event)
{
	this->setAttribute(Qt::WA_Mapped);//解决不能及时刷新的bug
	QMainWindow::showEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (this->isMaximized()) {
		return;
	}

	// 获取鼠标当前的位置
	int posFlag = countFlag(event->pos(), countRow(event->pos()));


	// 如果没有按下鼠标，改变光标形状
	if (!(event->buttons() & Qt::LeftButton) && m_stackedWidget->currentWidget() != m_loginWidget) {
		setCursorType(posFlag);
	}

	// 如果按下了左键，并且窗口不是最大化状态
	if ((event->buttons() & Qt::LeftButton) && m_bPressed) {
		QPoint ptemp = event->globalPosition().toPoint();
		ptemp = ptemp - pLast;  // 计算鼠标移动的距离

		// 判断当前是否是拖动窗口还是拉伸窗口
		if (m_curPos == 22) {  // 移动窗口
			ptemp = ptemp + pos();
			move(ptemp);
		}
		else {  // 改变窗口大小
			QRect wid = geometry();
			int minWidth = this->minimumWidth();
			int minHeight = this->minimumHeight();

			// 根据当前的窗口区域，调整窗口的大小
			switch (m_curPos) {
			case 11: {  // 左上角
				QPoint pos = wid.topLeft();
				if (wid.width() > minWidth || ptemp.x() < 0) pos.rx() += ptemp.x();
				if (wid.height() > minHeight || ptemp.y() < 0) pos.ry() += ptemp.y();
				wid.setTopLeft(pos);
				break;
			}
			case 13: {  // 右上角
				QPoint pos = wid.topRight();
				if (wid.width() > minWidth || ptemp.x() > 0) pos.rx() += ptemp.x();
				if (wid.height() > minHeight || ptemp.y() < 0) pos.ry() += ptemp.y();
				wid.setTopRight(pos);
				break;
			}
			case 31: {  // 左下角
				QPoint pos = wid.bottomLeft();
				if (wid.width() > minWidth || ptemp.x() < 0) pos.rx() += ptemp.x();
				if (wid.height() > minHeight || ptemp.y() > 0) pos.ry() += ptemp.y();
				wid.setBottomLeft(pos);
				break;
			}
			case 33: {  // 右下角
				QPoint pos = wid.bottomRight();
				if (wid.width() > minWidth || ptemp.x() > 0) pos.rx() += ptemp.x();
				if (wid.height() > minHeight || ptemp.y() > 0) pos.ry() += ptemp.y();
				wid.setBottomRight(pos);
				break;
			}
			case 12: {  // 中上
				int topY = wid.top();
				if (wid.height() > minHeight || ptemp.y() < 0) topY += ptemp.y();
				wid.setTop(topY);
				break;
			}
			case 21: {  // 中左
				int leftX = wid.left();
				if (wid.width() > minWidth || ptemp.x() < 0) leftX += ptemp.x();
				wid.setLeft(leftX);
				break;
			}
			case 23: {  // 中右
				int rightX = wid.right();
				if (wid.width() > minWidth || ptemp.x() > 0) rightX += ptemp.x();
				wid.setRight(rightX);
				break;
			}
			case 32: {  // 中下
				int botY = wid.bottom();
				if (wid.height() > minHeight || ptemp.y() > 0) botY += ptemp.y();
				wid.setBottom(botY);
				break;
			}
			}
			setGeometry(wid);
		}
		pLast = event->globalPosition().toPoint();  // 更新位置
	}
	event->ignore();
}

void MainWindow::setCursorType(int flag)
{
	Qt::CursorShape cursor;
	switch (flag) {
	case 11:
		cursor = Qt::SizeFDiagCursor;// 左上角
	case 33:
		cursor = Qt::SizeFDiagCursor;
		break;
	case 13:
		cursor = Qt::SizeBDiagCursor;// 右上角
	case 31:
		cursor = Qt::SizeBDiagCursor;
		break;
	case 21:  cursor = Qt::SizeHorCursor;// 中左
	case 23:
		cursor = Qt::SizeHorCursor;
		break;
	case 12: cursor = Qt::SizeVerCursor; // 中上
	case 32:
		cursor = Qt::SizeVerCursor;
		break;
	case 22:  // 中间
		cursor = Qt::ArrowCursor;
		break;
	default:
		cursor = Qt::ArrowCursor;  // 默认光标
		break;
	}
	setCursor(cursor);
}

int MainWindow::countFlag(QPoint p, int row)
{
	if (p.y() < MARGIN)  // 上边缘
		return 10 + row;
	else if (p.y() > this->height() - MARGIN)  // 下边缘
		return 30 + row;
	else
		return 20 + row;
}

int MainWindow::countRow(QPoint p)
{
	if (p.x() < MARGIN)  // 左边缘
		return 1;
	else if (p.x() > (this->width() - MARGIN))  // 右边缘
		return 3;
	else  // 中间区域
		return 2;
}

void MainWindow::enableMouseTracking(QWidget* w)
{
	w->setMouseTracking(true);
	const auto& children = w->findChildren<QWidget*>();
	for (QWidget* child : children)
		child->setMouseTracking(true);
}


