#include "client.h"

Client::Client(int userNumber, QString userID, QString name, QString phoneNumber, QString address, QString gender)
{
    setText(0, QString::number(userNumber));
    setText(1, userID);
    setText(2, name);
    setText(3, phoneNumber);
    setText(4, address);
    setText(5, gender);
}

//사용자 수를 호출하기 위해 사용
int Client::userNumber() const {
    return text(0).toInt();
}

//사용자 아이디를 호출하기 위해 사용
QString Client::getUserID() const {
    return text(1);
}

//사용자 이름을 호출하기 위해 사용
QString Client::getName() const {
    return text(2);
}

//사용자 전화번호를 호출하기 위해 사용
QString Client::getPhoneNumber() const {
    return text(3);
}

//사용자 주소를 호출하기 위해 사용
QString Client::getAddress() const {
    return text(4);
}

//사용자 성별을 호출하기 위해 사용
QString Client::get_Gender() const {
    return text(5);
}

//사용자 아이디를 지정하기 위해 사용
void Client::setUserID(QString &userID) {
    setText(1, userID);
}

//사용자 이름을 지정하기 위해 사용
void Client::setName(QString &name) {
    setText(2, name);
}

//사용자 전화번호를 지정하기 위해 사용
void Client::setPhoneNumber(QString &phoneNumber) {
    setText(3, phoneNumber);
}

//사용자 주소를 지정하기 위해 사용
void Client::setAddress(QString &address) {
    setText(4, address);
}

//사용자 성별을 지정하기 위해 사용
void Client::setGender(QString &gender) {
    setText(5, gender);
}
