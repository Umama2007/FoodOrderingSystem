#include "admin.h"
#include <QDebug>

Admin::Admin(QString uname, QString pass, QString code)
    : User(uname, pass, "admin"), adminCode(code) {}

void Admin::showDashboard()
{
    qDebug() << "Admin Dashboard for:" << username;
}

QString Admin::getInfo() const
{
    return username + "," + password + ",,," + "admin";
}