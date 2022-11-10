#include "chattingform_client.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDataStream>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

#define BLOCK_SIZE      1024

//생성자 - 채팅 및 파일 서버 입장을 위한 준비 및 위젯 생성
ChattingForm_Client::ChattingForm_Client(QWidget *parent)
    : QWidget(parent), isSent(false)
{
    //서버 접속을 위한 회원 이름을 입력하기 위한 LineEdit
    name = new QLineEdit(this);

    /*접속할 서버 IP주소를 입력할 LineEdit 생성과 IP주소 자동 지정*/
    serverAddress = new QLineEdit(this);
    serverAddress->setText("127.0.0.1");

    /*정규 표현식 사용*/
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);
    serverAddress->setPlaceholderText("Server IP Address");
    serverAddress->setValidator(&validator);

    /*접속할 서버 포트 번호를 입력할 LineEdit 생성과 포트 번호 자동 지정*/
    serverPort = new QLineEdit(this);
    serverPort->setText(QString::number(PORT_NUMBER));
    serverPort->setInputMask("00000;_");
    serverPort->setPlaceholderText("Server Port Number");

    //서버 접속 버튼 생성
    connectButton = new QPushButton(tr("Log In"), this);

    /*생성한 객체들을 HBoxLayout의 위젯으로 추가*/
    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addWidget(name);
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
    serverLayout->addWidget(connectButton);

    message = new QTextEdit(this);		//서버에서 오는 메시지 표시용
    message->setReadOnly(true);         //읽기 전용으로 지정

    /*서버로 보낼 메시지를 위한 객체 생성 및 연결*/
    inputLine = new QLineEdit(this);
    connect(inputLine, SIGNAL(returnPressed()), SLOT(sendData()));
    connect(inputLine, SIGNAL(returnPressed()), inputLine, SLOT(clear()));
    sendButton = new QPushButton("Send", this);
    connect(sendButton, SIGNAL(clicked()), SLOT(sendData()));
    connect(sendButton, SIGNAL(clicked()), inputLine, SLOT(clear()));
    inputLine->setDisabled(true);
    sendButton->setDisabled(true);

    /*생성한 객체들을 HBoxLayout의 위젯으로 추가*/
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sendButton);

    /*서버로 보낼 파일을 위한 버튼 생성 및 연결*/
    fileButton = new QPushButton("File Transfer", this);
    connect(fileButton, SIGNAL(clicked()), SLOT(sendFile()));
    fileButton->setDisabled(true);

    /*접속 종료를 위한 버튼 생성 및 연결*/
    QPushButton* exitButton = new QPushButton("Exit", this);
    connect(exitButton, SIGNAL(clicked()), this, SLOT(close()));

    /*생성한 버튼들을 HBoxLayout의 위젯으로 추가*/
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(fileButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(exitButton);

    /*생성한 HBoxLayout들을 QVBoxLayout 위젯으로 추가*/
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(serverLayout);
    mainLayout->addWidget(message);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    /*채팅을 위한 소켓 생성 및 연결*/
    clientSocket = new QTcpSocket(this);
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString(); });
    connect(clientSocket, SIGNAL(readyRead()), SLOT(receiveData()));
    connect(clientSocket, SIGNAL(disconnected()), SLOT(disconnect()));

    /*파일 전송을 위한 소켓 생성 및 연결*/
    fileClient = new QTcpSocket(this);
    connect(fileClient, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));

    /*파일 전송 시 전송률을 위한 progressDialog객체 생성*/
    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    /*서버 입장 시 클릭 버튼에 대한 설정*/
    connect(connectButton, &QPushButton::clicked,
            [=]{
        if(connectButton->text() == tr("Log In")) {
            clientSocket->connectToHost(serverAddress->text(),
                                        serverPort->text().toInt());
            clientSocket->waitForConnected();
            sendProtocol(Chat_Login, name->text().toStdString().data());
            connectButton->setText(tr("Chat In"));
            name->setReadOnly(true);
        }
        else if(connectButton->text() == tr("Chat In")) {
            sendProtocol(Chat_In, name->text().toStdString().data());
            connectButton->setText(tr("Chat Out"));
            inputLine->setEnabled(true);
            sendButton->setEnabled(true);
            fileButton->setEnabled(true);
        }
        else if(connectButton->text() == tr("Chat Out")) {
            sendProtocol(Chat_Out, name->text().toStdString().data());
            connectButton->setText(tr("Chat In"));
            inputLine->setDisabled(true);
            sendButton->setDisabled(true);
            fileButton->setDisabled(true);
        }
    } );

    setWindowTitle(tr("Chat Client"));  //열리는 윈도우의 제목을 Chat Client로 설정한다.
}

//소멸자 - 클라이언트 소켓 삭제
ChattingForm_Client::~ChattingForm_Client() {
    clientSocket->close();
    QSettings settings("ChatClient", "Chat Client");
    settings.setValue("ChatClient/ID", name->text());
}

//편리한 서버 접속을 위한 로그인한 회원의 이름을 자동 지정
void ChattingForm_Client::receivedLoginName(QString userName) {
    name->setText(userName);
    name->setDisabled(true);
}

//서버에서 전달되는 데이터를 받을 경우 처리
void ChattingForm_Client::receiveData()
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket*>(sender());    //sender: 연결 되어있는 소켓, 시그널을 보낼 객체(clientconnect에서)
    if (clientSocket->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);

    Chat_Status type;       //채팅 관련 상태
    char data[1020];        //전송되는 메시지/데이터
    memset(data, 0, 1020);  //memst이 쓰레기값이 들어가기 때문에 0으로 초기화

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;                     //패킷의 타입
    in.readRawData(data, 1020);     //실제 데이터

    /*채팅 관련 상태에 따라 분류*/
    switch(type) {

    /*패킷의 타입이 채팅일 경우*/
    case Chat_Talk:
        message->append(QString(data));     //메시지를 화면에 표시
        inputLine->setEnabled(true);        //메시지 입력 LineEdit 활성화
        sendButton->setEnabled(true);       //전송 버튼 활성화
        fileButton->setEnabled(true);       //파일 버튼 활성화

        connectButton->setText("Chat Out"); //버튼명 변경

        break;

    /*패킷의 타입이 강퇴일 경우*/
    case Chat_KickOut:
        QMessageBox::information(this, tr("Chatting Client"), tr("Kick out from Server"));
        inputLine->setDisabled(true);       //메시지 입력 LineEdit 비활성화
        sendButton->setDisabled(true);      //전송 버튼 비활성화
        fileButton->setDisabled(true);      //파일 버튼 비활성화
        name->setReadOnly(false);           //메시지 입력 불가능

        connectButton->setText("Chat In");  //버튼명 변경
        connectButton->setDisabled(true);   //버튼 비활성화

        break;

    /*패킷의 타입이 초대일 경우*/
    case Chat_Invite:
        QMessageBox::information(this, tr("Chatting Client"), tr("Invited from Server"));
        inputLine->setEnabled(true);        //메시지 입력 LineEdit 활성화
        sendButton->setEnabled(true);       //전송 버튼 활성화
        fileButton->setEnabled(true);       //파일 버튼 활성화
        name->setReadOnly(true);            //메시지 입력 가능

        connectButton->setText("Chat Out"); //버튼명 변경
        connectButton->setDisabled(false);  //버튼 활성화

        break;
    };
}

//메시지를 전송할 경우 실행
void ChattingForm_Client::sendData(  )
{
    QString str = inputLine->text();    //입력한 메시지를 QString타입의 변수에 담는다.
    QString userName = name->text();    //접속한 회원의 이름을 QString타입의 변수에 담는다.

    //입력한 메시지가 있을 경우
    if(str.length()) {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        /*메시지 전송 시 전송자 이름 추가*/
        message->append("<font color=black>" + userName +"</font> : " + str);
        sendProtocol(Chat_Talk, bytearray.data());
    }
}

//연결이 끊어졌을 경우 처리
void ChattingForm_Client::disconnect()
{
    QMessageBox::critical(this, tr("Chatting Client"), tr("Disconnect from Server"));
    inputLine->setEnabled(false);           //메시지 입력 LineEdit 비활성화
    name->setReadOnly(false);               //메시지 입력 불가능
    sendButton->setEnabled(false);          //전송 버튼 비활성화

    connectButton->setText(tr("Log in"));   //버튼명 변경
}

//프로토콜 생성 후 서버로의 전송 처리
void ChattingForm_Client::sendProtocol(Chat_Status type, char* data, int size)
{
    /*소켓으로 전송할 데이터 관련 처리*/
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);

    /*서버로의 전송*/
    clientSocket->write(dataArray);
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

//파일 전송 시 처리
void ChattingForm_Client::sendFile()
{
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    //전송할 파일의 파일명과 경로를 담는다.
    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);
        file->open(QFile::ReadOnly);

        progressDialog->setValue(0); //첫 전송률은 0으로 지정한다.

        //파일 서버에 접속되지 않았을 경우
        if (!isSent) {
            fileClient->connectToHost(serverAddress->text(),
                                      serverPort->text().toInt() + 1);
            isSent = true;
        }

        byteToWrite = totalSize = file->size(); //전송 후 남은 데이터의 크기를 담는다.
        loadSize = 1024;                        //전송되는 데이터의 크기

        QDataStream out(&outBlock, QIODevice::WriteOnly);
        //totalSize가 qint64타입으로 멤버 변수에 선언되었기에 자료형 타입을 맞춰서 비워놓음
        out << qint64(0) << qint64(0) << filename << name->text();

        totalSize += outBlock.size();   //총 크기: 파일 크기 + 파일 이름 및 정보의 크기
        byteToWrite += outBlock.size();

        //qint64(0)이 들어간 인덱스까지 파일 포인터를 지정한다.
        out.device()->seek(0);
        out << totalSize << qint64(outBlock.size());

        fileClient->write(outBlock); //파일을 소켓으로 전송

        progressDialog->setMaximum(totalSize);
        progressDialog->setValue(totalSize-byteToWrite);
        progressDialog->show();
    }
}

//용량이 큰 파일 전송 시 전송 용량 분할 처리
void ChattingForm_Client::goOnSend(qint64 numBytes)
{
    //데이터의 크기 지정
    byteToWrite -= numBytes;
    outBlock = file->read(qMin(byteToWrite, numBytes));
    fileClient->write(outBlock);

    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize-byteToWrite);

    //전송 완료 시 처리
    if (byteToWrite == 0) progressDialog->reset();
}

//windowTitle단의 X를 클릭하여 종료할 경우 실행
void ChattingForm_Client::closeEvent(QCloseEvent*)
{
    sendProtocol(Chat_LogOut, name->text().toStdString().data());
    clientSocket->disconnectFromHost();

    /*disconnectFromHost()를 했는데도 불구하고 연결이 끊어지지 않았을 경우*/
    if(clientSocket->state() != QAbstractSocket::UnconnectedState) clientSocket->waitForDisconnected();    //끊어질 때 까지 기다린다.
}
