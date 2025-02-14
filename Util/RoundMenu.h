#ifndef ROUNDMENU_H_
#define ROUNDMENU_H_

#include <QWidget>
#include <QMenu>

class RoundMenu :public QMenu
{
public:
    explicit RoundMenu(QWidget* parent = nullptr);
    void init();
protected:
    /*void paintEvent(QPaintEvent* event) override;*/
};


#endif // !ROUNDMENU_H_
