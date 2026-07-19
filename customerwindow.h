#ifndef CUSTOMERWINDOW_H
#define CUSTOMERWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QVector>
#include <QPair>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include "menuitem.h"
#include "cart.h"

class CustomerWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CustomerWindow(QString username, QWidget *parent = nullptr);
    ~CustomerWindow();

private:
    QString username;
    QVector<MenuItem *> menu;
    Cart cart;
    QString activeCat = "All";
    QString sortMode = "default";

    // ── FIX: track applied discount percentage ──
    double discountPct = 0.0;

    QNetworkAccessManager *netManager = nullptr;

    QWidget *gridWidget = nullptr;
    QGridLayout *gridLayout = nullptr;
    QWidget *cartContent = nullptr;
    QVBoxLayout *cartLayout = nullptr;
    QLabel *cartBadge = nullptr;
    QLabel *subtotalLabel = nullptr;
    QLabel *taxLabel = nullptr;
    QLabel *totalLabel = nullptr;
    QLabel *discountLabel = nullptr;

    void buildUI();
    void buildNavbar(QVBoxLayout *root);
    void buildLeftPanel(QHBoxLayout *body);
    void buildSearchRow(QVBoxLayout *left);
    void buildCategoryRow(QVBoxLayout *left);
    void buildFoodScroll(QVBoxLayout *left);
    void buildCartPanel(QHBoxLayout *body);
    void buildRecommendations(QVBoxLayout *cartCol);
    void refreshGrid();
    void refreshCart();
    QFrame *makeFoodCard(MenuItem *item);
    QFrame *makeRecCard(MenuItem *item);
    QLabel *makeSummaryRow(QVBoxLayout *parent, const QString &lbl, bool big = false);
    QString catEmoji(const QString &c);
    QString catColor(const QString &c);
    void addToCart(MenuItem *item, int qty = 1);
    void removeFromCart(int id);
    void placeOrder();
    void viewOrders();
    void showProfile();
    void showHelp();
    void sortByPrice();
    void sortByPopular();
    void loadFoodImage(int itemId, const QString &category, QLabel *imgLabel);
    void showItemDetail(MenuItem *item);
};
#endif
