#include <QApplication>

#include "Client-UI/MainWindow.h"
#include "WebClient.h"
#include "HttpClient.h"
#include "NetWorkService.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"



int main(int argc, char* argv[])
{

	QApplication a(argc, argv);

	//网络
	WebClient webClient;
	HttpClient httpClient;
	NetWorkService service(&webClient, &httpClient);
	NetWorkServiceLocator::setNetService(&service);
	
	//Ui
	MainWindow mainwindow;
	mainwindow.show();

	return a.exec();
}

