#ifndef CART_H
#define CART_H
#include <QString>
#include <QVector>
#include <QPair>
#include "menuitem.h"

class Cart
{
public:
    explicit Cart(QString username);

    void addItem(MenuItem *item, int qty = 1);
    void removeItem(int id);

    QVector<QPair<MenuItem *, int>> &getItems();
    const QVector<QPair<MenuItem *, int>> &getItems() const;

    double getSubtotal() const;
    double getTax() const;
    double getTotal() const;
    int totalItems() const;
    bool empty() const;
    void clear();

    QString getUsername() const;

private:
    QString username;
    QVector<QPair<MenuItem *, int>> items;
};
#endif