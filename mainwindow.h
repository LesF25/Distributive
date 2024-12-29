#pragma once
#include "connecteth.h"

#include <QWidget>
#include <QProcess>
#include <QVector>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void btUseVPN_clicked();
    void btChooseVPNConfig_clicked();
    void btScanWifiNetworks_clicked();
    void btConnectNetwork_clicked();
    void btApply_clicked();
    void btChooseRSAKey_clicked();
    void btChooseAuthorizationType(int);
    void chboxUseDomainSSH_clicked();
    void chboxUseDomainRDP_clicked();

private:
    void _showMessageBox(const QString&);
    bool _checkSSHInfo();
    bool _checkRDPInfo();
    QString _addNetworkInfo(const QString&);

private:
    Ui::MainWindow *ui;
    QProcess* proc;
    QVector<connectETH*> vecEth;
};

enum class protocol
{
    RDP = 0,
    SSH = 1
};

