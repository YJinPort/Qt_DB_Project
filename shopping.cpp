#include "shopping.h"

Shopping::Shopping(int shoppingCount, QString productName, int productPrice, int productCount, QString productType, QString clientAddress)
{
    setText(0, QString::number(shoppingCount));
    setText(1, productName);
    setText(2, QString::number(productPrice));
    setText(3, QString::number(productCount));
    setText(4, productType);
    setText(5, clientAddress);
}

int Shopping::shoppingCount() const {
    return text(0).toInt();
}

QString Shopping::getProductName() const {
    return text(1);
}

int Shopping::getProductPrice() const {
    return text(2).toInt();
}

int Shopping::getProductCount() const {
    return text(3).toInt();
}

QString Shopping::getProductType() const {
    return text(4);
}

QString Shopping::getClientAddress() const {
    return text(5);
}

void Shopping::setProductName(QString &productName) {
    setText(1, productName);
}

void Shopping::setProductPrice(int &productPrice) {
    setText(2, QString::number(productPrice));
}

void Shopping::setProductCount(int &productCount) {
    setText(3, QString::number(productCount));
}

void Shopping::setProductType(QString &productType) {
    setText(4, productType);
}

void Shopping::setClientAddress(QString &clientAddress) {
    setText(5, clientAddress);
}
