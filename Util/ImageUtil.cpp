#include "ImageUtil.h"
#include <QPainter>
#include <QPainterPath>


QPixmap ImageUtils::roundedPixmap(const QPixmap& pixmap, QSize size, int radius)
{
	QPixmap scaled = pixmap.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
	QPixmap dest(size);
	dest.fill(Qt::transparent);
	QPainter painter(&dest);
	painter.setRenderHint(QPainter::Antialiasing);
	QPainterPath path;
	path.addRoundedRect(0, 0, size.width(), size.height(), radius, radius);
	painter.setClipPath(path);
	painter.drawPixmap(0, 0, scaled);
	return dest;
}
QPixmap ImageUtils::roundedPixmap(const QPixmap& pixmap, QSize size)
{
	int radius = qMin(size.width(), size.height()) / 2;
	return roundedPixmap(pixmap, size, radius);
}