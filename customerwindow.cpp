#include "customerwindow.h"
#include "filehandler.h"
#include "specialmenuitem.h"
#include "order.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QScrollBar>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QPixmap>
#include <QDialog>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QDateTime>
#include <algorithm>

CustomerWindow::CustomerWindow(QString user, QWidget *parent)
    : QMainWindow(parent), username(user), cart(user)
{
    setWindowTitle("FoodOrder  —  " + user);
    resize(1280, 800);
    setMinimumSize(1000, 640);
    netManager = new QNetworkAccessManager(this);
    menu = FileHandler::loadMenu();
    auto saved = FileHandler::loadCart(username);
    for (auto &p : saved)
        for (auto *m : menu)
            if (m->getId() == p.first.toInt())
            {
                cart.addItem(m, p.second);
                break;
            }
    buildUI();
    refreshGrid();
    refreshCart();
}

CustomerWindow::~CustomerWindow()
{
    for (auto *m : menu)
        delete m;
}

QString CustomerWindow::catEmoji(const QString &c)
{
    if (c == "Burgers")
        return "🍔";
    if (c == "Pizza")
        return "🍕";
    if (c == "Desi")
        return "🍛";
    if (c == "Rice")
        return "🍚";
    if (c == "Drinks")
        return "🥤";
    if (c == "Snacks")
        return "🍟";
    if (c == "Specials")
        return "⭐";
    return "🍽";
}

QString CustomerWindow::catColor(const QString &c)
{
    if (c == "Burgers")
        return "#FFF3E0";
    if (c == "Pizza")
        return "#FCE4EC";
    if (c == "Desi")
        return "#F3E5F5";
    if (c == "Rice")
        return "#E8F5E9";
    if (c == "Drinks")
        return "#E3F2FD";
    if (c == "Snacks")
        return "#FFF9C4";
    if (c == "Specials")
        return "#FFF8E1";
    return "#F5F5F5";
}

void CustomerWindow::loadFoodImage(int itemId, const QString &cat, QLabel *imgLabel)
{
    static QMap<QString, QStringList> urls = {
        {"Burgers", {"https://images.unsplash.com/photo-1568901346375-23c9450c58cd?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1553979459-d2229ba7433b?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1586816001966-79b736744398?w=300&h=160&fit=crop"}},
        {"Pizza", {"https://images.unsplash.com/photo-1565299624946-b28f40a0ae38?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1513104890138-7c749659a591?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1574071318508-1cdbab80d002?w=300&h=160&fit=crop"}},
        {"Desi", {"https://images.unsplash.com/photo-1631515242808-497c3fbd5571?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1574653853027-5382a3d23a15?w=300&h=160&fit=crop"}},
        {"Rice", {"https://images.unsplash.com/photo-1603133872878-684f208fb84b?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1536304993881-ff6e9eefa2a6?w=300&h=160&fit=crop"}},
        {"Drinks", {"https://images.unsplash.com/photo-1544145945-f90425340c7e?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1570197571879-7a672621602d?w=300&h=160&fit=crop"}},
        {"Snacks", {"https://images.unsplash.com/photo-1585670904759-6b8c7dd8f15a?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1566478989037-eec170784d0b?w=300&h=160&fit=crop"}},
        {"Specials", {"https://images.unsplash.com/photo-1567620905732-2d1ec7ab7445?w=300&h=160&fit=crop", "https://images.unsplash.com/photo-1504674900247-0877df9cc836?w=300&h=160&fit=crop"}}};
    QStringList list = urls.value(cat, urls.value("Specials"));
    QString url = list.at(itemId % list.size());
    QNetworkRequest req{QUrl(url)};
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    QPointer<QLabel> safe(imgLabel);
    QNetworkReply *reply = netManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [reply, safe]()
            {
        if (safe && reply->error() == QNetworkReply::NoError) {
            QPixmap pix;
            if (pix.loadFromData(reply->readAll()))
                safe->setPixmap(pix.scaled(safe->size(),
                    Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            safe->setText("");
        }
        reply->deleteLater(); });
}

void CustomerWindow::showItemDetail(MenuItem *item)
{
    QDialog dlg(this);
    dlg.setWindowTitle(item->getName());
    dlg.setFixedSize(400, 420);
    dlg.setStyleSheet("background:white;");

    QVBoxLayout *vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    QLabel *imgLbl = new QLabel(catEmoji(item->getCategory()));
    imgLbl->setFixedSize(400, 160);
    imgLbl->setAlignment(Qt::AlignCenter);
    imgLbl->setStyleSheet("font-size:60px;background:" + catColor(item->getCategory()) + ";");
    loadFoodImage(item->getId(), item->getCategory(), imgLbl);
    vl->addWidget(imgLbl);

    QWidget *info = new QWidget;
    info->setStyleSheet("background:white;");
    QVBoxLayout *il = new QVBoxLayout(info);
    il->setContentsMargins(24, 18, 24, 18);
    il->setSpacing(6);

    if (dynamic_cast<SpecialMenuItem *>(item))
    {
        QLabel *badge = new QLabel("SPECIAL DEAL - Discounted Price!");
        badge->setStyleSheet("background:#FFF3CD;color:#856404;border-radius:8px;"
                             "padding:6px 12px;font-size:11px;font-weight:700;");
        il->addWidget(badge);
        il->addSpacing(4);
    }

    QLabel *name = new QLabel(item->getName());
    name->setStyleSheet("font-size:22px;font-weight:800;color:#1a202c;");
    il->addWidget(name);

    QLabel *cat = new QLabel(catEmoji(item->getCategory()) + "  " + item->getCategory());
    cat->setStyleSheet("font-size:13px;color:#718096;");
    il->addWidget(cat);
    il->addSpacing(10);

    QHBoxLayout *prow = new QHBoxLayout;
    QLabel *price = new QLabel("Rs." + QString::number((int)item->getFinalPrice()));
    price->setStyleSheet("font-size:26px;font-weight:900;color:#00A4A6;");
    prow->addWidget(price);
    prow->addStretch();

    QLabel *qtyLbl = new QLabel("Qty:");
    qtyLbl->setStyleSheet("font-size:13px;color:#4a5568;font-weight:600;");
    QSpinBox *qty = new QSpinBox;
    qty->setRange(1, 20);
    qty->setValue(1);
    qty->setFixedSize(70, 36);
    qty->setStyleSheet("border:2px solid #e2e8f0;border-radius:8px;padding:4px 8px;"
                       "font-size:14px;font-weight:700;color:#2d3748;background:white;");
    prow->addWidget(qtyLbl);
    prow->addWidget(qty);
    il->addLayout(prow);
    il->addSpacing(14);

    MenuItem *mi = item;
    QPushButton *addBtn = new QPushButton("Add to Cart");
    addBtn->setFixedHeight(50);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton{background:qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #00D4D6,stop:1 #00A4A6);color:white;border:none;"
        "border-radius:12px;font-size:16px;font-weight:800;}"
        "QPushButton:hover{background:#008C8E;}");
    connect(addBtn, &QPushButton::clicked, this, [this, mi, qty, &dlg]()
            {
        addToCart(mi, qty->value());
        dlg.accept(); });
    il->addWidget(addBtn);
    vl->addWidget(info, 1);
    dlg.exec();
}

void CustomerWindow::buildUI()
{
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QVBoxLayout *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    buildNavbar(root);
    QWidget *body = new QWidget;
    body->setStyleSheet("background:#F4F7F6;");
    QHBoxLayout *bl = new QHBoxLayout(body);
    bl->setContentsMargins(20, 20, 20, 20);
    bl->setSpacing(16);
    buildLeftPanel(bl);
    buildCartPanel(bl);
    root->addWidget(body, 1);
}

void CustomerWindow::buildNavbar(QVBoxLayout *root)
{
    QWidget *nav = new QWidget;
    nav->setObjectName("navbar");
    nav->setFixedHeight(60);
    QHBoxLayout *nl = new QHBoxLayout(nav);
    nl->setContentsMargins(24, 0, 24, 0);
    nl->setSpacing(0);

    QLabel *logo = new QLabel("  FOODORDER");
    logo->setObjectName("logoLabel");
    nl->addWidget(logo);
    nl->addSpacing(32);

    for (const QString t : {"Dashboard", "Menu", "Orders", "Profile", "Help"})
    {
        QPushButton *btn = new QPushButton(t);
        btn->setFixedHeight(60);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            t == "Dashboard"
                ? "QPushButton{background:transparent;color:#00d4ff;border:none;"
                  "border-bottom:3px solid #00d4ff;font-weight:800;font-size:13px;padding:0 16px;}"
                : "QPushButton{background:transparent;color:#a0aec0;border:none;"
                  "font-size:13px;padding:0 16px;}QPushButton:hover{color:#00d4ff;}");

        if (t == "Menu")
            connect(btn, &QPushButton::clicked, this, [this]()
                    {
                QDialog dlg(this);
                dlg.setWindowTitle("Full Menu");
                dlg.setFixedSize(480, 520);
                dlg.setStyleSheet("background:white;");

                QVBoxLayout *vl = new QVBoxLayout(&dlg);
                vl->setContentsMargins(0, 0, 0, 0);
                vl->setSpacing(0);

                QWidget *hdr = new QWidget;
                hdr->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                                   "stop:0 #0f3460,stop:1 #16213e);");
                hdr->setFixedHeight(64);
                QHBoxLayout *hl = new QHBoxLayout(hdr);
                hl->setContentsMargins(22, 0, 22, 0);
                QLabel *htitle = new QLabel("📋  Full Menu  —  " + QString::number(menu.size()) + " Items");
                htitle->setStyleSheet("font-size:16px;font-weight:800;color:white;");
                hl->addWidget(htitle);
                vl->addWidget(hdr);

                QScrollArea *scroll = new QScrollArea;
                scroll->setWidgetResizable(true);
                scroll->setStyleSheet("border:none;background:white;");
                QWidget *sw = new QWidget;
                sw->setStyleSheet("background:white;");
                QVBoxLayout *sl = new QVBoxLayout(sw);
                sl->setContentsMargins(20, 16, 20, 16);
                sl->setSpacing(8);

                QString lastCat;
                for (auto *m : menu) {
                    if (m->getCategory() != lastCat) {
                        lastCat = m->getCategory();
                        QLabel *catLbl = new QLabel(catEmoji(lastCat) + "  " + lastCat);
                        catLbl->setStyleSheet("font-size:14px;font-weight:800;color:#0096b4;"
                                              "margin-top:10px;padding:4px 0;");
                        sl->addWidget(catLbl);
                        QFrame *line = new QFrame;
                        line->setStyleSheet("background:#e2e8f0;min-height:1px;max-height:1px;border:none;");
                        sl->addWidget(line);
                    }
                    QWidget *row = new QWidget;
                    row->setStyleSheet("background:#f8fafc;border-radius:8px;border:1px solid #edf2f7;");
                    QHBoxLayout *rl2 = new QHBoxLayout(row);
                    rl2->setContentsMargins(12, 8, 12, 8);
                    QLabel *nm = new QLabel(m->getName());
                    nm->setStyleSheet("font-size:13px;font-weight:700;color:#2d3748;");
                    QLabel *pr = new QLabel("Rs." + QString::number((int)m->getFinalPrice()));
                    pr->setStyleSheet("font-size:13px;font-weight:800;color:#00A4A6;");

                    MenuItem *mi = m;
                    QPushButton *addB = new QPushButton("+ Add");
                    addB->setFixedSize(60, 28);
                    addB->setCursor(Qt::PointingHandCursor);
                    addB->setStyleSheet("background:#00b4d8;color:white;border:none;"
                                        "border-radius:8px;font-weight:700;font-size:11px;");
                    connect(addB, &QPushButton::clicked, this, [this, mi]() {
                        addToCart(mi, 1);
                    });
                    rl2->addWidget(nm, 1);
                    rl2->addWidget(pr);
                    rl2->addWidget(addB);
                    sl->addWidget(row);
                }
                sl->addStretch();
                scroll->setWidget(sw);
                vl->addWidget(scroll, 1);

                QPushButton *closeBtn = new QPushButton("Close");
                closeBtn->setFixedHeight(44);
                closeBtn->setCursor(Qt::PointingHandCursor);
                closeBtn->setStyleSheet("background:#e2e8f0;color:#4a5568;border:none;"
                                        "font-size:13px;font-weight:600;");
                connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
                vl->addWidget(closeBtn);
                dlg.exec(); });

        if (t == "Orders")
            connect(btn, &QPushButton::clicked, this, &CustomerWindow::viewOrders);
        if (t == "Profile")
            connect(btn, &QPushButton::clicked, this, &CustomerWindow::showProfile);
        if (t == "Help")
            connect(btn, &QPushButton::clicked, this, &CustomerWindow::showHelp);
        nl->addWidget(btn);
    }
    nl->addStretch();

    QPushButton *notif = new QPushButton("*");
    notif->setFixedSize(38, 38);
    notif->setCursor(Qt::PointingHandCursor);
    notif->setStyleSheet("background:rgba(255,255,255,0.1);border:none;border-radius:19px;font-size:16px;");
    connect(notif, &QPushButton::clicked, this, [this]()
            { QMessageBox::information(this, "Notifications",
                                       "No new notifications!\nYou will be notified about orders and deals."); });
    nl->addWidget(notif);
    nl->addSpacing(8);

    QPushButton *avatar = new QPushButton(username.left(2).toUpper());
    avatar->setFixedSize(38, 38);
    avatar->setCursor(Qt::PointingHandCursor);
    avatar->setStyleSheet("background:#00A4A6;color:white;border:none;"
                          "border-radius:19px;font-weight:800;font-size:13px;");
    connect(avatar, &QPushButton::clicked, this, &CustomerWindow::showProfile);
    nl->addWidget(avatar);
    nl->addSpacing(12);

    QPushButton *logout = new QPushButton("Logout");
    logout->setFixedHeight(32);
    logout->setCursor(Qt::PointingHandCursor);
    logout->setStyleSheet("QPushButton{background:rgba(255,255,255,0.15);color:white;border:none;"
                          "border-radius:8px;padding:0 14px;font-size:12px;}"
                          "QPushButton:hover{background:rgba(255,255,255,0.3);}");
    connect(logout, &QPushButton::clicked, this, [this]()
            {
        FileHandler::clearCart(username);
        close(); });
    nl->addWidget(logout);
    root->addWidget(nav);
}

void CustomerWindow::buildLeftPanel(QHBoxLayout *body)
{
    QWidget *left = new QWidget;
    left->setStyleSheet("background:transparent;");
    QVBoxLayout *ll = new QVBoxLayout(left);
    ll->setContentsMargins(0, 0, 0, 0);
    ll->setSpacing(14);

    QLabel *welcome = new QLabel("Welcome back, " + username + "!");
    welcome->setObjectName("welcomeLabel");
    ll->addWidget(welcome);

    QLabel *sub = new QLabel("Discover your favourite meals - fresh, fast and delicious");
    sub->setStyleSheet("font-size:13px;color:#A0AEC0;");
    ll->addWidget(sub);

    buildSearchRow(ll);
    buildCategoryRow(ll);
    buildFoodScroll(ll);
    body->addWidget(left, 3);
}

void CustomerWindow::buildSearchRow(QVBoxLayout *ll)
{
    QWidget *row = new QWidget;
    row->setStyleSheet("background:transparent;");
    QHBoxLayout *rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 0, 0, 0);
    rl->setSpacing(10);

    QLineEdit *search = new QLineEdit;
    search->setObjectName("searchBar");
    search->setPlaceholderText("Find your favourites...");
    search->setFixedHeight(44);
    connect(search, &QLineEdit::textChanged, this, [this](const QString &q)
            {
        activeCat = q.isEmpty() ? "All" : "__search__:" + q;
        refreshGrid(); });
    rl->addWidget(search, 1);

    QPushButton *priceBtn = new QPushButton("Price Up");
    priceBtn->setObjectName("sortBtn");
    priceBtn->setFixedHeight(44);
    priceBtn->setCursor(Qt::PointingHandCursor);
    connect(priceBtn, &QPushButton::clicked, this, &CustomerWindow::sortByPrice);
    rl->addWidget(priceBtn);

    QPushButton *popBtn = new QPushButton("Popular");
    popBtn->setObjectName("sortBtn");
    popBtn->setFixedHeight(44);
    popBtn->setCursor(Qt::PointingHandCursor);
    connect(popBtn, &QPushButton::clicked, this, &CustomerWindow::sortByPopular);
    rl->addWidget(popBtn);

    ll->addWidget(row);
}

void CustomerWindow::buildCategoryRow(QVBoxLayout *ll)
{
    QWidget *row = new QWidget;
    row->setStyleSheet("background:transparent;");
    QHBoxLayout *rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 0, 0, 0);
    rl->setSpacing(8);

    QButtonGroup *bg = new QButtonGroup(this);
    bg->setExclusive(true);
    for (const QString c : {"All", "Burgers", "Pizza", "Desi", "Rice", "Drinks", "Snacks", "Specials"})
    {
        QPushButton *btn = new QPushButton(catEmoji(c) + "  " + c);
        btn->setObjectName("catBtn");
        btn->setCheckable(true);
        btn->setFixedHeight(34);
        btn->setCursor(Qt::PointingHandCursor);
        if (c == "All")
            btn->setChecked(true);
        bg->addButton(btn);
        connect(btn, &QPushButton::clicked, this, [this, c]()
                {
            activeCat = c;
            refreshGrid(); });
        rl->addWidget(btn);
    }
    rl->addStretch();
    ll->addWidget(row);
}

void CustomerWindow::buildFoodScroll(QVBoxLayout *ll)
{
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet("background:transparent;border:none;");
    gridWidget = new QWidget;
    gridWidget->setStyleSheet("background:transparent;");
    gridLayout = new QGridLayout(gridWidget);
    gridLayout->setContentsMargins(0, 4, 4, 4);
    gridLayout->setSpacing(14);
    scroll->setWidget(gridWidget);
    ll->addWidget(scroll, 1);
}

void CustomerWindow::refreshGrid()
{
    while (gridLayout->count())
    {
        QLayoutItem *it = gridLayout->takeAt(0);
        if (it->widget())
            it->widget()->deleteLater();
        delete it;
    }
    bool isSearch = activeCat.startsWith("__search__:");
    QString q = isSearch ? activeCat.mid(11).toLower() : "";
    int col = 0, row = 0;
    const int COLS = 3;
    for (auto *m : menu)
    {
        bool show = isSearch ? m->getName().toLower().contains(q)
                             : (activeCat == "All" || m->getCategory() == activeCat);
        if (!show)
            continue;
        gridLayout->addWidget(makeFoodCard(m), row, col);
        if (++col >= COLS)
        {
            col = 0;
            row++;
        }
    }
    gridLayout->setRowStretch(row + 1, 1);
}

void CustomerWindow::sortByPrice()
{
    std::sort(menu.begin(), menu.end(), [](MenuItem *a, MenuItem *b)
              { return a->getFinalPrice() < b->getFinalPrice(); });
    refreshGrid();
}

void CustomerWindow::sortByPopular()
{
    std::sort(menu.begin(), menu.end(), [](MenuItem *a, MenuItem *b)
              { return a->getName() < b->getName(); });
    refreshGrid();
}

void CustomerWindow::showProfile()
{
    QMessageBox::information(this, "My Profile",
                             "Username:  " + username + "\n\n"
                                                        "UET Lahore - AI-134L OOP Lab\n\n"
                                                        "Team Members:\n"
                                                        "   Alisha Fatima    (2025-AI-253)\n"
                                                        "   Aiza Kashif      (2025-AI-251)\n"
                                                        "   Ayesha Aslam     (2025-AI-255)\n"
                                                        "   Umama Ume Amen   (2025-AI-264)\n\n"
                                                        "Instructor:  Sir Salman");
}

void CustomerWindow::showHelp()
{
    QMessageBox::information(this, "Help",
                             "HOW TO ORDER:\n\n"
                             "1. Browse or search for food\n"
                             "2. Click a category to filter\n"
                             "3. Click the Menu tab for full list with quick Add buttons\n"
                             "4. Click '+ Add' on any card to add to cart (qty 1)\n"
                             "5. Click a card image to open detail and choose quantity\n"
                             "6. Use − and + in cart to change quantities\n"
                             "7. Click X in cart to remove an item\n"
                             "8. Enter voucher code and press Apply — total updates instantly\n"
                             "9. Click Orders tab to track your order live\n\n"
                             "VOUCHER CODES:\n"
                             "  UETFOOD20 = 20 percent off\n"
                             "  SAVE10    = 10 percent off");
}

QFrame *CustomerWindow::makeFoodCard(MenuItem *item)
{
    QFrame *card = new QFrame;
    card->setObjectName("foodCard");
    card->setFixedSize(214, 272);
    card->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(0);

    QLabel *img = new QLabel(catEmoji(item->getCategory()));
    img->setAlignment(Qt::AlignCenter);
    img->setFixedSize(214, 120);
    img->setStyleSheet("font-size:46px;background:" + catColor(item->getCategory()) +
                       ";border-radius:14px 14px 0 0;");
    cl->addWidget(img);
    loadFoodImage(item->getId(), item->getCategory(), img);

    if (dynamic_cast<SpecialMenuItem *>(item))
    {
        QLabel *badge = new QLabel("SPECIAL DEAL");
        badge->setStyleSheet("background:#FF7043;color:white;font-size:10px;"
                             "font-weight:700;padding:3px 10px;");
        cl->addWidget(badge);
    }

    QWidget *cnt = new QWidget;
    cnt->setStyleSheet("background:transparent;");
    QVBoxLayout *vl = new QVBoxLayout(cnt);
    vl->setContentsMargins(12, 8, 12, 10);
    vl->setSpacing(2);

    QLabel *name = new QLabel(item->getName());
    name->setObjectName("foodName");
    name->setWordWrap(true);
    vl->addWidget(name);

    QLabel *cat = new QLabel(catEmoji(item->getCategory()) + "  " + item->getCategory());
    cat->setObjectName("foodCat");
    vl->addWidget(cat);
    vl->addSpacing(4);

    QHBoxLayout *pb = new QHBoxLayout;
    pb->setContentsMargins(0, 0, 0, 0);
    pb->setSpacing(0);

    QLabel *price = new QLabel("Rs." + QString::number((int)item->getFinalPrice()));
    price->setObjectName("foodPrice");
    pb->addWidget(price);
    pb->addStretch();

    MenuItem *mi = item;
    QPushButton *addBtn = new QPushButton("+ Add");
    addBtn->setObjectName("addCartBtn");
    addBtn->setFixedHeight(30);
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, [this, mi]()
            { addToCart(mi, 1); });
    pb->addWidget(addBtn);
    vl->addLayout(pb);
    cl->addWidget(cnt);

    QPushButton *overlay = new QPushButton(card);
    overlay->setGeometry(0, 0, 214, 180);
    overlay->setStyleSheet("background:transparent;border:none;");
    overlay->setCursor(Qt::PointingHandCursor);
    overlay->lower();
    addBtn->raise();
    connect(overlay, &QPushButton::clicked, this, [this, mi]()
            { showItemDetail(mi); });
    return card;
}

void CustomerWindow::buildCartPanel(QHBoxLayout *body)
{
    QFrame *panel = new QFrame;
    panel->setStyleSheet("QFrame{background:white;border-radius:16px;border:1px solid #EDF2F7;}");
    QVBoxLayout *rl = new QVBoxLayout(panel);
    rl->setContentsMargins(0, 0, 0, 0);
    rl->setSpacing(0);

    QWidget *hdr = new QWidget;
    hdr->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                       "stop:0 #0f3460,stop:1 #16213e);border-radius:14px 14px 0 0;");
    QHBoxLayout *hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(16, 14, 16, 14);
    QLabel *ctitle = new QLabel("Your Cart");
    ctitle->setStyleSheet("font-size:16px;font-weight:800;color:white;background:transparent;");
    cartBadge = new QLabel("0");
    cartBadge->setStyleSheet("background:#00A4A6;color:white;border-radius:12px;"
                             "padding:2px 10px;font-size:11px;font-weight:700;");
    hl->addWidget(ctitle);
    hl->addStretch();
    hl->addWidget(cartBadge);
    rl->addWidget(hdr);

    QScrollArea *cs = new QScrollArea;
    cs->setWidgetResizable(true);
    cs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cs->setStyleSheet("background:white;border:none;");
    cs->setFixedHeight(240);
    cartContent = new QWidget;
    cartContent->setStyleSheet("background:white;");
    cartLayout = new QVBoxLayout(cartContent);
    cartLayout->setContentsMargins(8, 8, 8, 8);
    cartLayout->setSpacing(6);
    cs->setWidget(cartContent);
    rl->addWidget(cs);

    // ── Summary rows ──
    QWidget *sumW = new QWidget;
    sumW->setStyleSheet("background:white;border-top:1px solid #EDF2F7;");
    QVBoxLayout *sl = new QVBoxLayout(sumW);
    sl->setContentsMargins(16, 12, 16, 8);
    sl->setSpacing(4);
    subtotalLabel = makeSummaryRow(sl, "Subtotal:");
    taxLabel = makeSummaryRow(sl, "Tax (16%):");
    discountLabel = makeSummaryRow(sl, "Discount:"); // FIX: shows applied discount
    QFrame *div1 = new QFrame;
    div1->setStyleSheet("background:#EDF2F7;min-height:1px;max-height:1px;border:none;");
    sl->addWidget(div1);
    totalLabel = makeSummaryRow(sl, "TOTAL:", true);
    rl->addWidget(sumW);

    // ── Voucher row ──
    QWidget *vW = new QWidget;
    vW->setStyleSheet("background:white;");
    QHBoxLayout *vl = new QHBoxLayout(vW);
    vl->setContentsMargins(14, 6, 14, 6);
    vl->setSpacing(8);
    QLineEdit *voucher = new QLineEdit;
    voucher->setPlaceholderText("Voucher code...");
    voucher->setFixedHeight(36);
    voucher->setStyleSheet("border:1.5px solid #E2E8F0;border-radius:9px;"
                           "padding:6px 12px;font-size:12px;background:#F8FAFC;color:#4a5568;");
    QPushButton *applyV = new QPushButton("Apply");
    applyV->setFixedHeight(36);
    applyV->setCursor(Qt::PointingHandCursor);
    applyV->setStyleSheet("background:#E6F7F7;color:#00A4A6;border:none;"
                          "border-radius:9px;padding:6px 14px;font-weight:700;font-size:12px;");

    // ── FIX: voucher actually changes discountPct and refreshes totals ──
    connect(applyV, &QPushButton::clicked, this, [this, voucher]()
            {
                QString code = voucher->text().trimmed().toUpper();
                if (code == "UETFOOD20")
                {
                    discountPct = 20.0;
                    QMessageBox::information(this, "Voucher Applied!", "20% discount applied!\nYour total has been updated.");
                }
                else if (code == "SAVE10")
                {
                    discountPct = 10.0;
                    QMessageBox::information(this, "Voucher Applied!", "10% discount applied!\nYour total has been updated.");
                }
                else
                {
                    QMessageBox::warning(this, "Invalid Code", "Invalid voucher code.\nTry: UETFOOD20 or SAVE10");
                    return;
                }
                refreshCart(); // re-draw totals with discount
            });

    vl->addWidget(voucher, 1);
    vl->addWidget(applyV);
    rl->addWidget(vW);

    QWidget *btnW = new QWidget;
    btnW->setStyleSheet("background:white;");
    QVBoxLayout *bvl = new QVBoxLayout(btnW);
    bvl->setContentsMargins(14, 4, 14, 8);
    bvl->setSpacing(8);

    QPushButton *placeBtn = new QPushButton("PLACE ORDER");
    placeBtn->setFixedHeight(50);
    placeBtn->setCursor(Qt::PointingHandCursor);
    placeBtn->setStyleSheet(
        "QPushButton{background:qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #00D4D6,stop:1 #00A4A6);color:white;border:none;"
        "border-radius:12px;font-size:15px;font-weight:800;}"
        "QPushButton:hover{background:#008C8E;}");
    connect(placeBtn, &QPushButton::clicked, this, &CustomerWindow::placeOrder);
    bvl->addWidget(placeBtn);

    QPushButton *ordBtn = new QPushButton("My Order History");
    ordBtn->setFixedHeight(36);
    ordBtn->setCursor(Qt::PointingHandCursor);
    ordBtn->setStyleSheet("background:transparent;color:#00A4A6;border:1.5px solid #00A4A6;"
                          "border-radius:10px;padding:8px;font-size:12px;font-weight:700;");
    connect(ordBtn, &QPushButton::clicked, this, &CustomerWindow::viewOrders);
    bvl->addWidget(ordBtn);
    rl->addWidget(btnW);

    buildRecommendations(rl);
    rl->addStretch();
    body->addWidget(panel, 1);
}

QLabel *CustomerWindow::makeSummaryRow(QVBoxLayout *parent, const QString &lbl, bool big)
{
    QHBoxLayout *r = new QHBoxLayout;
    r->setContentsMargins(0, 0, 0, 0);
    QLabel *l = new QLabel(lbl);
    l->setStyleSheet(big ? "font-size:14px;font-weight:800;color:#00A4A6;"
                         : "font-size:12px;color:#718096;");
    QLabel *v = new QLabel("Rs.0");
    v->setStyleSheet(big ? "font-size:16px;font-weight:900;color:#00A4A6;"
                         : "font-size:12px;font-weight:700;color:#2d3748;");
    r->addWidget(l);
    r->addStretch();
    r->addWidget(v);
    parent->addLayout(r);
    return v;
}

void CustomerWindow::buildRecommendations(QVBoxLayout *rl)
{
    QFrame *div = new QFrame;
    div->setStyleSheet("background:#EDF2F7;min-height:1px;max-height:1px;border:none;");
    QWidget *recW = new QWidget;
    recW->setStyleSheet("background:white;");
    QVBoxLayout *rv = new QVBoxLayout(recW);
    rv->setContentsMargins(16, 12, 16, 14);
    rv->setSpacing(10);
    QLabel *rt = new QLabel("Recommended for You");
    rt->setStyleSheet("font-size:13px;font-weight:700;color:#1a202c;");
    rv->addWidget(rt);

    QScrollArea *rs = new QScrollArea;
    rs->setWidgetResizable(true);
    rs->setStyleSheet("background:transparent;border:none;");
    rs->setFixedHeight(126);
    rs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget *rw = new QWidget;
    rw->setStyleSheet("background:transparent;");
    QHBoxLayout *rh = new QHBoxLayout(rw);
    rh->setContentsMargins(0, 0, 0, 0);
    rh->setSpacing(8);
    int shown = 0;
    for (auto *m : menu)
    {
        if (shown++ >= 4)
            break;
        rh->addWidget(makeRecCard(m));
    }
    rh->addStretch();
    rs->setWidget(rw);
    rv->addWidget(rs);
    rl->addWidget(div);
    rl->addWidget(recW);
}

QFrame *CustomerWindow::makeRecCard(MenuItem *item)
{
    QFrame *card = new QFrame;
    card->setStyleSheet(
        "QFrame{background:#F8FAFC;border-radius:12px;border:1px solid #EDF2F7;}"
        "QFrame:hover{border-color:#00A4A6;}");
    card->setFixedSize(88, 112);
    card->setCursor(Qt::PointingHandCursor);
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(6, 6, 6, 6);
    cl->setSpacing(3);

    QLabel *em = new QLabel(catEmoji(item->getCategory()));
    em->setAlignment(Qt::AlignCenter);
    em->setStyleSheet("font-size:26px;background:" + catColor(item->getCategory()) +
                      ";border-radius:8px;padding:4px;");
    em->setFixedHeight(54);
    cl->addWidget(em);
    loadFoodImage(item->getId(), item->getCategory(), em);

    QLabel *nm = new QLabel(item->getName());
    nm->setStyleSheet("font-size:10px;font-weight:700;color:#2d3748;");
    nm->setWordWrap(true);
    nm->setAlignment(Qt::AlignCenter);
    cl->addWidget(nm);

    QLabel *pr = new QLabel("Rs." + QString::number((int)item->getFinalPrice()));
    pr->setStyleSheet("font-size:10px;color:#00A4A6;font-weight:600;");
    pr->setAlignment(Qt::AlignCenter);
    cl->addWidget(pr);

    MenuItem *mi = item;
    QPushButton *ov = new QPushButton(card);
    ov->setStyleSheet("background:transparent;border:none;");
    ov->setGeometry(0, 0, 88, 112);
    ov->setCursor(Qt::PointingHandCursor);
    connect(ov, &QPushButton::clicked, this, [this, mi]()
            { showItemDetail(mi); });
    return card;
}

void CustomerWindow::addToCart(MenuItem *item, int qty)
{
    cart.addItem(item, qty);
    QVector<QPair<QString, int>> cs;
    for (auto &p : cart.getItems())
        cs.append({QString::number(p.first->getId()), p.second});
    FileHandler::saveCart(username, cs);
    refreshCart();
}

void CustomerWindow::removeFromCart(int id)
{
    cart.removeItem(id);
    QVector<QPair<QString, int>> cs;
    for (auto &p : cart.getItems())
        cs.append({QString::number(p.first->getId()), p.second});
    FileHandler::saveCart(username, cs);
    refreshCart();
}

void CustomerWindow::refreshCart()
{
    while (cartLayout->count())
    {
        QLayoutItem *it = cartLayout->takeAt(0);
        if (it->widget())
            it->widget()->deleteLater();
        delete it;
    }
    auto &items = cart.getItems();
    if (items.isEmpty())
    {
        QLabel *em = new QLabel("Cart is empty!\nBrowse and add items.");
        em->setStyleSheet("font-size:13px;color:#CBD5E0;");
        em->setAlignment(Qt::AlignCenter);
        cartLayout->addStretch();
        cartLayout->addWidget(em);
        cartLayout->addStretch();
    }
    else
    {
        for (const auto &p : items)
        {
            MenuItem *m = p.first;
            int qty = p.second;
            int lineTotal = (int)(m->getFinalPrice() * qty);

            QWidget *row = new QWidget;
            row->setStyleSheet("background:#F8FAFC;border-radius:10px;border:1px solid #EDF2F7;");
            QHBoxLayout *rl2 = new QHBoxLayout(row);
            rl2->setContentsMargins(8, 7, 8, 7);
            rl2->setSpacing(6);

            QLabel *emo = new QLabel(catEmoji(m->getCategory()));
            emo->setFixedSize(32, 32);
            emo->setAlignment(Qt::AlignCenter);
            emo->setStyleSheet("font-size:18px;background:" + catColor(m->getCategory()) +
                               ";border-radius:8px;");
            rl2->addWidget(emo);

            QVBoxLayout *info = new QVBoxLayout;
            info->setSpacing(1);
            QLabel *nm = new QLabel(m->getName());
            nm->setStyleSheet("font-size:12px;font-weight:700;color:#2d3748;");
            QLabel *sb = new QLabel("x" + QString::number(qty) +
                                    "  x  Rs." + QString::number((int)m->getFinalPrice()));
            sb->setStyleSheet("font-size:11px;color:#A0AEC0;");
            info->addWidget(nm);
            info->addWidget(sb);
            rl2->addLayout(info, 1);

            QLabel *pr = new QLabel("Rs." + QString::number(lineTotal));
            pr->setStyleSheet("font-size:12px;font-weight:700;color:#00A4A6;");
            rl2->addWidget(pr);

            int fid = m->getId();
            int curQty = qty;

            QPushButton *minusBtn = new QPushButton("−");
            minusBtn->setFixedSize(26, 26);
            minusBtn->setCursor(Qt::PointingHandCursor);
            minusBtn->setStyleSheet("background:#FED7D7;color:#E53E3E;border:none;"
                                    "border-radius:13px;font-weight:900;font-size:14px;");
            connect(minusBtn, &QPushButton::clicked, this, [this, fid, curQty]()
                    {
                if (curQty <= 1) {
                    removeFromCart(fid);
                } else {
                    cart.removeItem(fid);
                    for (auto *m2 : menu)
                        if (m2->getId() == fid) { cart.addItem(m2, curQty - 1); break; }
                    QVector<QPair<QString,int>> cs;
                    for (auto &p2 : cart.getItems())
                        cs.append({QString::number(p2.first->getId()), p2.second});
                    FileHandler::saveCart(username, cs);
                    refreshCart();
                } });

            QPushButton *plusBtn = new QPushButton("+");
            plusBtn->setFixedSize(26, 26);
            plusBtn->setCursor(Qt::PointingHandCursor);
            plusBtn->setStyleSheet("background:#C6F6D5;color:#276749;border:none;"
                                   "border-radius:13px;font-weight:900;font-size:14px;");
            connect(plusBtn, &QPushButton::clicked, this, [this, fid]()
                    {
                for (auto *m2 : menu)
                    if (m2->getId() == fid) { addToCart(m2, 1); break; } });

            QPushButton *rm = new QPushButton("x");
            rm->setFixedSize(26, 26);
            rm->setCursor(Qt::PointingHandCursor);
            rm->setStyleSheet("background:#718096;color:white;border:none;"
                              "border-radius:13px;font-weight:900;font-size:11px;");
            connect(rm, &QPushButton::clicked, this, [this, fid]()
                    { removeFromCart(fid); });

            rl2->addWidget(minusBtn);
            rl2->addWidget(plusBtn);
            rl2->addWidget(rm);
            cartLayout->addWidget(row);
        }
        cartLayout->addStretch();
    }

    // ── FIX: apply discount to displayed totals ──
    double sub = cart.getSubtotal();
    double discAmt = sub * (discountPct / 100.0);
    double discSub = sub - discAmt;
    double tax = discSub * 0.16;
    double total = discSub + tax;

    cartBadge->setText(QString::number(cart.totalItems()));
    subtotalLabel->setText("Rs." + QString::number((int)sub));
    taxLabel->setText("Rs." + QString::number((int)tax));
    totalLabel->setText("Rs." + QString::number((int)total));

    if (discountPct > 0.0)
    {
        discountLabel->setText("-Rs." + QString::number((int)discAmt) +
                               "  (" + QString::number((int)discountPct) + "% off)");
        discountLabel->setStyleSheet("font-size:12px;font-weight:700;color:#38A169;");
    }
    else
    {
        discountLabel->setText("Rs.0");
        discountLabel->setStyleSheet("font-size:12px;font-weight:700;color:#2d3748;");
    }
}

void CustomerWindow::placeOrder()
{
    if (cart.empty())
    {
        QMessageBox::information(this, "Cart Empty", "Add some items first!");
        return;
    }
    Order order(username);
    for (auto &p : cart.getItems())
        order.addItem(p.first, p.second);
    FileHandler::saveOrder(order);

    // calculate discounted total for receipt
    double sub = cart.getSubtotal();
    double discAmt = sub * (discountPct / 100.0);
    double discSub = sub - discAmt;
    double tax = discSub * 0.16;
    double total = discSub + tax;

    QString receipt = "Order Placed Successfully!\n\n";
    for (auto &p : cart.getItems())
        receipt += catEmoji(p.first->getCategory()) + "  " + p.first->getName() +
                   "  x" + QString::number(p.second) +
                   "  Rs." + QString::number((int)(p.first->getFinalPrice() * p.second)) + "\n";
    receipt += "\nSubtotal:  Rs." + QString::number((int)sub);
    if (discountPct > 0.0)
        receipt += "\nDiscount:  -Rs." + QString::number((int)discAmt) +
                   "  (" + QString::number((int)discountPct) + "% off)";
    receipt += "\nTax:       Rs." + QString::number((int)tax);
    receipt += "\nTOTAL:     Rs." + QString::number((int)total);
    receipt += "\n\nThank you, " + username + "! Your food is on its way!";
    QMessageBox::information(this, "Order Confirmed!", receipt);

    discountPct = 0.0; // reset discount after order
    cart.clear();
    FileHandler::clearCart(username);
    refreshCart();
}

// ══════════════════════════════════════════════════════════════
//  ORDER TRACKING
//  Status is simulated from time elapsed since order date:
//    0-3 min   → Pending
//    3-8 min   → Preparing
//    8-20 min  → Out for Delivery
//    >20 min   → Delivered
// ══════════════════════════════════════════════════════════════
void CustomerWindow::viewOrders()
{
    auto orders = FileHandler::loadOrders();

    // collect only this user's orders
    QVector<Order> mine;
    for (auto &o : orders)
        if (o.getCustomerName() == username)
            mine.append(o);

    QDialog dlg(this);
    dlg.setWindowTitle("My Orders");
    dlg.setFixedSize(520, 560);
    dlg.setStyleSheet("background:white;");

    QVBoxLayout *vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    // Header
    QWidget *hdr = new QWidget;
    hdr->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                       "stop:0 #0f3460,stop:1 #16213e);");
    hdr->setFixedHeight(64);
    QHBoxLayout *hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(22, 0, 22, 0);
    QLabel *htitle = new QLabel("📦  My Orders");
    htitle->setStyleSheet("font-size:17px;font-weight:800;color:white;");
    QLabel *hcount = new QLabel(QString::number(mine.size()) + " order(s)");
    hcount->setStyleSheet("font-size:12px;color:#90cdf4;");
    hl->addWidget(htitle);
    hl->addStretch();
    hl->addWidget(hcount);
    vl->addWidget(hdr);

    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("border:none;background:#f7fafc;");
    QWidget *sw = new QWidget;
    sw->setStyleSheet("background:#f7fafc;");
    QVBoxLayout *sl = new QVBoxLayout(sw);
    sl->setContentsMargins(16, 16, 16, 16);
    sl->setSpacing(12);

    if (mine.isEmpty())
    {
        QLabel *em = new QLabel("No orders yet.\nPlace your first order!");
        em->setAlignment(Qt::AlignCenter);
        em->setStyleSheet("font-size:14px;color:#a0aec0;padding:40px;");
        sl->addWidget(em);
    }
    else
    {
        // show newest first
        for (int i = mine.size() - 1; i >= 0; --i)
        {
            const Order &o = mine[i];

            // ── compute status from elapsed time ──
            QDateTime orderTime = QDateTime::fromString(o.getDate(), "yyyy-MM-dd hh:mm:ss");
            qint64 secsAgo = orderTime.secsTo(QDateTime::currentDateTime());

            QString statusText, statusColor, statusBg, stepLabel;
            int stepNum = 1;
            if (secsAgo < 3 * 60)
            {
                statusText = "Pending";
                statusColor = "#744210";
                statusBg = "#FEFCBF";
                stepLabel = "Step 1 / 4";
                stepNum = 1;
            }
            else if (secsAgo < 8 * 60)
            {
                statusText = "Preparing";
                statusColor = "#1a365d";
                statusBg = "#BEE3F8";
                stepLabel = "Step 2 / 4";
                stepNum = 2;
            }
            else if (secsAgo < 20 * 60)
            {
                statusText = "Out for Delivery";
                statusColor = "#7b341e";
                statusBg = "#FEEBC8";
                stepLabel = "Step 3 / 4";
                stepNum = 3;
            }
            else
            {
                statusText = "Delivered";
                statusColor = "#1c4532";
                statusBg = "#C6F6D5";
                stepLabel = "Step 4 / 4";
                stepNum = 4;
            }

            // ── order card ──
            QFrame *card = new QFrame;
            card->setStyleSheet("QFrame{background:white;border-radius:12px;"
                                "border:1px solid #e2e8f0;}");
            QVBoxLayout *cl = new QVBoxLayout(card);
            cl->setContentsMargins(16, 14, 16, 14);
            cl->setSpacing(10);

            // date + status badge on same row
            QHBoxLayout *topRow = new QHBoxLayout;
            QLabel *dateLbl = new QLabel("🗓  " + o.getDate());
            dateLbl->setStyleSheet("font-size:12px;color:#718096;font-weight:600;");
            topRow->addWidget(dateLbl);
            topRow->addStretch();
            QLabel *badge = new QLabel("  " + statusText + "  ");
            badge->setStyleSheet("background:" + statusBg + ";color:" + statusColor + ";"
                                                                                      "border-radius:10px;padding:3px 10px;"
                                                                                      "font-size:11px;font-weight:800;");
            topRow->addWidget(badge);
            cl->addLayout(topRow);

            // progress bar  ●──●──○──○
            QHBoxLayout *prog = new QHBoxLayout;
            prog->setSpacing(4);
            QStringList steps = {"Pending", "Preparing", "On the Way", "Delivered"};
            for (int s = 0; s < 4; ++s)
            {
                bool done = (s + 1) <= stepNum;
                QLabel *dot = new QLabel(done ? "●" : "○");
                dot->setStyleSheet(done
                                       ? "font-size:14px;color:#00A4A6;font-weight:900;"
                                       : "font-size:14px;color:#CBD5E0;");
                prog->addWidget(dot);
                QLabel *stLbl = new QLabel(steps[s]);
                stLbl->setStyleSheet(done
                                         ? "font-size:10px;color:#00A4A6;font-weight:700;"
                                         : "font-size:10px;color:#CBD5E0;");
                prog->addWidget(stLbl);
                if (s < 3)
                {
                    QLabel *line = new QLabel("───");
                    line->setStyleSheet(done && (s + 1) < stepNum
                                            ? "color:#00A4A6;font-size:10px;"
                                            : "color:#CBD5E0;font-size:10px;");
                    prog->addWidget(line);
                }
            }
            prog->addStretch();
            cl->addLayout(prog);

            // items from rawSummary
            QString raw = o.getSummary();
            QStringList rawLines = raw.split('\n', Qt::SkipEmptyParts);
            QString itemsText;
            double orderTotal = 0;
            for (const QString &line : rawLines)
            {
                if (line.startsWith("ITEM,"))
                {
                    QStringList p = line.split(',');
                    if (p.size() >= 4)
                    {
                        itemsText += "  • " + p[1] + "  x" + p[2] +
                                     "  Rs." + QString::number((int)p[3].toDouble() * p[2].toInt()) + "\n";
                    }
                }
                else if (line.startsWith("TOTAL,"))
                {
                    orderTotal = line.split(',').value(1).toDouble();
                }
            }
            if (!itemsText.isEmpty())
            {
                QLabel *items = new QLabel(itemsText.trimmed());
                items->setStyleSheet("font-size:12px;color:#4a5568;line-height:1.6;");
                items->setWordWrap(true);
                cl->addWidget(items);
            }

            // total
            if (orderTotal > 0)
            {
                QLabel *tot = new QLabel("Total Paid:  Rs." + QString::number((int)orderTotal));
                tot->setStyleSheet("font-size:13px;font-weight:800;color:#00A4A6;");
                cl->addWidget(tot);
            }

            sl->addWidget(card);
        }
    }

    sl->addStretch();
    scroll->setWidget(sw);
    vl->addWidget(scroll, 1);

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setFixedHeight(44);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background:#e2e8f0;color:#4a5568;border:none;"
                            "font-size:13px;font-weight:600;");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    vl->addWidget(closeBtn);
    dlg.exec();
}
