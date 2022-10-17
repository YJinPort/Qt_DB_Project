#include "productmanager.h"
#include "ui_productmanager.h"
#include "product.h"
#include "client.h"
#include <QMessageBox>
#include <QFile>

ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    QFile file("productlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int number = row[0].toInt();
            int price = row[2].toInt();
            int count = row[3].toInt();
            Product* p = new Product(number, row[1], price, count, row[4]);
            ui->treeWidget->addTopLevelItem(p);
            productList.insert(number, p);
        }
    }
    file.close();
}

ProductManager::~ProductManager()
{
    delete ui;

    QFile file("productlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : productList) {
        Product* p = v;
        out << p->getProNumber() << ", " << p->getProName() << ", ";
        out << p->getProPrice() << ", ";
        out << p->getProCount() << ", ";
        out << p->getProType() << "\n";
    }
    file.close( );
}

//제품 등록/변경
void ProductManager::on_pushButton_clicked()
{
    if(ui->lineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품 번호를 입력하여 주세요."));
    else if(ui->lineEdit_2->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품명을 입력하여 주세요."));
    else if(ui->lineEdit_3->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품 가격을 입력하여 주세요."));
    else if(ui->lineEdit_4->text().trimmed() == "") QMessageBox::warning(this, tr("등록 실패"), tr("제품 수량을 입력하여 주세요."));
    else {
        int proNumber, proPrice, proCount;
        QString proName, proType;
        bool newNum = true;

        proNumber = ui->lineEdit->text().toInt();
        proName = ui->lineEdit_2->text();
        proPrice = ui->lineEdit_3->text().toInt();
        proCount = ui->lineEdit_4->text().toInt();
        proType = ui->lineEdit_5->text();

        Q_FOREACH(auto v, productList) {
            Product *p = static_cast<Product*>(v);
            if(proNumber == p->getProNumber()) {    //제품 번호가 이미 등록되어 있을 경우
                //현재 lineEdit에 입력한 값으로 변경한다.
                p->setProName(proName);
                p->setProPrice(proPrice);
                p->setProCount(proCount);
                p->setProType(proType);

                //ui의 treeWidget에 입력되어있는 기존의 값을 삭제한다.
                ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(p));
                ui->treeWidget->update();

                //변경한 값을 제품리스트에 입력한다.(QMap의 경우 Id의 중복을 방지한다)
                productList.insert(proNumber, p);
                ui->treeWidget->addTopLevelItem(p);     //변경한 입력값을 ui의 treeWidget에 등록한다.

                newNum = false;                 //이미 등록된 제품 번호가 존재하므로 newNum을 false로 설정한다.

                QMessageBox::information(this, tr("등록 성공"), tr("제품 목록이 변경되었습니다."));
                break;
            }
        }

        //제품 번호의 중복 유무로 비교한다. (bool 형식)
        if(newNum) {
            Product *p = new Product(proNumber, proName, proPrice, proCount, proType);
            productList.insert(proNumber, p);       //새로운 제품의 값을 제품리스트에 입력한다.
            ui->treeWidget->addTopLevelItem(p);     //입력된 값을 ui의 treeWidget에 등록한다.
            QMessageBox::information(this, tr("등록 성공"), tr("새로운 제품이 등록되었습니다.."));
        }

        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
    }
}

//제품 삭제
void ProductManager::on_pushButton_4_clicked()
{
    int proNumber = ui->lineEdit->text().toInt();
    int cnt = 0;

    if(ui->lineEdit->text().trimmed() != "") {
        Q_FOREACH(auto v, productList) {
            Product *p = static_cast<Product*>(v);
            if(proNumber == p->getProNumber()) {
                productList.remove(proNumber);
                ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(p));
                ui->treeWidget->update();
                QMessageBox::warning(this, tr("삭제 성공"), tr("제품 삭제를 완료하였습니다."));
                ui->lineEdit->clear();
                ui->lineEdit_2->clear();
                ui->lineEdit_3->clear();
                ui->lineEdit_4->clear();
                ui->lineEdit_5->clear();
                cnt++;
                break;
            }
        }
        if(cnt == 0) QMessageBox::warning(this, tr("삭제 실패"), tr("등록된 제품이 없습니다."));
    }
    else QMessageBox::warning(this, tr("삭제 실패"), tr("제품 번호를 입력하여 주세요."));
}

//나가기
void ProductManager::on_pushButton_2_clicked()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();

    emit quitProduct();
}

//회원 정보 리스트 출력
void ProductManager::receivedClientInfo(Client *c) {
    ui->treeWidget_2->addTopLevelItem(c);
}

//제품 정보를 담아서 보내기(Shopping으로 보내기)
void ProductManager::containProductInfo() {
    int proNumber, proPrice, proCount;
    QString proName, proType;

    Q_FOREACH(auto v, productList) {
        Product *p = static_cast<Product*>(v);
        proNumber = p->getProNumber();
        proName = p->getProName();
        proPrice = p->getProPrice();
        proCount = p->getProCount();
        proType = p->getProType();

        Product *item = new Product(proNumber, proName, proPrice, proCount, proType);
        emit sendProductInfo(item);
    }
}

//제품 위젯에서 해당 제품을 클릭했을 경우 우측 라인Edit에 표시
void ProductManager::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    ui->lineEdit->setText(item->text(0));
    ui->lineEdit_2->setText(item->text(1));
    ui->lineEdit_3->setText(item->text(2));
    ui->lineEdit_4->setText(item->text(3));
    ui->lineEdit_5->setText(item->text(4));
}

//회원 위젯에서 해당 회원을 클릭했을 경우 우측 라인Edit에 표시
void ProductManager::on_treeWidget_2_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    ui->lineEdit_6->setText(item->text(1));
    ui->lineEdit_7->setText(item->text(2));
    ui->lineEdit_8->setText(item->text(3));
    ui->lineEdit_9->setText(item->text(4));
    ui->lineEdit_10->setText(item->text(5));
}




