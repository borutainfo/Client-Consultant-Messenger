#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QThread>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <ctime>
#include <iostream>

const int server_port = 6666;
const QString server_ip = "127.0.0.1";

struct user {
    int id;
    QString name;
    bool type;
    struct sockaddr_in addr;
    bool active = true;
};

#endif // CONFIG_H
