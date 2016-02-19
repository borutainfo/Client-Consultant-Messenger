#include "clientthread.h"

ClientThread::ClientThread(QString name, QString addr, QString port) {
    this->name = name;
    this->addr = addr;
    this->port = port;
}

ClientThread::~ClientThread() {
    ClientThread::stop();
}

void ClientThread::run() {
    memset(&client, 0, sizeof(struct sockaddr));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = 0;

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(gethostbyname_r(addr.toStdString().c_str(), &he, buf, sizeof(buf), &ip_addr, &status) || !ip_addr || sock < 0 || bind(sock, (struct sockaddr*)&client, sizeof(struct sockaddr)) < 0) {
        emit connectError();
        ::close(sock);
    }

    else {
        memset(&server, 0, sizeof(struct sockaddr));
        memcpy(&server.sin_addr.s_addr, ip_addr->h_addr, ip_addr->h_length);
        server.sin_family = AF_INET;
        server.sin_port = htons(port.toInt());

        sendto(sock, "new0"+name.toUtf8(), name.toUtf8().length()+4, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
        buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&server, &size);
        id = QString::fromUtf8(buf, buf_size);
        emit connectSuccess(id);

        while (true) {
            buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&consultant, &size);

            if(QString::fromUtf8(buf, 4) == "exit") {
                talk = false;
                emit newSysMessage("Rozmowa została zakończona.");
                sendto(sock, "exit"+id.toUtf8(), id.toUtf8().length()+4, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
                emit exitTalk();
                break;
            }

            else if(!talk && QString::fromUtf8(buf, 3) == "con") {
                talk = true;
                emit newSysMessage("Właśnie połączył się z Tobą konsultant "+QString::fromUtf8(buf, buf_size).mid(3,buf_size-3)+".");
                emit startTalk();
            }

            else if(talk && QString::fromUtf8(buf, 1) == ";") {
                emit newMessage(QString::fromUtf8(buf, buf_size).mid(1,buf_size-1));
            }
        }
    }
}

void ClientThread::stopTalk() {
    if(talk)
        sendto(sock, "exit", 4, 0, (struct sockaddr*)&consultant, sizeof(struct sockaddr));
    talk = false;
}

void ClientThread::send(QString message) {
    sendto(sock, ";"+message.toUtf8(), message.toUtf8().length()+1, 0, (struct sockaddr*)&consultant, sizeof(struct sockaddr));
}

void ClientThread::stop() {
    if(ClientThread::isRunning()) {
        stopTalk();
        sendto(sock, "exit"+id.toUtf8(), id.toUtf8().length()+4, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
        close(sock);
        ClientThread::terminate();
    }
}
