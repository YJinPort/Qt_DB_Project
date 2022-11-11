#include "productmanager.h"
#include "ui_productmanager.h"
#include <QMessageBox>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>

//생성자 - productlist.txt에 저장된 정보를 불러와 제품리스트에 저장한다.
ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    QSqlDatabase sqlDB = QSqlDatabase::addDatabase("QSQLITE", "productDatabase");
    sqlDB.setDatabaseName("productList.db");
    if(!sqlDB.open()) return;
    query = new QSqlQuery(sqlDB);
    query->exec("CREATE TABLE IF NOT EXISTS product(p_number INTEGER Primary Key, "
                "productName VARCHAR(30) NOT NULL, productPrice INTEGER NOT NULL, "
                "productCount INTEGER NOT NULL, productType VARCHAR(20) NOT NULL);");

    productModel = new QSqlTableModel(this, sqlDB);
    productModel->setTable("product");
    productModel->select();
    productModel->setHeaderData(0, Qt::Horizontal, QObject::tr("제품 번호"));
    productModel->setHeaderData(1, Qt::Horizontal, QObject::tr("제품 이름"));
    productModel->setHeaderData(2, Qt::Horizontal, QObject::tr("제품 가격"));
    productModel->setHeaderData(3, Qt::Horizontal, QObject::tr("제품 수량"));
    productModel->setHeaderData(4, Qt::Horizontal, QObject::tr("제품 종류"));

    ui->productTableView->setModel(productModel);

    setWindowTitle(tr("Product Side"));     //열리는 윈도우의 제목을 Product Side로 설정한다.
}

//소멸자 - 제품리스트에 저장된 정보를 productlist.txt에 저장한다.
ProductManager::~ProductManager()
{
    delete ui;
}

//제품 등록/변경 버튼 클릭 시 동작
void ProductManager::on_proRegisterPushButton_clicked()
{
    /*제품 번호, 이름, 가격, 수량을 입력하지 않았을 경우 경고메시지 발생*/
    if(ui->productNumberLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품 번호를 입력하여 주세요."));
    else if(ui->productNameLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품명을 입력하여 주세요."));
    else if(ui->productPriceLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품 가격을 입력하여 주세요."));
    else if(ui->productCountLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품 수량을 입력하여 주세요."));
    /*제품 등록 or 변경 시작*/
    else {
        int proNumber, proPrice, proCount;
        QString proName, proType, checkProductNumber;

        /*제품에 대한 정보는 입력된 LineEdit에 적혀있는 값을 가져온다.*/
        proNumber = ui->productNumberLineEdit->text().toInt();
        proName = ui->productNameLineEdit->text();
        proPrice = ui->productPriceLineEdit->text().toInt();
        proCount = ui->productCountLineEdit->text().toInt();
        proType = ui->productTypeLineEdit->text();

        query->exec("SELECT p_number FROM product WHERE p_number = " + QString::number(proNumber) + ";");
        QSqlRecord rec = query->record();
        int colIdx = rec.indexOf("p_number");

        while(query->next()) checkProductNumber = query->value(colIdx).toString();

        if(checkProductNumber != "") {
            query->exec("UPDATE product SET productName = '" + proName + "'"
                            ", productPrice = " + QString::number(proPrice) + ""
                            ", productCount = " + QString::number(proCount) + ""
                            ", productType = '" + proType + "' "
                        "WHERE p_number = " + QString::number(proNumber) + ";");
            qDebug() << query->lastError();
            productModel->select();

            //제품 변경 완료에 대한 메시지를 표시한다.
            QMessageBox::information(this, tr("등록 성공"), tr("제품 목록이 변경되었습니다."));
        }
        else {
            query->exec("INSERT INTO product VALUES(" + QString::number(proNumber) + ", '" + proName + "', " +
                        QString::number(proPrice) + ", " + QString::number(proCount) + ", '" + proType + "');");
            productModel->select();

            //제품 등록 완료에 대한 메시지를 표시한다.
            QMessageBox::information(this, tr("등록 성공"), tr("새로운 제품이 등록되었습니다."));
        }

        /*제품 정보가 입력된 LineEdit를 비운다.*/
        ui->productNumberLineEdit->clear();
        ui->productNameLineEdit->clear();
        ui->productPriceLineEdit->clear();
        ui->productCountLineEdit->clear();
        ui->productTypeLineEdit->clear();
    }
}

//제품 삭제 버튼 클릭 시 동작
void ProductManager::on_proRemovePushButton_clicked()
{
    //삭제할 제품의 제품 번호를 LineEdit에서 가져온다.
    int proNumber = ui->productNumberLineEdit->text().toInt();
    QString checkProductNumber;

    //제품 번호를 입력할 LineEdit가 빈칸이 아닐 경우(공백포함)
    if(ui->productNumberLineEdit->text().trimmed() != "") {
        query->exec("SELECT p_number FROM product WHERE p_number = " + QString::number(proNumber) + ";");
        QSqlRecord rec = query->record();
        int colIdx = rec.indexOf("p_number");

        while(query->next()) checkProductNumber = query->value(colIdx).toString();

        if(checkProductNumber == "") QMessageBox::warning(this, tr("삭제 실패"), tr("등록된 제품이 없습니다."));
        else {
            query->exec("DELETE FROM product WHERE p_number = " + QString::number(proNumber) + ";");

            productModel->select();

            //제품 삭제 완료에 대한 메시지를 표시한다.
            QMessageBox::warning(this, tr("삭제 성공"), tr("제품 삭제를 완료하였습니다."));

            /*제품 위젯을 클릭하거나 제품 번호를 입력하여 입력되었던 LineEdit을 비운다.*/
            ui->productNumberLineEdit->clear();
            ui->productNameLineEdit->clear();
            ui->productPriceLineEdit->clear();
            ui->productCountLineEdit->clear();
            ui->productTypeLineEdit->clear();
        }
    }
    //제품 번호를 입력할 LineEdit가 빈칸일 경우(공백포함) 삭제 실패에 대한 메시지를 표시한다.
    else QMessageBox::warning(this, tr("삭제 실패"), tr("제품 번호를 입력하여 주세요."));
}

//회원 정보 수정 버튼 클릭 시 동작
void ProductManager::on_clientUpdatePushButton_clicked()
{
    QString userId, userName, userCall, userAddress, userGender;
    QStringList updateCliList;  //수정할 회원 정보를 담아 보내기 위한 List변수

    /*회원에 대한 정보는 입력된 LineEdit에 적혀있는 값을 가져온다.*/
    userId = ui->userIdLineEdit->text();
    userName = ui->userNameLineEdit->text();
    userCall = ui->userCallLineEdit->text();
    userAddress = ui->userAddressLineEdit->text();
    userGender = ui->userGenderLineEdit->text();

    //updateCliList 변수에 수정할 회원의 정보를 담는다.
    updateCliList << userId << userName << userCall << userAddress << userGender;

    emit updateBtnClicked(updateCliList);   //updateCliList에 담은 회원 정보를 회원 리스트에서 수정하도록 하기 위해 호출하는 SIGNAL

    /*회원 수정을 위해 사용되었던 LineEdit을 비운다.*/
    ui->userIdLineEdit->clear();
    ui->userNameLineEdit->clear();
    ui->userCallLineEdit->clear();
    ui->userAddressLineEdit->clear();
    ui->userGenderLineEdit->clear();
}

//회원 삭제 버튼 클릭 시 동작
void ProductManager::on_clientRemovePushButton_clicked()
{
    QString userId;
    userId = ui->userIdLineEdit->text();    //회원 아이디에 대한 정보를 LineEdit에서 가져온다.

    emit deleteBtnClicked(userId);          //해당 아이디에 대한 회원 정보를 회원 리스트에서 수정하도록 하기 위해 호출하는 SIGNAL
}

//나가기 버튼 클릭 시 동작
void ProductManager::on_getOutPushButton_clicked()
{
    /*제품, 회원 위젯을 클릭하거나 제품 번호, 회원 아이디를 입력하여 입력되었던 LineEdit을 비운다.*/
    ui->productNumberLineEdit->clear();
    ui->productNameLineEdit->clear();
    ui->productPriceLineEdit->clear();
    ui->productCountLineEdit->clear();
    ui->productTypeLineEdit->clear();
    ui->userIdLineEdit->clear();
    ui->userNameLineEdit->clear();
    ui->userCallLineEdit->clear();
    ui->userAddressLineEdit->clear();
    ui->userGenderLineEdit->clear();

    emit quitProduct();                     //쇼핑 화면으로 돌아가기 위해 호출하는 SIGNAL
}

//제품 테이블에서 해당 제품을 클릭했을 경우 우측 라인Edit에 표시
void ProductManager::on_productTableView_clicked(const QModelIndex &index)
{
    QString productNumber = index.sibling(index.row(), 0).data().toString();
    QString productName = index.sibling(index.row(), 1).data().toString();
    QString productPrice = index.sibling(index.row(), 2).data().toString();
    QString productCount = index.sibling(index.row(), 3).data().toString();
    QString productType = index.sibling(index.row(), 4).data().toString();

    ui->productNumberLineEdit->setText(productNumber);
    ui->productNameLineEdit->setText(productName);
    ui->productPriceLineEdit->setText(productPrice);
    ui->productCountLineEdit->setText(productCount);
    ui->productTypeLineEdit->setText(productType);
}

//회원 테이블에서 해당 회원을 클릭했을 경우 우측 라인Edit에 표시
void ProductManager::on_clientListTableView_clicked(const QModelIndex &index)
{
    QString userID = index.sibling(index.row(), 0).data().toString();
    QString userName = index.sibling(index.row(), 1).data().toString();
    QString userCall = index.sibling(index.row(), 2).data().toString();
    QString userAddress = index.sibling(index.row(), 3).data().toString();
    QString userGender = index.sibling(index.row(), 4).data().toString();

    ui->userIdLineEdit->setText(userID);
    ui->userNameLineEdit->setText(userName);
    ui->userCallLineEdit->setText(userCall);
    ui->userAddressLineEdit->setText(userAddress);
    ui->userGenderLineEdit->setText(userGender);
}

//회원 정보 리스트 출력
//void ProductManager::receivedClientInfo(Client *c) {

//    //ui->clientListTreeWidget->addTopLevelItem(c);
//}

//clientManager에서 보내준 회원 정보를 관리자 페이지의 회원 테이블에 등록한다.
void ProductManager::loadToClientTable(QSqlTableModel* clientModel) {
    clientModel->select();
    ui->clientListTableView->setModel(clientModel);
}

//제품 정보를 담아서 보내기(Shopping으로 보내기)
void ProductManager::containProductInfo() {
    emit sendProductTable(productModel);    //쇼핑 화면으로 전달하기 위해 호출하는 SIGNAL
}

void ProductManager::selectProductInfo() {
    emit sendSelectTable(productModel);
}

void ProductManager::resetProductInfo() {
    emit sendResetTable(productModel);
}

//쇼핑에서 주문하기 or 주문변경 시 재고 확인 및 관리
int ProductManager::updateAfterUpCount(QString name, int cnt) {
    int afterCount;
    int checkProductCount;

    query->exec("SELECT productCount FROM product WHERE productName = '" + name + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("productCount");

    while(query->next()) checkProductCount = query->value(colIdx).toInt();
    afterCount = checkProductCount - cnt;           //해당 제품에 대한 기존의 재고량에서 주문된 수량을 뺀다.

    if(afterCount < 0) return -1;   //이후 재고량이 0개보다 적을 경우 재고 부족이라는 의미의 -1을 리턴한다.
    else {
        //상품의 재고량을 변경 이후의 값으로 조정한다.
        query->exec("UPDATE product SET productCount = " + QString::number(afterCount) + " "
                    "WHERE productName = '" + name + "';");
        productModel->select();
    }

    return 0;   //재고량 수정 완료라는 의미의 0을 리턴한다.
}

//쇼핑에서 주문변경, 주문취소 시 재고 관리
void ProductManager::updateAfterDownCount(QString name, int cnt) {
    int afterCount;
    QString checkProductCount;

    query->exec("SELECT productCount FROM product WHERE productName = '" + name + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("productCount");

    while(query->next()) checkProductCount = query->value(colIdx).toString();
    afterCount = checkProductCount.toInt() + cnt;           //해당 제품에 대한 기존의 재고량에서 주문된 수량을 더한다.

    //상품의 재고량을 변경 이후의 값으로 조정한다.
    query->exec("UPDATE product SET productCount = " + QString::number(afterCount) + " "
                "WHERE productName = '" + name + "';");
    productModel->select();
}
