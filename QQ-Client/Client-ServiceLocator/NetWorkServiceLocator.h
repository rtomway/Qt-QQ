#ifndef NETWORKSERVICELOCATOR_H_
#define NETWORKSERVICELOCATOR_H_

#include <QObject>
#include "NetWorkService.h"

class NetWorkServiceLocator:public QObject
{
	Q_OBJECT
public:
	static NetWorkService*instance();
	// 禁止外部构造
	NetWorkServiceLocator() = delete;
};


#endif // !NETWORKSERVICELOCATOR_H_
