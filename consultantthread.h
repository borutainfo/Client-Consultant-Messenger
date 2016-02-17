#ifndef CONSULTANTTHREAD_H
#define CONSULTANTTHREAD_H

#include <config.h>

class ConsultantThread : public QThread {
    Q_OBJECT

public:
    ConsultantThread(QString);
    void run();
    void stop();
    void getClientsList();
    void chooseClient(int);
    void closeClient();
    void send(QString);

signals:
    void newMessage(QString);
    void clientChoose(QString);
    void clientClose();
    void connectError();
    void connectSuccess(QString);
    void newList(QString);
    void refreshClients(QStringList);

private:
    int sock, buf_size = 0, optval = 1;
    char buf[2048];
    struct sockaddr_in server, client, consultant;
    struct hostent* ip_addr = gethostbyname(server_ip.toUtf8());
    socklen_t size = sizeof(struct sockaddr);
    std::vector<user> users;
    std::vector<std::string> names;
    QString name = "", id = "";
    user temp;
};

#endif // CONSULTANTTHREAD_H
