#ifndef PRODUCT_H
#define PRODUCT_H

#include <QTreeWidgetItem>

class Product : public QTreeWidgetItem
{
public:
    /*생성자의 인자값을 선언과 동시에 초기화*/
    explicit Product(int = 0, QString = "", int = 0, int = 0, QString = "");

    /*제품 정보를 호출하기 위해 사용되는 함수*/
    int getProNumber() const;       //제품 번호를 호출하기 위해 사용
    QString getProName() const;     //제품 이름을 호출하기 위해 사용
    int getProPrice() const;        //제품 가격을 호출하기 위해 사용
    int getProCount() const;        //제품 재고량을 호출하기 위해 사용
    QString getProType() const;     //제품 종류를 호출하기 위해 사용

    /*제품 정보를 지정하기 위해 사용되는 함수*/
    void setProNumber(int&);    //제품 번호를 지정하기 위해 사용
    void setProName(QString&);  //제품 이름을 지정하기 위해 사용
    void setProPrice(int&);     //제품 가격을 지정하기 위해 사용
    void setProCount(int&);     //제품 재고량을 지정하기 위해 사용
    void setProType(QString&);  //제품 타입을 지정하기 위해 사용
};

#endif // PRODUCT_H
