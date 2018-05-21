#include "partitiondetaildlg.h"
#include "ui_partitiondetaildlg.h"


PartitionDetailDlg::PartitionDetailDlg(KafkaConsumer::Ptr pConsumer,
                                       const std::string &topic,
                                       int partition, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PartitionDetailDlg)
    , _pConsumer(pConsumer)
    , _topic(topic)
    , _partition(partition)
{
    ui->setupUi(this);
    ui->messageCountEdit->setReadOnly(true);

    initView();
}

PartitionDetailDlg::~PartitionDetailDlg()
{
    delete ui;
}

bool PartitionDetailDlg::initView()
{
    int64_t low = 0, high = 0;
    auto ret = consumerPtr()->queryWatermarkOffsets(topic(), partition(),
                                                    low, high);
    if (!ret) {
        QMessageBox::critical(this, "提示", "获取高低水信息失败", QMessageBox::Ok);
        return false;
    }

    DLOG << low << " " << high;
    messageCount(high - low);

    return true;
}

int64_t PartitionDetailDlg::messageCount() const
{
    return ui->messageCountEdit->text().toLongLong();
}

void PartitionDetailDlg::messageCount(int64_t value)
{
    ui->messageCountEdit->setText(QString::number(value));
}

int64_t PartitionDetailDlg::targetOffset() const
{
    return ui->targetOffsetEdit->text().toLongLong();
}

void PartitionDetailDlg::messageDetail(const std::string &detail)
{
    ui->messageDetail->setText(QString::fromStdString(detail));
}

void PartitionDetailDlg::on_pushButton_clicked()
{
    std::string message;
    std::string errstr;
    auto ret = consumerPtr()->messageAtOffset(topic(), partition(),
                                              targetOffset(), message,
                                              errstr);
    if (!ret) {
        QMessageBox::critical(this, "获取消息失败", QString::fromStdString(errstr),
                              QMessageBox::Ok);
        messageDetail(errstr);
        return;
    }
    messageDetail(message);
}
