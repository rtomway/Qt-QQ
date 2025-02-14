
class User{
public:
    // 获取单例实例的静态方法
    static User& getInstance() {
        static User instance;
        return instance;
    }

    // 禁止拷贝构造函数和赋值操作符
    User(const User&) = delete;
    User& operator=(const User&) = delete;

    // 设置用户ID
    void setUserId(const QString& id) {
        userId = id;
        qDebug() << "User ID set to:" << userId;
    }

    // 获取用户ID
    QString getUserId() const {
        return userId;
    }
    // 设置用户名
    void setUserName(const QString& name) {
        username = name;
        qDebug() << "User name set to:" << username;
    }

    // 获取用户头像
    QString getUserName() const {
        return username;
    }

private:
    // 私有构造函数，防止外部实例化
    User() : userId("") {}

    QString userId; // 存储用户ID
    QString username; // 存储用户名
    QString userHead; // 存储用户头像
};