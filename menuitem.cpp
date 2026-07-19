#include "menuitem.h"

MenuItem::MenuItem(int id, QString name, double price, QString category)
    : id(id), name(name), price(price), category(category) {}

int MenuItem::getId() const { return id; }
QString MenuItem::getName() const { return name; }
double MenuItem::getPrice() const { return price; }
QString MenuItem::getCategory() const { return category; }

void MenuItem::setPrice(double p) { price = p; }
void MenuItem::setName(QString n) { name = n; }
void MenuItem::setCategory(QString c) { category = c; }

double MenuItem::getFinalPrice() const { return price; }
QString MenuItem::getDisplayInfo() const
{
    return QString("[%1] %2 - Rs.%3 (%4)")
        .arg(id)
        .arg(name)
        .arg(price, 0, 'f', 2)
        .arg(category);
}