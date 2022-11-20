#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientManager;
class ProductManager;
class ShoppingManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);  //각 객체 생성 및 connect()함수 지정
    ~MainWindow();                          //생성한 객체 삭제

private:
    Ui::MainWindow *ui;

    ClientManager *clientManager;       //사용자 화면 객체 생성을 위한 멤버 변수
    ProductManager *productManager;     //관리자 페이지 객체 생성을 위한 멤버 변수
    ShoppingManager *shoppingManager;   //쇼핑 화면 객체 생성을 위한 멤버 변수

public slots:
    /*각 객체에서 보낸 SIGNAL을 메인 윈도우에서 처리하는 SLOT함수*/
    void openClientWindow();        //쇼핑 화면에서 회원가입 버튼을 누를 경우 회원가입 화면으로 전환
    void connectMyPage(QString);    //쇼핑 화면에서 마이 페이지 버튼을 누를 경우 마이 페이지 화면으로 전환
    void joinClient();              //회원가입 화면에서 회원 가입 버튼을 누를 경우 쇼핑 화면으로 전환
    void cancellationClient();      //회원가입 화면에서 등록 취소 버튼을 누를 경우 쇼핑 화면으로 전환
    void openProductWindow();       //쇼핑 화면에서 관리자 페이지 이동 버튼을 누를 경우 관리자 페이지 화면으로 전환
    void quitProductWindow();       //관리자 페이지에서 나가기 버튼을 누를 경우 쇼핑 화면으로 전환

signals:
    void callMyPage(QString);       //마이 페이지 화면의 ui에 텍스트를 지정하기 위해 호출되는 신호
};
#endif // MAINWINDOW_H
