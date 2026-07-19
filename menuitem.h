#ifndef MENUITEM_H
#define MENUITEM_H
#include <QString>

class MenuItem
{
protected:
    int id;
    QString name;
    double price;
    QString category;

public:
    MenuItem(int id, QString name, double price, QString category);
    virtual ~MenuItem() {}

    int getId() const;
    QString getName() const;
    double getPrice() const;
    QString getCategory() const;

    void setPrice(double p);
    void setName(QString n);
    void setCategory(QString c);

    virtual double getFinalPrice() const;
    virtual QString getDisplayInfo() const;
};
#endif