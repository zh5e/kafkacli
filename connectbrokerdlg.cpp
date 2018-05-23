#include "connectbrokerdlg.h"

#include <string>

#include "ui_connectbrokerdlg.h"
#include "mainwindow.h"
#include "kafkaconsumer.h"
#include "kafkabasicconfig.h"


ConnectBrokerDlg::ConnectBrokerDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConnectBrokerDlg)
{
    ui->setupUi(this);
    ui->_brokerTextEdit->setText(QString("132.228.119.20:29091,132.228.119.20:29092,132.228.119.20:29093"));
    // ui->_brokerTextEdit->setText(QString("132.252.8.122:8299"));
}

ConnectBrokerDlg::~ConnectBrokerDlg()
{
    delete ui;
}

void ConnectBrokerDlg::on_pushButton_clicked()
{
    auto failureCb = [&](const std::string & errstr) {
        QMessageBox::critical(nullptr, QString("异常"),
                              QString::fromStdString(errstr),
                               QMessageBox::Ok);
    };

    const auto broker = ui->_brokerTextEdit->toPlainText().toUtf8().constData();

    KafkaBasicConfig kafkaConfig;
    kafkaConfig << std::make_pair(kafka_config::BROKER_LIST, broker)
                << std::make_pair(kafka_config::GROUP_ID, "kafkacli")
                << new KafkaEventCb(failureCb);

    // 检查是否打开了sasl验证
    if (ui->_enableSasl->checkState() == Qt::CheckState::Checked) {
        std::string username = ui->username->text().toUtf8().constData();
        std::string password = ui->password->text().toUtf8().constData();

        DLOG << ui->username->text() << " : " << ui->password->text();

        kafkaConfig << std::make_pair(kafka_config::SASL_MECHANISMS, "PLAIN")
                    << std::make_pair(kafka_config::SECURITY_PROTOCOL, "sasl_plaintext")
                    << std::make_pair(kafka_config::SASL_USERNAME, username)
                    << std::make_pair(kafka_config::SASL_PASSWORD, password);
    }

    KafkaConsumer *pConsumer = new KafkaConsumer(kafkaConfig);
    if (!pConsumer->init()) {
        QMessageBox::critical(nullptr, QString("异常"),
                              QString("连接失败"), QMessageBox::Ok);
        return;
    }

    KafkaConsumer::TopicVec topics;
    if (!pConsumer->topics(topics)) {
        QMessageBox::critical(nullptr, QString("异常"),
                              QString("连接失败"), QMessageBox::Ok);
        return;
    }

    accept();

    auto *pMainWnd = new MainWindow(nullptr);
    pMainWnd->show();

    pMainWnd->init(KafkaConsumer::Ptr(pConsumer), topics);
    pMainWnd->initTreeView();
}
