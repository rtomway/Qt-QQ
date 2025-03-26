#ifndef GROUP_H_
#define GROUP_H_

#include <QString>
#include <QHash>
#include <QSet>
#include <QPixmap>
#include <QJsonObject>

struct Member
{
    QString member_id;
    QString member_name;
    QString member_avatarPath;
};

class Group
{
public:
    void setGroup(const QJsonObject& groupObj);
    void addMember(const QJsonObject& memberObj);
    void setMemberName(QString& member_id,QString& member_name);
    void removeMember(const QString& member_id);
private:
    QString m_groupId;                      
    QString m_groupName;                    
    QString m_groupOwnerId;                 
    QHash<QString, Member>m_members;
};

#endif // !GROUP_H_
