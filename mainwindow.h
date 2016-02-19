#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QCloseEvent>
#include <serverthread.h>
#include <clientthread.h>
#include <consultantthread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QString getTime();
    ~MainWindow();

private slots:
    void closeEvent(QCloseEvent*);
    void on_startserverButton_clicked();
    void on_bokconnectButton_clicked();
    void on_serverconnectButton_clicked();
    void on_stopserverButton_clicked();
    void on_bokdisconnectButton_clicked();
    void on_serverdisconnectButton_clicked();
    void on_consultantrefreshButton_clicked();
    void on_consultantchooseButton_clicked();
    void on_consultantsendButton_clicked();
    void on_clientsendButton_clicked();

    void startServer();
    void stopServer();
    void stopServer(QString);
    void serverStarted();
    void bokConnect();
    void bokDisconnect();
    void conConnect();
    void conDisconnect();
    void refreshStatus();
    void refreshStatus(int, int);
    void newAction(QString);
    void bokMessage(QString);
    void bokSysMessage(QString);
    void bokError();
    void bokSuccess(QString);
    void bokStart();
    void conMessage(QString);
    void conError();
    void conSuccess(QString);
    void conRefresh(QStringList);
    void conClient(QString);
    void conTalkClose();
    void conTalkStart();

signals:
    void serverStatus(int, int);

protected:
    QMessageBox messageBox;
    Ui::MainWindow *ui;
    ServerThread *server;
    ClientThread *client;
    ConsultantThread *consultant;
    int clients = 0, consultants = 0;
};

#endif // MAINWINDOW_H
