#ifndef CONSULTANTTHREAD_H
#define CONSULTANTTHREAD_H

#include <config.h>

class ConsultantThread : public QThread {
    Q_OBJECT

public:
    ConsultantThread(QString, QString, QString);
    ~ConsultantThread();
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
    int sock, buf_size = 0, optval = 1, status = 0;
    char buf[2048];
    struct sockaddr_in server, client, consultant;
    struct hostent* ip_addr;
    struct hostent he;
    socklen_t size = sizeof(struct sockaddr);
    std::vector<user> users;
    std::vector<std::string> names;
    QString name = "", addr="", port="", id = "";
    user temp;
    bool talk = false;
};

#endif // CONSULTANTTHREAD_H
