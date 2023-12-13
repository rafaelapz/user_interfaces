#include "login.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>
#include <QLabel>

#include "login.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMap>

#include "login.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMap>

bool showLoginDialog() {
    QDialog *loginDialog = new QDialog();
    QVBoxLayout *layout = new QVBoxLayout(loginDialog);
    layout->setSpacing(10); // Set the spacing to 10 pixels

    QLabel *usernameLabel = new QLabel("Username");
    QLineEdit *usernameField = new QLineEdit();
    QLabel *passwordLabel = new QLabel("Password");
    QLineEdit *passwordField = new QLineEdit();
    passwordField->setEchoMode(QLineEdit::Password);
    QPushButton *loginButton = new QPushButton("Login");

    layout->addWidget(usernameLabel);
    layout->addWidget(usernameField);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);
    layout->setContentsMargins(50, 50, 50, 50);

    QMap<QString, QString> users;
    users.insert("johndoe", "password1");
    users.insert("janedoe", "password2");
    users.insert("admin", "admin");

    QObject::connect(loginButton, &QPushButton::clicked, [=]() {
        QString enteredUsername = usernameField->text();
        QString enteredPassword = passwordField->text();

        if (users.contains(enteredUsername) && users[enteredUsername] == enteredPassword) {
            loginDialog->accept();
        } else {
            // You can show an error message here
        }
    });

    // Set the size of the dialog
    loginDialog->setFixedSize(540, 960);
    return loginDialog->exec() == QDialog::Accepted;
}
