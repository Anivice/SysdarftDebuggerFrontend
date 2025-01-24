#include "sysdarft.h"
#include "./ui_sysdarft.h"
#include "dialogconnect.h"
#include "dialogwarning.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void replace_all(
    std::string & original,
    const std::string & target,
    const std::string & replacement)
{
    if (target.empty()) return; // Avoid infinite loop if target is empty

    size_t pos = 0;
    while ((pos = original.find(target, pos)) != std::string::npos) {
        original.replace(pos, target.length(), replacement);
        pos += replacement.length(); // Move past the replacement to avoid infinite loop
    }
}

Sysdarft::Sysdarft(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Sysdarft)
{
    ui->setupUi(this);
}

Sysdarft::~Sysdarft()
{
    delete ui;
}

void Sysdarft::on_b_Continue_clicked()
{
    try {
        if (http_client.get() == nullptr) {
            DialogWarning * pWarning = new DialogWarning("Debug server is not connected!", this);
            pWarning->show();
            return;
        }

        auto res = http_client->Post("/Continue");
        if (res == nullptr || res->status == -1)
        {
            http_client.reset();
            DialogWarning * pWarning = new DialogWarning("Debug server is not connected!", this);
            pWarning->show();
            return;
        }

        std::string bodyString = res->body;
        json clientJson = json::parse(bodyString);

        if (res->status != 200)
        {
            std::cerr << "Error received when attempting continue" << std::endl;
            const std::string result_literal = clientJson["Result"];
            DialogWarning * pWarning = new DialogWarning(result_literal.c_str(), this);
            pWarning->show();
            return;
        }
    } catch (const std::exception& e) {
        DialogWarning * pWarning = new DialogWarning(e.what(), this);
        pWarning->show();
    }
}

void Sysdarft::on_b_SendCommand_clicked()
{
}


void Sysdarft::on_line_Command_editingFinished()
{
}

void Sysdarft::on_line_Command_textEdited(const QString &arg1)
{
    std::lock_guard lock(text_content_access_mutex);
    content = arg1.toStdString();
}

void Sysdarft::on_b_Connect_clicked()
{
    DialogConnect * pConnect = new DialogConnect(ip, this);
    pConnect->show();
}

void Sysdarft::connect(const std::string & ip_)
{
    std::lock_guard lock(http_client_mutex);
    if (http_client.get() != nullptr) {
        http_client.reset();
    }

    ip = ip_;
    replace_all(ip, " ", "");

    if (ip_.empty()) {
        DialogWarning * pWarning = new DialogWarning("Invalid IP address", this);
        pWarning->show();
        return;
    }

    http_client = std::make_unique < httplib::Client >(httplib::Client(ip));
}
