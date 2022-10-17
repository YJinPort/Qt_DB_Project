#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QWidget>

class Product;
class Client;
class QTreeWidgetItem;

namespace Ui {
class ProductManager;
}

class ProductManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManager(QWidget *parent = nullptr);
    ~ProductManager();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void receivedClientInfo(Client*);

    void containProductInfo();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_2_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_4_clicked();

private:
    Ui::ProductManager *ui;

    QMap<int, Product*> productList;

signals:
    void quitProduct();
    void sendProductInfo(Product*);
};

#endif // PRODUCTMANAGER_H
