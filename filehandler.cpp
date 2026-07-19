#include "filehandler.h"
#include "specialmenuitem.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>

// ══════════════════════════════════════════════════════════════
//  DATA PATH — resolves to  <projectSource>/data/
//  Works for both Qt Creator debug runs and deployed exe.
// ══════════════════════════════════════════════════════════════
QString FileHandler::dataPath()
{
    // When run from Qt Creator the exe sits 3 dirs below the source root:
    //   build/Desktop_.../debug/   →   ../../../  →  source root
    // Falls back to next to exe if that folder doesn't exist.
    QString candidate = QDir::cleanPath(
        QCoreApplication::applicationDirPath() + "/../../../data");

    if (!QDir(candidate).exists())
        candidate = QCoreApplication::applicationDirPath() + "/data";

    QDir().mkpath(candidate);
    return candidate + "/";
}

// ══════════════════════════════════════════════════════════════
//  MENU
//  menu.txt format (comma-separated):
//    id,name,price,category
//    id,name,price,category,discountPercent,offerLabel   ← SpecialMenuItem
// ══════════════════════════════════════════════════════════════
QVector<MenuItem *> FileHandler::loadMenu()
{
    QVector<MenuItem *> menu;
    QFile f(dataPath() + "menu.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return menu;

    QTextStream in(&f);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        QStringList p = line.split(',');
        if (p.size() < 4)
            continue;

        int id = p[0].toInt();
        QString name = p[1].trimmed();
        double price = p[2].toDouble();
        QString cat = p[3].trimmed();

        if (p.size() >= 6)
        {
            double discount = p[4].toDouble();
            QString label = p[5].trimmed();
            menu.append(new SpecialMenuItem(id, name, price, cat, discount, label));
        }
        else
        {
            menu.append(new MenuItem(id, name, price, cat));
        }
    }
    f.close();
    return menu;
}

void FileHandler::saveMenu(QVector<MenuItem *> menu)
{
    QFile f(dataPath() + "menu.txt");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&f);

    for (auto *m : menu)
    {
        SpecialMenuItem *sm = dynamic_cast<SpecialMenuItem *>(m);
        if (sm)
        {
            out << sm->getId() << ","
                << sm->getName() << ","
                << sm->getPrice() << ","
                << sm->getCategory() << ","
                << sm->getDiscount() << ","
                << sm->getOfferLabel() << "\n";
        }
        else
        {
            out << m->getId() << ","
                << m->getName() << ","
                << m->getPrice() << ","
                << m->getCategory() << "\n";
        }
    }
    f.close();
}

void FileHandler::addMenuItem(MenuItem *item)
{
    auto menu = loadMenu();
    // Assign a new unique id
    int maxId = 0;
    for (auto *m : menu)
        if (m->getId() > maxId)
            maxId = m->getId();
    // We can't change item's id if it's set, so just append it as-is
    menu.append(item);
    saveMenu(menu);
    // Don't delete item here — caller owns it
}

void FileHandler::deleteMenuItem(int id)
{
    auto menu = loadMenu();
    QVector<MenuItem *> updated;
    for (auto *m : menu)
    {
        if (m->getId() == id)
            delete m;
        else
            updated.append(m);
    }
    saveMenu(updated);
    for (auto *m : updated)
        delete m;
}

// ══════════════════════════════════════════════════════════════
//  USERS
//  users.txt format (comma-separated):
//    username,password,fullName,phone,role
// ══════════════════════════════════════════════════════════════
bool FileHandler::validateUser(QString username, QString password, QString &role)
{
    QFile f(dataPath() + "users.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&f);

    while (!in.atEnd())
    {
        QStringList p = in.readLine().trimmed().split(',');
        if (p.size() < 5)
            continue;
        if (p[0].trimmed() == username && p[1].trimmed() == password)
        {
            role = p[4].trimmed();
            f.close();
            return true;
        }
    }
    f.close();
    return false;
}

bool FileHandler::userExists(QString username)
{
    QFile f(dataPath() + "users.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&f);
    while (!in.atEnd())
    {
        QStringList p = in.readLine().trimmed().split(',');
        if (!p.isEmpty() && p[0].trimmed() == username)
        {
            f.close();
            return true;
        }
    }
    f.close();
    return false;
}

void FileHandler::saveUser(QString username, QString password,
                           QString fullName, QString phone, QString role)
{
    QFile f(dataPath() + "users.txt");
    if (!f.open(QIODevice::Append | QIODevice::Text))
        return;
    QTextStream out(&f);
    out << username << "," << password << "," << fullName
        << "," << phone << "," << role << "\n";
    f.close();
}

// ══════════════════════════════════════════════════════════════
//  ORDERS
//  orders.txt format:
//    ---ORDER---
//    customer=username
//    date=2025-06-05 12:30:00
//    ITEM,itemName,qty,unitPrice
//    ITEM,itemName,qty,unitPrice
//    TOTAL,999.00
//    ---END---
// ══════════════════════════════════════════════════════════════
void FileHandler::saveOrder(Order order)
{
    QFile f(dataPath() + "orders.txt");
    if (!f.open(QIODevice::Append | QIODevice::Text))
        return;
    QTextStream out(&f);

    out << "---ORDER---\n";
    out << "customer=" << order.getCustomerName() << "\n";
    out << "date=" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << order.getSummary(); // getSummary writes ITEM lines + TOTAL line
    out << "---END---\n\n";
    f.close();
}

QVector<Order> FileHandler::loadOrders()
{
    QVector<Order> orders;
    QFile f(dataPath() + "orders.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return orders;
    QTextStream in(&f);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line != "---ORDER---")
            continue;

        QString customer, date, rawSummary;
        while (!in.atEnd())
        {
            QString l = in.readLine().trimmed();
            if (l == "---END---")
                break;
            if (l.startsWith("customer="))
                customer = l.mid(9);
            else if (l.startsWith("date="))
                date = l.mid(5);
            else
                rawSummary += l + "\n";
        }

        Order o(customer);
        o.setRawSummary(rawSummary); // Order stores raw text for display
        o.setDate(date);
        orders.append(o);
    }
    f.close();
    return orders;
}

// ══════════════════════════════════════════════════════════════
//  CART
//  cart.txt format — one line per user:
//    username|id:qty|id:qty|...
// ══════════════════════════════════════════════════════════════
void FileHandler::saveCart(QString username,
                           QVector<QPair<QString, int>> items)
{
    // Read existing lines for other users
    QFile f(dataPath() + "cart.txt");
    QStringList lines;
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&f);
        while (!in.atEnd())
        {
            QString l = in.readLine().trimmed();
            if (!l.isEmpty() && !l.startsWith(username + "|"))
                lines.append(l);
        }
        f.close();
    }

    // Build new line for this user
    if (!items.isEmpty())
    {
        QString newLine = username;
        for (auto &p : items)
            newLine += "|" + p.first + ":" + QString::number(p.second);
        lines.append(newLine);
    }

    // Write all back
    if (f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&f);
        for (auto &l : lines)
            out << l << "\n";
        f.close();
    }
}

QVector<QPair<QString, int>> FileHandler::loadCart(QString username)
{
    QVector<QPair<QString, int>> result;
    QFile f(dataPath() + "cart.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return result;

    QTextStream in(&f);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (!line.startsWith(username + "|"))
            continue;

        QStringList parts = line.split('|');
        for (int i = 1; i < parts.size(); ++i)
        {
            QStringList kv = parts[i].split(':');
            if (kv.size() == 2)
                result.append({kv[0], kv[1].toInt()});
        }
        break;
    }
    f.close();
    return result;
}

void FileHandler::clearCart(QString username)
{
    saveCart(username, {}); // save empty list = removes user's line
}