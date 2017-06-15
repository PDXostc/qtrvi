/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#ifndef QRVISERVICEINTERFACE_H
#define QRVISERVICEINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QtPlugin>

#include "qtrvinode_global.h"

QT_BEGIN_NAMESPACE

class Q_QTRVI_EXPORT QRviServiceInterface : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(QRviServiceInterface)

    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
    Q_PROPERTY(int socket READ getAssociatedConnection WRITE setSocketDescriptor NOTIFY socketChanged)

public:
    QRviServiceInterface(QObject *parent = Q_NULLPTR);
    QRviServiceInterface(int socketDescriptor, const QString &name, QObject *parent = Q_NULLPTR);
    virtual ~QRviServiceInterface();

    virtual void rviServiceCallback(int fd, void * serviceData, const char * parameters) = 0;

    QString serviceName();
    int getAssociatedConnection() const;

    void setServiceName(const QString &name);
    void setSocketDescriptor(int socket);

Q_SIGNALS:
    void socketChanged();
    void serviceNameChanged();

    void destroyRviService();

private:
    QString _serviceName;
    int _associatedConnection;

};

#define QRviServiceInterface_iid "com.jlr.qtrvi.QRviServiceInterface/1.0"
Q_DECLARE_INTERFACE(QRviServiceInterface, QRviServiceInterface_iid)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QRviServiceInterface*)

#endif //QRVISERVICEINTERFACE_H
