#include "clientmanager.h"
#include "ui_clientmanager.h"
#include "client.h"
#include <QMessageBox>
#include <QFile>

ClientManager::ClientManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManager)
{
    ui->setupUi(this);

    QFile file("clientlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int count = row[0].toInt();
            Client* c = new Client(count, row[1], row[2], row[3], row[4], row[5]);
            clientList.insert(count, c);
        }
    }

    file.close( );

    //ui->treeWidget->hide();
}

ClientManager::~ClientManager()
{
    delete ui;

    QFile file("clientlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : clientList) {
        Client* c = v;
        out << c->userCount() << ", " << c->getUserID() << ", ";
        out << c->getName() << ", ";
        out << c->getPhoneNumber() << ", ";
        out << c->getAddress() << ", ";
        out << c->get_Gender() << "\n";
    }
    file.close( );
}

int ClientManager::userCount() {
    if(clientList.size() == 0) return 1;
    else {
        auto cnt = clientList.lastKey();
        return ++cnt;
    }
}

//등록 취소
void ClientManager::on_pushButton_2_clicked()
{
    ui->checkBox->setChecked(false);
    ui->checkBox_2->setChecked(false);
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    emit cancellation();
}

//회원 등록
void ClientManager::on_pushButton_clicked()
{
    if(ui->lineEdit->text().trimmed() == "") {
        QMessageBox::warning(this, tr("가입 실패"), tr("아이디를 입력하여 주세요."));
    }
    else if(ui->lineEdit_2->text().trimmed() == "") {
        QMessageBox::warning(this, tr("가입 실패"), tr("성함을 입력하여 주세요."));
    }
    else if(ui->lineEdit_4->text().trimmed() == "") {
        QMessageBox::warning(this, tr("가입 실패"), tr("주소를 입력하여 주세요."));
    }
    else if(ui->checkBox->isChecked() == false || ui->checkBox_2->isChecked() == false) {
        QMessageBox::warning(this, tr("가입 실패"), tr("정보 수집을 동의하여 주세요"));
    }
    else {
        QString userId, name, call, address, gender;
        int ucnt = userCount();
        userId = ui->lineEdit->text();
        name = ui->lineEdit_2->text();
        call = ui->lineEdit_3->text();
        address = ui->lineEdit_4->text();
        gender = ui->comboBox->currentText();

        if(userId.length()) {
            Client *c = new Client(ucnt, userId, name, call, address, gender);
            clientList.insert(ucnt, c);
        }
        qDebug() << "회원 수: " << ucnt;
        qDebug() << "아이디: " << userId;
        qDebug() << "이름: " << name;
        qDebug() << "전화번호: " << call;
        qDebug() << "주소: " << address;
        qDebug() << "성별: " << gender;

        ui->checkBox->setChecked(false);
        ui->checkBox_2->setChecked(false);
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        emit join();
    }
    //errorMessage->exec();
}

//회원 정보를 담아서 보내기
void ClientManager::containClientInfo() {
    QString userId, name, call, address, gender;
    int ucnt;

    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);
        ucnt = c->userCount();
        userId = c->getUserID();
        name = c->getName();
        call = c->getPhoneNumber();
        address = c->getAddress();
        gender = c->get_Gender();
        Client *item = new Client(ucnt, userId, name, call, address, gender);
        emit sendClientInfo(item);
    }
}

//로그인 시도 시 아이디가 등록되어 있는지 체크
void ClientManager::checkLoginId(QString str) {
    QString userId = "";
    Q_FOREACH(auto v, clientList) {
        Client *c = static_cast<Client*>(v);
        if(str == c->getUserID()) {
            userId = c->getUserID();
            emit successLogin();
            break;
        }
    }

    if(userId != str) emit failedLogin();
}
