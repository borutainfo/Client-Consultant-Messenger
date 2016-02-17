#include "clientthread.h"

ClientThread::ClientThread(QString name) {
    this->name = name;
}

void ClientThread::run() {

    memset(&client, 0, sizeof(struct sockaddr));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = 0;

    memset(&server, 0, sizeof(struct sockaddr));
    memcpy(&server.sin_addr.s_addr, ip_addr->h_addr, ip_addr->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(sock < 0 || bind(sock, (struct sockaddr*)&client, sizeof(struct sockaddr)) < 0) {
        emit connectError();
        ::close(sock);
    }

    else {
        sendto(sock, "new0"+name.toUtf8(), name.length()+4, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
        buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&server, &size);
        id = QString::fromUtf8(buf, buf_size);
        emit connectSuccess(id);

        while (true) {
            buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&consultant, &size);

            if(QString::fromUtf8(buf, 4) == "exit") {
                talk = false;
                emit newSysMessage("Rozmowa została zakończona.");
                emit exitTalk();
                break;
            }

            else if(QString::fromUtf8(buf, 3) == "con") {
                emit newSysMessage("Właśnie połączył się z Tobą konsultant "+QString::fromUtf8(buf, buf_size).mid(3,buf_size-3)+".");
                emit startTalk();
            }

            else {
                if(!talk)
                    talk = true;
                emit newMessage(QString::fromUtf8(buf, buf_size).mid(1,buf_size-1));
            }
        }
    }
}

void ClientThread::stop() {
    stopTalk();
    sendto(sock, "exit"+id.toUtf8(), 4+id.length(), 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
    ::close(sock);
    QThread::quit();
}

void ClientThread::stopTalk() {
    if(talk)
        sendto(sock, "exit", 4, 0, (struct sockaddr*)&consultant, sizeof(struct sockaddr));
}

void ClientThread::send(QString message) {
    sendto(sock, ";"+message.toUtf8(), message.length()+1, 0, (struct sockaddr*)&consultant, sizeof(struct sockaddr));
}
