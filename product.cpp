#include "product.h"

Product::Product(int proNumber, QString proName, int proPrice, int proCount, QString proType)
{
    setText(0, QString::number(proNumber));
    setText(1, proName);
    setText(2, QString::number(proPrice));
    setText(3, QString::number(proCount));
    setText(4, proType);
}

//제품 번호를 호출하기 위해 사용
int Product::getProNumber() const {
    return text(0).toInt();
}

//제품 이름을 호출하기 위해 사용
QString Product::getProName() const {
    return text(1);
}

//제품 가격을 호출하기 위해 사용
int Product::getProPrice() const {
    return text(2).toInt();
}

//제품 재고량을 호출하기 위해 사용
int Product::getProCount() const {
    return text(3).toInt();
}

//제품 종류를 호출하기 위해 사용
QString Product::getProType() const {
    return text(4);
}

//제품 번호를 지정하기 위해 사용
void Product::setProNumber(int& proNumber) {
    setText(0, QString::number(proNumber));
}

//제품 이름을 지정하기 위해 사용
void Product::setProName(QString& proName) {
    setText(1, proName);
}

//제품 가격을 지정하기 위해 사용
void Product::setProPrice(int& proPrice) {
    setText(2, QString::number(proPrice));
}

//제품 재고량을 지정하기 위해 사용
void Product::setProCount(int& proCount) {
    setText(3, QString::number(proCount));
}

//제품 종류를 지정하기 위해 사용
void Product::setProType(QString& proType) {
    setText(4, proType);
}
