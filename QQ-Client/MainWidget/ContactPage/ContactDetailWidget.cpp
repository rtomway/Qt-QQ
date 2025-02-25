#include "ContactDetailWidget.h"
#include "ContactDetailWidget.h"
#include "ContactDetailWidget.h"
#include <QBoxLayout>
#include "ImageUtil.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include "User.h"

ContactDetailWidget::ContactDetailWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	,m_editDetail(new QLabel(this))
    ,m_exitBtn(new QPushButton(this))
    ,m_headLab(new QLabel(this))
    ,m_nickNameEdit(new LineEditwithButton(this))
    ,m_signaltureEdit(new LineEditwithButton(this))
    ,m_genderEdit(new LineEditwithButton(this)) 
    ,m_birthdayEdit(new LineEditwithButton(this))
    ,m_countryEdit(new LineEditwithButton(this))
    ,m_provinceEdit(new LineEditwithButton(this))
    ,m_areaEdit(new LineEditwithButton(this))
{
    this->setStyleSheet(R"(QWidget{background-color:rgb(240,240,240);border-radius: 10px;})");
    m_headLab->installEventFilter(this);
    qDebug() << "size:" << this->size();
   // this->setWindowFlag(Qt::FramelessWindowHint);
    init();
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_StyleSheet);
  //  this->setFixedSize(480, 600);
}

ContactDetailWidget::~ContactDetailWidget()
{

}

void ContactDetailWidget::init()
{
    auto mlayout = new QVBoxLayout(this);
    mlayout->setSpacing(0);
    mlayout->setContentsMargins(20, 0, 0, 0);

    m_editDetail->setText("编辑资料");
    m_nickNameEdit->setPreLab("昵称");
    m_signaltureEdit->setPreLab("个签");
    m_genderEdit->setPreLab("性别");
    m_genderEdit->addMenuItem("男");
    m_genderEdit->addMenuItem("女");
    m_birthdayEdit->setPreLab("生日");
    m_countryEdit->setPreLab("国家");
    m_provinceEdit->setPreLab("省份");
    m_areaEdit->setPreLab("地区");

    m_genderEdit->setEditEnable(false);
    m_birthdayEdit->setEditEnable(false);
    m_countryEdit->setEditEnable(false);
    m_provinceEdit->setEditEnable(false);
    m_areaEdit->setEditEnable(false);

    m_nickNameEdit->setFixedWidth(this->width()-150);
    m_signaltureEdit->setFixedWidth(this->width()-150);
    m_genderEdit->setFixedWidth(this->width()-150);
    m_birthdayEdit->setFixedWidth(this->width()-150);
    m_countryEdit->setFixedWidth(this->width()-150);
    m_nickNameEdit->setFixedHeight(55);
    m_signaltureEdit->setFixedHeight(55);
    m_genderEdit->setFixedHeight(55);
    m_birthdayEdit->setFixedHeight(55);
    m_countryEdit->setFixedHeight(55);

    //top
    m_exitBtn->setFixedSize(32, 32);
    m_exitBtn->setStyleSheet("border:none");

    m_exitBtn->setIcon(QIcon(":/icon/Resource/Icon/x.png"));
    auto toplayout = new QHBoxLayout;
    toplayout->addStretch();
    toplayout->addWidget(m_editDetail);
    toplayout->addStretch();
    toplayout->addWidget(m_exitBtn);
    mlayout->addLayout(toplayout);

    m_headPix = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/h1.jpg"), QSize(80, 80));
    m_headLab->setPixmap(m_headPix);
    m_headLab->setFixedHeight(90);

    QWidget* headWidget = new QWidget(this);
    headWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    headWidget->setMinimumHeight(100);
    auto headlayout = new QHBoxLayout(headWidget);
    headlayout->addStretch();
    headlayout->addWidget(m_headLab);
    headlayout->addStretch();

    mlayout->addWidget(headWidget);

    QWidget* editWidget = new QWidget(this);
    auto editLayout = new QVBoxLayout(editWidget);
    //editLayout->setSpacing(20);
    editLayout->setContentsMargins(0,0,0,0);
    
    editLayout->addWidget(m_nickNameEdit);
    editLayout->addWidget(m_signaltureEdit);
    editLayout->addWidget(m_genderEdit);
    editLayout->addWidget(m_birthdayEdit);
    editLayout->addWidget(m_countryEdit);

   /* mlayout->addWidget(m_nickNameEdit);
    mlayout->addWidget(m_signaltureEdit);
    mlayout->addWidget(m_genderEdit);
    mlayout->addWidget(m_birthdayEdit);
    mlayout->addWidget(m_countryEdit);*/

    auto hlayout = new QHBoxLayout;
    hlayout->addWidget(m_provinceEdit);
    hlayout->addWidget(m_areaEdit);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    mlayout->addWidget(editWidget);
    mlayout->addLayout(hlayout);

    auto saveLayout = new QHBoxLayout;
    saveLayout->setSpacing(10);
    auto okBtn = new QPushButton("保存", this);
    okBtn->setStyleSheet(
        QString("\
            QPushButton{\
            background-color: #0096FF;\
            color:white;\
            height:40px;\
            border-radius:5px;\
            width:75px;\
            }\
           QPushButton:hover{\
             background-color: #048eee;\
            }\
            "));
    auto cancelBtn = new QPushButton("取消", this);
    cancelBtn->setStyleSheet(
        QString("\
            QPushButton{  \
            border:1px solid gray;\
            background-color:rgb(240,240,240);\
            color:black;\
            height:40px;\
            border-radius:5px;\
            width:75px;}\
            QPushButton:hover{\
              background-color: #c1c1c1  \
            }\
            "\
        ));
    saveLayout->addStretch();
    saveLayout->addWidget(okBtn);
    saveLayout->addWidget(cancelBtn);

    mlayout->addStretch();
    mlayout->addLayout(saveLayout);
    mlayout->addStretch();

    m_genderEdit->setComboBox();
    m_countryEdit->setComboBox();
    m_provinceEdit->setComboBox();
    m_areaEdit->setComboBox(); 

    connect(m_exitBtn, &QPushButton::clicked, [=]
        {
            this->hide();
        });
    connect(cancelBtn, &QPushButton::clicked, [=]
        {
            this->hide();
        });
    connect(okBtn, &QPushButton::clicked, [=]
        {
            saveAvatarToLocal(m_avatarPath, User::instance()->getUserId());
            this->hide();
        });
   
}

QString ContactDetailWidget::getAvatarFolderPath()
{
    //保存目录
    QString avatarFolder = QStandardPaths::writableLocation
                    (QStandardPaths::AppDataLocation)+ "/avatars";
    // 如果目录不存在，则创建
    QDir dir;
    if (!dir.exists(avatarFolder)) {
        dir.mkpath(avatarFolder); 
    }
    return avatarFolder;
}

bool ContactDetailWidget::saveAvatarToLocal(const QString& avatarPath,const QString& user_id)
{
    QString avatarFolderPath = getAvatarFolderPath();
    // 使用用户 ID 来命名头像文件
    QString avatarFileName = avatarFolderPath + "/" + user_id + ".png";

    QPixmap avatar(avatarPath);
    if (avatar.isNull()) {
        qWarning() << "头像加载失败!";
        return false;
    }
    // 保存头像
    return avatar.save(avatarFileName);
}

bool ContactDetailWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_headLab && event->type() == QEvent::MouseButtonPress)
    {
        m_avatarPath = QFileDialog::getOpenFileName(this, "选择头像", "",
                     "Images(*.jpg *.png *.jpeg *.bnp)");
        qDebug() << "选择头像"<< m_avatarPath;
        if (!m_avatarPath.isEmpty())
        {
            QPixmap avatar(m_avatarPath);
            m_headPix = ImageUtils::roundedPixmap(avatar, QSize(80, 80));
            m_headLab->setPixmap(m_headPix);
        }
    }
    return false;
}
