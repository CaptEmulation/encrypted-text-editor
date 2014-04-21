#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
#ifdef Q_OS_MACX
    // Sheets are cool, but too bad I can't attach to an MDI child
    setWindowFlags(Qt::Sheet);
#endif
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::findText() const
{
    return ui->lineEdit->text();
}
