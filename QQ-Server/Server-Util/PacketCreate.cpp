#include "PacketCreate.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QIODevice>

//单个数据包
QByteArray PacketCreate::binaryPacket(const QString& type, const QVariantMap& params, const QByteArray& data)
{
	// 1️⃣ 构造 JSON 头部（metadata）
	QJsonObject jsonData;
	jsonData["type"] = type;

	QJsonObject paramsObject;
	paramsObject["size"] = data.size();
	for (auto it = params.begin(); it != params.end(); ++it) {
		paramsObject[it.key()] = QJsonValue::fromVariant(it.value());
	}
	jsonData["params"] = paramsObject;
	QJsonDocument doc(jsonData);
	QByteArray headerData = doc.toJson(QJsonDocument::Compact);

	// 2️⃣ 组合数据包（头部长度 + 头部 JSON + 二进制数据）
	QByteArray packet;
	QDataStream stream(&packet, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_6_5);
	stream.setByteOrder(QDataStream::BigEndian);

	// 先写入 JSON 头部大小
	stream << qint32(headerData.size());

	// 再写入 JSON 头部
	stream.writeRawData(headerData.constData(), headerData.size());

	// 最后写入二进制数据
	stream.writeRawData(data.constData(), data.size());

	return packet;
}

//包长
QByteArray PacketCreate::lenthOfbinaryPacket(qint32 packetSize)
{
	QByteArray packetSizeBytes;
	QDataStream sizeStream(&packetSizeBytes, QIODevice::WriteOnly);
	sizeStream.setVersion(QDataStream::Qt_6_5);
	sizeStream.setByteOrder(QDataStream::BigEndian);
	sizeStream << packetSize;
	return packetSizeBytes;
}

//添加包
void PacketCreate::addPacket(QByteArray& addData, const QByteArray& packet)
{
	auto packetSizeBytes = lenthOfbinaryPacket(packet.size());
	addData.append(packetSizeBytes);  // 先加上包长度
	addData.append(packet);       // 再加上包数据
}

//发送的总数据包
QByteArray PacketCreate::allBinaryPacket(const QByteArray& packet)
{
	QByteArray allData;
	qint32 totalSize = sizeof(qint32) + packet.size(); // 计算总大小
	qDebug() << "Server sending total size:" << totalSize;

	QDataStream stream(&allData, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_6_5);
	stream.setByteOrder(QDataStream::BigEndian); // 确保服务器和客户端一致
	stream << totalSize; // 先写入总大小
	stream.writeRawData(packet.constData(), packet.size()); // 再写入用户数据
	return allData;
}

//解析多个数据包
QList<ParsedPacket> PacketCreate::parseDataPackets(const QByteArray& allData)
{
	//存储的结构体列表
	QList<ParsedPacket> parsedPacketList;

	QDataStream stream(allData);
	stream.setByteOrder(QDataStream::BigEndian);
	stream.setVersion(QDataStream::Qt_6_5);
	// 1️⃣ 读取 `allData` 的总大小（4 字节）
	if (allData.size() < sizeof(qint32)) {
		qDebug() << "Incomplete packet: waiting for more data...";
		return parsedPacketList;
	}
	qint32 totalSize;
	stream >> totalSize;
	qDebug() << "Total data size:" << totalSize;

	// 2️⃣ 开始循环解析多个数据包
	while (!stream.atEnd())
	{
		if (allData.size() - stream.device()->pos() < sizeof(qint32)) {
			qDebug() << "Incomplete packet header size!";
			return parsedPacketList;
		}
		// 读取当前数据包的大小
		qint32 packetSize;
		stream >> packetSize;
		if (allData.size() - stream.device()->pos() < packetSize) {
			qDebug() << "Incomplete full packet!";
			return parsedPacketList;
		}
		// 读取 JSON 头部大小
		qint32 headerSize;
		stream >> headerSize;
		// 读取 JSON 头部
		QByteArray headerData(headerSize, Qt::Uninitialized);
		stream.readRawData(headerData.data(), headerSize);
		// 解析 JSON
		QJsonDocument jsonDoc = QJsonDocument::fromJson(headerData);
		if (jsonDoc.isNull()) {
			qDebug() << "Invalid JSON data!";
			return parsedPacketList;
		}
		//数据
		QJsonObject jsonObj = jsonDoc.object();
		QString type = jsonObj["type"].toString();
		QJsonObject params = jsonObj["params"].toObject();
		// 读取二进制数据（图片）
		int imageSize = params["size"].toInt();
		QByteArray imageData(imageSize, Qt::Uninitialized);
		stream.readRawData(imageData.data(), imageSize);
		//存储的结构体
		ParsedPacket parsedPacket;
		parsedPacket.params = params;
		parsedPacket.type = type;
		parsedPacket.data = imageData;
		parsedPacketList.append(parsedPacket);
	}
	return parsedPacketList;
}
