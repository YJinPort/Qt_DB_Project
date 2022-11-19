#include "productmanager.h"
#include "ui_productmanager.h"
#include "product.h"
#include "client.h"
#include <QMessageBox>
#include <QFile>

//생성자 - productlist.txt에 저장된 정보를 불러와 제품리스트에 저장한다.
ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    QFile file("productlist.txt");  //productlist.txt라는 파일을 불러온다.

    /*해당 파일을 텍스트 파일의 읽기 전용으로 열기*/
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;  //파일을 열기에 실패하면 return;

    QTextStream in(&file);  //파일의 정보를 textStream에 저장할 준비를 한다.

    /*저장된 정보가 끝날 때 까지 반복*/
    while (!in.atEnd()) {
        QString line = in.readLine();           //저장 되어있는 정보를 QString타입의 변수에 담는다.
        QList<QString> row = line.split(", ");  //리스트 변수 row에 ", " 구분자를 제외한 데이터를 담는다.
        if(row.size()) {    //리스트가 비어있지 않은 경우
            int number = row[0].toInt();    //0번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.
            int price = row[2].toInt();     //2번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.
            int count = row[3].toInt();     //3번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.

            //해당 정보를 담은 객체를 생성한다.
            Product* p = new Product(number, row[1], price, count, row[4]);
            ui->productListTreeWidget->addTopLevelItem(p);  //관리자 페이지의 제품 리스트 위젯에 출력한다.
            productList.insert(number, p);  //정보를 담은 객체를 제품 리스트에 저장한다.
        }
    }
    file.close();   //productlist.txt파일에 저장된 정보를 모두 제품 리스트에 저장했으므로 파일을 종료한다.

    setWindowTitle(tr("Product Side"));     //열리는 윈도우의 제목을 Product Side로 설정한다.
}

//소멸자 - 제품리스트에 저장된 정보를 productlist.txt에 저장한다.
ProductManager::~ProductManager()
{
    delete ui;

    QFile file("productlist.txt");  //productlist.txt라는 파일을 불러온다(없을 경우 생성한다).

    /*해당 파일을 텍스트 파일의 쓰기 전용으로 열기*/
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return; //파일을 열기에 실패하면 return;

    QTextStream out(&file); //파일의 정보를 textStream으로 출력할 준비를 한다.

    /*제품 리스트에 저장된 정보를 파일에 모두 저장하기 위한 반복문*/
    for (const auto& v : productList) {
        Product* p = v;

        /*제품 리스트에 대한 각 정보들을 , 를 구분자로 하여 파일(productlist.txt)에 저장한다.*/
        out << p->getProNumber() << ", " << p->getProName() << ", ";
        out << p->getProPrice() << ", ";
        out << p->getProCount() << ", ";
        out << p->getProType() << "\n";
    }

    file.close( );  //productlist.txt파일로 제품 리스트에 저장된 정보를 출력해 저장했으므로 파일을 종료한다.
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
        QString proName, proType;
        bool newNum = true;

        /*제품에 대한 정보는 입력된 LineEdit에 적혀있는 값을 가져온다.*/
        proNumber = ui->productNumberLineEdit->text().toInt();
        proName = ui->productNameLineEdit->text();
        proPrice = ui->productPriceLineEdit->text().toInt();
        proCount = ui->productCountLineEdit->text().toInt();
        proType = ui->productTypeLineEdit->text();

        /*등록 or 변경할 제품 번호가 제품 정보 리스트에 등록 되어있는지 확인하기 위한 반복문*/
        Q_FOREACH(auto v, productList) {
            Product *p = static_cast<Product*>(v);  //auto변수 v의 자료형을 Product*형으로 변환 후 고정
            if(proNumber == p->getProNumber()) {    //제품 번호가 이미 등록되어 있을 경우
                /*현재 lineEdit에 입력한 값으로 변경한다.*/
                p->setProName(proName);
                p->setProPrice(proPrice);
                p->setProCount(proCount);
                p->setProType(proType);

                //ui의 treeWidget에 입력되어있는 기존의 값을 삭제한다.
                ui->productListTreeWidget->takeTopLevelItem(ui->productListTreeWidget->indexOfTopLevelItem(p));

                //변경한 값을 제품리스트에 입력한다.(QMap의 경우 Id의 중복을 방지한다)
                productList.insert(proNumber, p);
                ui->productListTreeWidget->addTopLevelItem(p);     //변경한 입력값을 ui의 treeWidget에 등록한다.
                ui->productListTreeWidget->update();

                newNum = false;                 //이미 등록된 제품 번호가 존재하므로 newNum을 false로 설정한다.

                //제품 변경 완료에 대한 메시지를 표시한다.
                QMessageBox::information(this, tr("등록 성공"), tr("제품 목록이 변경되었습니다."));
                break;
            }
        }

        if(newNum) {    //제품 번호의 중복 유무로 비교한다. (bool 형식)
            //새로운 제품으로 등록할 정보를 객체에 담는다.
            Product *p = new Product(proNumber, proName, proPrice, proCount, proType);
            productList.insert(proNumber, p);                                               //새로운 제품의 값을 제품리스트에 입력한다.
            ui->productListTreeWidget->addTopLevelItem(p);                                  //입력된 값을 ui의 treeWidget에 등록한다.
            ui->productListTreeWidget->update();

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
    bool proDelete = true;

    //삭제할 제품의 제품 번호를 LineEdit에서 가져온다.
    int proNumber = ui->productNumberLineEdit->text().toInt();

    //제품 번호를 입력할 LineEdit가 빈칸이 아닐 경우(공백포함)
    if(ui->productNumberLineEdit->text().trimmed() != "") {

        /*입력된 번호의 제품이 제품 정보 리스트에 등록된 제품인지 확인하기 위한 반복문*/
        Q_FOREACH(auto v, productList) {
            Product *p = static_cast<Product*>(v);  //auto변수 v의 자료형을 Product*형으로 변환 후 고정
            if(proNumber == p->getProNumber()) {    //입력한 제품 번호가 제품 리스트에 등록된 제품일 경우
                productList.remove(proNumber);      //제품 리스트에서 해당 제품 번호가 속한 정보를 삭제한다.
                //해당 번호의 제품을 제품 리스트 위젯에서 삭제한다.
                ui->productListTreeWidget->takeTopLevelItem(ui->productListTreeWidget->indexOfTopLevelItem(p));
                ui->productListTreeWidget->update();

                //제품 삭제 완료에 대한 메시지를 표시한다.
                QMessageBox::warning(this, tr("삭제 성공"), tr("제품 삭제를 완료하였습니다."));

                /*제품 위젯을 클릭하거나 제품 번호를 입력하여 입력되었던 LineEdit을 비운다.*/
                ui->productNumberLineEdit->clear();
                ui->productNameLineEdit->clear();
                ui->productPriceLineEdit->clear();
                ui->productCountLineEdit->clear();
                ui->productTypeLineEdit->clear();

                proDelete = false;  //제품 삭제 알림 메시지 표시용 변수(bool)

                break;
            }
        }
        //제품 번호가 달라 삭제 실패에 대한 메시지를 표시한다.
        if(proDelete) QMessageBox::warning(this, tr("삭제 실패"), tr("등록된 제품이 없습니다."));
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

    ui->clientListTreeWidget->clear();      //회원 정보 리스트의 중복 출력 방지를 위해 트리위젯을 초기화

    emit quitProduct();                     //쇼핑 화면으로 돌아가기 위해 호출하는 SIGNAL
}

//제품 위젯에서 해당 제품을 클릭했을 경우 우측 라인Edit에 표시
void ProductManager::on_productListTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);   //현재의 함수 내에서 사용하지 않는 인자에 대한 처리

    /*제품의 수정 or 삭제를 편리하게 하기 위해 위젯에 마우스로 선택한 값의 정보를 LineEdit에 입력한다.*/
    ui->productNumberLineEdit->setText(item->text(0));
    ui->productNameLineEdit->setText(item->text(1));
    ui->productPriceLineEdit->setText(item->text(2));
    ui->productCountLineEdit->setText(item->text(3));
    ui->productTypeLineEdit->setText(item->text(4));
}

//회원 위젯에서 해당 회원을 클릭했을 경우 우측 라인Edit에 표시
void ProductManager::on_clientListTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);   //현재의 함수 내에서 사용하지 않는 인자에 대한 처리

    /*회원의 수정 or 삭제를 편리하게 하기 위해 위젯에 마우스로 선택한 값의 정보를 LineEdit에 입력한다.*/
    ui->userIdLineEdit->setText(item->text(1));
    ui->userNameLineEdit->setText(item->text(2));
    ui->userCallLineEdit->setText(item->text(3));
    ui->userAddressLineEdit->setText(item->text(4));
    ui->userGenderLineEdit->setText(item->text(5));
}

//회원 정보 리스트 출력
void ProductManager::receivedClientInfo(Client *c) {
    //clientManager에서 보내준 회원 정보를 관리자 페이지의 회원 위젯에 등록한다.
    ui->clientListTreeWidget->addTopLevelItem(c);
}

//회원 정보 수정 시 우측 라인Edit을 비우고 리스트를 새롭게 load하기 위해 비운다.
void ProductManager::clearClientWidget_N_LineEdit() {
    /*회원 수정을 위해 사용되었던 LineEdit을 비운다.*/
    ui->userIdLineEdit->clear();
    ui->userNameLineEdit->clear();
    ui->userCallLineEdit->clear();
    ui->userAddressLineEdit->clear();
    ui->userGenderLineEdit->clear();

    //수정된 정보를 정확하게 나타내기 위해 기존의 입력값을 비운다.
    ui->clientListTreeWidget->clear();
}

//제품 정보를 담아서 보내기(Shopping으로 보내기)
void ProductManager::containProductInfo() {
    int proNumber, proPrice, proCount;
    QString proName, proType;

    /*제품의 전체 목록의 정보를 보내기 위한 반복문*/
    Q_FOREACH(auto v, productList) {
        Product *p = static_cast<Product*>(v);  //auto변수 v의 자료형을 Product*형으로 변환 후 고정

        /*각 타입에 맞는 제품의 정보를 해당 변수에 담는다.*/
        proNumber = p->getProNumber();
        proName = p->getProName();
        proPrice = p->getProPrice();
        proCount = p->getProCount();
        proType = p->getProType();

        //해당 정보를 item이라는 객체에 담는다.
        Product *item = new Product(proNumber, proName, proPrice, proCount, proType);
        emit sendProductInfo(item); //쇼핑 화면으로 전달하기 위해 호출하는 SIGNAL
    }
}

//쇼핑에서 주문하기 or 주문변경 시 재고 확인 및 관리
int ProductManager::updateAfterUpCount(QString name, int cnt) {
    int afterCount;

    /*쇼핑 화면에서 주문한 제품에 대한 정보를 찾아 수정하기 위한 반복문*/
    Q_FOREACH(auto v, productList) {
        Product *p = static_cast<Product*>(v);          //auto변수 v의 자료형을 Product*형으로 변환 후 고정
        if(name == p->getProName()) {                   //QString 인자값이 제품 리스트에 등록 되어있는 제품의 이름과 같을 경우
            afterCount = p->getProCount() - cnt;        //해당 제품에 대한 기존의 재고량에서 주문된 수량을 뺀다.
            if(afterCount < 0) return -1;               //이후 재고량이 0개보다 적을 경우 재고 부족이라는 의미의 -1을 리턴한다.
            p->setProCount(afterCount);                 //상품의 재고량을 변경 이후의 값으로 조정한다.
            productList.insert(p->getProNumber(), p);   //변경된 정보를 productList에 저장한다.
            break;
        }
    }
    return 0;   //재고량 수정 완료라는 의미의 0을 리턴한다.
}

//쇼핑에서 주문변경, 주문취소 시 재고 관리
void ProductManager::updateAfterDownCount(QString name, int cnt) {
    int afterCount;

    /*쇼핑 화면에서 주문한 제품에 대한 정보를 찾아 수정하기 위한 반복문*/
    Q_FOREACH(auto v, productList) {
        Product *p = static_cast<Product*>(v);          //auto변수 v의 자료형을 Product*형으로 변환 후 고정
        if(name == p->getProName()) {                   //QString 인자값이 제품 리스트에 등록 되어있는 제품의 이름과 같을 경우
            afterCount = p->getProCount() + cnt;        //해당 제품에 대한 기존의 재고량에서 주문된 수량을 더한다.
            p->setProCount(afterCount);                 //상품의 재고량을 변경 이후의 값으로 조정한다.
            productList.insert(p->getProNumber(), p);   //변경된 정보를 productList에 저장한다.
            break;
        }
    }
}


