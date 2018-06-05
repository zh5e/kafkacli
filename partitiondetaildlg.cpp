#include "partitiondetaildlg.h"
#include "ui_partitiondetaildlg.h"

#include "logger.h"


PartitionDetailDlg::PartitionDetailDlg(KafkaConsumer::Ptr pConsumer,
                                       const std::string &topic,
                                       int partition, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PartitionDetailDlg)
    , _pConsumer(pConsumer)
    , _topic(topic)
    , _partition(partition)
    , _low(0)
    , _high(0)
{
    ui->setupUi(this);
    ui->messageCountEdit->setReadOnly(true);
    ui->messageCountEdit->setDisabled(true);

    setWindowTitle(QString("%1-%2").arg( topic.c_str()).arg(partition));
    setWindowFlags(Qt::WindowType::MaximizeUsingFullscreenGeometryHint
                   | Qt::WindowType::Window);

    initView();

}

PartitionDetailDlg::~PartitionDetailDlg()
{
    delete ui;
}

bool PartitionDetailDlg::initView()
{
    updateView();

    // 初始化解析方法列表
    for (const auto &funcName : LibFuncMgr::inst().parserFuncList()) {
        ui->parserComboBox->addItem(QString::fromStdString(funcName.desc));
    }

    // 初始化过滤方法列表
    for (const auto &funcInfo : LibFuncMgr::inst().filterFuncList()) {
        ui->filterFuncComBox->addItem(QString::fromStdString(funcInfo.desc));
    }

    return true;
}

bool PartitionDetailDlg::updateView()
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

    _low = low;
    _high = high;

    return true;
}

void PartitionDetailDlg::showMessage(int64_t messageOffset)
{
    std::string message;
    std::string errstr;

    auto ret = consumerPtr()->messageAtOffset(topic(), partition(),
                                              messageOffset, message, errstr);
    if (!ret) {
        QMessageBox::critical(this, "获取消息失败", QString::fromStdString(errstr),
                              QMessageBox::Ok);
        messageDetail(errstr);
        return;
    }

    auto parserLib = getParserLib();
    if (parserLib) {
        auto parserFunc = (ParserFunc::Func)parserLib->resolve(ParserFunc::PARSE_SYMBOL);
        if (parserFunc) {
            message = parserFunc(message);
        }
    }

    messageDetail(message);
}

bool PartitionDetailDlg::initParserComboBox()
{
    return true;
}

PartitionDetailDlg::LibraryPtr PartitionDetailDlg::getParserLib()
{
    const auto &txt = ui->parserComboBox->currentText().toUtf8().toStdString();
    for (const auto &func : LibFuncMgr::inst().parserFuncList()) {
        if (func.desc == txt) {
            LibraryPtr ptr(new QLibrary(QString::fromStdString(func.libPath)),
                           [](QLibrary *p){
                p->unload();
                delete p;
            });
            return ptr;
        }
    }

    return nullptr;
}

int64_t PartitionDetailDlg::messageCount() const
{
    return ui->messageCountEdit->text().toLongLong();
}

void PartitionDetailDlg::messageCount(int64_t value)
{
    ui->messageCountEdit->setText(QString::number(value));
}

int64_t PartitionDetailDlg::messageIndex() const
{
    return ui->targetOffsetEdit->text().toLongLong();
}

void PartitionDetailDlg::messageIndex(long index)
{
    ui->targetOffsetEdit->setText(QString::number(index));
}

void PartitionDetailDlg::messageDetail(const std::string &detail)
{
    ui->messageDetail->setText(QString::fromStdString(detail));
}

void PartitionDetailDlg::updateOffsetSlot(const QString &offset)
{
    this->ui->filterMessageIndex->setText(offset);
}

void PartitionDetailDlg::filterResultSlot(const QString &data)
{
    ui->messageDetail->setText(data);
}

void PartitionDetailDlg::on_pushButton_clicked()
{
    int64_t messageOffset = messageIndex() + _low;
    DLOG << "low: " << _low << ", high: " << _high
         << ", offset: " << messageOffset;

    showMessage(messageOffset);
}

void PartitionDetailDlg::on_updateBtn_clicked()
{
    updateView();
}

void PartitionDetailDlg::on_lastMessageBtn_clicked()
{
    updateView();

    int64_t messageOffset = _high - _low - 1;
    messageIndex(messageOffset);

    DLOG << "low: " << _low << ", high: " << _high
         << ", offset: " << messageOffset;


    showMessage(_high - 1);
}

void PartitionDetailDlg::on_filterBtn_clicked()
{
    if (_filterThread) {
        _filterThread->stop();
        _filterThread->wait();
    }

    _filterThread.reset(new FilterThread);

    connect(_filterThread.get(), SIGNAL(reportOffsetSignal(const QString&)),
            this, SLOT(updateOffsetSlot(const QString&)));
    connect(_filterThread.get(), SIGNAL(filterResultSignal(const QString&)),
            this, SLOT(filterResultSlot(const QString&)));

    long messageIndex = ui->filterMessageIndex->text().toLong();

    _filterThread->topic(topic());
    _filterThread->partition(partition());
    _filterThread->offset(_low + messageIndex);
    _filterThread->consumerPtr(consumerPtr());

    _filterThread->start();

    ui->filterBtn->setEnabled(false);
}

void PartitionDetailDlg::on_resetFilterBtn_clicked()
{
    _filterThread->stop();
    _filterThread->wait();
    _filterThread.reset();

    ui->filterBtn->setEnabled(true);
}
