#ifndef DIALOGCONNECT_H
#define DIALOGCONNECT_H

#include <QDialog>
class Sysdarft;

namespace Ui {
class DialogConnect;
}

class DialogConnect : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConnect(const std::string & content_, QWidget *parent = nullptr);
    ~DialogConnect();
    [[nodiscard]] std::string return_text();

    explicit operator bool() const {
        return finished;
    }

private slots:
    void on_tcp_target_box_textEdited(const QString &arg1);
    void on_b_YesNo_accepted();
    void on_b_YesNo_rejected();

private:
    Ui::DialogConnect *ui;
    std::mutex content_mutex;
    std::string content;
    std::atomic < bool > finished = false;
    Sysdarft * parent;
};

#endif // DIALOGCONNECT_H
