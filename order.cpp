#include "order.h"

Order::Order(QString customer) : customerName(customer) {}

void Order::addItem(MenuItem *item, int qty)
{
    items.append({item, qty});
    total += item->getFinalPrice() * qty;
}

QString Order::getSummary() const
{
    // If loaded from file, return stored raw text
    if (!rawSummary.isEmpty())
    {
        return "📦 Order  (" + date + ")\n" + rawSummary;
    }

    // Otherwise build from items
    QString s;
    for (const auto &p : items)
    {
        s += "ITEM," + p.first->getName() + "," + QString::number(p.second) + "," + QString::number(p.first->getFinalPrice()) + "\n";
    }
    s += "TOTAL," + QString::number(total, 'f', 2) + "\n";
    return s;
}

QString Order::getCustomerName() const { return customerName; }
QString Order::getDate() const { return date; }
void Order::setRawSummary(QString raw) { rawSummary = raw; }
void Order::setDate(QString d) { date = d; }