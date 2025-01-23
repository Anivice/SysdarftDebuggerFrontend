#ifndef DIALOGWARNING_H
#define DIALOGWARNING_H

#include <QDialog>

namespace Ui {
class DialogWarning;
}

class DialogWarning : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWarning(const std::string & content, QWidget *parent = nullptr);
    ~DialogWarning();

private:
    Ui::DialogWarning *ui;
};

#endif // DIALOGWARNING_H
