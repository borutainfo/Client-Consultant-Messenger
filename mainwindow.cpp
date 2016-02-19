#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QtCore>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete consultant;
    delete client;
    delete server;
    delete ui;
    QCoreApplication::quit();
}

void MainWindow::closeEvent (QCloseEvent *event) {
    if (QMessageBox::question(this, "Zamknięcie aplikacji", "Jesteś pewien, że chcesz wyłączyć aplikację?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        delete this;
        event->accept();
    }
    else {
        event->ignore();
    }
}

QString MainWindow::getTime() {
    return QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss");
}


/*
 * Kod odpowiedzialny za aplikację serwera
*/

void MainWindow::startServer() {
    server = new ServerThread;
    connect(server, SIGNAL(serverStatus(int, int)), SLOT(refreshStatus(int, int)));
    connect(server, SIGNAL(serverAction(QString)), SLOT(newAction(QString)));
    connect(server, SIGNAL(serverError(QString)), SLOT(stopServer(QString)));
    connect(server, SIGNAL(serverStarted()), SLOT(serverStarted()));
    server->start();
}

void MainWindow::stopServer(QString info) {
    ui->serveractionsTextBox->insertHtml("<font style=\"color: grey\">"+getTime()+":</font><font style=\"color: black\"> "+info+"</font><br>");
    ui->serveractionsTextBox->moveCursor(QTextCursor::End);
    stopServer();
}

void MainWindow::stopServer() {
    server->stop();
    ui->tabWidget->insertTab(1, ui->clientTab, "Klient");
    ui->tabWidget->insertTab(2, ui->consultantTab, "Konsultant");
    ui->startserverButton->setEnabled(1);
    ui->stopserverButton->setEnabled(0);
    ui->serverstatusTextBox->clear();
    ui->serverstatusTextBox->insertHtml("<font style=\"color: black\">Serwer jest obecnie wyłączony.</font>");
}

void MainWindow::serverStarted() {
    refreshStatus();
    ui->serveractionsTextBox->insertHtml("<font style=\"color: grey\">"+getTime()+":</font><font style=\"color: black\"> Serwer został poprawnie uruchomiony.</font><br>");
    ui->serveractionsTextBox->moveCursor(QTextCursor::End);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(1);
    ui->startserverButton->setEnabled(0);
    ui->stopserverButton->setEnabled(1);
}

void MainWindow::refreshStatus() {
    ui->serverstatusTextBox->clear();
    ui->serverstatusTextBox->insertHtml("<font style=\"color: black\">Serwer jest aktywny. Oczekujących klientów: <b>"+QString::number(clients)+"</b>, dostępnych konsultantów: <b>"+QString::number(consultants)+"</b>.</font>");
}

void MainWindow::refreshStatus(int clients, int consultants) {
    this->clients = clients;
    this->consultants = consultants;
    refreshStatus();
}

void MainWindow::newAction(QString info) {
    ui->serveractionsTextBox->insertHtml("<font style=\"color: grey\">"+getTime()+":</font><font style=\"color: black\"> "+info+"</font><br>");
    ui->serveractionsTextBox->moveCursor(QTextCursor::End);
}

void MainWindow::on_startserverButton_clicked()
{
    startServer();
}

void MainWindow::on_stopserverButton_clicked()
{
    stopServer();
    ui->serveractionsTextBox->insertHtml("<font style=\"color: grey\">"+getTime()+":</font><font style=\"color: black\"> Praca serwera została zakończona.</font><br>");
    ui->serveractionsTextBox->moveCursor(QTextCursor::End);
}


/*
 * Kod odpowiedzialny za aplikację klienta
*/

void MainWindow::bokConnect() {
    if(ui->clientnameEdit->text() == "") {
        ui->clientmessageTextEdit->insertHtml("<b style=\"color: red\">System:</b><font style=\"color: black\"> Wpisz swoje imię i / lub nazwisko!</font><br>");
    }
    else {
        ui->tabWidget->removeTab(0);
        ui->tabWidget->removeTab(1);
        ui->bokconnectButton->setEnabled(0);
        ui->clientproblemBox->setEnabled(0);
        ui->clientnameEdit->setEnabled(0);
        ui->clientServAddrEdit->setEnabled(0);
        ui->clientServPortEdit->setEnabled(0);
        ui->bokdisconnectButton->setEnabled(1);
        ui->bokdisconnectButton->setText("Przerwij");
        ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Trwa nawiązywanie połączenia z serwerem...</font><br>");
        ui->clientmessageTextEdit->moveCursor(QTextCursor::End);

        client = new ClientThread(ui->clientnameEdit->text(), ui->clientServAddrEdit->text(), ui->clientServPortEdit->text());
        connect(client, SIGNAL(newSysMessage(QString)), SLOT(bokSysMessage(QString)));
        connect(client, SIGNAL(newMessage(QString)), SLOT(bokMessage(QString)));
        connect(client, SIGNAL(connectError()), SLOT(bokError()));
        connect(client, SIGNAL(connectSuccess(QString)), SLOT(bokSuccess(QString)));
        connect(client, SIGNAL(exitTalk()), SLOT(bokDisconnect()));
        connect(client, SIGNAL(startTalk()), SLOT(bokStart()));
        client->start();
    }
}

void MainWindow::bokDisconnect() {
    client->stop();
    ui->tabWidget->insertTab(0, ui->serverTab, "Serwer");
    ui->tabWidget->insertTab(2, ui->consultantTab, "Konsultant");
    ui->bokconnectButton->setEnabled(1);
    ui->bokdisconnectButton->setText("Rozłacz");
    ui->bokdisconnectButton->setEnabled(0);
    ui->clientServAddrEdit->setEnabled(1);
    ui->clientServPortEdit->setEnabled(1);
    ui->clientnameEdit->setEnabled(1);
    ui->clientproblemBox->setEnabled(1);
    ui->clientsendButton->setEnabled(0);
    ui->clientTextEdit->setEnabled(0);
    ui->clientTextEdit->clear();
}

void MainWindow::bokMessage(QString message) {
    ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">Konsultant:</b><font style=\"color: black\"> "+message+"</font><br>");
    ui->clientmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::bokSysMessage(QString message) {
    ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> "+message+"</font><br>");
    ui->clientmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::bokStart() {
    ui->clientsendButton->setEnabled(1);
    ui->clientTextEdit->setEnabled(1);
}

void MainWindow::bokError() {
    bokDisconnect();
    ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Nie można nawiązać połączenia z serwerem.</font><br>");
    ui->clientmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::bokSuccess(QString id) {
    ui->bokdisconnectButton->setEnabled(1);
    ui->bokdisconnectButton->setText("Rozłacz");
    ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Połączono (id "+id+"). Czekaj na przydział konsultanta.</font><br>");
    ui->clientmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::on_bokconnectButton_clicked() {
    bokConnect();
}

void MainWindow::on_bokdisconnectButton_clicked() {
    bokDisconnect();
    ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Połączenie zostało zakończone.</font><br>");
    ui->clientmessageTextEdit->moveCursor(QTextCursor::End);
}


void MainWindow::on_clientsendButton_clicked(){
    client->send(ui->clientTextEdit->text());
    ui->clientmessageTextEdit->insertHtml("<b style=\"color: grey\">Ty:</b><font style=\"color: black\"> "+ui->clientTextEdit->text()+"</font><br>");
    ui->clientmessageTextEdit->moveCursor(QTextCursor::End);
    ui->clientTextEdit->clear();
}

/*
 * Kod odpowiedzialny za aplikację konsultanta
*/

void MainWindow::conConnect() {
    if(ui->consultantnameEdit->text() == "") {
        ui->consultantmessageTextEdit->insertHtml("<b style=\"color: red\">System:</b><font style=\"color: black\"> Wpisz swoje imię!</font><br>");
        ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
    }
    else {
        ui->tabWidget->removeTab(0);
        ui->tabWidget->removeTab(0);
        ui->serverdisconnectButton->setEnabled(1);
        ui->serverconnectButton->setEnabled(0);
        ui->consultantServAddrEdit->setEnabled(0);
        ui->consultantServPortEdit->setEnabled(0);
        ui->serverdisconnectButton->setText("Przerwij");
        ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Trwa nawiązywanie połączenia z serwerem...</font><br>");
        ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);

        consultant = new ConsultantThread(ui->consultantnameEdit->text(), ui->consultantServAddrEdit->text(), ui->consultantServPortEdit->text());
        connect(consultant, SIGNAL(newMessage(QString)), SLOT(conMessage(QString)));
        connect(consultant, SIGNAL(connectError()), SLOT(conError()));
        connect(consultant, SIGNAL(connectSuccess(QString)), SLOT(conSuccess(QString)));
        connect(consultant, SIGNAL(newList(QString)), SLOT(conMessage(QString)));
        connect(consultant, SIGNAL(clientChoose(QString)), SLOT(conClient(QString)));
        connect(consultant, SIGNAL(clientClose()), SLOT(conTalkClose()));
        connect(consultant, SIGNAL(refreshClients(QStringList)), SLOT(conRefresh(QStringList)));
        consultant->start();
    }
}

void MainWindow::conDisconnect() {
    consultant->stop();
    ui->tabWidget->insertTab(0, ui->serverTab, "Serwer");
    ui->tabWidget->insertTab(1, ui->clientTab, "Klient");
    ui->serverdisconnectButton->setText("Rozłacz");
    ui->serverconnectButton->setEnabled(1);
    ui->consultantchooseButton->setText("Wybierz");
    ui->serverdisconnectButton->setEnabled(0);
    ui->consultantnameEdit->setEnabled(1);
    ui->consultantclientchooseBox->setEnabled(0);
    ui->consultantrefreshButton->setEnabled(0);
    ui->consultantchooseButton->setEnabled(0);
    ui->consultantTextBox->setEnabled(0);
    ui->consultantsendButton->setEnabled(0);
    ui->consultantServAddrEdit->setEnabled(1);
    ui->consultantServPortEdit->setEnabled(1);
    ui->consultantTextBox->clear();
    ui->consultantclientdataTextBox->clear();
    ui->serverdisconnectButton->setText("Rozłącz");
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Połączenie zostało zakończone.</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
}


void MainWindow::conMessage(QString message) {
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">Klient:</b><font style=\"color: black\"> "+message+"</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::conError() {
    conDisconnect();
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Nie można nawiązać połączenia z serwerem.</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::conClient(QString message) {
    ui->consultantclientdataTextBox->insertHtml("<b style=\"color: grey\">Klient:</b><font style=\"color: black\"> "+message+"</font><br>");
}

void MainWindow::conSuccess(QString id) {
    ui->serverconnectButton->setEnabled(0);
    ui->serverdisconnectButton->setEnabled(1);
    ui->consultantnameEdit->setEnabled(0);
    ui->consultantclientchooseBox->setEnabled(1);
    ui->consultantrefreshButton->setEnabled(1);
    ui->consultantchooseButton->setEnabled(1);
    ui->serverdisconnectButton->setText("Rozłącz");
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Połączono (id "+id+"). Wybierz klienta.</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::conRefresh(QStringList names) {
    ui->consultantclientchooseBox->clear();
    if(names.length() <= 0) {
        ui->consultantclientchooseBox->setEnabled(0);
        ui->consultantchooseButton->setEnabled(0);
        ui->consultantclientchooseBox->addItem("brak klientów");
    }
    else {
        ui->consultantclientchooseBox->setEnabled(1);
        ui->consultantchooseButton->setEnabled(1);
        for (QStringList::Iterator it = names.begin(); it != names.end(); ++it ) {
            ui->consultantclientchooseBox->addItem(*it);
        }
    }
}

void MainWindow::conTalkStart() {
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Rozpocząłeś rozmowę z klientem "+ui->consultantclientchooseBox->itemText(ui->consultantclientchooseBox->currentIndex())+".</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
    ui->consultantclientchooseBox->setEnabled(0);
    ui->consultantrefreshButton->setEnabled(0);
    ui->consultantTextBox->setEnabled(1);
    ui->consultantsendButton->setEnabled(1);
    ui->consultantchooseButton->setEnabled(1);
    ui->consultantchooseButton->setText("Zakończ");
    consultant->chooseClient(ui->consultantclientchooseBox->currentIndex());
}

void MainWindow::conTalkClose() {
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">System:</b><font style=\"color: black\"> Rozmowa została zakończona.</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
    ui->consultantclientdataTextBox->clear();
    ui->consultantTextBox->setEnabled(0);
    ui->consultantsendButton->setEnabled(0);
    ui->consultantrefreshButton->setEnabled(1);
    ui->consultantchooseButton->setText("Połącz");
    consultant->closeClient();
    consultant->getClientsList();
}

void MainWindow::on_serverconnectButton_clicked() {
    conConnect();
}

void MainWindow::on_serverdisconnectButton_clicked(){
    conDisconnect();
}

void MainWindow::on_consultantrefreshButton_clicked() {
    consultant->getClientsList();
}

void MainWindow::on_consultantchooseButton_clicked() {
    if(ui->consultantchooseButton->text() != "Zakończ") {
        conTalkStart();
    }
    else {
        conTalkClose();
    }
}

void MainWindow::on_consultantsendButton_clicked() {
    consultant->send(ui->consultantTextBox->text());
    ui->consultantmessageTextEdit->insertHtml("<b style=\"color: grey\">Ty:</b><font style=\"color: black\"> "+ui->consultantTextBox->text()+"</font><br>");
    ui->consultantmessageTextEdit->moveCursor(QTextCursor::End);
    ui->consultantTextBox->clear();
}
