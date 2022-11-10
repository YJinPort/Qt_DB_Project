#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QMdiSubWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientManager;
class ProductManager;
class ShoppingManager;
class ServerSide;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ClientManager *clientManager;
    ProductManager *productManager;
    ShoppingManager *shoppingManager;
    ServerSide *server;

public slots:
    void openClientWindow();
    void openProductWindow();
    void cancellationClient();
    void joinClient();
    void quitProductWindow();
};
#endif // MAINWINDOW_H
