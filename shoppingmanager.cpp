#include "shoppingmanager.h"
#include "ui_shoppingmanager.h"
#include "shopping.h"
#include "product.h"
#include "chattingform_client.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QSpinBox>

//생성자 - shoppinglist.txt에 저장된 정보를 불러와 쇼핑리스트에 저장한다.
ShoppingManager::ShoppingManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShoppingManager)
{
    ui->setupUi(this);

    QFile file("shoppinglist.txt");  //shoppinglist.txt라는 파일을 불러온다.

    /*해당 파일을 텍스트 파일의 읽기 전용으로 열기*/
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;  //파일을 열기에 실패하면 return;

    QTextStream in(&file);  //파일의 정보를 textStream에 저장할 준비를 한다.

    /*저장된 정보가 끝날 때 까지 반복*/
    while (!in.atEnd()) {
        QString line = in.readLine();            //저장 되어있는 정보를 QString타입의 변수에 담는다.
        QList<QString> row = line.split(", ");   //리스트 변수 row에 ", " 구분자를 제외한 데이터를 담는다.
        if(row.size()) {    //리스트가 비어있지 않은 경우
            int shoppingNumber = row[0].toInt(); //0번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.
            int proPrice = row[2].toInt();       //2번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.
            int proCount = row[3].toInt();       //3번째 인덱스에 있는 정보를 int타입으로 변환하여 변수에 담는다.

            //해당 정보를 담은 객체를 생성한다.
            Shopping* s = new Shopping(shoppingNumber, row[1], proPrice, proCount, row[4], row[5], row[6]);
            shoppingList.insert(shoppingNumber, s);  //정보를 담은 객체를 주문 리스트에 저장한다.
        }
    }
    file.close();   //shoppinglist.txt파일에 저장된 정보를 모두 회원 리스트에 저장했으므로 파일을 종료한다.

    setWindowTitle(tr("Shopping Side"));    //열리는 윈도우의 제목을 Shopping Side로 설정한다.
}

//소멸자 - 쇼핑리스트에 저장된 정보를 shoppinglist.txt에 저장한다.
ShoppingManager::~ShoppingManager()
{
    delete ui;

    QFile file("shoppinglist.txt"); //shoppinglist.txt라는 파일을 불러온다(없을 경우 생성한다).

    /*해당 파일을 텍스트 파일의 쓰기 전용으로 열기*/
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return; //파일을 열기에 실패하면 return;

    QTextStream out(&file); //파일의 정보를 textStream으로 출력할 준비를 한다.

    /*주문 리스트에 저장된 정보를 파일에 모두 저장하기 위한 반복문*/
    for (const auto& v : shoppingList) {
        Shopping* s = v;

        /*주문 리스트에 대한 각 정보들을 , 를 구분자로 하여 파일(shoppinglist.txt)에 저장한다.*/
        out << s->shoppingNumber() << ", " << s->getProductName() << ", ";
        out << s->getProductPrice() << ", " << s->getProductCount() << ", ";
        out << s->getProductType() << ", " << s->getClientAddress() << ", ";
        out << s->getClientName() << "\n";
    }

    file.close( );  //shoppinglist.txt파일로 회원 리스트에 저장된 정보를 출력해 저장했으므로 파일을 종료한다.
}

//제품 리스트의 정보를 불러오기 위한 신호를 보내는 함수
void ShoppingManager::dataLoad() {
    emit viewProductList(); //제품 리스트를 불러오기 위해 호출하는 SIGANL
}

//등록되어 있는 제품 정보를 쇼핑화면의 제품 정보 리스트에 출력하는 SLOT 함수
void ShoppingManager::receivedProductInfo(Product *p) {
    //관리자 페이지에서 받아온 제품 정보를 쇼핑 화면의 제품 리스트 위젯에 등록
    ui->productInfoTreeWidget->addTopLevelItem(p);
}

//제품 정보의 리스트를 초기화하는 함수
void ShoppingManager::dataClear() {
    //쇼핑 화면의 제품 리스트 위젯을 비운다.
    ui->productInfoTreeWidget->clear();
}

//주문 번호를 자동으로 생성하여 전달해주기 위한 함수
int ShoppingManager::shoppingNumber() {
    if(shoppingList.size() == 0) return 1;  //주문 정보 리스트에 저장된 정보가 없으면 1을 반환한다.
    else {
        auto cnt = shoppingList.lastKey();  //주문 정보 리스트에 저장된 마지막 키값을 얻는다.
        return ++cnt;                       //얻은 키값에 1을 더한 값을 반환한다.
    }
}

//회원가입 버튼 클릭 시 동작
void ShoppingManager::on_addNewClientPushButton_clicked()
{
    emit newClient();   //회원 가입 화면을 열기 위해 호출하는 SIGNAL
}

//회원탈퇴 버튼 클릭 시 동작
void ShoppingManager::on_removeClientPushButton_clicked()
{
    bool questionCheck, inputUserId;
    int checkUserId;
    QString question, userId;

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
            if(checkUserId <= 0) QMessageBox::warning(this, tr("탈퇴 실패"), tr("존재하지 않는 아이디 입니다."));
            else QMessageBox::warning(this, tr("탈퇴 성공"), tr("회원 탈퇴되었습니다."));
        } while(checkUserId <= 0);
    }
}

//로그인 버튼 클릭 시 동작
void ShoppingManager::on_shoppingLoginPushButton_clicked()
{
    emit login(ui->userIdLoginLineEdit->text());    //입력한 로그인 아이디가 clientList에 등록되어 있는지 확인
    ui->userIdLoginLineEdit->clear();               //로그인할 아이디를 입력한 LineEdit를 비운다.
}

//로그인 성공 시 동작하는 SLOT 함수
void ShoppingManager::successLoginCheck(QString clientName) {
    ui->orderListLabel->setText(clientName + "님의 주문내역");   //Label에 사용자 이름 표시

    loadShoppingWidget(clientName);     //로그인한 사용자가 주문한 주문내역을 불러오기 위한 함수
}

//로그인 성공 시 주문 내역 리스트에 해당 사용자가 주문한 리스트 출력
void ShoppingManager::loadShoppingWidget(QString name) {
    /*사용자가 주문한 주문내역을 확인하기 위한 반복문*/
    Q_FOREACH(auto v, shoppingList)
    {
        Shopping *s = static_cast<Shopping*>(v);    //auto변수 v의 자료형을 Shopping*형으로 변환 후 고정

        //현재 표시되어있는 주문 리스트를 삭제한다.
        ui->orderListTreeWidget->takeTopLevelItem(ui->orderListTreeWidget->indexOfTopLevelItem(s));

        if(name == s->getClientName())  //인자로 받은 사용자의 이름이 주문 내역 리스트의 사용자 이름과 같은 경우
        {
            ui->orderListTreeWidget->addTopLevelItem(s);    //해당 주문 정보를 주문 내역 리스트에 표시한다.
        }
    }
}

//로그인 실패 시 동작하는 SLOT 함수
void ShoppingManager::failedLoginCheck() {
    //로그인 실패 메시지
    QMessageBox::critical(this, tr("로그인 실패"), tr("아이디가 일치하지 않습니다."));
}

//주문하기 버튼 클릭 시 동작
void ShoppingManager::on_takeOrderPushButton_clicked()
{
    int orderNumber, proPrice, proCount, checkCount;
    QString proName, proType, address;
    QString clientName;
    QList<QString> labelText;   //로그인한 아이디의 회원 이름을 구하기 위해 사용한 List변수
    bool ok;

    /*inputDialog에서 숫자 이외의 문자에 대해 예외처리를 위해 사용*/
    QLineEdit *onlyNum = new QLineEdit(this);
    QIntValidator *intValidator = new QIntValidator(this);

    onlyNum->setValidator(intValidator);                //입력할 LineEdit을 숫자 값인지 검사하도록 지정
    labelText = ui->orderListLabel->text().split("님");  //OOO님의 주문내역 Label에서 이름을 구해오기 위해 실행
    clientName = labelText[0];                           //split으로 자른 문장에서 사용자의 이름 부분을 clientName 변수에 저장

    /*로그인을 성공하여 Label의 길이가 길어지고 주문할 제품을 제품 위젯에서 선택했을 경우 실행*/
    if(ui->orderListLabel->text().length() > 5 && ui->productInfoTreeWidget->currentItem() != nullptr) {
        orderNumber = shoppingNumber();   //주문 번호의 경우 숫자를 자동으로 리턴해주는 함수를 사용하여 설정

        /*제품 이름, 제품 가격은 제품 위젯에서 인텍스로 가져온다.*/
        proName = ui->productInfoTreeWidget->currentItem()->text(1);
        proPrice = ui->productInfoTreeWidget->currentItem()->text(2).toInt();
        proType = ui->productInfoTreeWidget->currentItem()->text(4);

        /*숫자 이외의 문자가 inputDialog에 입력되었을 경우 0을 리턴한다.*/
        proCount = QInputDialog::getText(this, "Order", "주문 수량을 입력하세요.", onlyNum->Normal, NULL, &ok).toInt();

        if(ok == false) return;     //취소 버튼을 클릭했을 경우 실행
        else if(proCount <= 0) {    //숫자 이외의 문자가 inputDialog에 입력되었을 경우 실행
            QMessageBox::warning(this, tr("주문 실패"), tr("주문 수량을 확인해주세요."));
            return;
        }

        //회원 주소의 경우 clientList에서 구해온다.
        address = emit takeOrderSign(clientName);   //회원 주소를 구하기 위해 호출하는 SIGNAL

        //주문 시 제품의 재고량을 확인하기 위한 SIGNAL(리턴 값을 받아옴)
        checkCount = emit updateAfter_upCount(ui->productInfoTreeWidget->currentItem()->text(1), proCount);
        if(checkCount < 0) {
            QMessageBox::information(this, tr("주문 실패"), tr("재고가 부족합니다."));
            return;
        }

        //주문한 값이 저장된 객체를 생성한다.
        Shopping *s = new Shopping(orderNumber, proName, proPrice, proCount, proType, address, clientName);
        shoppingList.insert(orderNumber, s);             //주문한 내역을 주문 리스트에 저장한다.
        ui->orderListTreeWidget->addTopLevelItem(s);    //주문한 내역을 주문 위젯에 추가한다.

        QMessageBox::information(this, tr("주문 성공"), tr("주문이 완료되었습니다."));

        /*주문 이후의 수정된 제품 값에 대한 출력을 위해 실행한다.*/
        dataClear();
        dataLoad();
    }
    else return;
}

//주문변경 버튼 클릭 시 동작
void ShoppingManager::on_updateOrderPushButton_clicked()
{
    bool choice, change;
    int updateCount, checkCount;
    QString choiceUpdate, updateAddress;

    /*inputDialog에서 숫자 이외의 문자에 대해 예외처리를 위해 사용*/
    QLineEdit *onlyNum = new QLineEdit(this);
    QIntValidator *intValidator = new QIntValidator(this);

    onlyNum->setValidator(intValidator);    //입력할 LineEdit을 숫자 값인지 검사하도록 지정

    /*주문 내역 위젯의 주문한 내역을 선택했을 경우에 실행한다.*/
    if(ui->orderListTreeWidget->currentItem() != nullptr) {
        //주문 내역에서 주문수량 or 배송주소 중 어느것을 변경할 것인지에 대해 확인한다.
        choiceUpdate = QInputDialog::getText(this, "변경 정보 선택", "주문수량 / 배송주소", QLineEdit::Normal, NULL, &choice);

        //주문수량을 입력하였을 경우
        if(choiceUpdate.trimmed() == "주문수량") {

            /*주문수량에 대한 예외처리를 위한 do-while문*/
            do {
                updateCount = QInputDialog::getText(this, "Update", "변경하실 수량을 입력해주세요.", onlyNum->Normal, NULL, &change).toInt();
                if(change == false) return;
            } while(change != true || updateCount <= 0);

            /*주문 변경 후 처리를 위한 반복문*/
            Q_FOREACH(auto v, shoppingList) {
                Shopping *s = static_cast<Shopping*>(v);    //auto변수 v의 자료형을 Shopping*형으로 변환 후 고정

                /*변경한 주문에 대한 번호가 주문 내역 리스트의 번호와 일치할 경우*/
                if(ui->orderListTreeWidget->currentItem()->text(0).toInt() == s->shoppingNumber()) {

                    /*기존의 주문수량이 변경할 수량보다 작을 경우*/
                    if(s->getProductCount() < updateCount) {
                        int upCount = updateCount - s->getProductCount();   //변경이 있을 수량을 계산하여 upCount변수에 담는다.
                        //주문 변경 시 제품의 재고량을 확인하기 위한 SIGNAL(리턴 값을 받아옴)
                        checkCount = emit updateAfter_upCount(ui->orderListTreeWidget->currentItem()->text(1), upCount);
                        if(checkCount < 0) {
                            QMessageBox::information(this, tr("변경 실패"), tr("재고가 부족합니다."));
                            break;
                        }
                    }
                    /*기존의 주문수량이 변경할 수량보다 많을 경우*/
                    else {
                        int downCount = s->getProductCount() - updateCount; //변경이 있을 수량을 계산하여 downCount변수에 담는다.
                        //주문 변경 후 제품의 재고량 관리를 위한 SIGNAL
                        emit updateAfter_downCount(ui->orderListTreeWidget->currentItem()->text(1), downCount);
                    }

                    s->setProductCount(updateCount);    //주문 내역의 주문 수량을 수정한다.
                    ui->orderListTreeWidget->currentItem()->setText(3, QString::number(updateCount));   //주문 위젯에서의 내용도 변경한다.
                    shoppingList.insert(ui->orderListTreeWidget->currentItem()->text(0).toInt(), s);    //변경된 내용을 주문 리스트에 저장한다.

                    QMessageBox::information(this, tr("변경 성공"), tr("주문 수량이 변경되었습니다."));

                    /*주문 변경 이후의 수정된 제품 값에 대한 출력을 위해 실행한다.*/
                    dataClear();
                    dataLoad();

                    break;
                }
            }

        }
        //배송주소를 입력하였을 경우
        else if(choiceUpdate.trimmed() == "배송주소") {

            /*배송주소가 적히지 않는 위험 방지를 위한 do-while문*/
            do {
                updateAddress = QInputDialog::getText(this, "Update", "변경하실 주소를 입력해주세요.", QLineEdit::Normal, NULL, &change);
                if(change == false) return;
            } while(change != true || updateAddress.trimmed() == "");

            /*주문 변경 후 처리를 위한 반복문*/
            Q_FOREACH(auto v, shoppingList) {
                Shopping *s = static_cast<Shopping*>(v);    //auto변수 v의 자료형을 Shopping*형으로 변환 후 고정

                /*변경할 주문에 대한 번호가 주문 내역 리스트의 번호와 일치할 경우*/
                if(ui->orderListTreeWidget->currentItem()->text(0).toInt() == s->shoppingNumber()) {
                    s->setClientAddress(updateAddress);     //주문 내역의 배송 주소를 수정한다.
                    ui->orderListTreeWidget->currentItem()->setText(5, updateAddress);                  //주문 위젯에서의 내용도 변경한다.
                    shoppingList.insert(ui->orderListTreeWidget->currentItem()->text(0).toInt(), s);    //변경된 내용을 주문 리스트에 저장한다.

                    QMessageBox::information(this, tr("변경 성공"), tr("배송 주소가 수정되었습니다."));

                    break;
                }
            }
        }
    }
}

//주문 취소 버튼 클릭 시 동작
void ShoppingManager::on_cancelOrderPushButton_clicked()
{
    int eraseNum;

    /*주문 내역 위젯의 주문한 내역을 선택했을 경우에 실행한다.*/
    if(ui->orderListTreeWidget->currentItem() != nullptr) {
        //주문 취소의 key가 되는 주문 번호는 주문 위젯에서 인덱스로 가져온다.
        eraseNum = ui->orderListTreeWidget->currentItem()->text(0).toInt();

        /*주문 취소 후 처리를 위한 반복문*/
        Q_FOREACH(auto v, shoppingList) {
            Shopping *s = static_cast<Shopping*>(v);    //auto변수 v의 자료형을 Shopping*형으로 변환 후 고정
            if(eraseNum == s->shoppingNumber()) {        //취소할 주문에 대한 번호가 주문 내역 리스트의 번호와 일치할 경우
                //주문 취소 후 제품의 재고량 관리를 위한 SIGNAL
                emit updateAfter_downCount(ui->orderListTreeWidget->currentItem()->text(1), ui->orderListTreeWidget->currentItem()->text(3).toInt());
                shoppingList.remove(eraseNum);          //주문 리스트에서 취소할 주문에 대한 정보를 삭제한다.
                //주문 위젯에서도 주문 취소한 제품을 삭제한다.
                ui->orderListTreeWidget->takeTopLevelItem(ui->orderListTreeWidget->indexOfTopLevelItem(s));

                QMessageBox::information(this, tr("취소 성공"), tr("주문이 취소되었습니다."));

                /*주문 취소 이후의 수정된 제품 값에 대한 출력을 위해 실행한다.*/
                dataClear();
                dataLoad();
                break;
            }
        }
    }
    /*주문 내역 위젯의 주문한 내역을 선택하지 않았을 경우에 실행한다.*/
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
