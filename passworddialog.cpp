#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);
#ifdef Q_OS_MACX
    // Sheets are cool, but too bad I can't attach to an MDI child
    setWindowFlags(Qt::Sheet);
#endif
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

QString PasswordDialog::password()
{
    return ui->lineEdit->text();
}
