#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <config.h>

class ClientThread : public QThread {
    Q_OBJECT

public:
    ClientThread(QString, QString, QString);
    ~ClientThread();
    void run();
    void stop();
    void send(QString);
    void stopTalk();

signals:
    void newMessage(QString);
    void newSysMessage(QString);
    void connectError();
    void connectSuccess(QString);
    void startTalk();
    void exitTalk();

private:
    int sock, buf_size = 0, optval = 1, status = 0;
    char buf[2048];
    bool talk = false;
    struct sockaddr_in server, client, consultant;
    struct hostent* ip_addr;
    struct hostent he;
    socklen_t size = sizeof(struct sockaddr);
    QString name = "", addr = "", port = "", id = "";
};

#endif // CLIENTTHREAD_H
