#ifndef USER_H
#define USER_H
#include <QString>

class User {
protected:
    QString username;
    QString password;
    QString role; // "customer" or "admin"

public:
    User(QString uname, QString pass, QString role);
    virtual ~User() {}

    QString getUsername() const;
    QString getPassword() const;
    QString getRole() const;

    // Pure virtual — forces subclasses to implement (Abstraction)
    virtual void showDashboard() = 0;
    virtual QString getInfo() const = 0;
};
#endif