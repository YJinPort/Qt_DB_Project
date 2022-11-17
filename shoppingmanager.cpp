#include "shoppingmanager.h"
#include "ui_shoppingmanager.h"
#include "chattingform_client.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QSpinBox>
#include <QSqlRecord>
#include <QSqlError>

//생성자 - 주문용 DB와 모델을 생성하고 형식에 맞게 지정한다.
ShoppingManager::ShoppingManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShoppingManager)
{
    ui->setupUi(this);

    /*주문용 DB 생성*/
    sqlDB = QSqlDatabase::addDatabase("QSQLITE", "shoppingDatabase");
    sqlDB.setDatabaseName("shoppingList.db");
    if(!sqlDB.open()) return;

    /*주문 번호를 기본키로 하는 제품 테이블 생성*/
    query = new QSqlQuery(sqlDB);
    query->exec("CREATE TABLE IF NOT EXISTS shopping(orderNumber INTEGER Primary Key, "
                "orderProName VARCHAR(30) NOT NULL, orderProPrice INTEGER NOT NULL, "
                "orderProCount INTEGER NOT NULL, orderProType VARCHAR(20), "
                "orderTotPrice INTEGER NOT NULL, orderUserID VARCHAR(30) NOT NULL, "
                "orderAddress VARCHAR(100) NOT NULL);");

    /*주문 테이블용 모델 생성 및 헤더 지정*/
    shoppingModel = new QSqlTableModel(this, sqlDB);
    shoppingModel->setTable("shopping");
    shoppingModel->select();
    shoppingModel->setHeaderData(0, Qt::Horizontal, QObject::tr("주문 번호"));
    shoppingModel->setHeaderData(1, Qt::Horizontal, QObject::tr("주문 제품"));
    shoppingModel->setHeaderData(2, Qt::Horizontal, QObject::tr("제품 가격"));
    shoppingModel->setHeaderData(3, Qt::Horizontal, QObject::tr("주문 수량"));
    shoppingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("제품 종류"));
    shoppingModel->setHeaderData(5, Qt::Horizontal, QObject::tr("총 가격"));
    shoppingModel->setHeaderData(6, Qt::Horizontal, QObject::tr("회원 아이디"));
    shoppingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("배송 주소"));

    ui->orderListTableView->setModel(shoppingModel);    //생성한 모델을 테이블 뷰로 출력하기 위해 지정
    ui->orderListTableView->setColumnHidden(6, true);   //불필요한 정보를 출력하지 않음

    /*각 컬럼의 길이 지정*/
    ui->orderListTableView->setColumnWidth(0, 85);
    ui->orderListTableView->setColumnWidth(1, 150);
    ui->orderListTableView->setColumnWidth(2, 100);
    ui->orderListTableView->setColumnWidth(3, 85);
    ui->orderListTableView->setColumnWidth(4, 100);
    ui->orderListTableView->setColumnWidth(5, 100);
    ui->orderListTableView->setColumnWidth(7, 200);

    /*사용자에게 전체 주문 내역을 공개하지 않기 위해 로그인 전까지 숨김*/
    query->exec("SELECT orderNumber FROM shopping;");
    while(query->next()) rowHiddenCount++;
    for(int i = 0; i < rowHiddenCount; i++) ui->orderListTableView->setRowHidden(i, true);

    setWindowTitle(tr("Shopping Side"));    //열리는 윈도우의 제목을 Shopping Side로 설정한다.
}

//소멸자 - 생성한 객체를 삭제한다.
ShoppingManager::~ShoppingManager()
{
    delete ui;

    /*생성한 DB 객체 삭제*/
    QSqlDatabase db = QSqlDatabase::database("shoppingDatabase");
    if(db.isOpen()) {
        shoppingModel->submitAll();
        delete shoppingModel;
        db.commit();
        db.close();
    }
}

//제품 리스트의 정보를 불러오기 위한 신호를 보내는 함수
void ShoppingManager::dataLoad() {
    emit viewProductList(); //제품 리스트를 불러오기 위해 호출하는 SIGANL
}

//등록되어 있는 제품 정보를 쇼핑화면의 제품 정보 리스트에 출력하는 SLOT 함수
void ShoppingManager::receivedProductInfo(QSqlTableModel *productModel) {
    //관리자 페이지에서 받아온 제품 정보를 쇼핑 화면의 제품 리스트 위젯에 등록
    productModel->select();
    ui->productInfoTableView->setModel(productModel);   //생성한 모델을 테이블 뷰로 출력하기 위해 지정

    /*각 컬럼의 길이 지정*/
    ui->productInfoTableView->setColumnWidth(0, 85);
    ui->productInfoTableView->setColumnWidth(1, 210);
    ui->productInfoTableView->setColumnWidth(2, 120);
    ui->productInfoTableView->setColumnWidth(3, 85);
    ui->productInfoTableView->setColumnWidth(4, 210);
}

//제품 검색 버튼 클릭 시 동작
void ShoppingManager::on_selectPushButton_clicked()
{
    emit clickedSelectButton(); //제품 검색에 필요한 정보를 가져오기 위해 호출하는 SIGNAL
}

//검색한 제품을 출력하는 SLOT 함수
void ShoppingManager::viewSelectProductList(QSqlTableModel* selectModel) {
    /*검색 LineEdit에 입력된 정보가 포함된 제품을 검색한다.*/
    selectModel->setFilter("(productName LIKE '%" + ui->selectLineEdit->text() + "%') "
                           "or (productType LIKE '%" + ui->selectLineEdit->text() + "%');");
    selectModel->select();

    ui->selectLineEdit->clear();    //검색 후 검색 LineEdit를 비운다.
}

//검색 초기화 버튼 클릭 시 동작
void ShoppingManager::on_resetPushButton_clicked()
{
    emit resetProductList();  //검색한 제품리스트를 초기화 하기 위해 호출하는 SIGNAL
}

//제품 목록을 검색 전 상태로 초기화하는 SLOT 함수
void ShoppingManager::productViewReset(QSqlTableModel* selectModel) {
    /*제품 리스트 출력의 조건을 제거한다.(전체 리스트를 출력하게 한다.)*/
    selectModel->setFilter("");
    selectModel->select();

    ui->selectLineEdit->clear();    //검색 LineEdit를 비운다.
}

//주문 번호를 자동으로 생성하여 전달해주기 위한 함수
int ShoppingManager::shoppingNumber() {
    QString checkNumber;
    int orderNumber;

    /*주문 시 필요한 주문 번호를 구하기 위해 전체 주문 번호를 검색한다.*/
    query->exec("SELECT orderNumber FROM shopping;");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("orderNumber");

    //마지막 주문 번호를 구한다.
    while(query->next()) checkNumber = query->value(colIdx).toString();
    orderNumber = checkNumber.toInt();

    if(checkNumber == "") return 101;    //주문 정보 리스트에 저장된 정보가 없으면 1을 반환한다.
    else return ++orderNumber;           //얻은 키값에 1을 더한 값을 반환한다.
}

//회원가입 버튼 클릭 시 동작
void ShoppingManager::on_addNewClientPushButton_clicked()
{
    //'회원가입'버튼이 '마이 페이지'버튼으로 변경된 경우
    if(ui->addNewClientPushButton->text() == "마이 페이지") {
        emit myPage(currentUserID); //회원가입 버튼 이름이 '마이 페이지'일 경우 호출되는 신호
    }
    //'회원가입'버튼일 경우
    else emit newClient();   //회원 가입 화면을 열기 위해 호출하는 SIGNAL
}

//회원탈퇴 버튼 클릭 시 동작
void ShoppingManager::on_removeClientPushButton_clicked()
{
    bool questionCheck, inputUserId;
    int cnt = 0, orderIdx, nameIdx, countIdx;
    int checkUserId, eraseNum, eraseCount;
    QString question, userId;
    QString eraseName;
    QSqlQuery *deleteQuery = new QSqlQuery(sqlDB);

   /*입력값의 예외처리를 위한 do-while문*/
    do {
        question = QInputDialog::getText(this, "회원 탈퇴", "탈퇴하시겠습니까? (입력: 회원탈퇴)", QLineEdit::Normal, NULL, &questionCheck);
        if(questionCheck == false) break;
    } while(question.trimmed() != "회원탈퇴");

    if(question.trimmed() == "회원탈퇴") {  //"회원탈퇴"를 온전히 입력했을 경우
        /*탈퇴하기 위해 입력한 아이디의 예외처리, 검사를 위한 do-while문*/
        do {
            //탈퇴할 아이디의 입력을 받는다.
            userId = QInputDialog::getText(this, "Manager", "회원 아이디를 입력해주세요.", QLineEdit::Normal, NULL, &inputUserId);
            if(inputUserId == false) break; //inputDialog의 취소를 누를 경우 종료한다.
            checkUserId = emit deleteClient(userId);    //입력한 회원 아이디 값이 회원 정보 리스트에 등록되어 있는지 확인하기 위해 호출하는 SIGNAL

            //SIGNAL이 결과 여부에 따라 출력하는 메시지를 구분한다.
            if(checkUserId <= 0) {
                QMessageBox::warning(this, tr("탈퇴 실패"), tr("존재하지 않는 아이디 입니다."));
                return;
            }
            else QMessageBox::warning(this, tr("탈퇴 성공"), tr("회원 탈퇴되었습니다."));
        } while(checkUserId <= 0);

        query->exec("SELECT orderNumber, orderProName, orderProCount FROM shopping "
                    "WHERE orderUserID = '" + userId + "';");
        QSqlRecord rec = query->record();
        orderIdx = rec.indexOf("orderNumber");
        nameIdx = rec.indexOf("orderProName");
        countIdx = rec.indexOf("orderProCount");

        while(query->next()) {
            eraseNum = query->value(orderIdx).toInt();
            eraseName = query->value(nameIdx).toString();
            eraseCount = query->value(countIdx).toInt();
            emit updateAfter_downCount(eraseName, eraseCount);  //제품의 재고 관리를 위해 호출하는 SIGNAL
            deleteQuery->exec("DELETE FROM shopping WHERE orderNumber = " + QString::number(eraseNum) + ";");
            cnt++;
        }

        /*ui의 초기화를 위해 버튼 텍스트를 변경한다.*/
        ui->orderListLabel->setText("주문 내역");
        ui->shoppingLoginPushButton->setText("로그인");
        ui->addNewClientPushButton->setText("회원가입");

        for(int i = 0; i < cnt; i++) ui->orderListTableView->setRowHidden(i, true);
    }
}

//로그인 버튼 클릭 시 동작
void ShoppingManager::on_shoppingLoginPushButton_clicked()
{
    //로그인 버튼일 경우
    if(ui->shoppingLoginPushButton->text() == "로그인") {
        emit login(ui->userIdLoginLineEdit->text());    //입력한 로그인 아이디가 clientList에 등록되어 있는지 확인
        ui->userIdLoginLineEdit->clear();               //로그인할 아이디를 입력한 LineEdit를 비운다.
    }
    //로그아웃 버튼일 경우
    else {
        /*로그인 상태였던 회원의 주문내역을 초기화한다.*/
        query->exec("SELECT orderNumber FROM shopping WHERE orderUserID = '" + currentUserID + "';");
        while(query->next()) rowHiddenCount++;
        for(int i = 0; i < rowHiddenCount; i++) ui->orderListTableView->setRowHidden(i, true);

        ui->orderListLabel->setText("주문 내역");           //주문하기 버튼의 예외처리를 위해 재지정한다.
        ui->userIdLoginLineEdit->clear();                  //입력된 문자가 있을 경우를 대비하여 LineEdit을 비운다.
        ui->addNewClientPushButton->setText("회원가입");    //로그아웃 하므로 회원가입 버튼으로 되돌린다.
        ui->shoppingLoginPushButton->setText("로그인");     //로그아웃 하므로 로그인 버튼으로 되돌린다.
    }
}

//로그인 성공 시 동작하는 SLOT 함수
void ShoppingManager::successLoginCheck(QString userName, QString userID) {
    ui->orderListLabel->setText(userName + "님의 주문내역");   //Label에 사용자 이름 표시
    ui->addNewClientPushButton->setText("마이 페이지");        //'회원가입'버튼을 '마이 페이지'버튼으로 변경
    ui->shoppingLoginPushButton->setText("로그아웃");         //'로그인'버튼을 '로그아웃'버튼으로 변경

    currentUserID = userID;         //현재 로그인한 회원의 아이디를 저장한다.
    loadShoppingView(userID);     //로그인한 사용자가 주문한 주문내역을 불러오기 위한 함수
}

//로그인 성공 시 주문 내역 리스트에 해당 사용자가 주문한 리스트 출력
void ShoppingManager::loadShoppingView(QString userID) {
    /*주문내역을 출력하기 위해 숨겼던 주문 리스트를 출력한다.*/
    query->exec("SELECT orderNumber FROM shopping;");
    while(query->next()) rowHiddenCount++;
    for(int i = 0; i < rowHiddenCount; i++) ui->orderListTableView->setRowHidden(i, false);

    //주문내역 중 로그인한 회원의 주문내역만 출력하기 위해 조건을 추가한다.
    shoppingModel->setFilter("orderUserID = '" + userID + "';");
    shoppingModel->select();
}

//로그인 실패 시 동작하는 SLOT 함수
void ShoppingManager::failedLoginCheck() {
    //로그인 실패 메시지
    QMessageBox::critical(this, tr("로그인 실패"), tr("아이디가 일치하지 않습니다."));
}

//회원 정보 수정 후 라벨에서의 이름 표시
void ShoppingManager::updateLabelName(QString name) {
    //Label에 마이 페이지에서 수정된 사용자 이름 표시
    ui->orderListLabel->setText(name + "님의 주문내역");
}

//주문하기 버튼 클릭 시 동작
void ShoppingManager::on_takeOrderPushButton_clicked()
{
    int orderNumber, proPrice, proCount, checkCount, totPrice;
    QString proName, proType, address, userName, userID;
    QList<QString> labelText;   //로그인한 아이디의 회원 이름을 구하기 위해 사용한 List변수
    bool ok;

    /*inputDialog에서 숫자 이외의 문자에 대해 예외처리를 위해 사용*/
    QLineEdit *onlyNum = new QLineEdit(this);
    QIntValidator *intValidator = new QIntValidator(this);

    onlyNum->setValidator(intValidator);                 //입력할 LineEdit을 숫자 값인지 검사하도록 지정
    labelText = ui->orderListLabel->text().split("님");  //OOO님의 주문내역 Label에서 이름을 구해오기 위해 실행
    userName = labelText[0];                           //split으로 자른 문장에서 사용자의 이름 부분을 userName 변수에 저장

    /*로그인을 성공하여 Label의 길이가 길어지고 주문할 제품을 제품 위젯에서 선택했을 경우 실행*/
    if(ui->orderListLabel->text().length() > 5 && ui->productInfoTableView->currentIndex().isValid()) {
        orderNumber = shoppingNumber();   //주문 번호의 경우 숫자를 자동으로 리턴해주는 함수를 사용하여 설정

        /*제품 이름, 제품 가격은 제품 테이블 뷰의 값을 가져온다.*/
        proName = ui->productInfoTableView->currentIndex().sibling(ui->productInfoTableView->currentIndex().row(), 1).data().toString();
        proPrice = ui->productInfoTableView->currentIndex().sibling(ui->productInfoTableView->currentIndex().row(), 2).data().toInt();
        proType = ui->productInfoTableView->currentIndex().sibling(ui->productInfoTableView->currentIndex().row(), 4).data().toString();

        /*숫자 이외의 문자가 inputDialog에 입력되었을 경우 0을 리턴한다.*/
        proCount = QInputDialog::getText(this, "Order", "주문 수량을 입력하세요.", onlyNum->Normal, NULL, &ok).toInt();

        if(ok == false) return;     //취소 버튼을 클릭했을 경우 실행
        else if(proCount <= 0) {    //숫자 이외의 문자가 inputDialog에 입력되었을 경우 실행
            QMessageBox::warning(this, tr("주문 실패"), tr("주문 수량을 확인해주세요."));
            return;
        }

        //회원 주소의 경우 clientList에서 구해온다.
        address = emit takeOrderSign(currentUserID);   //회원 주소를 구하기 위해 호출하는 SIGNAL

        //주문 시 제품의 재고량을 확인하기 위한 SIGNAL(리턴 값을 받아옴)
        checkCount = emit updateAfter_upCount(ui->productInfoTableView->currentIndex().sibling
                                             (ui->productInfoTableView->currentIndex().row(), 1).data().toString(),
                                              proCount);

        if(checkCount < 0) {
            QMessageBox::information(this, tr("주문 실패"), tr("재고가 부족합니다."));
            return;
        }

        totPrice = proPrice * proCount; //제품의 가격과 주문 수량을 이용해 총 가격을 구한다.

        //주문한 내역을 shopping테이블에 저장한다.
        query->exec("INSERT INTO shopping VALUES(" + QString::number(orderNumber) + ", '" + proName + "', " +
                    QString::number(proPrice) + ", " + QString::number(proCount) + ", '" + proType + "', " +
                    QString::number(totPrice) + ", '" + currentUserID + "', '" + address + "');");

        QMessageBox::information(this, tr("주문 성공"), tr("주문이 완료되었습니다."));

        /*주문 이후의 수정된  주문에 대한 출력을 위해 실행한다.*/
        shoppingModel->select();
    }
    else if(ui->orderListLabel->text().length() <= 5) {
        QMessageBox::warning(this, tr("주문 실패"), tr("로그인 후 주문 가능합니다."));
        return;
    }
    else {
        QMessageBox::information(this, tr("주문 실패"), tr("주문하실 제품을 선택해주세요."));
        return;
    }
}

//주문변경 버튼 클릭 시 동작
void ShoppingManager::on_updateOrderPushButton_clicked()
{
    bool choice, change;
    int updateCount, checkCount;
    int colIdx, priceIdx, orderCount, orderPrice;
    QString choiceUpdate, updateAddress;
    QString orderAddress;
    QModelIndex orderListIdx;
    QSqlRecord rec;

    /*inputDialog에서 숫자 이외의 문자에 대해 예외처리를 위해 사용*/
    QLineEdit *onlyNum = new QLineEdit(this);
    QIntValidator *intValidator = new QIntValidator(this);

    onlyNum->setValidator(intValidator);    //입력할 LineEdit을 숫자 값인지 검사하도록 지정
    orderListIdx = ui->orderListTableView->currentIndex();  //변경할 주문내역을 확인하기 위해 구하는 Index

    /*주문 내역 위젯의 주문한 내역을 선택했을 경우에 실행한다.*/
    if(ui->orderListTableView->currentIndex().isValid()) {
        //주문 내역에서 주문수량 or 배송주소 중 어느것을 변경할 것인지에 대해 확인한다.
        choiceUpdate = QInputDialog::getText(this, "변경 정보 선택", "주문수량 / 배송주소", QLineEdit::Normal, NULL, &choice);

        //주문수량을 입력하였을 경우
        if(choiceUpdate.trimmed() == "주문수량") {

            /*주문수량에 대한 예외처리를 위한 do-while문*/
            do {
                updateCount = QInputDialog::getText(this, "Update", "변경하실 수량을 입력해주세요.", onlyNum->Normal, NULL, &change).toInt();
                if(change == false) return;
            } while(change != true || updateCount <= 0);

            /*변경할 주문의 주문 수량, 제품 가격을 구한다.*/
            query->exec("SELECT orderProCount, orderProPrice FROM shopping "
                        "WHERE orderNumber = " + QString::number(orderListIdx.sibling(orderListIdx.row(), 0).data().toInt()) + ";");
            rec = query->record();
            colIdx = rec.indexOf("orderProCount");
            priceIdx = rec.indexOf("orderProPrice");

            while(query->next()) {
                orderCount = query->value(colIdx).toInt();
                orderPrice = query->value(priceIdx).toInt();
            }

            /*기존의 주문수량이 변경할 수량보다 적을 경우*/
            if(orderCount < updateCount) {
                int upCount = updateCount - orderCount; //변경이 있을 수량을 계산하여 upCount변수에 담는다.
                //주문 변경 시 제품의 재고량을 확인하기 위한 SIGNAL(리턴 값을 받아옴)
                checkCount = emit updateAfter_upCount(orderListIdx.sibling(orderListIdx.row(), 1).data().toString(), upCount);
                if(checkCount < 0) QMessageBox::information(this, tr("변경 실패"), tr("재고가 부족합니다."));
            }
            /*기존의 주문수량이 변경할 수량보다 많을 경우*/
            else {
                int downCount = orderCount - updateCount; //변경이 있을 수량을 계산하여 downCount변수에 담는다.
                //주문 변경 후 제품의 재고량 관리를 위한 SIGNAL
                emit updateAfter_downCount(orderListIdx.sibling(orderListIdx.row(), 1).data().toString(), downCount);
            }

            //변경 후의 결과로 shopping테이블을 변경한다.
            query->exec("UPDATE shopping SET orderProCount = " + QString::number(updateCount) + ", "
                                            "orderTotPrice = " + QString::number(updateCount*orderPrice) + " "
                        "WHERE orderNumber = " + QString::number(orderListIdx.sibling(orderListIdx.row(), 0).data().toInt()) + ";");

            QMessageBox::information(this, tr("변경 성공"), tr("주문 수량이 변경되었습니다."));
        }
        //배송주소를 입력하였을 경우
        else if(choiceUpdate.trimmed() == "배송주소") {

            /*배송주소가 적히지 않는 위험 방지를 위한 do-while문*/
            do {
                updateAddress = QInputDialog::getText(this, "Update", "변경하실 주소를 입력해주세요.", QLineEdit::Normal, NULL, &change);
                if(change == false) return;
            } while(change != true || updateAddress.trimmed() == "");

            //변경 후의 결과로 shopping테이블을 변경한다.
            query->exec("UPDATE shopping SET orderAddress = '" + updateAddress + "' "
                        "WHERE orderNumber = " + QString::number(orderListIdx.sibling(orderListIdx.row(), 0).data().toInt()) + ";");

            QMessageBox::information(this, tr("변경 성공"), tr("배송 주소가 수정되었습니다."));
        }
        //변경할 정보를 입력하는 과정에서 오타가 발생할 경우
        else QMessageBox::warning(this, tr("입력 값 오류"), tr("정확히 입력하세요."));

        /*변경 이후의 수정된 주문에 대한 출력을 위해 실행한다.*/
        shoppingModel->select();
    }
}

//주문 취소 버튼 클릭 시 동작
void ShoppingManager::on_cancelOrderPushButton_clicked()
{
    int eraseNum, eraseCount;
    QString eraseName;

    /*주문 내역 테이블 뷰의 주문한 내역을 선택했을 경우에 실행한다.*/
    if(ui->orderListTableView->currentIndex().isValid()) {
        /*삭제할 제품에 대한 정보를 가져온다.*/
        eraseNum = ui->orderListTableView->currentIndex().sibling(ui->orderListTableView->currentIndex().row(), 0).data().toInt();
        eraseName = ui->orderListTableView->currentIndex().sibling(ui->orderListTableView->currentIndex().row(), 1).data().toString();
        eraseCount = ui->orderListTableView->currentIndex().sibling(ui->orderListTableView->currentIndex().row(), 3).data().toInt();

        emit updateAfter_downCount(eraseName, eraseCount);  //제품의 재고 관리를 위해 호출하는 SIGNAL

        //선택한 주문을 삭제한다.
        query->exec("DELETE FROM shopping WHERE orderNumber = "
                    "" + QString::number(eraseNum) + ";");
        shoppingModel->select();

        QMessageBox::information(this, tr("취소 성공"), tr("주문이 취소되었습니다."));

        /*삭제 이후의 수정된 주문에 대한 출력을 위해 실행한다.*/
        shoppingModel->select();
    }
    /*주문 내역 테이블 뷰의 주문한 내역을 선택하지 않았을 경우에 실행한다.*/
    else QMessageBox::warning(this, tr("취소 실패"), tr("취소하실 주문을 선택해주세요."));
}

//제품/회원 정보 관리 버튼 클릭 시 동작
void ShoppingManager::on_managementPushButton_clicked()
{
    bool ok;
    QString passwd;

    /*관리자 번호 입력에 대한 예외처리를 위한 do-while 반복문*/
    do {
        passwd = QInputDialog::getText(this, "Manager", "관리자 번호를 입력하세요.", QLineEdit::Normal, NULL, &ok);
        if(ok == false) break;
    } while(/*ok != true || */passwd.trimmed() != "ossmall");

    if(ok == true) {
        emit viewClientList();  //관리자 페이지로 이동 시 회원 정보 리스트를 출력하기 위해 호출하는 SIGNAL
        emit onlyStaff();       //관리자 페이지로 이동하기 위해 호출하는 SIGNAL
    }
}

//채팅하기 버튼 클릭 시 동작
void ShoppingManager::on_chatClientPushButton_clicked()
{
    QList<QString> labelText;   //로그인한 아이디의 회원 이름을 구하기 위해 사용한 List변수
    QString name;               //클라이언트 채팅창으로 이름을 보내주기 위한 변수

    //로그인을 성공하여 Label의 길이가 길어진 경우
    if(ui->orderListLabel->text().length() > 5) {
        ChattingForm_Client *clientForm = new ChattingForm_Client();    //채팅 클라이언트 객체 생성

        //로그인 후 회원 이름을 채팅 클라이언트에 보내주기 위한 연결
        connect(this, SIGNAL(sendNameToClient(QString)), clientForm, SLOT(receivedLoginName(QString)));

        labelText = ui->orderListLabel->text().split("님");  //OOO님의 주문내역 Label에서 이름을 구해오기 위해 실행
        name = labelText[0];                                 //split으로 자른 문장에서 사용자의 이름 부분을 name 변수에 저장
        emit sendNameToClient(name);                         //클라이언트 채팅창으로 이름을 보내주기 위해 호출하는 SIGNAL

        clientForm->show();
    }
    //로그인 하지 않은 경우
    else QMessageBox::warning(this, tr("입장 실패"), tr("로그인 후 사용 가능합니다."));
}

//서버오픈 버튼 클릭 시 동작
void ShoppingManager::on_chatServerPushButton_clicked()
{
    bool ok;
    QString passwd;

    /*관리자 번호의 입력에 관한 제약조건용 do-while문*/
    do {
        passwd = QInputDialog::getText(this, "Manager", "관리자 번호를 입력하세요.", QLineEdit::Normal, NULL, &ok);
        if(ok == false) break;
    } while(/*ok != true || */passwd.trimmed() != "ossmall");

    if(ok == true) {
        serverForm = new ServerSide();  //서버 객체 생성

        /*서버에 회원 아이디 및 이름을 보내주기 위한 연결*/
        connect(this, SIGNAL(sendClientToServer(QString, QString)), serverForm, SLOT(addClient(QString, QString)));
        connect(this, SIGNAL(sendNameToSeverFromClient(QStringList)), serverForm, SLOT(inputNameComboBox(QStringList)));
        emit serverBtnClicked();
        emit serverInputComboBox();

        serverForm->show();
    }
}

//회원의 아이디와 이름 리스트를 받아서 채팅서버로 전달하기 위한 SLOT 함수
void ShoppingManager::clientSignalReceived(QString id, QString name) {
    //사용자의 아이디와 리스트를 받아서 채팅서버로 전달하기 위해 호출하는 SIGNAL
    emit sendClientToServer(id, name);
}

//회원의 이름을 QStringList타입으로 받아서 채팅 서버로 전달해주기 위한 SLOT 함수
void ShoppingManager::inputNameServerCombobox(QStringList nameList) {
    //채팅 서버의 콤보 박스에 회원 이름을 전달해주기 위해 호출하는 SIGNAL
    emit sendNameToSeverFromClient(nameList);
}

//쇼핑 끝내기 버튼 클릭 시 동작
void ShoppingManager::on_exitShoppingPushButton_clicked()
{
    emit exitShopping();    //메인 윈도우를 종료하기 위한 SIGNAL
}
