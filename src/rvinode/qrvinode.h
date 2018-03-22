/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/


#ifndef QRVINODE_H
#define QRVINODE_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSocketNotifier>

#include <rvi.h>

#include "qtrvinode_global.h"
#include "qrviserviceinterface.h"

QT_BEGIN_NAMESPACE

class QRviSocketNotifier;

class Q_QTRVI_EXPORT QRviNode : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QRviNode)

    Q_ENUM(ERviStatus)

    Q_PROPERTY(QString configFile READ configFile WRITE setConfigFile NOTIFY configFileChanged)

public:

    QRviNode(QObject *parent = Q_NULLPTR);
    ~QRviNode();

    // property readers
    QString configFile() const;

    // property writers
    void setConfigFile(const QString &file);

    // public interface, QML exposed
    Q_INVOKABLE void nodeInit();
    Q_INVOKABLE void nodeCleanup();
    Q_INVOKABLE void nodeDisconnect(int fd);
    Q_INVOKABLE int nodeConnect(const QString &address = QString(), const QString &port = QString());
    Q_INVOKABLE void registerService(const QString &serviceName, QRviServiceInterface *serviceObject);
    Q_INVOKABLE void invokeService(const QString &serviceName, const QString &parameters = QString(QStringLiteral("{}")));
    Q_INVOKABLE int findAssociatedConnectionId(const QString &address = QString(), const QString &port = QString());

public Q_SLOTS:
    // socket watcher notifier handler
    void onReadyRead(int socket);

Q_SIGNALS:
    // property signals
    void configFileChanged();

    // error signals
    void initError();
    void invalidRviHandle();
    void processInputError();
    void invalidDisconnection();
    void remoteConnectionError();
    void cleanupFailure(int error);
    void noConfigPathSetInEnvironment();
    void nodeMonitorBadPointer(int error);
    void unknownErrorDuringDisconnection(int error);
    void addConnectionDuplicateFileDescriptorError();
    void invokeServiceError(const QString &serviceName, int error);
    void registerServiceError(const QString &serviceName, int error);

    // success signals
    void initSuccess();
    void cleanupSuccess();
    void remoteNodeConnected();
    void newActiveConnection();
    void disconnectSuccess(int fd);
    void processInputSuccess(int fd);
    void registerServiceSuccess(const QString &serviceName);
    void invokeServiceSuccess(const QString &serviceName, const QString &parameters);

    // node signals to affect connected services
    void signalServicesForNodeCleanup();

private:
    // rvi_lib context handle
    TRviHandle _rviHandle;

    // absolute path to rvi configuration file
    QString _confFile;

    // collection associating a given socket with it's watcher
    QMap<int, QRviSocketNotifier*> _readerWatchers;

    // data members containing open source rvi core test server address
    QString _serverNodePort;
    QString _serverNodeAddress;


    // private methods
    bool addNewConnection(int fd, const QString &address, const QString &port);
};

QT_END_NAMESPACE

#endif // QRVINODE_H

