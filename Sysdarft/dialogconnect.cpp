#include "dialogconnect.h"
#include "ui_dialogconnect.h"
#include "sysdarft.h"

DialogConnect::DialogConnect(const std::string & content_, QWidget *parent_)
    : QDialog(parent_)
    , ui(new Ui::DialogConnect)
{
    ui->setupUi(this);
    assert(parent_ != nullptr);
    this->parent = (Sysdarft*)parent_;
    ui->tcp_target_box->setText(content_.c_str());
    content = content_;
}

DialogConnect::~DialogConnect()
{
    delete ui;
}

void DialogConnect::on_tcp_target_box_textEdited(const QString &arg1)
{
    std::lock_guard lock(content_mutex);
    content = arg1.toStdString();
}

std::string DialogConnect::return_text()
{
    std::lock_guard lock(content_mutex);
    return this->content;
}

void DialogConnect::on_b_YesNo_accepted()
{
    parent->connect(content);
    finished = true;
}

void DialogConnect::on_b_YesNo_rejected()
{
    finished = true;
}

