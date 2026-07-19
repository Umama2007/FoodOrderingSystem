#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFontDatabase>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Use Fusion as base — consistent across Windows/Mac/Linux
    app.setStyle("Fusion");

    // Global stylesheet embedded directly (no file needed)
    app.setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #f0f4f8;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
            color: #2d3748;
        }
        QScrollArea { border: none; background: transparent; }
        QScrollBar:vertical {
            background: #f0f4f8; width: 8px; border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #cbd5e0; border-radius: 4px; min-height: 30px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

        /* ── Navbar ── */
        #navbar {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #0f3460, stop:1 #16213e);
            border: none;
        }
        #logoLabel {
            font-size: 22px; font-weight: 800;
            color: #00d4ff; letter-spacing: 1px;
        }
        #userLabel {
            font-size: 13px; color: #a0aec0; padding: 0 16px;
        }

        /* ── Welcome ── */
        #welcomeLabel {
            font-size: 28px; font-weight: 800;
            color: #1a202c; padding: 20px 24px 2px 24px;
        }
        #sectionLabel {
            font-size: 16px; font-weight: 700;
            color: #2d3748; padding: 12px 24px 8px 24px;
        }

        /* ── Category pills ── */
        #catBtn {
            background: white;
            border: 2px solid #e2e8f0;
            border-radius: 20px;
            padding: 6px 18px;
            color: #4a5568;
            font-weight: 600;
            font-size: 12px;
        }
        #catBtn:hover {
            border-color: #00b4d8;
            color: #00b4d8;
        }
        #catBtn:checked {
            background: #00b4d8;
            border-color: #00b4d8;
            color: white;
        }

        /* ── Search ── */
        #searchBar {
            border: 2px solid #e2e8f0;
            border-radius: 24px;
            padding: 10px 20px;
            background: white;
            font-size: 13px;
            color: #2d3748;
        }
        #searchBar:focus {
            border-color: #00b4d8;
            outline: none;
        }

        /* ── Sort buttons ── */
        #sortBtn {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 5px 12px;
            color: #718096;
            font-size: 12px;
        }
        #sortBtn:hover { background: #ebf8ff; color: #00b4d8; }

        /* ── Food Card ── */
        #foodCard {
            background: white;
            border-radius: 16px;
            border: 1px solid #e2e8f0;
        }
        #foodCard:hover { border-color: #00b4d8; }
        #foodName {
            font-size: 13px; font-weight: 700;
            color: #1a202c; padding: 8px 12px 2px 12px;
        }
        #foodCat {
            font-size: 11px; color: #718096;
            padding: 0 12px 2px 12px;
        }
        #foodPrice {
            font-size: 15px; font-weight: 800;
            color: #0096b4; padding: 0 12px;
        }
        #addCartBtn {
            background: #00b4d8;
            color: white; border: none;
            border-radius: 10px;
            padding: 8px 12px;
            margin: 8px 12px 12px 12px;
            font-weight: 700; font-size: 12px;
        }
        #addCartBtn:hover { background: #0096c7; }
        #addCartBtn:pressed { background: #0077b6; }

        /* ── Cart Panel ── */
        #cartPanel {
            background: white;
            border-left: 2px solid #e2e8f0;
        }
        #cartHeader {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #0f3460, stop:1 #16213e);
            padding: 18px 20px;
            border: none;
        }
        #cartHeaderLabel {
            font-size: 16px; font-weight: 800;
            color: white; letter-spacing: 1px;
        }
        #cartCountLabel {
            font-size: 12px; color: #00d4ff;
        }
        #cartItemWidget {
            background: #f8fafc;
            border-radius: 10px;
            border: 1px solid #e2e8f0;
            margin: 4px 16px;
            padding: 8px 12px;
        }
        #cartItemName { font-size: 13px; font-weight: 700; color: #2d3748; }
        #cartItemPrice { font-size: 12px; color: #00b4d8; font-weight: 600; }
        #removeBtn {
            background: #fed7d7; color: #e53e3e;
            border: none; border-radius: 14px;
            min-width: 26px; max-width: 26px;
            min-height: 26px; max-height: 26px;
            font-weight: 900; font-size: 13px;
        }
        #removeBtn:hover { background: #e53e3e; color: white; }
        #divider {
            background: #e2e8f0;
            max-height: 1px; min-height: 1px;
            margin: 4px 0;
        }
        #totalLabel { font-size: 12px; color: #718096; }
        #totalValue { font-size: 12px; font-weight: 700; color: #2d3748; }
        #grandLabel { font-size: 14px; font-weight: 800; color: #0096b4; }
        #grandValue { font-size: 16px; font-weight: 900; color: #0096b4; }
        #placeOrderBtn {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #00d4ff, stop:1 #0096b4);
            color: white; border: none;
            border-radius: 12px;
            padding: 14px 20px;
            font-size: 14px; font-weight: 800;
            margin: 8px 16px 4px 16px;
            letter-spacing: 1px;
        }
        #placeOrderBtn:hover { background: #0096b4; }
        #placeOrderBtn:pressed { background: #0077b6; }
        #viewOrdersBtn {
            background: transparent;
            color: #00b4d8; border: 2px solid #00b4d8;
            border-radius: 12px; padding: 10px 20px;
            font-size: 13px; font-weight: 700;
            margin: 4px 16px 16px 16px;
        }
        #viewOrdersBtn:hover { background: #ebf8ff; }
        #emptyCartLabel { font-size: 13px; color: #a0aec0; padding: 20px; }

        /* ── Login Window ── */
        #loginBg {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 #0f3460, stop:0.5 #16213e, stop:1 #0d1b2a);
        }
        #loginCard {
            background: white;
            border-radius: 24px;
        }
        #loginTitle {
            font-size: 32px; font-weight: 900;
            color: #0096b4;
        }
        #loginBrand {
            font-size: 13px; color: #00b4d8;
            font-weight: 600; letter-spacing: 2px;
        }
        #loginSub { font-size: 13px; color: #a0aec0; }
        #fieldLabel { font-size: 12px; font-weight: 700; color: #4a5568; }
        #loginInput {
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 12px 16px;
            font-size: 14px;
            background: #f8fafc;
            color: #2d3748;
        }
        #loginInput:focus { border-color: #00b4d8; background: white; }
        #loginBtn {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #00d4ff, stop:1 #0096b4);
            color: white; border: none;
            border-radius: 12px; padding: 14px;
            font-size: 15px; font-weight: 800;
            letter-spacing: 1px;
        }
        #loginBtn:hover { background: #0096b4; }
        #loginBtn:pressed { background: #0077b6; }
        #registerBtn {
            background: transparent;
            color: #0096b4; border: 2px solid #0096b4;
            border-radius: 12px; padding: 12px;
            font-size: 14px; font-weight: 700;
        }
        #registerBtn:hover { background: #ebf8ff; }
        #orLabel { color: #a0aec0; font-size: 12px; }

        /* ── Admin ── */
        #adminHeader {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #0f3460, stop:1 #16213e);
            padding: 20px 24px;
        }
        #adminTitle { font-size: 22px; font-weight: 800; color: white; }
        #adminSubtitle { font-size: 13px; color: #a0aec0; }
        #adminMenuList {
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            background: white;
            padding: 4px;
        }
        #adminMenuList::item { padding: 12px 16px; border-radius: 8px; color: #2d3748; }
        #adminMenuList::item:selected { background: #ebf8ff; color: #0096b4; }
        #adminMenuList::item:hover { background: #f7fafc; }
        #addBtn {
            background: #00b4d8; color: white; border: none;
            border-radius: 10px; padding: 11px 20px;
            font-weight: 700; font-size: 13px;
        }
        #addBtn:hover { background: #0096b4; }
        #deleteBtn {
            background: #fc8181; color: white; border: none;
            border-radius: 10px; padding: 11px 20px;
            font-weight: 700; font-size: 13px;
        }
        #deleteBtn:hover { background: #e53e3e; }
        #editBtn {
            background: #f6ad55; color: white; border: none;
            border-radius: 10px; padding: 11px 20px;
            font-weight: 700; font-size: 13px;
        }
        #editBtn:hover { background: #ed8936; }
        #ordersBtn {
            background: #68d391; color: white; border: none;
            border-radius: 10px; padding: 11px 20px;
            font-weight: 700; font-size: 13px;
        }
        #ordersBtn:hover { background: #48bb78; }
    )");

    QDir dir(QCoreApplication::applicationDirPath());
    dir.mkpath("data");
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QFile usersFile("data/users.txt");
    if (!usersFile.exists() || usersFile.size() == 0)
    {
        if (usersFile.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&usersFile);
            out << "admin,admin123,,,admin\n";
            usersFile.close();
        }
    }

    LoginWindow w;
    w.show();
    return app.exec();
}