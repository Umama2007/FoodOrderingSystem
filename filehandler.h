#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QString>
#include <QVector>
#include <QPair>
#include "menuitem.h"
#include "order.h"

class FileHandler
{
public:
    // ── Menu CRUD ──────────────────────────────────────────
    static QVector<MenuItem *> loadMenu();
    static void saveMenu(QVector<MenuItem *> menu);
    static void addMenuItem(MenuItem *item);
    static void deleteMenuItem(int id);

    // ── Users ──────────────────────────────────────────────
    static bool validateUser(QString username, QString password, QString &role);
    static bool userExists(QString username);
    static void saveUser(QString username, QString password,
                         QString fullName, QString phone, QString role);

    // ── Orders ─────────────────────────────────────────────
    static void saveOrder(Order order);
    static QVector<Order> loadOrders();

    // ── Cart ───────────────────────────────────────────────
    static void saveCart(QString username,
                         QVector<QPair<QString, int>> items);
    static QVector<QPair<QString, int>> loadCart(QString username);
    static void clearCart(QString username);

private:
    static QString dataPath();
};

#endif