#ifndef MESSAGEFILTERDLG_H
#define MESSAGEFILTERDLG_H

#include <QDialog>

namespace Ui {
class MessageFilterDlg;
}

class MessageFilterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MessageFilterDlg(QWidget *parent = 0);
    ~MessageFilterDlg();

private:
    Ui::MessageFilterDlg *ui;
};

#endif // MESSAGEFILTERDLG_H
