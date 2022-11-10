#ifndef SHOPPING_H
#define SHOPPING_H

#include <QTreeWidgetItem>

class Shopping : public QTreeWidgetItem
{
public:
    /*생성자의 인자값을 선언과 동시에 초기화*/
    explicit Shopping(int = 0, QString = "", int = 0, int = 0, QString = "", QString = "", QString = "");

    /*주문 정보를 호출하기 위해 사용되는 함수*/
    QString getProductName() const;     //주문한 제품 이름을 호출하기 위해 사용
    int getProductPrice() const;        //주문한 제품 가격을 호출하기 위해 사용
    int getProductCount() const;        //주문한 제품의 수량을 호출하기 위해 사용
    QString getProductType() const;     //주문한 제품 종류를 호출하기 위해 사용
    QString getClientAddress() const;   //주문자의 주소를 호출하기 위해 사용
    QString getClientName() const;      //주문자의 이름을 호출하기 위해 사용

    /*주문 정보를 지정하기 위해 사용되는 함수*/
    void setProductName(QString&);      //주문한 제품 이름을 지정하기 위해 사용
    void setProductPrice(int&);         //주문한 제품 가격을 지정하기 위해 사용
    void setProductCount(int&);         //주문한 제품의 수량을 지정하기 위해 사용
    void setProductType(QString&);      //주문한 제품 종류를 지정하기 위해 사용
    void setClientAddress(QString&);    //주문자의 주소를 지정하기 위해 사용
    void setClientName(QString&);       //주문자의 이름을 지정하기 위해 사용

    /*주문 수를 호출하기 위해 사용되는 함수*/
    int shoppingNumber() const;      //주문 번호를 호출하기 위해 사용
};

#endif // SHOPPING_H
