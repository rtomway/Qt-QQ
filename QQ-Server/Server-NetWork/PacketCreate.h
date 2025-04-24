#ifndef PACKETCREATE
#define PACKETCREATE

#include <QByteArray>
#include <QJsonObject>
#include <QList>

struct ParsedPacket {
	QString type;
	QJsonObject params;
	QByteArray data;
};

class PacketCreate
{
public:
	static QByteArray binaryPacket(const QString& type, const QVariantMap& params, const QByteArray& data);
	static QByteArray lenthOfbinaryPacket(qint32 packetSize);
	static void addPacket(QByteArray& addData, const QByteArray& packet);
	static QByteArray allBinaryPacket(const QByteArray& packet);
	//解析
	static QList<ParsedPacket> parseDataPackets(const QByteArray& allData);
};

#endif // !PACKETCREATE
