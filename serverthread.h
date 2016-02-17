#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <config.h>

class ServerThread : public QThread {
    Q_OBJECT

public:
    void run();
    void stop();
    void refreshStatus();

signals:
    void serverStatus(int clients, int consultants);
    void serverAction(QString);
    void serverError(QString);
    void serverStarted();

private:
    int addUser(user);
    void removeUser(int);

    int sock, buf_size = 0, clients = 0, consultants = 0, newID = 0, optval = 1;
    char buf[2048];
    struct sockaddr_in server;
    socklen_t size = sizeof(struct sockaddr);
    std::vector <user> users;
    QString send;
    user temp;

};

#endif // SERVERTHREAD_H
