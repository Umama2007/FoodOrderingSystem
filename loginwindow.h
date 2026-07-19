// loginwindow.h  — FIXED: Admin tab added
#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private:
    // Input fields
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;

    // Tab toggle buttons
    QPushButton *customerTabBtn;
    QPushButton *adminTabBtn;

    // Dynamic labels that change per tab
    QLabel *titleLabel;
    QLabel *subtitleLabel;

    // Widgets shown only on Customer tab
    QLabel *orLabel;
    QPushButton *regBtn;

    // Widget shown only on Admin tab
    QLabel *adminHintLabel;

    // Main login button (style changes per tab)
    QPushButton *loginBtn;

    // Tracks which tab is active
    bool isAdminMode;

    void buildUI();
    void switchToCustomer();
    void switchToAdmin();
    void doLogin();
    void doRegister();
};
#endif
