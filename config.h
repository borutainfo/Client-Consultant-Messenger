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

struct user {
    int id;
    QString name;
    bool type;
    struct sockaddr_in addr;
    bool active = true;
};

#endif // CONFIG_H
