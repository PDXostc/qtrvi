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

    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)

public://int socketDescriptor
    QRviServiceInterface(QObject *parent = Q_NULLPTR);
    QRviServiceInterface(const QString &name, QObject *parent = Q_NULLPTR);
    ~QRviServiceInterface();

    virtual void rviServiceCallback(int fd, void * serviceData, const char * parameters) = 0;

    QString serviceName();

    void setServiceName(const QString &name);

Q_SIGNALS:
    void serviceNameChanged();

private:
    QString _serviceName;

};

#define QRviServiceInterface_iid "com.genivi.qtrvi.QRviServiceInterface/1.0"
Q_DECLARE_INTERFACE(QRviServiceInterface, QRviServiceInterface_iid)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QRviServiceInterface*)

#endif //QRVISERVICEINTERFACE_H
