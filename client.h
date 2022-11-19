#ifndef CLIENT_H
#define CLIENT_H

#include <QTreeWidgetItem>

class Client : public QTreeWidgetItem
{
public:
    /*생성자의 인자값을 선언과 동시에 초기화*/
    explicit Client(int = 0, QString = "", QString = "", QString = "", QString = "", QString = "");

    /*사용자 정보를 호출하기 위해 사용되는 함수*/
    QString getUserID() const;          //사용자 아이디를 호출하기 위해 사용
    QString getName() const;            //사용자 이름을 호출하기 위해 사용
    QString getPhoneNumber() const;     //사용자 전화번호를 호출하기 위해 사용
    QString getAddress() const;         //사용자 주소를 호출하기 위해 사용
    QString get_Gender() const;         //사용자 성별을 호출하기 위해 사용

    /*사용자 정보를 지정하기 위해 사용되는 함수*/
    void setUserID(QString&);       //사용자 아이디를 지정하기 위해 사용
    void setName(QString&);         //사용자 이름을 지정하기 위해 사용
    void setPhoneNumber(QString&);  //사용자 전화번호를 지정하기 위해 사용
    void setAddress(QString&);      //사용자 주소를 지정하기 위해 사용
    void setGender(QString&);       //사용자 성별을 지정하기 위해 사용

    /*사용자 수를 호출하기 위해 사용되는 함수*/
    int userNumber() const;          //사용자 번호를 호출하기 위해 사용
};

#endif // CLIENT_H
