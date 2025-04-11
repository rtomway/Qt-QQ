#ifndef PACKETCREATE
#define PACKETCREATE

#include <QByteArray>

class PacketCreate
{
public:
	static QByteArray binaryPacket(const QString& type, const QVariantMap& params, const QByteArray& data);
	static QByteArray lenthOfbinaryPacket(qint32 packetSize);
	static void addPacket(QByteArray& targetData, const QByteArray& packet);
	static QByteArray allBinaryPacket(const QByteArray& packet);
};

#endif // !PACKETCREATE
