#ifndef LOGDATA_H
#define LOGDATA_H

#include <QThread>
#include <QList>

class QTreeWidgetItem;

class LogData : public QThread
{
    Q_OBJECT
public:
    explicit LogData(QObject *parent = nullptr);    //생성자 - 로그 저장 시의 날짜 및 시간 형식 지정

private:
    void run();     //로그 자동 저장을 위한 함수

    QList<QTreeWidgetItem*> itemList;   //기록할 로그의 데이터를 저장할 List
    QString fileName;                   //파일 전송 로그 기록 시 사용할 파일명

public slots:
    /*로그 기록 저장 시 사용할 SLOT 함수*/
    void appendData(QTreeWidgetItem*);  //저장된 로그 리스트에 데이터를 추가할 경우 사용
    void saveData();                    //서버에서 저장 버튼 클릭 시 실행
};

#endif // LOGDATA_H
