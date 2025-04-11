#include <QApplication>
#include <crtdbg.h>
#include <QWidget>

#include "Client-UI/MainWindow.h"
#include "Client-NetWork/Client.h"
#include "Client-NetWork/MessageSender.h"


int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	QApplication a(argc, argv);
	Client client;
	MessageSender::instance()->setClient(&client);
	MainWindow mainwindow;
	mainwindow.show();


	// 程序退出时调试内存泄漏
	int ret = a.exec();

	// 程序退出时输出内存泄漏情况
	_CrtDumpMemoryLeaks();

	return ret;
}

