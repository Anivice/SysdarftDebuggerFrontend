#include "dialogwarning.h"
#include "ui_dialogwarning.h"

DialogWarning::DialogWarning(const std::string & content, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogWarning)
{
    ui->setupUi(this);
    ui->WarningLabel->setText(content.c_str());
}

DialogWarning::~DialogWarning()
{
    delete ui;
}
