#ifndef SHOPPINGMANAGER_H
#define SHOPPINGMANAGER_H
#include "serverside.h"
#include <QWidget>

class Shopping;
class Product;

namespace Ui {
class ShoppingManager;
}

class ShoppingManager : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingManager(QWidget *parent = nullptr);    //생성자 - shoppinglist.txt에 저장된 정보를 불러와 쇼핑리스트에 저장한다.
    ~ShoppingManager();                                     //소멸자 - 쇼핑리스트에 저장된 정보를 shoppinglist.txt에 저장한다.

    void dataLoad();                        //제품 정보의 리스트를 불러오기 위한 신호를 보내는 함수
    void dataClear();                       //제품 정보의 리스트를 초기화하는 함수
    void loadShoppingWidget(QString);       //로그인 성공 시 주문 내역 리스트에 해당 사용자가 주문한 리스트 출력

private slots:
    /*---쇼핑 화면에 생성된 버튼을 클릭했을 경우 실행하는 함수---*/
    /*회원가입, 회원 탈퇴 버튼*/
    void on_addNewClientPushButton_clicked();       //회원가입 버튼 클릭 시 동작
    void on_removeClientPushButton_clicked();       //회원탈퇴 버튼 클릭 시 동작

    /*로그인 버튼*/
    void on_shoppingLoginPushButton_clicked();      //로그인 버튼 클릭 시 동작

    /*주문하기, 주문변경, 주문취소 버튼*/
    void on_takeOrderPushButton_clicked();          //주문하기 버튼 클릭 시 동작
    void on_updateOrderPushButton_clicked();        //주문변경 버튼 클릭 시 동작
    void on_cancelOrderPushButton_clicked();        //주문취소 버튼 클릭 시 동작

    /*관리자 페이지 입장 버튼*/
    void on_managementPushButton_clicked();         //제품/회원 정보 관리 버튼 클릭 시 동작

    /*채팅 프로그램 버튼*/
    void on_chatClientPushButton_clicked();         //채팅하기 버튼 클릭 시 동작
    void on_chatServerPushButton_clicked();         //서버오픈 버튼 클릭 시 동작

    /*프로그램 종료 버튼*/
    void on_exitShoppingPushButton_clicked();       //쇼핑 끝내기 버튼 클릭 시 동작

    /*---connect()에서 SIGNAL을 받아 처리하는 SLOT 함수---*/
    void receivedProductInfo(Product*);             //등록되어 있는 제품 정보를 쇼핑화면의 제품 정보 리스트에 출력하는 SLOT 함수
    void successLoginCheck(QString);                //로그인 성공 시 동작하는 SLOT 함수
    void failedLoginCheck();                        //로그인 실패 시 동작하는 SLOT 함수
    void clientSignalReceived(QString, QString);    //사용자의 아이디와 리스트를 받아서 채팅서버로 전달하기 위한 SLOT 함수
    void inputNameServerCombobox(QStringList);      //사용자의 이름 리스트를 받아서 채팅서버로 전달하기 위한 SLOT 함수

private:
    Ui::ShoppingManager *ui;

    int shoppingNumber();                    //주문 번호를 자동으로 생성하여 전달해주기 위한 함수
    QMap<int, Shopping*> shoppingList;      //주문 정보(내역)을 저장하기 위한 QMap타입의 멤버 변수
    ServerSide *serverForm;

signals:
    /*회원 가입, 탈퇴*/
    void newClient();                           //회원가입 버튼 클릭 시 회원가입 화면을 출력하기 위해 호출되는 신호
    int deleteClient(QString);                  //회원탈퇴 시 입력한 아이디가 존재하는지 알아보기 위해 호출되는 신호

    /*로그인*/
    void login(QString);                        //로그인 버튼 클릭 시 로그인 아이디가 사용자 정보에 등록되어있는지 확인하기 위해 호출되는 신호

    /*주문 추가, 변경, 취소*/
    QString takeOrderSign(QString);             //주문하기 버튼 클릭 시 주문자의 배송 주소를 불러오기 위해 호출되는 신호
    int updateAfter_upCount(QString, int);      //주문한 제품의 재고를 확인하기 위해 호출되는 신호
    void updateAfter_downCount(QString, int);   //주문한 수량이 감소, 주문 취소할 경우 해당 제품의 재고를 추가하기 위해 호출되는 신호

    /*관리자 페이지*/
    void onlyStaff();                           //제품/회원 정보 관리 버튼 클릭 시 관리화면을 출력하기 위해 호출되는 신호
    void viewClientList();                      //제품/회원 정보 관리 버튼 클릭 시 관리화면에 사용자 정보를 출력하기 위해 호출되는 신호
    void viewProductList();                     //제품 정보의 리스트를 불러오기 위해 호출되는 신호

    /*채팅 프로그램*/
    void serverBtnClicked();                        //서버오픈 버튼 클릭 시 사용자 정보를 요청하기 위해 호출되는 신호
    void sendClientToServer(QString, QString);      //사용자의 아이디와 리스트를 받아서 채팅서버로 전달하기 위해 호출되는 신호
    void serverInputComboBox();                     //서버의 콤보 박스를 채우기 위한 정보를 받아오기 위해 호출되는 신호
    void sendNameToSeverFromClient(QStringList);    //클라이언트로부터 받아온 이름 리스트를 서버로 보내기 위해 호출되는 신호
    void sendNameToClient(QString);                 //채팅 클라이언트로 사용자 이름을 보내주기 위해 호출하는 SIGNAL

    /*프로그램 종료*/
    void exitShopping();                        //쇼핑 끝내기 버튼 클릭 시 프로그램 종료를 위해 호출되는 신호
};

#endif // SHOPPINGMANAGER_H
