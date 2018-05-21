#include <QApplication>

#include "connectbrokerdlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConnectBrokerDlg dlg(nullptr);
    dlg.show();

    return a.exec();
}
