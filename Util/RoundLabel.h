// RoundedLabel.h
#ifndef ROUNDEDLABEL_H
#define ROUNDEDLABEL_H

#include <QLabel>
#include <QPixmap>

class RoundedLabel : public QLabel {
    Q_OBJECT

public:
    explicit RoundedLabel(QWidget* parent = nullptr);
    void setPixmap(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_pixmap;
};

#endif // ROUNDEDLABEL_H