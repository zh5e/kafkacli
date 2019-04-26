#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItemModel>

#include "partitiondetaildlg.h"
#include "kafkaconsumer.h"
#include "logger.h"

namespace  {
void insertTopic2View(QStandardItemModel *pItemModel, int row, const TopicMetadata &topic)
{
    auto *pPartent = new QStandardItem(
                         QString::fromStdString(topic.topic));
    pItemModel->insertRow(row, pPartent);

    int childRow = 0;

    for (auto it = topic.partitions.rbegin();
         it != topic.partitions.rend(); ++ it) {

        QList<QStandardItem*> itemList;

        itemList.push_back(new QStandardItem());
        itemList.push_back(new QStandardItem(QString::number(it->id)));
        itemList.push_back(new QStandardItem(QString::number(it->leader)));

        QString qs;
        for (auto p : it->replicasVector) {
            qs.append(QString::number(p) + ",");
        }
        qs.remove(qs.length()-1, 1);

        itemList.push_back(new QStandardItem(qs));

        qs.clear();
        for (auto p : it->isrsVector) {
            qs.append(QString::number(p) + ",");
        }
        qs.remove(qs.length()-1, 1);

        itemList.push_back(new QStandardItem(qs));

        pPartent->insertRow(childRow, itemList);
    }
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("主题信息");

    // ui->treeView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStandardItemModel *pItemModel = new QStandardItemModel(ui->treeView);
    ui->treeView->setModel(pItemModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initTreeView(const QString &substr) {
    QStandardItemModel *pItemModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    pItemModel->setHorizontalHeaderLabels(QStringList() << "主题" << "分区id"
                                          << "主节点" << "replicas" << "isr");

    QRegExp regExp(substr, Qt::CaseInsensitive);

    int row = 0;
    for (const auto &topic : _topics) {
        if (substr.length() > 0) {
            DLOG << regExp.indexIn(QString::fromStdString(topic.topic));
            if (regExp.indexIn(QString::fromStdString(topic.topic)) >= 0) {
                insertTopic2View(pItemModel, row ++, topic);
            }
        } else {
            insertTopic2View(pItemModel, row ++, topic);
        }
    }

    resizeClolumsSize();

    return true;
}

void MainWindow::resizeClolumsSize()
{
    QStandardItemModel *pModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    int columnCount = pModel->columnCount();

    for (int i = 0; i < columnCount; ++ i) {
        ui->treeView->resizeColumnToContents(i);
    }
}

void MainWindow::clearAllTopics()
{
    QStandardItemModel *pItemModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    DLOG << "filter text: " << ui->filterEdit->text();

    int rowCount = pItemModel->rowCount();
    int columnCount = pItemModel->columnCount();

    for (int i = 0; i < rowCount; ++ i) {
        for (int j = 0; j < columnCount; ++ j) {
            auto *pItem = pItemModel->item(i, j);
            if (pItem) {
                delete pItem;
            }
        }
    }

    pItemModel->clear();
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel *pModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    auto *pParent = pModel->itemFromIndex(index.parent());
    if (!pParent) {
        return;
    }
    const QModelIndex indexColumn0 = index.sibling(index.row(), 1);
    auto *pItem = pModel->itemFromIndex(indexColumn0);

    DLOG << "clicked item" << pItem->text() << ", parent: " << pParent->text();

    const auto *pTopic = pParent->text().toUtf8().data();
    int partition = pItem->text().toInt();
    auto *pPartitionDetailDlg = new PartitionDetailDlg(consumerPtr(),
                                                       pTopic, partition, this);
    pPartitionDetailDlg->open();
    // pPartitionDetailDlg->show();
}

void MainWindow::on_filterEdit_textChanged(const QString &arg1)
{
    clearAllTopics();

    initTreeView(arg1);
}

void MainWindow::on_pushButton_clicked()
{
    _topics.clear();
    consumerPtr()->topics(_topics);

    clearAllTopics();

    auto filterString = ui->filterEdit->text();
    initTreeView(filterString);
}
