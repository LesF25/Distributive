#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QNetworkInterface>
#include <QHBoxLayout>
#include <QSettings>
#include <QVariant>
#include "ip_validator.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int i = 0;
    for(const QNetworkInterface& eth : QNetworkInterface::allInterfaces())
    {
        QString interfaceName = eth.humanReadableName();
        if (interfaceName == "lo" || interfaceName.at(0) == 'w')
            continue;

        connectETH* widgetEth = new connectETH(this);
        widgetEth->setEthName(interfaceName);

        widgetEth->setLayout(new QVBoxLayout());
        ui->tabConnectionType->addTab(widgetEth, interfaceName);
        vecEth.push_back(widgetEth);
        ++i;
    }

    IPValidator* ip_validator = new IPValidator(this);
    ui->_edIPRDP->setValidator(ip_validator);
    ui->_edIPSSH->setValidator(ip_validator);

    proc = new QProcess(this);
    connect(ui->_btCancel, &QPushButton::clicked, QApplication::instance(), &QCoreApplication::quit);
    connect(ui->_btChooseVPNConfig, &QPushButton::clicked, this, &MainWindow::btChooseVPNConfig_clicked);
    connect(ui->_btUseVPN, &QCheckBox::clicked, this, &MainWindow::btUseVPN_clicked);
    connect(ui->cboxAuthorization, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::btChooseAuthorizationType);
    connect(ui->btChoiceRSAKey, &QPushButton::clicked, this, &MainWindow::btChooseRSAKey_clicked);
    connect(ui->_btScanWifiNetworks, &QPushButton::clicked, this, &MainWindow::btScanWifiNetworks_clicked);
    connect(ui->_btConnectNetwork, &QPushButton::clicked, this, &MainWindow::btConnectNetwork_clicked);
    connect(ui->_btApply, &QPushButton::clicked, this, &MainWindow::btApply_clicked);
    connect(ui->chboxUseDomainRDP, &QCheckBox::clicked, this, &MainWindow::chboxUseDomainRDP_clicked);
    connect(ui->chboxUseDomainSSH, &QCheckBox::clicked, this, &MainWindow::chboxUseDomainSSH_clicked);
}

void MainWindow::chboxUseDomainRDP_clicked()
{
    if (ui->chboxUseDomainRDP->isChecked())
    {
        ui->_edDomainRDP->setEnabled(true);
        ui->_edIPRDP->setEnabled(false);

        return;
    }

    ui->_edDomainRDP->setEnabled(false);
    ui->_edIPRDP->setEnabled(true);

    return;
}


void MainWindow::chboxUseDomainSSH_clicked()
{
    if (ui->chboxUseDomainSSH->isChecked())
    {
        ui->_edDomainSSH->setEnabled(true);
        ui->_edIPSSH->setEnabled(false);

        return;
    }

    ui->_edDomainSSH->setEnabled(false);
    ui->_edIPSSH->setEnabled(true);

    return;
}


MainWindow::~MainWindow()
{
    delete ui;
    if (proc)
    {
        proc->close();
        proc->waitForFinished();
    }
}


void MainWindow::btChooseAuthorizationType(int index)
{
    if (index == 1)
    {
        ui->btChoiceRSAKey->setEnabled(true);
        ui->_edPathRSAKey->setEnabled(true);

        return;
    }

    ui->btChoiceRSAKey->setEnabled(false);
    ui->_edPathRSAKey->setEnabled(false);
    return;
}


void MainWindow::btChooseVPNConfig_clicked()
{
    QString path = QFileDialog::getOpenFileName(0, QObject::tr("Укажите файл VPN"), QDir::homePath(), QObject::tr("Файл VPN (*.ovpn);;Все файлы (*.*)"));
    ui->_edPathVPNConfig->setText(path);
}


void MainWindow::btChooseRSAKey_clicked()
{
    QString path = QFileDialog::getOpenFileName(0, QObject::tr("Выберите RSA-ключ"), QDir::homePath(), QObject::tr("RSA-ключ (*.rsa);;Все файлы (*.*)"));
    ui->_edPathRSAKey->setText(path);
}


void MainWindow::btUseVPN_clicked()
{
    if (ui->_btUseVPN->isChecked())
    {
        ui->_btUseVPN->setText("Выкл");
        ui->_btChooseVPNConfig->setEnabled(true);
        ui->_edPathVPNConfig->setEnabled(true);

        return;
    }

    ui->_btUseVPN->setText("Вкл");
    ui->_btChooseVPNConfig->setEnabled(false);
    ui->_edPathVPNConfig->setEnabled(false);

    return;
}


void MainWindow::btScanWifiNetworks_clicked()
{
    ui->_tableWifiNetworks->clearContents();
    ui->_tableWifiNetworks->model()->removeRows(0, ui->_tableWifiNetworks->rowCount());

    QStringList arguments;
    QString command = QString("nmcli -m tabular -t -f SSID,SECURITY dev wifi list");

    arguments << "-c" << command;
    proc->start("/bin/sh", arguments);
    proc->waitForFinished();

    QString output = '\n' + proc->readAllStandardOutput();

    int it = 0, countWiFiNetworks = 0;
    while (true)
    {
        it = output.indexOf("\n");
        if (it == output.length() - 1 || it == -1)
            break;
        ++it;

        QString name_connect;
        while(output[it] != ':')
        {
            name_connect += output[it];
            ++it;
        }
        ++it;

        bool secure_connect = false;
        if (output[it] == 'W')
            secure_connect = true;

        ui->_tableWifiNetworks->insertRow(countWiFiNetworks);
        ui->_tableWifiNetworks->setItem(countWiFiNetworks, 0, new QTableWidgetItem(name_connect));

        if (secure_connect)
            ui->_tableWifiNetworks->setItem(countWiFiNetworks, 1, new QTableWidgetItem("Secure"));
        else
            ui->_tableWifiNetworks->setItem(countWiFiNetworks, 1, new QTableWidgetItem("Not secure"));

        ui->_tableWifiNetworks->item(countWiFiNetworks, 0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui->_tableWifiNetworks->item(countWiFiNetworks, 1)->setFlags(Qt::ItemIsEnabled);

        ++countWiFiNetworks;
        output.remove(0, it);
    }
}


// todo: refactor
void MainWindow::btConnectNetwork_clicked()
{
    QStringList arguments;
    QString command;

    QString tabName = ui->tabConnectionType->tabText(ui->tabConnectionType->currentIndex());
    if (tabName == "Wi-Fi")
    {
        if (!ui->_tableWifiNetworks->currentItem() || !ui->_tableWifiNetworks->currentItem()->isSelected() || ui->_tableWifiNetworks->currentColumn() == 1)
        {
            this->_showMessageBox("Не выбрано Wi-Fi соединение.");
            return;
        }

        int col = ui->_tableWifiNetworks->currentColumn();
        int row = ui->_tableWifiNetworks->currentRow();

        command = QString("nmcli -f CONNECTION,TYPE dev status | grep wifi");
        arguments << "-c" << command;
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();

        QString output = "\n" + proc->readAllStandardOutput();
        int it = 0;
        while (true)
        {
            it = output.indexOf("\n");
            if (it == output.length() - 1 || it == -1)
                break;
            ++it;

            QString name_connect;
            while(output[it] != ' ')
            {
                name_connect += output[it];
                ++it;
            }

            if (name_connect == ui->_tableWifiNetworks->item(row, col)->text())
            {
                ui->_tableWifiNetworks->clearSelection();
                return;
            }

            arguments.clear();
            command = QString("nmcli con down %1").arg(name_connect);
            arguments << "-c" << command;
            proc->start("/bin/sh", arguments);
            proc->waitForFinished();

            output.remove(0, it);
        }

        arguments.clear();
        command = QString("nmcli dev wifi connect %1").arg(ui->_tableWifiNetworks->item(row, col)->text());
        arguments << "-c" << command;
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();

        ui->_tableWifiNetworks->clearSelection();

        return;
    }

//  Ethernet
    QString dirEtcNetworkInterfaces = "/etc/network/interfaces";
    QString command_params = QString(
                "auto lo\n"
                "iface lo inet loopback\n\n");

    command = QString("echo -n > %1;").arg(dirEtcNetworkInterfaces);
    QString ifup_comand = "ifup lo;";

    for (int i = 0; i < vecEth.size(); ++i)
        if (vecEth.at(i)->getBtTurn()->isChecked())
        {
            if(vecEth.at(i)->getRDBAuto()->isChecked())
            {
                command_params += QString(
                            "auto %1\n"
                            "iface %1 inet dhcp\n\n"
                            ).arg(vecEth.at(i)->getEthName());

                ifup_comand += QString("ifup %1;").arg(vecEth.at(i)->getEthName());
            }
            else if(vecEth.at(i)->getRDBManually()->isChecked())
            {
                if (vecEth.at(i)->getGateway()->text().isEmpty() || vecEth.at(i)->getIpAddress()->text().isEmpty() || vecEth.at(i)->getSubnetMask()->text().isEmpty())
                {
                    this->_showMessageBox("Все поля должны быть заполнены.");
                    return;
                }

                command_params += QString(
                            "auto %1\n"
                            "iface %1 inet static\n"
                            "address %2\n"
                            "gateway %3\n"
                            "netmask %4\n\n").arg(
                            vecEth.at(i)->getEthName(),
                            vecEth.at(i)->getIpAddress()->text(),
                            vecEth.at(i)->getGateway()->text(),
                            vecEth.at(i)->getSubnetMask()->text());

                ifup_comand += QString("ifup %1;").arg(vecEth.at(i)->getEthName());
            }
        }
    command += QString("echo \"" + command_params + "\" >> %1;" + ifup_comand).arg(dirEtcNetworkInterfaces);

    arguments << "-c" << command;
    proc->start("/bin/sh", arguments);
    proc->waitForFinished();
}

// todo: fix creation custom config. Also fix situation when we write exists hosts in ssh and custom configs.
void MainWindow::btApply_clicked()
{
    QStringList arguments;
    QString write_config_command, config_params, path_config;
    QSettings general_config("~/.config/distributive/remote-hosts.conf", QSettings::IniFormat);

    QString tabName = ui->tabProtocolForConnection->tabText(ui->tabProtocolForConnection->currentIndex());
    if (tabName == "RDP")
    {
        if (!this->_checkRDPInfo())
            return;

        if (!ui->_edUsernameRDP->text().isEmpty())
            config_params += QString("username:s:%1:\n").arg(
                        ui->_edUsernameRDP->text());

        if (!ui->_edPasswordRDP->text().isEmpty())
            config_params += QString("password:s:%1\n").arg(
                        ui->_edPasswordRDP->text());

        config_params += ui->chboxUseDomainRDP->isChecked() ?
            QString("full address:s:%1:%2\n").arg(
                        ui->_edDomainRDP->text(), ui->_edPortRDP->text()) :
            QString("full address:s:%1:%2\n").arg(
                        ui->_edIPRDP->text(), ui->_edPortRDP->text());

        path_config = QString("~/.config/distributive/rdp/%1.rdp").arg(ui->_edServerNameRDP->text());

        general_config.beginGroup(ui->_edServerNameRDP->text());
        general_config.setValue("protocol", tabName);
        general_config.setValue("config", ui->_edServerNameRDP->text() + QString(".rdp"));
        if (ui->_btUseVPN->isChecked())
            general_config.setValue("vpn_settings", ui->_edPathVPNConfig->text());
        general_config.endGroup();

        write_config_command = QString("echo -n > %1;").arg(path_config);
    }
    else if (tabName == "SSH")
    {
        if (!this->_checkSSHInfo())
            return;

        config_params += ui->chboxUseDomainSSH->isChecked() ?
                    QString("Host %1\n").arg(ui->_edDomainSSH->text()) :
                    QString("Host %1\n").arg(ui->_edIPSSH->text());

        config_params += QString(
                    "    User %1\n"
                    "    Port %2\n"
                    ).arg(ui->_edUsernameSSH->text(),ui->_edPortSSH->text());

        config_params += ui->cboxAuthorization->currentText() == "Password" ?
                    QString("    PasswordAuthentication yes\n"):
                    QString("    PasswordAuthentication no\n"
                            "    IdentityFile %1\n").arg(ui->_edPathRSAKey->text());

        path_config =QString("~/.ssh/config");

        general_config.beginGroup(ui->_edServerNameSSH->text());
        general_config.setValue("protocol", tabName);
        ui->chboxUseDomainSSH->isChecked() ?
                            general_config.setValue("url", QString("Host %1\n").arg(ui->_edDomainSSH->text())) :
                            general_config.setValue("url", QString("Host %1\n").arg(ui->_edIPSSH->text()));
        if (ui->_btUseVPN->isChecked())
            general_config.setValue("vpn_settings", ui->_edPathVPNConfig->text());
        general_config.endGroup();
    }

    write_config_command += QString("echo \"" + config_params + "\" >> %1").arg(path_config);

    arguments << "-c" << write_config_command;

    proc->start("/bin/sh", arguments);
    proc->waitForFinished();
}


bool MainWindow::_checkRDPInfo()
{
    if (!ui->_edPathVPNConfig->text().isEmpty()) {
        this->_showMessageBox("Укажите файл с настройками VPN.");
        return false;
    }

    if (ui->chboxUseDomainRDP->isChecked() && ui->_edDomainRDP->text().isEmpty()){
        this->_showMessageBox("Заполните доменное имя.");
        return false;
    }

    if (!ui->chboxUseDomainRDP->isChecked() && ui->_edIPRDP->text().isEmpty()){
        this->_showMessageBox("Заполните IP-адрес.");
        return false;
    }

    if (ui->_edPortRDP->text().isEmpty()){
        this->_showMessageBox("Выберите порт.");
        return false;
    }

    return true;
}


bool MainWindow::_checkSSHInfo()
{
    if (ui->_edUsernameSSH->text().isEmpty()){
        this->_showMessageBox("Заполните имя пользователя.");
        return false;
    }

    if (ui->cboxAuthorization->currentText() == "RSA-key" && ui->_edPathRSAKey->text().isEmpty()){
        this->_showMessageBox("Укажите расположение RSA-ключа.");
        return false;
    }

    if (!ui->_edPathVPNConfig->text().isEmpty()) {
        this->_showMessageBox("Укажите файл с настройками VPN.");
        return false;
    }

    if (ui->chboxUseDomainSSH->isChecked() && ui->_edDomainSSH->text().isEmpty()){
        this->_showMessageBox("Заполните доменное имя.");
        return false;
    }

    if (!ui->chboxUseDomainSSH->isChecked() && ui->_edIPSSH->text().isEmpty()){
        this->_showMessageBox("Заполните IP-адрес.");
        return false;
    }

    if (ui->_edPortSSH->text().isEmpty()){
        this->_showMessageBox("Выберите порт.");
        return false;
    }

    return true;
}


void MainWindow::_showMessageBox(const QString& text){
    QMessageBox::warning(this, "Error", text);
}
