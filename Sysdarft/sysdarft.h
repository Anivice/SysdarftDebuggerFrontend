#ifndef SYSDARFT_H
#define SYSDARFT_H

#include "httplib.h"
#include <QMainWindow>
#include <mutex>

QT_BEGIN_NAMESPACE
namespace Ui {
class Sysdarft;
}
QT_END_NAMESPACE

class Sysdarft : public QMainWindow
{
    Q_OBJECT

public:
    Sysdarft(QWidget *parent = nullptr);
    ~Sysdarft();
    void connect(const std::string & ip);

private slots:
    void on_b_Continue_clicked();
    void on_b_SendCommand_clicked();
    void on_line_Command_editingFinished();
    void on_line_Command_textEdited(const QString &arg1);
    void on_b_Connect_clicked();

private:
    Ui::Sysdarft *ui;
    std::string content;
    std::mutex text_content_access_mutex;
    std::unique_ptr < httplib::Client > http_client = nullptr;
    std::mutex http_client_mutex;
};

#endif // SYSDARFT_H
