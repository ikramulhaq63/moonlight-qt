// CustomNetworkManager.h
#ifndef CUSTOMNETWORKMANAGER_H
#define CUSTOMNETWORKMANAGER_H

#include <QObject>

class CustomNetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit CustomNetworkManager(QObject *parent = nullptr);
    Q_INVOKABLE void sendRequest();

    // private:
    // int customErrorCallback(const char *str, size_t len, void *logStream);
    // void logOpenSSLErrors(std::ostream &logFile);
};

#endif // CUSTOMNETWORKMANAGER_H
