#ifndef CHATTINGFORM_CLIENT_H
#define CHATTINGFORM_CLIENT_H

#include <QWidget>
#include <QDataStream>
#include "serverside.h"

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QPushButton;
class QFile;
class QProgressDialog;

class ChattingForm_Client : public QWidget
{
    Q_OBJECT

public:
    const int PORT_NUMBER = 8000;

    ChattingForm_Client(QWidget *parent = nullptr); //생성자 - 채팅 및 파일 서버 입장을 위한 준비 및 위젯 생성
    ~ChattingForm_Client();                         //소멸자 - 클라이언트 소켓 삭제

private slots:
    void receivedLoginName(QString);                    //편리한 서버 접속을 위한 로그인한 회원의 이름을 자동 지정
    void receiveData();                                 //서버에서 전달되는 데이터를 받을 경우 처리
    void sendData();                                    //메시지를 전송할 경우 실행
    void disconnect();                                  //연결이 끊어졌을 경우 처리
    void sendProtocol(Chat_Status, char*, int = 1020);  //프로토콜 생성 후 서버로의 전송 처리
    void sendFile();                                    //파일 전송 시 처리
    void goOnSend(qint64);                              //용량이 큰 파일 전송 시 전송 용량 분할 처리

private:
    QLineEdit *name;                    //회원 이름을 입력할 LineEdit
    QTextEdit *message;                 //서버 전달되는 메시지용 TextEdit
    QLineEdit* serverAddress;           //서버 IP주소를 입력할 LineEdit
    QLineEdit* serverPort;              //서버 포트 번호를 입력할 LineEdit
    QLineEdit *inputLine;               //서버로 전송할 메시지를 입력할 LineEdit
    QPushButton *connectButton;         //서버로의 접속 처리를 위한 버튼
    QPushButton *sendButton;            //메시지 전송을 위한 버튼
    QPushButton* fileButton;            //파일 전송을 위한 버튼
    QTcpSocket *clientSocket;           //클라이언트 채팅용 소켓 생성
    QTcpSocket *fileClient;             //클라이언트 파일용 소켓 생성
    QProgressDialog* progressDialog;    //파일 전송률 확인을 위한 progressDialog
    QFile* file;                        //서버로 전송할 파일
    qint64 loadSize;                    //전송할 파일의 크기
    qint64 byteToWrite;                 //전송하고 남은 파일의 크기
    qint64 totalSize;                   //전체 파일의 크기
    QByteArray outBlock;                //파일 전송을 위한 데이터
    bool isSent;                        //파일 서버에 대한 접속 확인

    void closeEvent(QCloseEvent*) override;     //윈도우Title 창의 X를 눌러 종료하였을 경우 처리하는 이벤트 핸들러
};

#endif // CHATTINGFORM_CLIENT_H
