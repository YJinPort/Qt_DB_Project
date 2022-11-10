#include "serverside.h"
#include "ui_serverside.h"
#include "logdata.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

//생성자 - 채팅, 파일 서버와 관련된 내용을 생성한다.
ServerSide::ServerSide(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerSide), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);  //생성한 Ui를 현재 클래스(this)위에 올리는 역할을 한다.

    /*대기중인 회원 리스트의 사이즈 지정*/
    QList<int> sizes;
    sizes << 220 << 500;
    ui->splitter->setSizes(sizes);

    chatServer = new QTcpServer(this);
    connect(chatServer, SIGNAL(newConnection()), SLOT(clientConnect()));
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"), tr("Unable to start the server: %1.").arg(chatServer->errorString()));

        return;
    }

    /*채팅과 파일의 서버를 이원화한 이유: 파일 전송 중에도 채팅을 할 수 있도록 하기 위해 이원화한다.*/

    fileServer = new QTcpServer(this);
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), tr("Unable to start the server: %1.").arg(fileServer->errorString()));

        return;
    }

    /*초대 액션 생성 및 실행 로직 연결*/
    QAction* inviteAction = new QAction(tr("&Invite"));     //초대 액션 생성과 동시에 액션 이름 초기화
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    /*강퇴 액션 생성 및 실행 로직 연결*/
    QAction* kickOutAction = new QAction(tr("&Kick out"));  //강퇴 액션 생성과 동시에 액션 이름 초기화
    connect(kickOutAction, SIGNAL(triggered()), SLOT(kickOut()));

    menu = new QMenu;               //메뉴 생성
    menu->addAction(inviteAction);  //메뉴에 초대 액션 추가
    menu->addAction(kickOutAction); //메뉴에 강퇴 액션 추가

    //해당 메뉴를 컨텍스트 메뉴로 지정
    ui->clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    /*파일 전송 관련 다이얼로그 객체 생성*/
    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    /*로그 데이터 관련 객체 생성*/
    logData = new LogData(this);
    logData->start();

    //저장 버튼 클릭 시 로그 데이터 저장 SLOT 연결
    connect(ui->savePushButton, SIGNAL(clicked()), logData, SLOT(saveData()));

    setWindowTitle(tr("Server_Admin")); //열리는 윈도우의 제목을 Server_Admin로 설정한다.
}

//소멸자 - 로그, 채팅 서버, 파일 서버 종료
ServerSide::~ServerSide()
{
    delete ui;

    logData->terminate();
    chatServer->close();
    fileServer->close();
}

//회원과의 채팅 연결 관리
void ServerSide::clientConnect()
{
    //연결 관련 소켓 객체 생성
    QTcpSocket *clientConnection = chatServer->nextPendingConnection();

    //채팅 준비가 왼료된 경우 실행
    connect(clientConnection, SIGNAL(readyRead()), SLOT(receiveData()));

    //연결이 끊어진 경우 실행
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeClient()));
}

//회원과의 채팅에 대한 관리
void ServerSide::receiveData()
{
    /*어떤 클라이언트가 데이터를 보낸 것인지 구분하는 역할*/
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket*>(sender());   //sender: 연결 되어있는 소켓, 시그널을 보낼 객체(clientconnect에서)
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    Chat_Status type;       //채팅 관련 상태
    char data[1020];        //전송되는 메시지/데이터
    memset(data, 0, 1020);  //memst이 쓰레기값이 들어가기 때문에 0으로 초기화

    /*채팅 데이터를 읽는다.*/
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    /*연결된 회원의 IP주소, 포트 번호, 이름을 가져온다.*/
    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();
    QString name = QString::fromStdString(data);

    /*채팅 관련 상태에 따라 분류*/
    switch(type) {

    /*대기방에 입장하였을 경우*/
    case Chat_Login:
        int idx;    //회원 리스트의 인덱스를 가져오기 위한 변수
        //대기방에 입장한 회원의 이름과 동일한 이름의 정보를 clientQuit위젯에서 찾는다.
        foreach(auto item, ui->clientQuitTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
            idx = ui->clientQuitTreeWidget->indexOfTopLevelItem(item);  //찾은 회원 정보의 인덱스를 담는다.
            ui->clientQuitTreeWidget->takeTopLevelItem(idx);            //해당 회원의 정보를 인덱스를 이용하여 삭제한다.
            /*client위젯에 추가하기 위해 찾은 item의 정보를 지정한다.*/
            item->setIcon(0, QIcon("stay.png"));
            item->setText(1, name);
            item->setText(2, "대기중");
            ui->clientTreeWidget->addTopLevelItem(item);        //해당 아이템을 client위젯에 추가한다.
            ui->clientTreeWidget->resizeColumnToContents(0);    //위젯의 컬럼 사이즈를 지정한다.

            clientSocketHash[name] = clientConnection;  //이름에다가 클라이언트 소켓을 연결 - 누구의 소켓인지 구별하기 위함
        }
        break;

    /*채팅방에 참여하였을 경우*/
    case Chat_In:
        //채팅방에 입장한 회원의 이름과 동일한 이름의 정보를 client위젯에서 찾는다.
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
            /*찾은 item의 정보를 지정한다.*/
            if(item->text(2) != "채팅중") {
                item->setIcon(0, QIcon("talk.png"));
                item->setText(2, "채팅중");
            }

            /*해당 포트 번호 key값에 이름을 저장하여 관리한다.*/
            clientNameHash[port] = name;
            if(clientSocketHash.contains(name)) clientSocketHash[name] = clientConnection;
        }
        break;

    /*메시지를 보낼 경우*/
    case Chat_Talk: {
        /*clientSocketHash에 저장된 모든 회원(클라이언트)*/
        foreach(QTcpSocket *sock, clientSocketHash.values()) {
            //채팅을 입력한 회원 외의 모든 회원에게 채팅한 내용을 전달한다.
            if(clientNameHash.contains(sock->peerPort()) && port != sock->peerPort()) {
                /*다른 회원들에게 보낼 메시지의 형식을 지정한다.*/
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;
                sendArray.append("<font color=lightsteelblue>");
                sendArray.append(clientNameHash[port].toStdString().data());
                sendArray.append("</font> : ");
                sendArray.append(name.toStdString().data());
                sock->write(sendArray);
            }
        }

        /*채팅한 내역에 대한 로그를 남긴다.*/
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget); //로그를 남기기 위한 객체 생성

        /*생성한 객체에 로그에 대한 정보를 지정한다.*/
        item->setText(0, ip);                                       //회원 IP주소
        item->setText(1, QString::number(port));                    //회원 포트 번호
        item->setText(2, clientIDHash[clientNameHash[port]]);       //회원 아이디
        item->setText(3, clientNameHash[port]);                     //회원 이름
        item->setText(4, QString(data));                            //회원이 입력한 메시지
        item->setText(5, QDateTime::currentDateTime().toString());  //채팅한 현재 날짜와 시간
        item->setToolTip(4, QString(data));                         //입력한 메시지에 대한 툴팁

        ui->messageTreeWidget->addTopLevelItem(item);               //서버의 로그 위젯에 item정보를 추가한다.

        /*로그에 표현될 컬럼의 사이즈를 메시지의 길이에 따라 결정한다.*/
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        logData->appendData(item);  //로그파일로의 저장을 위해 채팅 내역을 보낸다.
    }
        break;

    /*채팅방에서 대기방으로 이동하였을 경우*/
    case Chat_Out:
        //대기방으로 이동한 회원의 이름과 동일한 이름의 정보를 client위젯에서 찾는다.
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            /*찾은 item의 정보를 지정한다.*/
            if(item->text(2) != "대기중") {
                item->setIcon(0, QIcon("stay.png"));
                item->setText(2, "대기중");
            }

            clientNameHash.remove(port);    //회원의 포트번호에 대한 정보를 제거한다.
        }
        break;

    /*채팅 프로그램 사용을 종료하였을 경우*/
    case Chat_LogOut:
        //사용 종료한 회원의 이름과 동일한 이름의 정보를 client위젯에서 찾는다.
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            idx = ui->clientTreeWidget->indexOfTopLevelItem(item);  //찾은 회원 정보의 인덱스를 담는다.
            ui->clientTreeWidget->takeTopLevelItem(idx);            //해당 회원의 정보를 인덱스를 이용하여 삭제한다.
            /*clientQuit위젯에 추가하기 위해 찾은 item의 정보를 지정한다.*/
            item->setIcon(0, QIcon("disconnect.png"));
            item->setText(1, name);
            item->setText(2, "종료");
            ui->clientQuitTreeWidget->addTopLevelItem(item);    //해당 아이템을 clientQuit위젯에 추가한다.

            clientSocketHash.remove(name);  //회원의 이름에 대한 소켓을 제거한다.
        }
        break;

    }
}

//클라이언트가 종료되었는데 로그아웃 신호를 받지 못할 경우를 위한 안전장치
void ServerSide::removeClient()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket*>(sender());    //sender: 연결 되어있는 소켓, 시그널을 보낼 객체(clientconnect에서)
    if(clientConnection != nullptr) {
        QString name = clientNameHash[clientConnection->peerPort()];
        //사용 종료한 회원의 이름과 동일한 이름의 정보를 client위젯에서 찾는다.
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            /*clientQuit위젯에 추가하기 위해 찾은 item의 정보를 지정한다.*/
            item->setIcon(0, QIcon("disconnect.png"));
            item->setText(2, "종료");
            ui->clientQuitTreeWidget->addTopLevelItem(item);    //해당 아이템을 clientQuit위젯에 추가한다.
        }
        clientSocketHash.remove(name);      //회원의 이름에 대한 소켓을 제거한다.
        clientConnection->deleteLater();    //서버에서 올 데이터가 남아있을 수 있기 때문에 deleteLater를 사용하였다.
    }
}

//클라이언트가 추가되었을 경우
void ServerSide::addClient(QString id, QString name)
{
    //회원 리스트를 만들기 위한 객체 생성
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->clientQuitTreeWidget);
    /*clientQuit위젯에 추가하기 위해 item의 정보를 지정한다.*/
    item->setIcon(0, QIcon("disconnect.png"));
    item->setText(1, name);
    item->setText(2, "종료");
    ui->clientQuitTreeWidget->addTopLevelItem(item);        //해당 아이템을 clientQuit위젯에 추가한다.
    clientIDHash[name] = id;                                //회원 아이디를 value 값으로 저장한다.
    ui->clientQuitTreeWidget->resizeColumnToContents(0);    //위젯의 컬럼 사이즈를 지정한다.

    //포커스의 자동지정을 해제한다.
    ui->clientQuitTreeWidget->setCurrentItem(nullptr);
}

//클라이언트 초대하기
void ServerSide::inviteClient()
{
    if(ui->clientTreeWidget->topLevelItemCount()) {
        QString name = ui->clientTreeWidget->currentItem()->text(1);    //대기 회원 리스트에서 우클릭한 회원의 이름을 담는다.

        /*클라이언트로 초대한 상태를 ByteArray타입으로 상태와 이름을 전송*/
        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out << Chat_Invite;
        out.writeRawData("", 1020);

        QTcpSocket* sock = clientSocketHash[name];  //회원 이름으로 현재 연결된 소켓을 찾는다.

        if (sock == nullptr) return;    //회원이 선택되어 있지 않으면 Invite(초대), Kick out(강퇴) 메뉴가 표시되지 않는다.

        sock->write(sendArray);

        /*이름을 트리위젯에서 검색해서 고객의 현재 상태 아이콘을 Chat in으로 변경*/
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
            /*고객이 채팅방에 들어왔다고 알리는 아이콘 변경*/
            if(item->text(2) != "채팅중") {
                item->setText(2, "채팅중");
                item->setIcon(0, QIcon("talk.png"));
            }
        }

        /*소켓의 포트 번호를 키값으로 지정하여 회원 이름을 clientNameHash에 저장한다.*/
        quint64 port = sock->peerPort();
        clientNameHash[port] = name;
    }
}

//클라이언트 강퇴하기
void ServerSide::kickOut()
{
    /*클라이언트를 강퇴한 상태를 ByteArray타입으로 상태와 이름을 전송*/
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_KickOut;
    out.writeRawData("", 1020);

    //실행할 액션에 대한 회원이 지정되어 있는지에 대한 예외처리
    if(ui->clientTreeWidget->currentItem() != nullptr) {
        QString name = ui->clientTreeWidget->currentItem()->text(1);    //위젯에서 클릭한 회원 이름을 담는다.
        QTcpSocket* sock = clientSocketHash[name];                      //사용자 이름으로 해당 사용자에게 연결된 소켓을 찾는다.
        sock->write(sendArray);

        /*위젯에 표시된 회원 상태를 변경한다.*/
        ui->clientTreeWidget->currentItem()->setIcon(0, QIcon("stay.png"));
        ui->clientTreeWidget->currentItem()->setText(2, "대기중");
    }
}

//메시지 콤보 박스를 채우기 위해 회원 이름을 받아오는 SLOT 함수
void ServerSide::inputNameComboBox(QStringList nameList) {
    /*메시지 콤보 박스의 0번째 인덱스의 글자는 전체, 이후는 회원 이름으로 채운다.*/
    ui->clientListComboBox->addItem("전체");
    ui->clientListComboBox->addItems(nameList);
}

//회원 리스트에서 회원을 초대, 강퇴를 클릭할 경우 실행
void ServerSide::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    //실행할 액션에 대한 회원이 지정되어 있는지에 대한 예외처리
    if(ui->clientTreeWidget->currentItem()) {
        foreach(QAction *action, menu->actions()) {
            if(action->objectName() == "Invite")        //초대일 경우
                action->setEnabled(ui->clientTreeWidget->currentItem()->text(2) == "대기중");
            else                                        //강퇴일 경우
                action->setEnabled(ui->clientTreeWidget->currentItem()->text(2) == "채팅중");
        }
        /*우클릭한 위치에 액션 메뉴를 띄워주기 위한 객체*/
        QPoint globalPos = ui->clientTreeWidget->mapToGlobal(pos);
        menu->exec(globalPos);
    }
    else return;
}

//서버가 클라이언트에게 채탕하기
void ServerSide::on_sendPushButton_clicked()
{
    if(ui->clientTreeWidget->topLevelItemCount()) {
        /*전체 공지로 메시지를 보낼 경우*/
        if(ui->clientListComboBox->currentIndex() == 0) {
            /*clientSocketHash에 저장된 모든 회원(클라이언트)에 메시지 전송*/
            foreach(QTcpSocket *sock, clientSocketHash.values()) {
                /*모든 회원들에게 보낼 메시지의 형식을 지정한다.*/
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;
                sendArray.append("<font color=blue>");
                sendArray.append("Administrator");
                sendArray.append("</font> : ");
                sendArray.append(ui->sendLineEdit->text().toStdString().data());
                sock->write(sendArray);
            }
        }
        /*개인으로 메시지를 보낼 경우*/
        else {
            /*콤보 박스에서 클릭한 회원에게만 메시지 전송*/
            QString name = ui->clientListComboBox->currentText();
            if(clientSocketHash[name] == nullptr) return;
            QTcpSocket *sock = clientSocketHash[name];
            /*지정한 회원에게 보낼 메시지의 형식을 지정한다.*/
            QByteArray sendArray;
            sendArray.clear();
            QDataStream out(&sendArray, QIODevice::WriteOnly);
            out << Chat_Talk;
            sendArray.append("<font color=blue>");
            sendArray.append("Administrator");
            sendArray.append("</font> : ");
            sendArray.append(ui->sendLineEdit->text().toStdString().data());
            sock->write(sendArray);
        }

        //채팅 내역에 대한 로그를 만들기 위한 객체 생성
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);

        /*생성한 객체에 로그에 대한 정보를 지정한다.*/
        item->setText(0, "Host");                                   //관리자 호스트
        item->setText(1, QString::number(PORT_NUMBER));             //관리자 포트번호
        item->setText(2, "0");                                      //관리자번호
        item->setText(3, "Administrator");                          //관리자명
        item->setText(4, ui->sendLineEdit->text());                 //입력한 메시지
        item->setText(5, QDateTime::currentDateTime().toString());  //채팅한 현재 날짜와 시간
        item->setToolTip(4, ui->sendLineEdit->text());              //입력한 메시지에 대한 툴팁

        ui->messageTreeWidget->addTopLevelItem(item);               //로그 위젯에 item정보를 추가한다.

        /*로그에 표현될 컬럼의 사이즈를 메시지의 길이에 따라 결정한다.*/
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        logData->appendData(item);  //로그파일로의 저장을 위해 채팅 내역을 보낸다.
    }

    ui->sendLineEdit->clear();  //메시지 전송 후 메시지 입력란을 비운다.
}

//파일 전송을 위한 소켓 생성
void ServerSide::acceptConnection()
{
    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

//파일 전송
void ServerSide::readClient()
{
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket*>(sender());  //sender: 연결 되어있는 소켓, 시그널을 보낼 객체(clientconnect에서)
    QString filename, name;

    if (byteReceived == 0) {        //파일의 첫 전송 - 파일에 대한 정보로 QFile 객체 생성
        progressDialog->reset();
        progressDialog->show();

        QString ip = receivedSocket->peerAddress().toString();
        quint16 port = receivedSocket->peerPort();

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name;
        progressDialog->setMaximum(totalSize);

        //파일 전송에 대한 로그를 만들기 위한 객체 생성
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);

        /*생성한 객체에 로그에 대한 정보를 지정한다.*/
        item->setText(0, ip);                                       //회원 IP주소
        item->setText(1, QString::number(port));                    //회원 포트 번호
        item->setText(2, clientIDHash[name]);                       //회원 아이디
        item->setText(3, name);                                     //회원 이름
        item->setText(4, filename);                                 //전송한 파일의 절대경로, 파일명
        item->setText(5, QDateTime::currentDateTime().toString());  //전송한 현재 날짜와 시간
        item->setToolTip(4, filename);                              //전송한 파일의 절대경로, 파일명에 대한 툴팁

        /*로그에 표현될 컬럼의 사이즈를 길이에 따라 결정한다.*/
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        ui->messageTreeWidget->addTopLevelItem(item);   //로그 위젯에 item정보를 추가한다.

        logData->appendData(item);  //로그파일로의 저장을 위해 파일 전송 내역을 보낸다.

        QFileInfo info(filename);
        QString currentFileName = info.fileName();  //경로에서 이름만 가져온다. 이유: 보안상의 이유
        file = new QFile(currentFileName);
        file->open(QFile::WriteOnly | QFile::Truncate);   //덮어쓰기까지 추가
    }
    else {    //파일의 데이터를 읽어서 저장한다.
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }

    progressDialog->setValue(byteReceived); //progressDialog의 상태를 지정한다.

    /*파일 전송이 완료된 후 progressDialog와 파일의 객체를 삭제한다.*/
    if (byteReceived == totalSize) {
        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();

        file->close();
        delete file;
    }
}

//exit 버튼을 클릭했을 경우 실행
void ServerSide::on_exitPushButton_clicked()
{
    /*로그, 채팅 서버, 파일 서버 종료*/
    logData->terminate();
    chatServer->close();
    fileServer->close();

    close();    //소켓 제거
}

//windowTitle단의 X를 클릭하여 종료할 경우 실행
void ServerSide::closeEvent(QCloseEvent*) {
    /*로그, 채팅 서버, 파일 서버 종료*/
    logData->terminate();
    chatServer->close();
    fileServer->close();

    close();    //소켓 제거
}
