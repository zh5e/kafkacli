#include "connectbrokerdlg.h"

#include <string>

#include "ui_connectbrokerdlg.h"
#include "mainwindow.h"
#include "kafkaconsumer.h"


ConnectBrokerDlg::ConnectBrokerDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConnectBrokerDlg)
{
    ui->setupUi(this);
    ui->_brokerTextEdit->setText(QString("gw-dpicj01:9092,"
                                         "gw-dpicj02:9092,"
                                         "gw-dpicj03:9092,"
                                         "gw-dpicj04:9092,"
                                         "gw-dpicj05:9092,"
                                         "gw_dpicj06:9092"));
}

ConnectBrokerDlg::~ConnectBrokerDlg()
{
    delete ui;
}

void ConnectBrokerDlg::on_pushButton_clicked()
{
    this->accept();

    auto *pMainWnd = new MainWindow(nullptr);
    pMainWnd->show();

    auto failureCb = [&](const std::string & errstr) {
        QMessageBox::critical(nullptr, QString("异常"),
                              QString::fromStdString(errstr),
                               QMessageBox::Ok);
        // pMainWnd->close();

        // auto *pDlg = new ConnectBrokerDlg;
        // pDlg->show();
    };
    const std::string broker = ui->_brokerTextEdit->toPlainText().toUtf8().constData();
    KafkaConsumer *pConsumer = new KafkaConsumer(broker, failureCb);

    pMainWnd->consumerPtr(KafkaConsumer::Ptr(pConsumer));
    pMainWnd->initTreeView();
}
