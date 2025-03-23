#ifndef CREATE_H_
#define CREATE_H_

#include <QString>

class CreateId
{
public:
	enum Id
	{
		User,
		Group
	};
	static QString getRandomID(int length);
	static QString generateUserID(Id type);

};

#endif // !CREATE_H_
