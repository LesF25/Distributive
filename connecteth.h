#pragma once
#include <QWidget>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class connectETH;
}
QT_END_NAMESPACE


class connectETH : public QWidget
{
    Q_OBJECT

public:
    explicit connectETH(QWidget *parent = nullptr);
    ~connectETH();

    void setEthName(QString);
    QString getEthName();
    QPushButton* getBtTurn();
    QRadioButton* getRDBAuto();
    QRadioButton* getRDBManually();
    QLineEdit* getIpAddress();
    QLineEdit* getGateway();
    QLineEdit* getSubnetMask();

public slots:
    void rdbAuto_clicked();
    void rdbManually_clicked();
    void btUseETH_clicked();

private:
    Ui::connectETH *ui;
    QString ethName;
};
