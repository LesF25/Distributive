#include "connecteth.h"
#include "ui_connecteth.h"
#include "ip_validator.h"

connectETH::connectETH(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connectETH)
{
    ui->setupUi(this);

    IPValidator *ip_validator = new IPValidator(this);
    ui->_edSubnetMaskETH->setValidator(ip_validator);
    ui->_edIpAddressETH->setValidator(ip_validator);
    ui->_edGatewayETH->setValidator(ip_validator);

    connect(ui->_btUseETH, &QPushButton::clicked, this, &connectETH::btUseETH_clicked);
    connect(ui->rdbAuto, &QRadioButton::clicked, this, &connectETH::rdbAuto_clicked);
    connect(ui->rdbManually, &QRadioButton::clicked, this, &connectETH::rdbManually_clicked);
}

connectETH::~connectETH()
{
    delete ui;
}


void connectETH::btUseETH_clicked()
{
    if(ui->_btUseETH->isChecked())
    {
        ui->_btUseETH->setText("Выкл");
        if (ui->rdbManually->isChecked())
        {
            ui->_lbGatewayETH->setEnabled(true);
            ui->_lbIpAddrETH->setEnabled(true);
            ui->_lbSubnetMaskETH->setEnabled(true);

            ui->_edGatewayETH->setEnabled(true);
            ui->_edIpAddressETH->setEnabled(true);
            ui->_edSubnetMaskETH->setEnabled(true);
        }

        ui->rdbAuto->setEnabled(true);
        ui->rdbManually->setEnabled(true);
        ui->_lbSettingsConnect->setEnabled(true);
    }
    else
    {
        ui->_btUseETH->setText("Вкл");
        if (ui->rdbManually->isChecked())
        {
            ui->_lbGatewayETH->setEnabled(false);
            ui->_lbIpAddrETH->setEnabled(false);
            ui->_lbSubnetMaskETH->setEnabled(false);

            ui->_edGatewayETH->setEnabled(false);
            ui->_edIpAddressETH->setEnabled(false);
            ui->_edSubnetMaskETH->setEnabled(false);
        }

        ui->rdbAuto->setEnabled(false);
        ui->rdbManually->setEnabled(false);
        ui->_lbSettingsConnect->setEnabled(false);
    }
}


void connectETH::rdbAuto_clicked()
{
    if (!ui->rdbAuto->isChecked())
        return;

    ui->_lbGatewayETH->setEnabled(false);
    ui->_lbIpAddrETH->setEnabled(false);
    ui->_lbSubnetMaskETH->setEnabled(false);

    ui->_edGatewayETH->setEnabled(false);
    ui->_edIpAddressETH->setEnabled(false);
    ui->_edSubnetMaskETH->setEnabled(false);
}


void connectETH::rdbManually_clicked()
{
    if (!ui->rdbManually->isChecked())
        return;

    ui->_lbGatewayETH->setEnabled(true);
    ui->_lbIpAddrETH->setEnabled(true);
    ui->_lbSubnetMaskETH->setEnabled(true);

    ui->_edGatewayETH->setEnabled(true);
    ui->_edIpAddressETH->setEnabled(true);
    ui->_edSubnetMaskETH->setEnabled(true);
}



QPushButton* connectETH::getBtTurn() {
    return ui->_btUseETH;
}


QRadioButton* connectETH::getRDBAuto() {
    return ui->rdbAuto;
}

QRadioButton* connectETH::getRDBManually() {
    return ui->rdbManually;
}

void connectETH::setEthName(QString name) {
    this->ethName = name;
}

QString connectETH::getEthName() {
    return this->ethName;
}

QLineEdit* connectETH::getIpAddress() {
    return ui->_edIpAddressETH;
}

QLineEdit* connectETH::getGateway(){
    return ui->_edGatewayETH;
}

QLineEdit* connectETH::getSubnetMask(){
    return ui->_edSubnetMaskETH;
}







