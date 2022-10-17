#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanager.h"
#include "productmanager.h"
#include "shoppingmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(1400, 800);

    shoppingManager = new ShoppingManager(this);
    clientManager = new ClientManager(this);
    productManager = new ProductManager(this);

    ui->mdiArea->addSubWindow(shoppingManager);
    ui->mdiArea->addSubWindow(clientManager);
    ui->mdiArea->addSubWindow(productManager);
    shoppingManager->showMaximized();

    connect(shoppingManager, SIGNAL(newClient()), this, SLOT(openClientWindow()));
    connect(shoppingManager, SIGNAL(onlyStaff()), this, SLOT(openProductWindow()));
    connect(shoppingManager, SIGNAL(exitShopping()), this, SLOT(close()));
    connect(shoppingManager, SIGNAL(viewClientList()), clientManager, SLOT(containClientInfo()));
    connect(shoppingManager, SIGNAL(viewProductList()), productManager, SLOT(containProductInfo()));
    connect(shoppingManager, SIGNAL(login(QString)), clientManager, SLOT(checkLoginId(QString)));

    connect(clientManager, SIGNAL(cancellation()), this, SLOT(cancellationClient()));
    connect(clientManager, SIGNAL(join()), this, SLOT(joinClient()));
    connect(clientManager, SIGNAL(sendClientInfo(Client*)), productManager, SLOT(receivedClientInfo(Client*)));
    connect(clientManager, SIGNAL(successLogin()), shoppingManager, SLOT(successLoginCheck()));
    connect(clientManager, SIGNAL(failedLogin()), shoppingManager, SLOT(failedLoginCheck()));

    connect(productManager, SIGNAL(quitProduct()), this, SLOT(quitProductWindow()));
    connect(productManager, SIGNAL(sendProductInfo(Product*)), shoppingManager, SLOT(receivedProductInfo(Product*)));

    shoppingManager->dataLoad();
}

void MainWindow::openClientWindow() {
    clientManager->setFocus();
}

void MainWindow::joinClient() {
    shoppingManager->setFocus();
}

void MainWindow::cancellationClient() {
    shoppingManager->setFocus();
}

void MainWindow::openProductWindow() {
    productManager->setFocus();
}

void MainWindow::quitProductWindow() {
    shoppingManager->dataClear();
    shoppingManager->dataLoad();
    shoppingManager->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
