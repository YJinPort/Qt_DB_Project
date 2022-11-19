#include "clientmanager.h"
#include "ui_clientmanager.h"
#include "client.h"
#include <QMessageBox>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>

//생성자 - clientlist.txt에 저장된 정보를 불러와 사용자리스트에 저장한다.
ClientManager::ClientManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManager)
{
    ui->setupUi(this);

    QFile file("clientlist.txt");   //clientlist.txt라는 파일을 불러온다.

    /*해당 파일을 텍스트 파일의 읽기 전용으로 열기*/
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;  //파일을 열기에 실패하면 return;

    QTextStream in(&file);  //파일의 정보를 textStream에 저장할 준비를 한다.

    /*저장된 정보가 끝날 때 까지 반복*/
    while (!in.atEnd()) {
        QString line = in.readLine();           //저장 되어있는 정보를 QString타입의 변수에 담는다.
        QList<QString> row = line.split(", ");  //리스트 변수 row에 ", " 구분자를 제외한 데이터를 담는다.
        if(row.size()) {    //리스트가 비어있지 않은 경우
            int number = row[0].toInt();     //0번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.

            //해당 정보를 담은 객체를 생성한다.
            Client* c = new Client(number, row[1], row[2], row[3], row[4], row[5]);
            clientList.insert(number, c);    //정보를 담은 객체를 회원 리스트에 저장한다.
        }
    }

    file.close( );  //clientlist.txt파일에 저장된 정보를 모두 회원 리스트에 저장했으므로 파일을 종료한다.

    setWindowTitle(tr("Client Side"));  //열리는 윈도우의 제목을 Client Side로 설정한다.
}

//소멸자 - 사용자리스트에 저장된 정보를 clientlist.txt에 저장한다.
ClientManager::~ClientManager()
{
    delete ui;

    QFile file("clientlist.txt");   //clientlist.txt라는 파일을 불러온다(없을 경우 생성한다).

    /*해당 파일을 텍스트 파일의 쓰기 전용으로 열기*/
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return; //파일을 열기에 실패하면 return;

    QTextStream out(&file); //파일의 정보를 textStream으로 출력할 준비를 한다.

    /*회원 리스트에 저장된 정보를 파일에 모두 저장하기 위한 반복문*/
    for (const auto& v : clientList) {
        Client* c = v;

        /*회원 리스트에 대한 각 정보들을 , 를 구분자로 하여 파일(clientlist.txt)에 저장한다.*/
        out << c->userNumber() << ", " << c->getUserID() << ", ";
        out << c->getName() << ", ";
        out << c->getPhoneNumber() << ", ";
        out << c->getAddress() << ", ";
        out << c->get_Gender() << "\n";
    }

    file.close( );  //clientlist.txt파일로 회원 리스트에 저장된 정보를 출력해 저장했으므로 파일을 종료한다.
}

//회원 번호를 자동으로 생성하여 전달해주기 위한 함수
int ClientManager::userNumber() {
    if(clientList.size() == 0) return 1;    //회원 정보 리스트에 저장된 정보가 없으면 1을 반환한다.
    else {
        auto cnt = clientList.lastKey();    //회원 정보 리스트에 저장된 마지막 키값을 얻는다.
        return ++cnt;                       //얻은 키값에 1을 더한 값을 반환한다.
    }
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
        int uNumber = userNumber();     //회원 수의 경우 자동 생성 함수의 반환값을 받아온다.

        /*나머지 정보의 경우 입력된 LineEdit에 적혀있는 값을 가져온다.*/
        userId = ui->userIdLineEdit->text();
        name = ui->userNameLineEdit->text();
        call = ui->userCallLineEdit->text();
        address = ui->userAddressLineEdit->text();
        gender = ui->userGenderComboBox->currentText();

        //해당 정보를 c라는 객체에 담는다.
        Client *c = new Client(uNumber, userId, name, call, address, gender);
        clientList.insert(uNumber, c);     //사용자 리스트에 회원 정보를 입력(저장)한다.

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
    int userNumber;
    bool checkUser = true;

    /*변경하려는 회원 정보가 리스트에 등록되어 있는 확인하기 위한 반복문*/
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정
        if(updateList[0] == c->getUserID()) {   //회원 아이디가 이미 등록 되어있을 경우
            userNumber = c->userNumber();     //리스트의 키값을 위한 변수에 값을 입력
            /*현재 키값의 회원 정보를 변경될 값으로 지정한다*/
            c->setName(updateList[1]);
            c->setPhoneNumber(updateList[2]);
            c->setAddress(updateList[3]);
            c->setGender(updateList[4]);

            clientList.insert(userNumber, c);    //사용자 리스트에 회원 정보를 입력(저장)한다.
            checkUser = false;  //등록된 회원이라는 것이 확인되어 변경에 성공했기에 아래의 조건문을 위해 값을 변경한다.

            QMessageBox::information(this, tr("수정 성공"), tr("회원 정보가 수정되었습니다."));     //수정에 성공했다는 메시지를 띄운다.

            break;
        }
    }

    if(checkUser) QMessageBox::warning(this, tr("수정 실패"), tr("회원 아이디를 확인해주세요."));   //등록된 회원 아이디가 아니기 때문에 경고 메시지를 띄운다.
    /*관리자 페이지의 위젯 리스트에 변경된 회원 정보를 등록 시작*/
    else {
        emit clear_Widget_N_LineEdit();     //관리자 페이지의 회원 위젯 리스트를 비우기 위해 호출되는 신호

        /*비운 위젯 리스트에 변경된 회원 정보를 등록하기 위한 반복문*/
        Q_FOREACH(auto v, clientList) {
            Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정

            /*변경된 정보를 변수에 담는다.*/
            userNumber = c->userNumber();
            updateList[0] = c->getUserID();
            updateList[1] = c->getName();
            updateList[2] = c->getPhoneNumber();
            updateList[3] = c->getAddress();
            updateList[4] = c->get_Gender();

            //변경된 정보를 item이라는 객체에 담는다.
            Client *item = new Client(userNumber, updateList[0], updateList[1], updateList[2], updateList[3], updateList[4]);
            emit sendClientInfo(item);  //관리자 페이지로 전달하기 위해 호출하는 SIGNAL
        }
    }
}

//관리자 페이지에서 회원 삭제 시 등록된 회원을 삭제하기 위한 SLOT 함수
void ClientManager::deleteClientInfo(QString userId) {
    bool checkUser = true;

    /*인자로 받아온 회원 아이디가 회원 리스트에 등록된 정보인지 확인한다.*/
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정
        if(userId == c->getUserID()) {          //사용자 아이디가 등록되어 있던 아이디와 일치할 경우
            clientList.remove(c->userNumber());  //회원 정보 리스트에서 해당 아이디가 속한 정보를 삭제한다.
            QMessageBox::information(this, tr("삭제 성공"), tr("회원 삭제가 완료되었습니다.")); //삭제가 왼료되었으므로 삭제 완료 메시지를 표출한다.
            checkUser = false;  //등록된 회원이라는 것이 확인되어 삭제에 성공했기에 아래의 조건문을 위해 값을 변경한다.
            break;
        }
    }

    if(checkUser) QMessageBox::information(this, tr("삭제 실패"), tr("회원 아이디를 확인해주세요."));   //등록된 회원 아이디가 아니기 때문에 경고 메시지를 띄운다.
    else {
        emit clear_Widget_N_LineEdit();     //관리자 페이지의 회원 위젯 리스트를 비우기 위해 호출되는 신호

        /*비운 위젯 리스트에 삭제된 회원 정보를 삭제하고 새로 호출하기 위한 반복문*/
        Q_FOREACH(auto v, clientList) {
            Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정

            //변경된 정보를 item이라는 객체에 담는다.
            Client *item = new Client(c->userNumber(), c->getUserID(), c->getName(), c->getPhoneNumber(), c->getAddress(), c->get_Gender());
            emit sendClientInfo(item);  //관리자 페이지로 전달하기 위해 호출하는 SIGNAL
        }
    }
}

//쇼핑 화면에서 관리자 페이지로 이동 버튼 클릭 시 회원 정보를 담아서 보내기 위한 SLOT 함수
void ClientManager::containClientInfo() {
    QString userId, name, call, address, gender;
    int uNumber;

    /*관리자 페이지의 회원 위젯에 표시될 회원 정보들을 보내기 위한 반목문*/
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정

        /*등록 되어있는 회원 정보를 각각의 변수에 담는다.*/
        uNumber = c->userNumber();
        userId = c->getUserID();
        name = c->getName();
        call = c->getPhoneNumber();
        address = c->getAddress();
        gender = c->get_Gender();

        //등록 되어있는 회원 정보를 item이라는 객체에 담는다.
        Client *item = new Client(uNumber, userId, name, call, address, gender);
        emit sendClientInfo(item);  //관리자 페이지로 전달하기 위해 호출하는 SIGNAL
    }
}

//쇼핑 화면에서 로그인 시도 시 아이디가 등록되어 있는지 체크하는 SLOT 함수
void ClientManager::checkLoginId(QString str) {
    QString userId = "";

    /*인자로 받아온 회원 아이디가 회원 리스트에 등록된 정보인지 확인한다.*/
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정
        if(str == c->getUserID()) {             //인자로 받아온 회원 아이디가 회원 리스트에 등록된 아이디인 경우
            userId = c->getUserID();            //새로운 변수 userId를 등록된 아이디로 초기화한다.
            emit successLogin(c->getName());    //등록된 아이디에 대한 회원 이름을 전달하기 위한 SIGNAL
            break;
        }
    }

    if(userId != str) emit failedLogin();       //등록된 아이디로 초기화 받지 못할 경우 등록된 아이디가 아니므로 로그인 실패를 알리는 SIGNAL을 보낸다.
}

//쇼핑 화면에서 주문하기 버튼 클릭 시 주문자의 주소 정보를 찾아주기 위한 SLOT 함수
QString ClientManager::findAddressForOrder(QString orderName) {
    QString orderAddress;

    /*인자로 받아온 회원 이름이 회원 리스트에 등록된 정보인지 확인한다.*/
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정
        if(orderName == c->getName()) {         //인자로 받아온 회원 이름이 회원 리스트에 등록된 이름인 경우
            orderAddress = c->getAddress();     //등록된 회원 이름에 대한 주소를 orderAddress란 변수에 담는다.
            break;
        }
    }

    return orderAddress;    //담아온 회원에 대한 주소를 반환한다.
}

//쇼핑 화면에서 회원 탈퇴 버튼 클릭 시 해당 아이디 검색 후 List에서 삭제하기 위한 SLOT 함수
int ClientManager::deleteId_List(QString id) {
    int cnt = 0;

    /*인자로 받아온 회원 아이디가 회원 리스트에 등록된 정보인지 확인한다.*/
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);    //auto변수 v의 자료형을 Client*형으로 변환 후 고정
        if(id == c->getUserID()) {              //인자로 받아온 회원 아이디가 회원 리스트에 등록된 아이디인 경우
            clientList.remove(c->userNumber());  //해당 아이디에 등록된 정보를 회원 리스트에서 삭제한다.
            cnt++;      //삭제가 왼료되었는지 확인하기 위한 cnt변수읙 값을 1 증가시킨다.
            break;
        }
    }
    return cnt; //삭제 완료의 성공 여부를 알리는 cnt값을 반환한다.
}

//쇼핑 화면에서 서버오픈 시 사용자의 ID와 이름을 전달해주기 위한 SLOT 함수
void ClientManager::serverOpenFromShopping() {
    QString sendSeverName;
    QString sendServerId;

    Q_FOREACH(auto v, clientList) {
        sendServerId = v->getUserID();
        sendSeverName = v->getName();
        emit sendToServer(sendServerId, sendSeverName);
    }
}

//쇼핑화면에서 서버 오픈 시 콤보박스에 등록될 사용자의 이름을 보내주기 위한 SLOT 함수
void ClientManager::sendNameListToServer() {
    QStringList nameList;

    Q_FOREACH(auto v, clientList) {
        nameList << v->getName();       //QStringList타입 변수에 회원의 이름을 담는다.
    }

    emit sendNameToServer(nameList);    //list에 담은 회원 이름을 보내주기 위해 호출하는 SIGNAL
}
