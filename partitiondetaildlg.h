#ifndef PARTITIONDETAILDLG_H
#define PARTITIONDETAILDLG_H

#include <QDialog>

#include <string>
#include <functional>

#include "kafkaconsumer.h"
#include "parserfuncmgr.h"


namespace Ui {
class PartitionDetailDlg;
}

class PartitionDetailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PartitionDetailDlg(KafkaConsumer::Ptr pConsumer,
                                const std::string &topic,
                                int partition,
                                QWidget *parent);
    ~PartitionDetailDlg();

private slots:
    void on_pushButton_clicked();

private:
    bool initView();

    bool initParserComboBox();

    ParserFunc::Func getParserFunc();

    KafkaConsumer::Ptr consumerPtr() {
        return _pConsumer;
    }

    const std::string topic() const {
        return _topic;
    }

    int partition() const {
        return _partition;
    }

    int64_t messageCount() const;
    void messageCount(int64_t value);

    int64_t messageIndex() const;

    void messageDetail(const std::string &detail);

private:
    Ui::PartitionDetailDlg  *ui;
    KafkaConsumer::Ptr      _pConsumer;
    std::string             _topic;
    int                     _partition;

    int _low;
    int _high;
};

#endif // PARTITIONDETAILDLG_H
