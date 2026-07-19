#include "adminwindow.h"
#include "filehandler.h"
#include "specialmenuitem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialogButtonBox>

AdminWindow::AdminWindow(QString username, QWidget *parent)
    : QMainWindow(parent), adminUser(username)
{
    setWindowTitle("Admin Panel — " + username);
    resize(900, 640);
    setMinimumSize(700, 500);
    buildUI();
    loadMenu();
}

AdminWindow::~AdminWindow()
{
    for (auto *m : menu)
        delete m;
}

void AdminWindow::buildUI()
{
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QVBoxLayout *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Header ─────────────────────────────────────────────
    QWidget *hdr = new QWidget;
    hdr->setObjectName("adminHeader");
    hdr->setFixedHeight(76);
    QHBoxLayout *hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(28, 0, 28, 0);
    QVBoxLayout *htxt = new QVBoxLayout;
    QLabel *title = new QLabel("🛠️  Admin Panel");
    title->setObjectName("adminTitle");
    QLabel *sub = new QLabel("Manage menu items and view customer orders");
    sub->setObjectName("adminSubtitle");
    htxt->addWidget(title);
    htxt->addWidget(sub);
    hl->addLayout(htxt, 1);
    statLabel = new QLabel();
    statLabel->setStyleSheet("color:#00d4ff;font-size:13px;font-weight:700;");
    hl->addWidget(statLabel);
    root->addWidget(hdr);

    // ── Toolbar ────────────────────────────────────────────
    QWidget *toolbar = new QWidget;
    toolbar->setStyleSheet("background:#f7fafc;border-bottom:1px solid #e2e8f0;");
    toolbar->setFixedHeight(60);
    QHBoxLayout *tl = new QHBoxLayout(toolbar);
    tl->setContentsMargins(20, 8, 20, 8);
    tl->setSpacing(10);

    auto makeBtn = [](const QString &label, const QString &color, const QString &hover)
    {
        QPushButton *b = new QPushButton(label);
        b->setFixedHeight(40);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton{background:" + color + ";color:white;border:none;border-radius:10px;"
                                                "padding:0 18px;font-weight:700;font-size:13px;}"
                                                "QPushButton:hover{background:" +
            hover + ";}");
        return b;
    };

    QPushButton *addBtn = makeBtn("＋  Add Item", "#00b4d8", "#0096b4");
    QPushButton *editBtn = makeBtn("✏  Edit Item", "#f6ad55", "#ed8936");
    QPushButton *delBtn = makeBtn("🗑  Delete", "#fc8181", "#e53e3e");
    QPushButton *ordBtn = makeBtn("📦  View Orders", "#68d391", "#48bb78");

    connect(addBtn, &QPushButton::clicked, this, &AdminWindow::addItem);
    connect(editBtn, &QPushButton::clicked, this, &AdminWindow::editItem);
    connect(delBtn, &QPushButton::clicked, this, &AdminWindow::deleteItem);
    connect(ordBtn, &QPushButton::clicked, this, &AdminWindow::viewOrders);

    tl->addWidget(addBtn);
    tl->addWidget(editBtn);
    tl->addWidget(delBtn);
    tl->addStretch();
    tl->addWidget(ordBtn);
    root->addWidget(toolbar);

    // ── Table ──────────────────────────────────────────────
    menuTable = new QTableWidget;
    menuTable->setColumnCount(5);
    menuTable->setHorizontalHeaderLabels({"ID", "Item Name", "Category", "Price (Rs.)", "Type"});
    menuTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    menuTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    menuTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    menuTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    menuTable->setColumnWidth(0, 50);
    menuTable->verticalHeader()->setVisible(false);
    menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    menuTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    menuTable->setAlternatingRowColors(true);
    menuTable->setShowGrid(false);
    menuTable->setStyleSheet(R"(
        QTableWidget{
            background:white; border:none;
            alternate-background-color:#f8fafc;
            font-size:13px; color:#2d3748;
        }
        QTableWidget::item{ padding:12px 14px; }
        QTableWidget::item:selected{ background:#ebf8ff; color:#0096b4; }
        QHeaderView::section{
            background:#f7fafc; border:none;
            border-bottom:2px solid #e2e8f0;
            font-weight:700; font-size:12px;
            color:#718096; padding:10px 14px;
        }
    )");
    connect(menuTable, &QTableWidget::doubleClicked, this, &AdminWindow::editItem);
    root->addWidget(menuTable, 1);
}

void AdminWindow::loadMenu()
{
    for (auto *m : menu)
        delete m;
    menu = FileHandler::loadMenu();
    refreshTable();
}

void AdminWindow::refreshTable()
{
    menuTable->setRowCount(0);
    for (auto *m : menu)
    {
        int r = menuTable->rowCount();
        menuTable->insertRow(r);
        bool isSpecial = (dynamic_cast<SpecialMenuItem *>(m) != nullptr);
        auto cell = [&](int col, const QString &txt, Qt::AlignmentFlag align = Qt::AlignLeft)
        {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align | Qt::AlignVCenter);
            menuTable->setItem(r, col, it);
        };
        cell(0, QString::number(m->getId()), Qt::AlignCenter);
        cell(1, m->getName());
        cell(2, m->getCategory());
        cell(3, "Rs." + QString::number((int)m->getFinalPrice()), Qt::AlignCenter);

        QTableWidgetItem *typeIt = new QTableWidgetItem(isSpecial ? "🏷️  Special Deal" : "Regular");
        typeIt->setForeground(isSpecial ? QColor("#e67e22") : QColor("#718096"));
        typeIt->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        menuTable->setItem(r, 4, typeIt);
        menuTable->setRowHeight(r, 46);
    }
    statLabel->setText(QString("Total Items: %1").arg(menu.size()));
}

// ── ADD ──────────────────────────────────────────────────────
void AdminWindow::addItem()
{
    QDialog dlg(this);
    dlg.setWindowTitle("➕  Add Menu Item");
    dlg.setFixedSize(420, 380);
    dlg.setStyleSheet("background:white;");

    QVBoxLayout *vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(28, 24, 28, 20);
    vl->setSpacing(14);

    QLabel *hdr = new QLabel("Add New Menu Item");
    hdr->setStyleSheet("font-size:18px;font-weight:800;color:#1a202c;");
    vl->addWidget(hdr);

    QFormLayout *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignRight);

    auto fieldStyle = "border:2px solid #e2e8f0;border-radius:10px;padding:9px 14px;"
                      "font-size:13px;background:#f8fafc;color:#2d3748;";

    QLineEdit *nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("e.g. Chicken Shawarma");
    nameEdit->setStyleSheet(fieldStyle);
    form->addRow("Name:", nameEdit);

    QComboBox *catBox = new QComboBox;
    catBox->addItems({"Burgers", "Pizza", "Desi", "Rice", "Drinks", "Snacks", "Specials"});
    catBox->setStyleSheet("border:2px solid #e2e8f0;border-radius:10px;padding:8px 12px;"
                          "font-size:13px;background:#f8fafc;");
    form->addRow("Category:", catBox);

    QDoubleSpinBox *priceBox = new QDoubleSpinBox;
    priceBox->setRange(10, 99999);
    priceBox->setValue(300);
    priceBox->setPrefix("Rs. ");
    priceBox->setDecimals(0);
    priceBox->setStyleSheet(fieldStyle);
    form->addRow("Price:", priceBox);

    QCheckBox *specialChk = new QCheckBox("Mark as Special Deal");
    specialChk->setStyleSheet("font-size:13px;color:#4a5568;");
    form->addRow("", specialChk);

    QDoubleSpinBox *discBox = new QDoubleSpinBox;
    discBox->setRange(1, 90);
    discBox->setValue(10);
    discBox->setSuffix(" %");
    discBox->setDecimals(0);
    discBox->setEnabled(false);
    discBox->setStyleSheet(fieldStyle);
    form->addRow("Discount:", discBox);

    connect(specialChk, &QCheckBox::toggled, discBox, &QDoubleSpinBox::setEnabled);
    vl->addLayout(form);
    vl->addSpacing(10);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    btns->button(QDialogButtonBox::Ok)->setText("Add Item");
    btns->button(QDialogButtonBox::Ok)->setStyleSheet("background:#00b4d8;color:white;border:none;border-radius:10px;"
                                                      "padding:10px 22px;font-weight:700;font-size:13px;");
    btns->button(QDialogButtonBox::Cancel)->setStyleSheet("background:#e2e8f0;color:#4a5568;border:none;border-radius:10px;"
                                                          "padding:10px 18px;font-size:13px;");
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    vl->addWidget(btns);

    if (dlg.exec() != QDialog::Accepted)
        return;
    if (nameEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Missing", "Item name cannot be empty.");
        return;
    }

    int newId = menu.isEmpty() ? 1 : menu.last()->getId() + 1;
    MenuItem *item = nullptr;
    if (specialChk->isChecked())
        item = new SpecialMenuItem(newId, nameEdit->text().trimmed(),
                                   priceBox->value(), catBox->currentText(),
                                   discBox->value(), "Special Offer");
    else
        item = new MenuItem(newId, nameEdit->text().trimmed(),
                            priceBox->value(), catBox->currentText());

    menu.append(item);
    FileHandler::addMenuItem(item);
    refreshTable();
    QMessageBox::information(this, "✅ Added", "\"" + item->getName() + "\" added to menu!");
}

// ── EDIT ─────────────────────────────────────────────────────
void AdminWindow::editItem()
{
    int row = menuTable->currentRow();
    if (row < 0 || row >= menu.size())
    {
        QMessageBox::information(this, "Select Item", "Click a row first, then press Edit.");
        return;
    }
    MenuItem *m = menu[row];

    QDialog dlg(this);
    dlg.setWindowTitle("✏  Edit: " + m->getName());
    dlg.setFixedSize(420, 320);
    dlg.setStyleSheet("background:white;");

    QVBoxLayout *vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(28, 24, 28, 20);
    vl->setSpacing(14);

    QLabel *hdr = new QLabel("Edit Menu Item");
    hdr->setStyleSheet("font-size:18px;font-weight:800;color:#1a202c;");
    vl->addWidget(hdr);

    QFormLayout *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignRight);

    auto fieldStyle = "border:2px solid #e2e8f0;border-radius:10px;padding:9px 14px;"
                      "font-size:13px;background:#f8fafc;color:#2d3748;";

    QLineEdit *nameEdit = new QLineEdit(m->getName());
    nameEdit->setStyleSheet(fieldStyle);
    form->addRow("Name:", nameEdit);

    QComboBox *catBox = new QComboBox;
    catBox->addItems({"Burgers", "Pizza", "Desi", "Rice", "Drinks", "Snacks", "Specials"});
    catBox->setCurrentText(m->getCategory());
    catBox->setStyleSheet("border:2px solid #e2e8f0;border-radius:10px;padding:8px 12px;"
                          "font-size:13px;background:#f8fafc;");
    form->addRow("Category:", catBox);

    QDoubleSpinBox *priceBox = new QDoubleSpinBox;
    priceBox->setRange(10, 99999);
    priceBox->setValue(m->getPrice());
    priceBox->setPrefix("Rs. ");
    priceBox->setDecimals(0);
    priceBox->setStyleSheet(fieldStyle);
    form->addRow("Price:", priceBox);

    vl->addLayout(form);
    vl->addSpacing(10);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    btns->button(QDialogButtonBox::Ok)->setText("Save Changes");
    btns->button(QDialogButtonBox::Ok)->setStyleSheet("background:#f6ad55;color:white;border:none;border-radius:10px;"
                                                      "padding:10px 22px;font-weight:700;font-size:13px;");
    btns->button(QDialogButtonBox::Cancel)->setStyleSheet("background:#e2e8f0;color:#4a5568;border:none;border-radius:10px;"
                                                          "padding:10px 18px;font-size:13px;");
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    vl->addWidget(btns);

    if (dlg.exec() != QDialog::Accepted)
        return;

    m->setName(nameEdit->text().trimmed());
    m->setCategory(catBox->currentText());
    m->setPrice(priceBox->value());
    FileHandler::saveMenu(menu);
    refreshTable();
    QMessageBox::information(this, "✅ Updated", "\"" + m->getName() + "\" updated!");
}

// ── DELETE ────────────────────────────────────────────────────
void AdminWindow::deleteItem()
{
    int row = menuTable->currentRow();
    if (row < 0 || row >= menu.size())
    {
        QMessageBox::information(this, "Select Item", "Click a row first, then press Delete.");
        return;
    }
    auto *m = menu[row];
    if (QMessageBox::question(this, "Confirm Delete",
                              "Delete \"" + m->getName() + "\"?\nThis cannot be undone.",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    FileHandler::deleteMenuItem(m->getId());
    delete m;
    menu.removeAt(row);
    refreshTable();
}

// ── ORDERS ────────────────────────────────────────────────────
void AdminWindow::viewOrders()
{
    auto orders = FileHandler::loadOrders();

    QDialog dlg(this);
    dlg.setWindowTitle("📦  All Customer Orders");
    dlg.resize(600, 500);
    dlg.setStyleSheet("background:white;");

    QVBoxLayout *vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(0, 0, 0, 0);

    QWidget *hdr = new QWidget;
    hdr->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                       "stop:0 #0f3460,stop:1 #16213e);");
    hdr->setFixedHeight(60);
    QHBoxLayout *hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(22, 0, 22, 0);
    QLabel *htitle = new QLabel("📦  Order History — " + QString::number(orders.size()) + " orders");
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
    sl->setSpacing(12);

    if (orders.isEmpty())
    {
        QLabel *empty = new QLabel("No orders placed yet.\n\nOrders will appear here after customers check out.");
        empty->setStyleSheet("font-size:14px;color:#a0aec0;");
        empty->setAlignment(Qt::AlignCenter);
        sl->addWidget(empty);
    }
    else
    {
        for (auto &o : orders)
        {
            QFrame *card = new QFrame;
            card->setStyleSheet("QFrame{background:#f8fafc;border-radius:12px;"
                                "border:1px solid #e2e8f0;}");
            QVBoxLayout *cl = new QVBoxLayout(card);
            cl->setContentsMargins(16, 12, 16, 12);
            cl->setSpacing(4);
            QLabel *cust = new QLabel("👤  " + o.getCustomerName() +
                                      "   |   📅  " + o.getDate());
            cust->setStyleSheet("font-size:13px;font-weight:700;color:#2d3748;");
            cl->addWidget(cust);
            QLabel *det = new QLabel(o.getSummary());
            det->setStyleSheet("font-size:12px;color:#718096;");
            det->setWordWrap(true);
            cl->addWidget(det);
            sl->addWidget(card);
        }
    }
    sl->addStretch();
    scroll->setWidget(sw);
    vl->addWidget(scroll, 1);

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setFixedHeight(42);
    closeBtn->setStyleSheet("background:#e2e8f0;color:#4a5568;border:none;"
                            "border-radius:0 0 0 0;font-size:13px;font-weight:600;");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    vl->addWidget(closeBtn);

    dlg.exec();
}