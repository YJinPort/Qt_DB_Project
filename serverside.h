#ifndef SERVERSIDE_H
#define SERVERSIDE_H

#include <QWidget>
#include <QHash>

class QLabel;
class QTcpServer;
class QTcpSocket;
class QFile;
class QProgressDialog;
class LogData;

namespace Ui {
class ServerSide;
}

typedef enum {
    Chat_Login,             // 로그인(서버 접속)   --> 초대를 위한 정보 저장
    Chat_In,                // 채팅방 입장
    Chat_Talk,              // 채팅
    Chat_Out,               // 채팅방 퇴장         --> 초대 가능
    Chat_LogOut,            // 로그 아웃(서버 단절) --> 초대 불가능
    Chat_Invite,            // 초대
    Chat_KickOut,           // 강퇴
} Chat_Status;

class ServerSide : public QWidget
{
    Q_OBJECT

public:
    explicit ServerSide(QWidget *parent = nullptr); //생성자 - 채팅, 파일 서버와 관련된 내용을 생성한다.
    ~ServerSide();                                  //소멸자 - 로그, 채팅 서버, 파일 서버 종료

private slots:
    /*채팅 서버에서 사용되는 SLOT 함수*/
    void clientConnect( );                  //회원과의 채팅 연결 관리
    void receiveData( );                    //회원과의 채팅에 대한 관리
    void removeClient( );                   //클라이언트가 종료되었는데 로그아웃 신호를 받지 못할 경우를 위한 안전장치
    void addClient(QString, QString);       //클라이언트가 추가되었을 경우
    void inviteClient();                    //클라이언트 초대하기
    void kickOut();                         //클라이언트 강퇴하기
    void inputNameComboBox(QStringList);    //메시지 콤보 박스를 채우기 위해 회원 이름을 받아오는 SLOT 함수
    void on_clientTreeWidget_customContextMenuRequested(const QPoint &pos); //회원 리스트에서 회원을 초대, 강퇴를 클릭할 경우 실행

    /*채팅 서버에서 버튼 클릭시 실행되는 함수*/
    void on_sendPushButton_clicked();       //서버가 클라이언트에게 채탕하기
    void on_exitPushButton_clicked();       //exit 버튼을 클릭했을 경우 실행

    /*파일 서버에서 사용되는 SLOT 함수*/
    void acceptConnection();    //파일 전송을 위한 소켓 생성
    void readClient();          //파일 전송

private:
    const int BLOCK_SIZE = 1024;    //전송할 데이터 사이즈 지정
    const int PORT_NUMBER = 8000;   //서버의 포트 번호 지정

    Ui::ServerSide *ui;

    /*채팅*/
    QTcpServer *chatServer;                         //채팅 서버 생성을 위한 멤버 변수
    QTcpServer *fileServer;                         //파일 서버 생성을 위한 멤버 변수
    QHash<quint16, QString> clientNameHash;         //회원 이름의 저장을 위한 Hash
    QHash<QString, QTcpSocket*> clientSocketHash;   //회원과 연결할 소켓 저장을 위한 Hash
    QHash<QString, QString> clientIDHash;           //회원 아이디의 저장을 위한 Hash
    QMenu* menu;                                    //메뉴 생성을 위한 멤버 변수

    /*파일*/
    QFile* file;                        //파일 전송을 위한 멤버 변수
    QProgressDialog* progressDialog;    //파일 전송 시 전송률을 나타내기 위한 멤버 변수
    qint64 totalSize;                   //파일의 크기를 나타내기 위한 멤버 변수
    qint64 byteReceived;                //파일 전송 시 사용되는 멤버 변수
    QByteArray inBlock;                 //파일 전송 크기를 위한 멤버 변수

    /*로그*/
    LogData* logData;   //서버의 전송에 대한 로그를 남기기 위한 멤버 변수

    /*사용자 정의 이벤트 핸들러*/
    void closeEvent(QCloseEvent*) override;     //윈도우Title 창의 X를 눌러 종료하였을 경우 처리하는 이벤트 핸들러
};

#endif // SERVERSIDE_H
