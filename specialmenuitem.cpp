#include "specialmenuitem.h"

SpecialMenuItem::SpecialMenuItem(int id, QString name, double price,
                                 QString category,
                                 double discount, QString label)
    : MenuItem(id, name, price, category),
      discountPercent(discount), offerLabel(label)
{
}

double SpecialMenuItem::getFinalPrice() const
{
    return getPrice() * (1.0 - discountPercent / 100.0);
}

double SpecialMenuItem::getDiscount() const { return discountPercent; }
QString SpecialMenuItem::getOfferLabel() const { return offerLabel; }