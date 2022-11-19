#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QWidget>
#include <QSqlDatabase>

class Client;

namespace Ui {
class ClientManager;
}

class ClientManager : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManager(QWidget *parent = nullptr);  //생성자 - clientlist.txt에 저장된 정보를 불러와 사용자리스트에 저장한다.
    ~ClientManager();                                   //소멸자 - 사용자리스트에 저장된 정보를 clientlist.txt에 저장한다.

private slots:
    /*---회원 가입 화면에 생성된 버튼을 클릭했을 경우 실행하는 함수---*/
    void on_clientRegisterPushButton_clicked();     //회원 등록 버튼 클릭 시 동작
    void on_cancelRegisterPushButton_clicked();     //등록 취소 버튼 클릭 시 동작

    /*---connect()에서 SIGNAL을 받아 처리하는 SLOT 함수---*/
    /*관리자 페이지에서 보낸 SIGNAL 처리*/
    void updateClientInfo(QStringList);     //관리자 페이지에서 회원 정보 수정 시 회원 정보 리스트에 등록된 회원 정보를 변경하기 위한 SLOT 함수
    void deleteClientInfo(QString);         //관리자 페이지에서 회원 삭제 시 등록된 회원을 삭제하기 위한 SLOT 함수

    /*쇼핑 화면에서 보낸 SIGNAL 처리*/
    void containClientInfo();               //쇼핑 화면에서 관리자 페이지로 이동 버튼 클릭 시 회원 정보를 담아서 보내기 위한 SLOT 함수
    void checkLoginId(QString);             //쇼핑 화면에서 로그인 시도 시 아이디가 등록되어 있는지 체크하는 SLOT 함수
    QString findAddressForOrder(QString);   //쇼핑 화면에서 주문하기 버튼 클릭 시 주문자의 주소 정보를 찾아주기 위한 SLOT 함수
    int deleteId_List(QString);             //쇼핑 화면에서 회원 탈퇴 버튼 클릭 시 해당 아이디 검색 후 List에서 삭제하기 위한 SLOT 함수
    void serverOpenFromShopping();          //쇼핑 화면에서 서버오픈 시 사용자의 ID와 이름을 전달해주기 위한 SLOT 함수
    void sendNameListToServer();

private:
    Ui::ClientManager *ui;

    int userNumber();                //회원 수를 자동으로 생성하여 전달해주기 위한 함수
    QMap<int, Client*> clientList;  //사용자(회원) 정보를 저장하기 위한 QMap타입의 멤버 변수

signals:
    /*회원 가입*/
    void join();                            //회원 등록 버튼 클릭 시 쇼핑 화면을 출력하기 위해 호출되는 신호
    void cancellation();                    //등록 취소 버튼 클릭 시 쇼핑 화면을 출력하기 위해 호출되는 신호

    /*관리자 페이지*/
    void sendClientInfo(Client*);           //회원 정보 수정, 삭제 시 관리자 페이지에 사용자 정보를 보내기 위해 호출되는 신호
    void clear_Widget_N_LineEdit();         //관리자 페이지에서 회원 정보 수정 시 회원 위젯 리스트를 비우기 위해 호출되는 신호

    /*쇼핑 화면 로그인*/
    void successLogin(QString);             //쇼핑 화면에서 로그인 시도가 성공 시 해당 사용자의 이름을 보내기 위해 호출되는 신호
    void failedLogin();                     //쇼핑 화면에서 로그인 시도가 실패 시 알려주기 위해 호출되는 신호

    /*채팅 프로그램*/
    void sendToServer(QString, QString);    //서버에 보내줄 사용자의 ID와 이름을 보내주기 위해 호출되는 신호
    void sendNameToServer(QStringList);
};

#endif // CLIENTMANAGER_H
