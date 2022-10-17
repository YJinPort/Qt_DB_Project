#ifndef SHOPPING_H
#define SHOPPING_H

#include <QTreeWidgetItem>

class Shopping : public QTreeWidgetItem
{
public:
    explicit Shopping(int = 0, QString = "", int = 0, int = 0, QString = "", QString = "");

    QString getProductName() const;
    int getProductPrice() const;
    int getProductCount() const;
    QString getProductType() const;
    QString getClientAddress() const;

    void setProductName(QString&);
    void setProductPrice(int&);
    void setProductCount(int&);
    void setProductType(QString&);
    void setClientAddress(QString&);

    int shoppingCount() const;
};

#endif // SHOPPING_H
