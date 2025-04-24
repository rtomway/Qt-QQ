#ifndef PACKETCREATE
#define PACKETCREATE

#include <QByteArray>
#include <QJsonObject>

struct ParsedPacket {
	QString type;
	QJsonObject params;
	QByteArray data;
};

class PacketCreate
{
public:
	//打包
	static QString textPacket(const QString& type, const QJsonObject& params);
	static QByteArray binaryPacket(const QString& type, const QVariantMap& params, const QByteArray& data);
	static QByteArray lenthOfbinaryPacket(qint32 packetSize);
	static void addPacket(QByteArray& targetData, const QByteArray& packet);
	static QByteArray allBinaryPacket(const QByteArray& packet);
	//解析
	static QList<ParsedPacket> parseDataPackets(const QByteArray& allData);
};

#endif // !PACKETCREATE
