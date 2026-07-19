#ifndef ADMIN_H
#define ADMIN_H
#include "user.h"

class Admin : public User
{
private:
    QString adminCode;

public:
    Admin(QString uname, QString pass, QString adminCode);
    void showDashboard() override;
    QString getInfo() const override;
};
#endif