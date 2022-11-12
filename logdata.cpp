#include "logdata.h"
#include <QTreeWidgetItem>
#include <QFile>
#include <QDateTime>

//생성자 - 로그 저장 시의 날짜 및 시간 형식 지정
LogData::LogData(QObject *parent)
    : QThread{parent}
{
    QString format = "yyyyMMdd_hhmmss";
    //서버의 로그를 저장할 파일명을 지정
    fileName = QString("log_%1.txt").arg(QDateTime::currentDateTime().toString(format));
    //클라이언트의 메시지 전송 시간을 지정
    saveTime = QString("%1").arg(QDateTime::currentDateTime().toString(format));
}

//로그 자동 저장을 위한 함수
void LogData::run()
{
    Q_FOREVER {
        saveData();
        sleep(60);      // 1분마다 저장
    }
}

//저장된 로그 리스트에 데이터를 추가할 경우 사용
void LogData::appendData(QTreeWidgetItem* item)
{
    itemList.append(item);
}

//클라이언트가 메시지 전송 시 실행
void LogData::saveClientData(QString name, QString chat)
{
    QFile file(name + ".txt");
    //QIODevice::Append - 새로 입력할 내용을 현재 데이터 다음에 추가한다.
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);
\
    //전송한 메시지의 내용과 시간을 저장한다.
    out << chat << " / " << saveTime << "\n";

    file.close();
}

//서버에서 저장 버튼 클릭 시 실행
void LogData::saveData()
{
    if(itemList.count() > 0) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        /*객체의 내용을 ,를 구분자로 하여 저장한다.*/
        foreach(auto item, itemList) {
            out << item->text(0) << ", ";
            out << item->text(1) << ", ";
            out << item->text(2) << ", ";
            out << item->text(3) << ", ";
            out << item->text(4) << ", ";
            out << item->text(5) << "\n";
        }
        file.close();
    }
}
