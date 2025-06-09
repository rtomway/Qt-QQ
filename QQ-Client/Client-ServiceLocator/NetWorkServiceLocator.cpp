#include "NetWorkServiceLocator.h"

NetWorkService* NetWorkServiceLocator::instance()
{
	static NetWorkService instance;
	return &instance;
}
