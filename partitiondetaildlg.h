#ifndef PARTITIONDETAILDLG_H
#define PARTITIONDETAILDLG_H

#include <QDialog>

#include <QLibrary>

#include <string>
#include <functional>
#include <memory>
#include <mutex>

#include "kafkaconsumer.h"
#include "filterthread.h"


namespace Ui {
class PartitionDetailDlg;
}

class PartitionDetailDlg : public QDialog
{
    Q_OBJECT

public:
    using LibraryPtr = std::shared_ptr<QLibrary>;
    using LockGuard = std::lock_guard<std::mutex>;

    explicit PartitionDetailDlg(KafkaConsumer::Ptr pConsumer,
                                const std::string &topic,
                                int partition,
                                QWidget *parent);
    ~PartitionDetailDlg();

private slots:
    void on_pushButton_clicked();

    void on_updateBtn_clicked();

    void on_lastMessageBtn_clicked();

    void on_filterBtn_clicked();

    void on_resetFilterBtn_clicked();

    void updateOffsetSlot(const QString &offset);
    void filterResultSlot(const QString &data);

    void on_nextBtn_clicked();

private:
    bool initView();
    bool updateView();

    void showMessage(int64_t messageOffset);

    bool initParserComboBox();


    const ParserFunc &getParserFunc() const;
    const FilterFunc &getFilterFunc() const;

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
    void messageIndex(long index);

    void messageDetail(const std::string &detail);

    void startFilterThread(int64_t offset);
    void stopFilterThread();

private:
    Ui::PartitionDetailDlg  *ui;
    KafkaConsumer::Ptr      _pConsumer;
    std::string             _topic;
    int                     _partition;

    int _low;   // 低水位
    int _high;  // 高水位

    // 过滤查找状态任务
    FilterThread::Ptr _filterThread;

    // ui互斥变量
    std::mutex _uiMtx;
};

#endif // PARTITIONDETAILDLG_H
