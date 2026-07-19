#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "user.h"

class Customer : public User
{
private:
    QString email;
    QString phone;

public:
    Customer(QString uname, QString pass, QString email, QString phone);

    QString getEmail() const;
    QString getPhone() const;

    void showDashboard() override;
    QString getInfo() const override;
};
#endif