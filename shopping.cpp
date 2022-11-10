#include "shopping.h"

Shopping::Shopping(int shoppingNumber, QString productName, int productPrice, int productCount, QString productType, QString clientAddress, QString clientName)
{
    setText(0, QString::number(shoppingNumber));
    setText(1, productName);
    setText(2, QString::number(productPrice));
    setText(3, QString::number(productCount));
    setText(4, productType);
    setText(5, clientAddress);
    setText(6, clientName);
}

//주문 수를 호출하기 위해 사용
int Shopping::shoppingNumber() const {
    return text(0).toInt();
}

//주문한 제품 이름을 호출하기 위해 사용
QString Shopping::getProductName() const {
    return text(1);
}

//주문한 제품 가격을 호출하기 위해 사용
int Shopping::getProductPrice() const {
    return text(2).toInt();
}

//주문한 제품의 수량을 호출하기 위해 사용
int Shopping::getProductCount() const {
    return text(3).toInt();
}

//주문한 제품 종류를 호출하기 위해 사용
QString Shopping::getProductType() const {
    return text(4);
}

//주문자의 주소를 호출하기 위해 사용
QString Shopping::getClientAddress() const {
    return text(5);
}

//주문자의 이름을 호출하기 위해 사용
QString Shopping::getClientName() const {
    return text(6);
}

//주문한 제품 이름을 지정하기 위해 사용
void Shopping::setProductName(QString &productName) {
    setText(1, productName);
}

//주문한 제품 가격을 지정하기 위해 사용
void Shopping::setProductPrice(int &productPrice) {
    setText(2, QString::number(productPrice));
}

//주문한 제품의 수량을 지정하기 위해 사용
void Shopping::setProductCount(int &productCount) {
    setText(3, QString::number(productCount));
}

//주문한 제품 종류를 지정하기 위해 사용
void Shopping::setProductType(QString &productType) {
    setText(4, productType);
}

//주문자의 주소를 지정하기 위해 사용
void Shopping::setClientAddress(QString &clientAddress) {
    setText(5, clientAddress);
}

//주문자의 이름을 지정하기 위해 사용
void Shopping::setClientName(QString &clientName) {
    setText(6, clientName);
}
