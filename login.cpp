#include "login.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>

bool showLoginDialog() {
    QDialog *loginDialog = new QDialog();
    QVBoxLayout *layout = new QVBoxLayout(loginDialog);

    QLineEdit *usernameField = new QLineEdit();
    QLineEdit *passwordField = new QLineEdit();
    passwordField->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Login");

    layout->addWidget(usernameField);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);

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
