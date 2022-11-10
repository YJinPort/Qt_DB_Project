#include "clientmanager.h"
#include "ui_clientmanager.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>

//생성자 - clientlist.txt에 저장된 정보를 불러와 사용자리스트에 저장한다.
ClientManager::ClientManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManager)
{
    ui->setupUi(this);

    QSqlDatabase sqlDB = QSqlDatabase::addDatabase("QSQLITE", "clientDatabase");
    sqlDB.setDatabaseName("clientList.db");
    if(!sqlDB.open()) return;

    query = new QSqlQuery(sqlDB);
    query->exec("CREATE TABLE IF NOT EXISTS client(userID VARCHAR(30) Primary Key, "
               "userName VARCHAR(20) NOT NULL, userCall VARCHAR(13) NOT NULL, "
               "userAddress VARCHAR(100) NOT NULL, userGender VARCHAR(15));");

    clientModel = new QSqlTableModel(this, sqlDB);
    clientModel->setTable("client");
    clientModel->select();
    clientModel->setHeaderData(0, Qt::Horizontal, QObject::tr("아이디"));
    clientModel->setHeaderData(1, Qt::Horizontal, QObject::tr("이름"));
    clientModel->setHeaderData(2, Qt::Horizontal, QObject::tr("전화번호"));
    clientModel->setHeaderData(3, Qt::Horizontal, QObject::tr("주소"));
    clientModel->setHeaderData(4, Qt::Horizontal, QObject::tr("성별"));

    setWindowTitle(tr("Client Side"));  //열리는 윈도우의 제목을 Client Side로 설정한다.
}

//소멸자 - 사용자리스트에 저장된 정보를 clientlist.txt에 저장한다.
ClientManager::~ClientManager()
{
    delete ui;
}

void ClientManager::loadDBInProduct() {
    emit sendClientTable(clientModel);
}

//회원 등록 버튼 클릭 시 동작
void ClientManager::on_clientRegisterPushButton_clicked()
{
    /*아이디, 이름, 주소를 입력 혹은 정보 수집을 동의하지 않았을 경우 경고메시지 발생*/
    if(ui->userIdLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("가입 실패"), tr("아이디를 입력하여 주세요."));
    else if(ui->userNameLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("가입 실패"), tr("성함을 입력하여 주세요."));
    else if(ui->userAddressLineEdit->text().trimmed() == "") QMessageBox::warning(this, tr("가입 실패"), tr("주소를 입력하여 주세요."));
    else if(ui->agreeClientInfoCheckBox->isChecked() == false || ui->agreeAddressCheckBox->isChecked() == false) {
        QMessageBox::warning(this, tr("가입 실패"), tr("정보 수집을 동의하여 주세요"));
    }
    /*회원가입 시작*/
    else {
        QString userId, name, call, address, gender;
        QString checkUserID;

        /*나머지 정보의 경우 입력된 LineEdit에 적혀있는 값을 가져온다.*/
        userId = ui->userIdLineEdit->text();
        name = ui->userNameLineEdit->text();
        call = ui->userCallLineEdit->text();
        address = ui->userAddressLineEdit->text();
        gender = ui->userGenderComboBox->currentText();

        query->exec("SELECT userID FROM client WHERE userID = '" + userId + "';");
        QSqlRecord rec = query->record();
        int colIdx = rec.indexOf("userID");

        while(query->next()) checkUserID = query->value(colIdx).toString();

        if(checkUserID != "") {
            QMessageBox::information(this, tr("가입 실패"), tr("이미 등록된 아이디입니다."));
            ui->userIdLineEdit->clear();
        }
        else {
            query->exec("INSERT INTO client VALUES('" + userId + "', '" + name + "', '" +
                                              call + "', '" + address + "', '" + gender + "');");

            /*현재 입력 되어있는 LineEdit를 비우고 체크 되어있는 CheckBox를 체크 해제시킨다*/
            ui->agreeClientInfoCheckBox->setChecked(false);
            ui->agreeAddressCheckBox->setChecked(false);

            ui->userIdLineEdit->clear();
            ui->userNameLineEdit->clear();
            ui->userCallLineEdit->clear();
            ui->userAddressLineEdit->clear();

            emit join();    //쇼핑 화면으로 돌아가기 위해 SIGNAL 신호를 보낸다.
        }

    }


}

//등록 취소 버튼 클릭 시 동작
void ClientManager::on_cancelRegisterPushButton_clicked()
{
    /*현재 입력 되어있는 LineEdit를 비우고 체크 되어있는 CheckBox를 체크 해제시킨다*/
    ui->agreeClientInfoCheckBox->setChecked(false);
    ui->agreeAddressCheckBox->setChecked(false);
    ui->userIdLineEdit->clear();
    ui->userNameLineEdit->clear();
    ui->userCallLineEdit->clear();
    ui->userAddressLineEdit->clear();

    emit cancellation();    //쇼핑 화면으로 돌아가기 위해 SIGNAL 신호를 보낸다.
}

//관리자 페이지에서 회원 정보 수정 시 회원 정보 리스트에 등록된 회원 정보를 변경하기 위한 SLOT 함수
void ClientManager::updateClientInfo(QStringList updateList) {
    bool checkUser = true;
    QString checkUserID;

    query->exec("SELECT userID FROM client WHERE userID = '" + updateList[0] + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userID");

    while(query->next()) checkUserID = query->value(colIdx).toString();

    if(checkUserID != "") {
        query->exec("UPDATE client SET userName = '" + updateList[1] + "'"
                        ", userCall = '" + updateList[2] + "'"
                        ", userAddress = '" + updateList[3] + "'"
                        ", userGender = '" + updateList[4] + "' "
                    "WHERE userID = '" + updateList[0] + "';");

        checkUser = false;  //등록된 회원이라는 것이 확인되어 변경에 성공했기에 아래의 조건문을 위해 값을 변경한다.

        QMessageBox::information(this, tr("수정 성공"), tr("회원 정보가 수정되었습니다."));     //수정에 성공했다는 메시지를 띄운다.
    }

    if(checkUser) QMessageBox::warning(this, tr("수정 실패"), tr("회원 아이디를 확인해주세요."));   //등록된 회원 아이디가 아니기 때문에 경고 메시지를 띄운다.
    /*관리자 페이지의 위젯 리스트에 변경된 회원 정보를 등록 시작*/
    else loadDBInProduct();
}

//관리자 페이지에서 회원 삭제 시 등록된 회원을 삭제하기 위한 SLOT 함수
void ClientManager::deleteClientInfo(QString userId) {
    bool checkUser = true;
    QString checkUserID;

    query->exec("SELECT userID FROM client WHERE userID = '" + userId + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userID");

    while(query->next()) checkUserID = query->value(colIdx).toString();

    if(checkUserID != "") {
        query->exec("DELETE FROM client WHERE userID = '" + userId + "';");

        checkUser = false;  //등록된 회원이라는 것이 확인되어 삭제에 성공했기에 아래의 조건문을 위해 값을 변경한다.

        QMessageBox::information(this, tr("삭제 성공"), tr("회원 삭제가 완료되었습니다.")); //삭제가 왼료되었으므로 삭제 완료 메시지를 표출한다.
    }

    if(checkUser) QMessageBox::information(this, tr("삭제 실패"), tr("회원 아이디를 확인해주세요."));   //등록된 회원 아이디가 아니기 때문에 경고 메시지를 띄운다.
    else loadDBInProduct();
}

//쇼핑 화면에서 관리자 페이지로 이동 버튼 클릭 시 회원 정보를 담아서 보내기 위한 SLOT 함수
void ClientManager::containClientInfo() {
    loadDBInProduct();
}

//쇼핑 화면에서 로그인 시도 시 아이디가 등록되어 있는지 체크하는 SLOT 함수
void ClientManager::checkLoginId(QString ID) {
    QString userName, checkUserID;

    query->exec("SELECT userID, userName FROM client WHERE userID = '" + ID + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userID");
    int nameIdx = rec.indexOf("userName");

    while(query->next()) {
        checkUserID = query->value(colIdx).toString();
        userName = query->value(nameIdx).toString();
    }

    if(checkUserID != "") emit successLogin(userName);
    else emit failedLogin();       //등록된 아이디가 아니므로 로그인 실패를 알리는 SIGNAL을 보낸다.
}

//쇼핑 화면에서 주문하기 버튼 클릭 시 주문자의 주소 정보를 찾아주기 위한 SLOT 함수
QString ClientManager::findAddressForOrder(QString name) {
    QString orderAddress;

    query->exec("SELECT userAddress FROM client WHERE userName = '" + name + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userAddress");

    while(query->next()) orderAddress = query->value(colIdx).toString();

    return orderAddress;    //담아온 회원에 대한 주소를 반환한다.
}

//쇼핑 화면에서 회원 탈퇴 버튼 클릭 시 해당 아이디 검색 후 List에서 삭제하기 위한 SLOT 함수
int ClientManager::deleteId_List(QString ID) {
    int cnt = 0;
    QString checkUserID;

    query->exec("SELECT userID FROM client WHERE userID = '" + ID + "';");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userID");

    while(query->next()) checkUserID = query->value(colIdx).toString();

    if(checkUserID != "") cnt++;

    return cnt; //삭제 완료의 성공 여부를 알리는 cnt값을 반환한다.
}

//쇼핑 화면에서 서버오픈 시 사용자의 ID와 이름을 전달해주기 위한 SLOT 함수
void ClientManager::serverOpenFromShopping() {
    QString sendServerId, sendSeverName;

    query->exec("SELECT userID, userName FROM client;");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userID");
    int nameIdx = rec.indexOf("userName");

    while(query->next()) {
        sendServerId = query->value(colIdx).toString();
        sendSeverName = query->value(nameIdx).toString();
        emit sendToServer(sendServerId, sendSeverName);
    }
}

//쇼핑화면에서 서버 오픈 시 콤보박스에 등록될 사용자의 이름을 보내주기 위한 SLOT 함수
void ClientManager::sendNameListToServer() {
    QStringList nameList;

    query->exec("SELECT userName FROM client;");
    QSqlRecord rec = query->record();
    int colIdx = rec.indexOf("userName");

    while(query->next()) {
        nameList << query->value(colIdx).toString();    //QStringList타입 변수에 회원의 이름을 담는다.
    }

    emit sendNameToServer(nameList);    //list에 담은 회원 이름을 보내주기 위해 호출하는 SIGNAL
}
