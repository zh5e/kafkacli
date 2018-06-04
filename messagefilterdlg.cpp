#include "messagefilterdlg.h"
#include "ui_messagefilterdlg.h"

MessageFilterDlg::MessageFilterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageFilterDlg)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowType::Window | Qt::WindowType::MaximizeUsingFullscreenGeometryHint);
}

MessageFilterDlg::~MessageFilterDlg()
{
    delete ui;
}
