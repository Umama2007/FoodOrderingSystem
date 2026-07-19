#include "user.h"

User::User(QString uname, QString pass, QString role)
    : username(uname), password(pass), role(role) {}

QString User::getUsername() const { return username; }
QString User::getPassword() const { return password; }
QString User::getRole() const    { return role; }