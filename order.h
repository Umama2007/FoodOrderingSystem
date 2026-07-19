#ifndef ORDER_H
#define ORDER_H
#include <QString>
#include <QVector>
#include <QPair>
#include "menuitem.h"

class Order
{
public:
    Order(QString customer);

    void addItem(MenuItem *item, int qty);
    QString getSummary() const; // returns ITEM + TOTAL lines for file
    QString getCustomerName() const;
    QString getDate() const;

    // Used by FileHandler when loading from file
    void setRawSummary(QString raw);
    void setDate(QString date);

private:
    QString customerName;
    QString date;
    QString rawSummary;
    QVector<QPair<MenuItem *, int>> items;
    double total = 0;
};
#endif