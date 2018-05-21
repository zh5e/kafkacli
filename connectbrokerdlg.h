#ifndef CONNECTBROKERDLG_H
#define CONNECTBROKERDLG_H

#include <QDialog>

namespace Ui {
class ConnectBrokerDlg;
}

class ConnectBrokerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectBrokerDlg(QWidget *parent = 0);
    ~ConnectBrokerDlg();


private slots:
    void on_pushButton_clicked();

private:
    Ui::ConnectBrokerDlg *ui;
};

#endif // CONNECTBROKERDLG_H
