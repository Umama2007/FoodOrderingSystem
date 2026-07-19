// loginwindow.cpp  — FIXED: Admin tab added
#include "loginwindow.h"
#include "filehandler.h"
#include "customerwindow.h"
#include "adminwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>
#include <QSpacerItem>
#include <QButtonGroup>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), isAdminMode(false)
{
    setWindowTitle("FoodOrder — Sign In");
    setFixedSize(900, 600);
    buildUI();
}

LoginWindow::~LoginWindow() {}

void LoginWindow::buildUI()
{
    QWidget *central = new QWidget;
    central->setObjectName("loginBg");
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Left branding panel ──────────────────────────────────────
    QWidget *brandPanel = new QWidget;
    brandPanel->setStyleSheet("background: transparent;");
    QVBoxLayout *bl = new QVBoxLayout(brandPanel);
    bl->setContentsMargins(50, 0, 30, 0);
    bl->setAlignment(Qt::AlignCenter);

    QLabel *bigEmoji = new QLabel("🍔");
    bigEmoji->setStyleSheet("font-size: 80px; background: transparent;");
    bigEmoji->setAlignment(Qt::AlignCenter);
    bl->addWidget(bigEmoji);

    QLabel *brandName = new QLabel("FoodOrder");
    brandName->setStyleSheet("font-size: 40px; font-weight: 900; color: #00d4ff;"
                             " background: transparent; letter-spacing: 2px;");
    brandName->setAlignment(Qt::AlignCenter);
    bl->addWidget(brandName);

    QLabel *tagline = new QLabel("Delicious food,\ndelivered fast 🚀");
    tagline->setStyleSheet("font-size: 16px; color: #a0aec0; background: transparent;");
    tagline->setAlignment(Qt::AlignCenter);
    bl->addWidget(tagline);

    mainLayout->addWidget(brandPanel, 1);

    // ── Right login card ─────────────────────────────────────────
    QWidget *cardWrapper = new QWidget;
    cardWrapper->setStyleSheet("background: transparent;");
    QVBoxLayout *cw = new QVBoxLayout(cardWrapper);
    cw->setContentsMargins(30, 40, 50, 40);
    cw->setAlignment(Qt::AlignCenter);

    QFrame *card = new QFrame;
    card->setObjectName("loginCard");
    card->setFixedWidth(380);
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(36, 30, 36, 36);
    cl->setSpacing(12);

    // ── Tab toggle: Customer | Admin ─────────────────────────────
    QFrame *tabBar = new QFrame;
    tabBar->setObjectName("tabBar");
    tabBar->setStyleSheet(
        "#tabBar { background: #f0f4f8; border-radius: 12px; padding: 4px; }");
    tabBar->setFixedHeight(46);
    QHBoxLayout *tabLayout = new QHBoxLayout(tabBar);
    tabLayout->setContentsMargins(4, 4, 4, 4);
    tabLayout->setSpacing(4);

    customerTabBtn = new QPushButton("🧑‍💼  Customer");
    customerTabBtn->setObjectName("activeTab");
    customerTabBtn->setCheckable(true);
    customerTabBtn->setChecked(true);
    customerTabBtn->setFixedHeight(34);
    customerTabBtn->setStyleSheet(
        "QPushButton#activeTab:checked {"
        "  background: white; color: #0096b4;"
        "  border-radius: 9px; font-weight: 700; font-size: 13px;"
        "  border: none;"
        "}"
        "QPushButton#activeTab:!checked {"
        "  background: transparent; color: #a0aec0;"
        "  border: none; font-weight: 600; font-size: 13px;"
        "}");
    tabLayout->addWidget(customerTabBtn);

    adminTabBtn = new QPushButton("🔐  Admin");
    adminTabBtn->setObjectName("activeTab");
    adminTabBtn->setCheckable(true);
    adminTabBtn->setChecked(false);
    adminTabBtn->setFixedHeight(34);
    adminTabBtn->setStyleSheet(
        "QPushButton#activeTab:checked {"
        "  background: #0f3460; color: #00d4ff;"
        "  border-radius: 9px; font-weight: 700; font-size: 13px;"
        "  border: none;"
        "}"
        "QPushButton#activeTab:!checked {"
        "  background: transparent; color: #a0aec0;"
        "  border: none; font-weight: 600; font-size: 13px;"
        "}");
    tabLayout->addWidget(adminTabBtn);

    // Make the two tabs mutually exclusive
    QButtonGroup *tabGroup = new QButtonGroup(this);
    tabGroup->setExclusive(true);
    tabGroup->addButton(customerTabBtn);
    tabGroup->addButton(adminTabBtn);

    connect(customerTabBtn, &QPushButton::clicked, this, &LoginWindow::switchToCustomer);
    connect(adminTabBtn, &QPushButton::clicked, this, &LoginWindow::switchToAdmin);

    cl->addWidget(tabBar);
    cl->addSpacing(4);

    // ── Dynamic title ────────────────────────────────────────────
    titleLabel = new QLabel("Welcome back!");
    titleLabel->setObjectName("loginTitle");
    cl->addWidget(titleLabel);

    subtitleLabel = new QLabel("Sign in to your customer account");
    subtitleLabel->setObjectName("loginSub");
    cl->addWidget(subtitleLabel);

    cl->addSpacing(4);

    QLabel *uLbl = new QLabel("USERNAME");
    uLbl->setObjectName("fieldLabel");
    cl->addWidget(uLbl);

    usernameEdit = new QLineEdit;
    usernameEdit->setObjectName("loginInput");
    usernameEdit->setPlaceholderText("Enter your username");
    usernameEdit->setFixedHeight(46);
    cl->addWidget(usernameEdit);

    QLabel *pLbl = new QLabel("PASSWORD");
    pLbl->setObjectName("fieldLabel");
    cl->addWidget(pLbl);

    passwordEdit = new QLineEdit;
    passwordEdit->setObjectName("loginInput");
    passwordEdit->setPlaceholderText("Enter your password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedHeight(46);
    cl->addWidget(passwordEdit);

    cl->addSpacing(4);

    loginBtn = new QPushButton("LOGIN →");
    loginBtn->setObjectName("loginBtn");
    loginBtn->setFixedHeight(48);
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::doLogin);
    cl->addWidget(loginBtn);

    // ── Customer-only: register link ─────────────────────────────
    orLabel = new QLabel("— or —");
    orLabel->setObjectName("orLabel");
    orLabel->setAlignment(Qt::AlignCenter);
    cl->addWidget(orLabel);

    regBtn = new QPushButton("Create New Account");
    regBtn->setObjectName("registerBtn");
    regBtn->setFixedHeight(44);
    connect(regBtn, &QPushButton::clicked, this, &LoginWindow::doRegister);
    cl->addWidget(regBtn);

    // ── Admin-only: hint label (hidden by default) ───────────────
    adminHintLabel = new QLabel("Default credentials:  admin / admin123");
    adminHintLabel->setStyleSheet(
        "color: #718096; font-size: 11px; background: #f7fafc;"
        " border: 1px solid #e2e8f0; border-radius: 8px; padding: 6px 10px;");
    adminHintLabel->setAlignment(Qt::AlignCenter);
    adminHintLabel->setVisible(false);
    cl->addWidget(adminHintLabel);

    cw->addWidget(card);
    mainLayout->addWidget(cardWrapper, 1);
}

// ── Switch to Customer tab ───────────────────────────────────────
void LoginWindow::switchToCustomer()
{
    isAdminMode = false;
    titleLabel->setText("Welcome back!");
    subtitleLabel->setText("Sign in to your customer account");
    loginBtn->setStyleSheet(""); // let QSS handle it
    loginBtn->setObjectName("loginBtn");
    loginBtn->style()->unpolish(loginBtn);
    loginBtn->style()->polish(loginBtn);
    orLabel->setVisible(true);
    regBtn->setVisible(true);
    adminHintLabel->setVisible(false);
    usernameEdit->clear();
    passwordEdit->clear();
    usernameEdit->setPlaceholderText("Enter your username");
}

// ── Switch to Admin tab ──────────────────────────────────────────
void LoginWindow::switchToAdmin()
{
    isAdminMode = true;
    titleLabel->setText("Admin Portal");
    subtitleLabel->setText("Restricted — authorised personnel only");
    loginBtn->setObjectName("adminLoginBtn");
    loginBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #1a4a8a, stop:1 #0f3460);"
        "color: #00d4ff; border: none; border-radius: 12px;"
        "padding: 14px; font-size: 15px; font-weight: 800; letter-spacing: 1px; }"
        "QPushButton:hover { background: #0f3460; }"
        "QPushButton:pressed { background: #0a2444; }");
    orLabel->setVisible(false);
    regBtn->setVisible(false);
    adminHintLabel->setVisible(true);
    usernameEdit->clear();
    passwordEdit->clear();
    usernameEdit->setPlaceholderText("Admin username");
}

void LoginWindow::doLogin()
{
    QString uname = usernameEdit->text().trimmed();
    QString pass = passwordEdit->text().trimmed();

    if (uname.isEmpty() || pass.isEmpty())
    {
        QMessageBox::warning(this, "Missing Fields",
                             "Please enter both username and password.");
        return;
    }

    QString role;
    if (FileHandler::validateUser(uname, pass, role))
    {
        if (role == "admin")
        {
            // If the user tried to log in from the Customer tab with admin creds,
            // still let them in but show a small note.
            AdminWindow *w = new AdminWindow(uname);
            w->show();
            this->close();
        }
        else
        {
            if (isAdminMode)
            {
                QMessageBox::warning(this, "Not an Admin",
                                     "That account is a customer account.\n"
                                     "Please use the Customer tab to log in.");
                return;
            }
            CustomerWindow *w = new CustomerWindow(uname);
            w->show();
            this->close();
        }
    }
    else
    {
        if (isAdminMode)
            QMessageBox::warning(this, "Login Failed",
                                 "Invalid admin credentials.\n\nDefault: admin / admin123");
        else
            QMessageBox::warning(this, "Login Failed",
                                 "Invalid username or password.");
    }
}

void LoginWindow::doRegister()
{
    QString uname = usernameEdit->text().trimmed();
    QString pass = passwordEdit->text().trimmed();

    if (uname.isEmpty() || pass.isEmpty())
    {
        QMessageBox::warning(this, "Missing Fields",
                             "Enter a username and password to register.");
        return;
    }
    if (FileHandler::userExists(uname))
    {
        QMessageBox::warning(this, "Taken",
                             "Username already exists. Try another.");
        return;
    }
    FileHandler::saveUser(uname, pass, "", "", "customer");
    QMessageBox::information(this, "Account Created! 🎉",
                             "Welcome, " + uname + "!\nYou can now log in.");
}
