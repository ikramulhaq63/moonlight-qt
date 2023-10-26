// CustomNetworkManager.h
#include "../settings/streamingpreferences.h"

#ifndef CUSTOMNETWORKMANAGER_H
#define CUSTOMNETWORKMANAGER_H

#include <QObject>

class CustomNetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit CustomNetworkManager(QObject *parent = nullptr);
    Q_INVOKABLE void sendRequest();
};

#endif // CUSTOMNETWORKMANAGER_H
