#include "cart.h"

Cart::Cart(QString username) : username(username) {}

void Cart::addItem(MenuItem *item, int qty)
{
    // If item already in cart, increase quantity
    for (auto &p : items)
    {
        if (p.first->getId() == item->getId())
        {
            p.second += qty;
            return;
        }
    }
    items.append({item, qty});
}

void Cart::removeItem(int id)
{
    for (int i = 0; i < items.size(); ++i)
    {
        if (items[i].first->getId() == id)
        {
            items.removeAt(i);
            return;
        }
    }
}

QVector<QPair<MenuItem *, int>> &Cart::getItems()
{
    return items;
}

const QVector<QPair<MenuItem *, int>> &Cart::getItems() const
{
    return items;
}

double Cart::getSubtotal() const
{
    double sum = 0;
    for (const auto &p : items)
        sum += p.first->getFinalPrice() * p.second;
    return sum;
}

double Cart::getTax() const
{
    return getSubtotal() * 0.16;
}

double Cart::getTotal() const
{
    return getSubtotal() + getTax();
}

int Cart::totalItems() const
{
    int cnt = 0;
    for (const auto &p : items)
        cnt += p.second;
    return cnt;
}

bool Cart::empty() const
{
    return items.isEmpty();
}

void Cart::clear()
{
    items.clear();
}

QString Cart::getUsername() const
{
    return username;
}