#ifndef SPECIALMENUITEM_H
#define SPECIALMENUITEM_H
#include "menuitem.h"

class SpecialMenuItem : public MenuItem
{
public:
    SpecialMenuItem(int id, QString name, double price,
                    QString category, double discount, QString label);

    double getFinalPrice() const override;

    double getDiscount() const;
    QString getOfferLabel() const;

private:
    double discountPercent;
    QString offerLabel;
};
#endif