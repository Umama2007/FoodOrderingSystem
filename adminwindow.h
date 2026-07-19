#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QVector>
#include "menuitem.h"

class AdminWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AdminWindow(QString username, QWidget *parent = nullptr);
    ~AdminWindow();

private:
    QString adminUser;
    QVector<MenuItem *> menu;
    QTableWidget *menuTable = nullptr;
    QLabel *statLabel = nullptr;
    void buildUI();
    void loadMenu();
    void refreshTable();
    void addItem();
    void editItem();
    void deleteItem();
    void viewOrders();
};
#endif