#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanager.h"
#include "productmanager.h"
#include "shoppingmanager.h"
//#include "serverside.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(1400, 800);

    shoppingManager = new ShoppingManager(this);
    clientManager = new ClientManager(this);
    productManager = new ProductManager(this);

    ui->mdiArea->addSubWindow(shoppingManager);
    ui->mdiArea->addSubWindow(clientManager);
    ui->mdiArea->addSubWindow(productManager);
    shoppingManager->showMaximized();

    /*쇼핑몰에서 보낸 SIGNAL을 처리하기 위한 연결*/
    //클라이언트 객체의 회원가입 화면을 열기 위한 연결
    connect(shoppingManager, SIGNAL(newClient()), this, SLOT(openClientWindow()));
    //클라이언트 객체의 마이 페이지 화면을 열기 위한 연결
    connect(shoppingManager, SIGNAL(myPage(QString)), this, SLOT(connectMyPage(QString)));
    //프로덕트 객체의 관리자 페이지 화면을 열기 위한 연결
    connect(shoppingManager, SIGNAL(onlyStaff()), this, SLOT(openProductWindow()));
    //메인 윈도우의 종료를 위한 연결
    connect(shoppingManager, SIGNAL(exitShopping()), this, SLOT(close()));
    //클라이언트 객체의 회원 정보를 얻기 위한 연결
    connect(shoppingManager, SIGNAL(viewClientList()), clientManager, SLOT(containClientInfo()));
    //프로덕트 객체의 제품 정보를 얻기 위한 연결
    connect(shoppingManager, SIGNAL(viewProductList()), productManager, SLOT(containProductInfo()));
    //제품 검색 후 프로덕트 객체의 제품 정보를 얻기 위한 연결
    connect(shoppingManager, SIGNAL(clickedSelectButton()), productManager, SLOT(selectProductInfo()));
    //검색 초기화 후 프로덕트 객체의 제품 정보를 얻기 위한 연결
    connect(shoppingManager, SIGNAL(resetProductList()), productManager, SLOT(resetProductInfo()));
    //로그인 시도 시 아이디의 유/무를 확인하기 위한 연결
    connect(shoppingManager, SIGNAL(login(QString)), clientManager, SLOT(checkLoginId(QString)));
    //주문 시 회원의 주소 정보를 얻기 위한 연결
    connect(shoppingManager, SIGNAL(takeOrderSign(QString)), clientManager, SLOT(findAddressForOrder(QString)));
    //주문하기/주문변경 시 제품의 재고 관리를 위한 연결
    connect(shoppingManager, SIGNAL(updateAfter_upCount(QString, int)), productManager, SLOT(updateAfterUpCount(QString, int)));
    //주문변경/주문취소 시 제품의 재고 관리를 위한 연결
    connect(shoppingManager, SIGNAL(updateAfter_downCount(QString, int)), productManager, SLOT(updateAfterDownCount(QString, int)));
    //회원탈퇴 시 회원의 정보 삭제를 위한 연결
    connect(shoppingManager, SIGNAL(deleteClient(QString)), clientManager, SLOT(deleteId_List(QString)));
    //채팅 서버 오픈 시 회원의 정보를 출력하기 위한 연결
    connect(shoppingManager, SIGNAL(serverBtnClicked()), clientManager, SLOT(serverOpenFromShopping()));
    //채팅 서버 오픈 시 회원의 정보를 콤보박스에 담아주기 위한 연결
    connect(shoppingManager, SIGNAL(serverInputComboBox()), clientManager, SLOT(sendNameListToServer()));

    /*사용자 화면에서 보낸 SIGNAL을 처리하기 위한 연결*/
    //등록 취소 시 쇼핑 객체의 쇼핑 화면을 열기 위한 연결
    connect(clientManager, SIGNAL(cancellation()), this, SLOT(cancellationClient()));
    //회원 등록 시 쇼핑 객체의 쇼핑 화면을 열기 위한 연결
    connect(clientManager, SIGNAL(join()), this, SLOT(joinClient()));
    //마이 페이지에서 저장하기 버튼 클릭 시 쇼핑 화면을 열기 위한 연결
    connect(clientManager, SIGNAL(updateInMyPage(QString)), shoppingManager, SLOT(updateLabelName(QString)));
    //로그인 성공 시 쇼핑 화면의 텍스트를 변경하기 위한 연결
    connect(clientManager, SIGNAL(successLogin(QString, QString)), shoppingManager, SLOT(successLoginCheck(QString, QString)));
    //로그인 실패 시 로그인 실패에 대한 알림을 출력하기 위한 연결
    connect(clientManager, SIGNAL(failedLogin()), shoppingManager, SLOT(failedLoginCheck()));
    //관리자 페이지 이동 시 회원의 정보를 출력하기 위한 연결
    connect(clientManager, SIGNAL(sendClientTable(QSqlTableModel*)), productManager, SLOT(loadToClientTable(QSqlTableModel*)));
    //서버 오픈 시 회원의 정보를 서버로 전달해주기 위한 연결(회원 리스트용)
    connect(clientManager, SIGNAL(sendToServer(QString, QString)), shoppingManager, SLOT(clientSignalReceived(QString, QString)));
    //서버 오픈 시 회원의 정보를 서버로 전달해주기 위한 연결(채팅 콤보박스용)
    connect(clientManager, SIGNAL(sendNameToServer(QStringList)), shoppingManager, SLOT(inputNameServerCombobox(QStringList)));

    /*관리자 화면에서 보낸 SIGNAL을 처리하기 위한 연결*/
    //나가기 버튼 클릭 시 쇼핑 화면을 열기 위한 연결
    connect(productManager, SIGNAL(quitProduct()), this, SLOT(quitProductWindow()));
    //쇼핑 화면에서 제품의 정보를 출력하기 위한 연결
    connect(productManager, SIGNAL(sendProductTable(QSqlTableModel*)), shoppingManager, SLOT(receivedProductInfo(QSqlTableModel*)));
    //관리자 페이지에서 회원의 정보를 수정하기 위한 연결
    connect(productManager, SIGNAL(updateBtnClicked(QStringList)), clientManager, SLOT(updateClientInfo(QStringList)));
    //관리자 페이지에서 회원의 정보를 삭제하기 위한 연결
    connect(productManager, SIGNAL(deleteBtnClicked(QString)), clientManager, SLOT(deleteClientInfo(QString)));
    //쇼핑 화면에서 제품 검색 시 검색한 제품 리스트를 출력하기 위한 연결
    connect(productManager, SIGNAL(sendSelectTable(QSqlTableModel*)), shoppingManager, SLOT(viewSelectProductList(QSqlTableModel*)));
    //쇼핑 화면에서 검색 초기화 시 초기화한 제품 리스트를 출력하기 위한 연결
    connect(productManager, SIGNAL(sendResetTable(QSqlTableModel*)), shoppingManager, SLOT(productViewReset(QSqlTableModel*)));

    /*MainWindow 스스로가 호출한 SIGNAL을 처리하기 위한 연결*/
    //마이 페이지 오픈 시 마이 페이지의 텍스트를 변경하기 위한 연결
    connect(this, SIGNAL(callMyPage(QString)), clientManager, SLOT(viewMyPage(QString)));

    shoppingManager->dataLoad();    //프로그램 첫 실행 시 쇼핑 화면에 데이터를 출력한다.
}

//쇼핑 화면에서 회원가입 버튼을 누를 경우 회원가입 화면으로 전환
void MainWindow::openClientWindow() {
    clientManager->setFocus();
}

//쇼핑 화면에서 마이 페이지 버튼을 누를 경우 마이 페이지 화면으로 전환
void MainWindow::connectMyPage(QString userID) {
    //마이 페이지 화면의 ui에 텍스트를 지정하기 위해 호출되는 신호
    emit callMyPage(userID);
    clientManager->setFocus();
}

//회원가입 화면에서 회원 가입 버튼을 누를 경우 쇼핑 화면으로 전환
void MainWindow::joinClient() {
    shoppingManager->setFocus();
}

//회원가입 화면에서 등록 취소 버튼을 누를 경우 쇼핑 화면으로 전환
void MainWindow::cancellationClient() {
    shoppingManager->setFocus();
}

//쇼핑 화면에서 관리자 페이지 이동 버튼을 누를 경우 관리자 페이지 화면으로 전환
void MainWindow::openProductWindow() {
    productManager->dataLoad();
    productManager->setFocus();
}

//관리자 페이지에서 나가기 버튼을 누를 경우 쇼핑 화면으로 전환
void MainWindow::quitProductWindow() {
    shoppingManager->dataLoad();
    shoppingManager->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
