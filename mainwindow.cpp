#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItemModel>

#include "partitiondetaildlg.h"
#include "kafkaconsumer.h"

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
        itemList.push_back(new QStandardItem(qs));

        qs.clear();
        for (auto p : it->isrsVector) {
            qs.append(QString::number(p) + ",");
        }
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

bool MainWindow::initTreeView() {
    QStandardItemModel *pItemModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    pItemModel->setHorizontalHeaderLabels(QStringList() << "主题" << "分区id"
                                          << "主节点" << "replicas" << "isr");
    int row = 0;
    for (const auto &topic : _topics) {
        insertTopic2View(pItemModel, row ++, topic);
    }

    resizeClolumsSize();

    return true;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    DLOG << "resize";
}

void MainWindow::resizeClolumsSize()
{
    QStandardItemModel *pModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    int columnCount = pModel->columnCount();

    for (int i = 0; i < columnCount; ++ i) {
        ui->treeView->resizeColumnToContents(i);
    }
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
    PartitionDetailDlg *pPartitionDetailDlg = new PartitionDetailDlg(consumerPtr(),
                                                                     pTopic,
                                                                     partition,
                                                                     this);
    // pPartitionDetailDlg->open();
    pPartitionDetailDlg->show();
}

void MainWindow::on_filterEdit_textChanged(const QString &arg1)
{
    QStandardItemModel *pItemModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    if (arg1.length()) {
        pItemModel->clear();

        pItemModel->setHorizontalHeaderLabels(QStringList() << "主题" << "分区id"
                                              << "主节点" << "replicas" << "isr");

        int row = 0;
        QRegExp regExp(arg1, Qt::CaseInsensitive);
        for (const auto &topic : _topics) {
            DLOG << regExp.indexIn(QString::fromStdString(topic.topic));
            if (regExp.indexIn(QString::fromStdString(topic.topic)) >= 0) {
                insertTopic2View(pItemModel, row ++, topic);
            }
        }

        resizeClolumsSize();
    } else {
        initTreeView();
    }
}

void MainWindow::on_pushButton_clicked()
{
    _topics.clear();

    consumerPtr()->topics(_topics);
    QStandardItemModel *pItemModel = dynamic_cast<QStandardItemModel*>(ui->treeView->model());

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
    initTreeView();
}
