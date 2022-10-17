#include "shoppingmanager.h"
#include "ui_shoppingmanager.h"
#include "shopping.h"
#include "product.h"

#include <QInputDialog>
#include <QMessageBox>

ShoppingManager::ShoppingManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShoppingManager)
{
    ui->setupUi(this);
}

void ShoppingManager::dataLoad() {
    emit viewProductList();
}

void ShoppingManager::dataClear() {
    ui->treeWidget->clear();
}

ShoppingManager::~ShoppingManager()
{
    delete ui;
}

//회원 가입
void ShoppingManager::on_pushButton_clicked()
{
    emit newClient();
}

//회원정보, 제품정보 관리
void ShoppingManager::on_pushButton_6_clicked()
{
    bool ok;
    QString passwd;

    do {
        passwd = QInputDialog::getText(this, "Manager", "관리자 번호를 입력하세요.", QLineEdit::Normal, NULL, &ok);
        if(ok == false) break;
    } while(ok != true || passwd != "ossmall");

    if(ok == true) {
        emit viewClientList();
        emit onlyStaff();
    }
    //수정 필요
    //관리 화면 들어갈때마다 회원 정보가 중복되어 나타남
}

//쇼핑 종료
void ShoppingManager::on_pushButton_7_clicked()
{
    emit exitShopping();
}

//로그인 버튼
void ShoppingManager::on_pushButton_2_clicked()
{
    emit login(ui->lineEdit->text());
}

void ShoppingManager::successLoginCheck() {
    ui->label_2->setText(ui->lineEdit->text() + "님의 주문내역");
}

void ShoppingManager::failedLoginCheck() {
    ui->lineEdit->clear();

    //로그인 실패 메시지
    QMessageBox::critical(this, tr("로그인 실패"), tr("아이디가 일치하지 않습니다."));
}

//주문하기
void ShoppingManager::on_pushButton_3_clicked()
{
    qDebug("주문 성공");

    int proPrice, proCount;
    QString proName, proType, address;


}

//주문 변경
void ShoppingManager::on_pushButton_5_clicked()
{
    qDebug("주문 변경");
}

//주문 취소
void ShoppingManager::on_pushButton_4_clicked()
{
    qDebug("주문 취소");
}

//제품 정보 리스트 출력
void ShoppingManager::receivedProductInfo(Product *p) {
    ui->treeWidget->addTopLevelItem(p);
}
