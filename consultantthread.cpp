#include "consultantthread.h"

ConsultantThread::ConsultantThread(QString name, QString addr, QString port) {
    this->name = name;
    this->addr = addr;
    this->port = port;
}

ConsultantThread::~ConsultantThread() {
    ConsultantThread::stop();
}

void ConsultantThread::run() {
    memset(&consultant, 0, sizeof(struct sockaddr));
    consultant.sin_family = AF_INET;
    consultant.sin_addr.s_addr = INADDR_ANY;
    consultant.sin_port = 0;

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(gethostbyname_r(addr.toStdString().c_str(), &he, buf, sizeof(buf), &ip_addr, &status) || !ip_addr || sock < 0 || bind(sock, (struct sockaddr*)&consultant, sizeof(struct sockaddr)) < 0) {
        emit connectError();
        ::close(sock);
    }

    else {
        memset(&server, 0, sizeof(struct sockaddr));
        memcpy(&server.sin_addr.s_addr, ip_addr->h_addr, ip_addr->h_length);
        server.sin_family = AF_INET;
        server.sin_port = htons(port.toInt());

        sendto(sock, "new1"+name.toUtf8(), name.toUtf8().length()+4, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
        buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&server, &size);
        id = QString::fromUtf8(buf, buf_size);
        emit connectSuccess(id);

        while (true) {
            buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&temp.addr, &size);

            if(!talk && QString::fromUtf8(buf, 4) == "list") {
                QStringList names;
                users.clear();
                if(buf_size > 6) {

                    QStringList list = QString::fromUtf8(buf, buf_size).mid(5,buf_size-6).split("}{");
                    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {

                        QStringList client =  QString(*it).split(";");

                        memset(&temp.addr, 0, sizeof(struct sockaddr));
                        temp.id = QString(client.value(0)).toInt();
                        struct hostent* addr = gethostbyname(QString(client.value(1)).toUtf8());
                        temp.addr.sin_family = AF_INET;
                        temp.addr.sin_port = htons(QString(client.value(2)).toUtf8().toInt());
                        memcpy(&temp.addr.sin_addr.s_addr, addr->h_addr, addr->h_length);
                        temp.name = QString(client.value(3));
                        temp.type = false;

                        users.push_back(temp);
                        names.push_back(temp.name);
                    }
                }
                emit refreshClients(names);
            }

            else if(QString::fromUtf8(buf, 4) == "exit") {
                talk = false;
                emit clientClose();
            }

            else if(talk && QString::fromUtf8(buf, 1) == ";") {
                    emit newMessage(QString::fromUtf8(buf, buf_size).mid(1,buf_size-1));
            }
        }
    }
}

void ConsultantThread::getClientsList() {
    sendto(sock, "list", 4, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
}

void ConsultantThread::chooseClient(int id) {
    talk = true;
    client = users[id].addr;
    sendto(sock, "got"+QString::number(users[id].id).toUtf8(), QString::number(users[id].id).toUtf8().length()+3, 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
    sendto(sock, "con"+name.toUtf8(), name.toUtf8().length()+3, 0, (struct sockaddr*)&client, sizeof(struct sockaddr));
    emit clientChoose(users[id].name);
}

void ConsultantThread::closeClient() {
    sendto(sock, "exit", 4, 0, (struct sockaddr*)&client, sizeof(struct sockaddr));
    talk = false;
}

void ConsultantThread::send(QString message) {
    sendto(sock, ";"+message.toUtf8(), message.toUtf8().length()+1, 0, (struct sockaddr*)&client, sizeof(struct sockaddr));
}

void ConsultantThread::stop() {
    if(ConsultantThread::isRunning()) {
        if(talk)
            closeClient();
        sendto(sock, "exit"+id.toUtf8(), 4+id.toUtf8().length(), 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
        close(sock);
        ConsultantThread::terminate();
    }
}
