#include "serverthread.h"

ServerThread::~ServerThread() {
    ServerThread::stop();
}

void ServerThread::run() {

    memset(&server, 0, sizeof(struct sockaddr));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(server_port);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(sock < 0 || bind(sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0) {
        emit serverError("Nie można uruchomić serwera.");
    }

    else {
        emit serverStarted();

        while(true) {
            buf_size = recvfrom(sock, buf, 2048, 0, (struct sockaddr*)&temp.addr, &size);

            // nowy użytkownik
            if(QString::fromUtf8(buf, 3) == "new") {
                temp.id = newID++;
                temp.name = QString::fromUtf8(buf, buf_size).mid(4,buf_size-4);

                if(QString::fromUtf8(buf, 4).mid(3,1) == "0") {
                    emit serverAction("Pojawił się nowy klient: "+temp.name+" (id "+QString::number(temp.id)+").");
                    temp.type = false;
                }

                else {
                    emit serverAction("Pojawił się nowy konsultant: "+temp.name+" (id "+QString::number(temp.id)+").");
                    temp.type = true;
                }

                addUser(temp);
                send = QString::number(temp.id);
                sendto(sock, send.toUtf8(), send.toUtf8().length(), 0, (struct sockaddr*)&temp.addr, sizeof(struct sockaddr));
                sendInfo();
            }

            // użytkownik odłączył się
            else if(QString::fromUtf8(buf, 4) == "exit") {
                int id = QString::fromUtf8(buf, buf_size).mid(4,buf_size-4).toInt();
                removeUser(id);
                emit serverAction("Użytkownik "+users[id].name+" rozłączył się (id "+QString::number(id)+").");
                sendInfo();
            }

            // konsultant prosi o listę klientów
            else if(QString::fromUtf8(buf, 4) == "list") {
                QString lista = getList();
                sendto(sock, lista.toUtf8(), lista.toUtf8().length(), 0, (struct sockaddr*)&temp.addr, sizeof(struct sockaddr));
            }

            // konsultant informuje serwer o wybraniu użytkownika
            else if(QString::fromUtf8(buf, 3) == "got") {
                int id = QString::fromUtf8(buf, buf_size).mid(3,buf_size-3).toInt();
                removeUser(id, true);
                sendInfo();
            }
        }
    }
}

void ServerThread::refreshStatus() {
    emit serverStatus(clients, consultants);
}

int ServerThread::addUser(user temp) {
    if(temp.type)
        consultants++;
    else
        clients++;

    users.push_back(temp);
    refreshStatus();
    return users.size()-1;
}

void ServerThread::removeUser(int id) {
    removeUser(id, false);
}

void ServerThread::removeUser(int id, bool status) {
    for(size_t i = 0; i < users.size(); i++) {
        if(users[i].id == id && users[i].active) {
            if(users[i].type) {
                consultants--;
            }

            else {
                clients--;
            }

            if(status)
                emit serverAction("Klient "+users[id].name+" rozmawia właśnie z konsultantem.");

            users[i].active = false;
        }
    }
    refreshStatus();
}

QString ServerThread::getList() {
    QString lista = "list";
    char str[16];

    for(size_t i = 0; i < users.size(); i++ ) {
        if(!users[i].type && users[i].active) {
            inet_ntop(AF_INET, &(users[i].addr.sin_addr), str, INET_ADDRSTRLEN);
            lista += "{" + QString::number(users[i].id) + ";" + QString::fromUtf8(str) + ";" + QString::number(ntohs(users[i].addr.sin_port)) + ";" + users[i].name + "}";
        }
    }

    return lista;
}

void ServerThread::sendInfo() {
    QString lista = getList();
    for(size_t i = 0; i < users.size(); i++) {
        if(users[i].type) {
            sendto(sock, lista.toUtf8(), lista.toUtf8().length(), 0, (struct sockaddr*)&users[i].addr, sizeof(struct sockaddr));
        }
    }
}

void ServerThread::stop() {
    if(ServerThread::isRunning()) {
        emit serverStatus(0, 0);
        close(sock);
        ServerThread::terminate();
    }
}
