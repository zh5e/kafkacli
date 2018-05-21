#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

#include "kafkaconsumer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using TopicMetadataVec = std::vector<TopicMetadata>;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool initTreeView();

    KafkaConsumer::Ptr consumerPtr() {
        return _pConsumer;
    }

    void consumerPtr(KafkaConsumer::Ptr ptr) {
        _pConsumer = ptr;
        _topics = _pConsumer->topics();
    }

private:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_filterEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::MainWindow      *ui;
    KafkaConsumer::Ptr  _pConsumer;
    TopicMetadataVec    _topics;
};

#endif // MAINWINDOW_H
