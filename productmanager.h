#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlTableModel>

class Product;
class Client;
class QTreeWidgetItem;

namespace Ui {
class ProductManager;
}

class ProductManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManager(QWidget *parent = nullptr); //생성자 - productlist.txt에 저장된 정보를 불러와 제품리스트에 저장한다.
    ~ProductManager();                                  //소멸자 - 제품리스트에 저장된 정보를 productlist.txt에 저장한다.

private slots:
    /*관리자 페이지에 생성된 버튼을 클릭했을 경우 실행하는 함수*/
    void on_proRegisterPushButton_clicked();    //제품 등록/변경 버튼 클릭 시 동작
    void on_proRemovePushButton_clicked();      //제품 삭제 버튼 클릭 시 동작
    void on_clientUpdatePushButton_clicked();       //회원 정보 수정 버튼 클릭 시 동작
    void on_clientRemovePushButton_clicked();       //회원 삭제 버튼 클릭 시 동작
    void on_getOutPushButton_clicked();             //나가기 버튼 클릭 시 동작

    /*관리자 페이지에서 리스트를 클릭하였을 경우 실행하는 함수*/
    void on_productTableView_clicked(const QModelIndex &index);         //제품 테이블에서 해당 제품을 클릭했을 경우 우측 라인Edit에 표시
    void on_clientListTableView_clicked(const QModelIndex &index);      //회원 테이블에서 해당 회원을 클릭했을 경우 우측 라인Edit에 표시

    /*---connect()에서 SIGNAL을 받아 처리하는 SLOT 함수---*/
    /*사용자 화면에서 보낸 SIGNAL 처리*/
    //void receivedClientInfo(Client*);       //관리자 페이지에서 회원 정보를 수정, 삭제할 경우 갱신된 사용자 정보를 받아오기 위한 SLOT 함수
    void loadToClientTable(QSqlTableModel*);    //관리자 페이지에서 회원 정보를 수정, 삭제할 경우 사용자 정보 입력 LineEdit를 비우기 위한 SLOT 함수

    /*쇼핑 화면에서 보낸 SIGNAL 처리*/
    void containProductInfo();                  //쇼핑 화면에서 제품 정보의 리스트를 요청할 경우 제품 정보를 쇼핑 화면으로 보내기 위한 SLOT 함수
    int updateAfterUpCount(QString, int);       //쇼핑 화면에서 주문하기, 주문변경을 하였을 경우 해당 제품의 재고를 확인하고 관리하기 위한 SLOT 함수
    void updateAfterDownCount(QString, int);    //쇼핑 화면에서 주문취소를 하였을 경우 해당 제품의 재고에 취소한 갯수만큼 추가해주기 위한 SLOT 함수



private:
    Ui::ProductManager *ui;

    QSqlQuery *query;
    QSqlTableModel *productModel;

signals:
    /*관리자 페이지*/
    void quitProduct();                     //관리자 페이지에서 나가기 버튼을 클릭했을 경우 호출되는 신호
    void updateBtnClicked(QStringList);     //관리자 페이지에서 회원 정보 수정 시 해당 정보를 사용자 화면에 전달하기 위해 호출되는 신호
    void deleteBtnClicked(QString);         //관리자 페이지에서 회원 정보 삭제 시 해당 정보를 사용자 화면에 전달하기 위해 호출되는 신호

    /*쇼핑 화면*/
    void sendProductInfo(Product*);         //쇼핑 화면에서 요청한 제품 정보를 전달하기 위해 호출되는 신호
    void sendProductTable(QSqlTableModel*);
};

#endif // PRODUCTMANAGER_H
