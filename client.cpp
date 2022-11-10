#include "client.h"

Client::Client(QString userID, QString name, QString phoneNumber, QString address, QString gender)
{
    setText(0, userID);
    setText(1, name);
    setText(2, phoneNumber);
    setText(3, address);
    setText(4, gender);
}

//사용자 아이디를 호출하기 위해 사용
QString Client::getUserID() const {
    return text(0);
}

//사용자 이름을 호출하기 위해 사용
QString Client::getName() const {
    return text(1);
}



//사용자 전화번호를 호출하기 위해 사용
QString Client::getPhoneNumber() const {
    return text(2);
}

//사용자 주소를 호출하기 위해 사용
QString Client::getAddress() const {
    return text(3);
}

//사용자 성별을 호출하기 위해 사용
QString Client::get_Gender() const {
    return text(4);
}

//사용자 아이디를 지정하기 위해 사용
void Client::setUserID(QString &userID) {
    setText(0, userID);
}

//사용자 이름을 지정하기 위해 사용
void Client::setName(QString &name) {
    setText(1, name);
}

//사용자 전화번호를 지정하기 위해 사용
void Client::setPhoneNumber(QString &phoneNumber) {
    setText(2, phoneNumber);
}

//사용자 주소를 지정하기 위해 사용
void Client::setAddress(QString &address) {
    setText(3, address);
}

//사용자 성별을 지정하기 위해 사용
void Client::setGender(QString &gender) {
    setText(4, gender);
}
