/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include "qrviserviceinterface.h"

QRviServiceInterface::QRviServiceInterface(QObject * parent)
    : QObject(parent)
{

}

QRviServiceInterface::QRviServiceInterface(int socketDescriptor, const QString &name, QObject * parent)
    : QObject(parent), _serviceName(QStringLiteral("")), _associatedConnection(0)
{
    _associatedConnection = socketDescriptor;
    _serviceName = name;
}

QRviServiceInterface::~QRviServiceInterface()
{

}


QString QRviServiceInterface::serviceName()
{
    return _serviceName;
}

void QRviServiceInterface::setServiceName(const QString &name)
{
    if (_serviceName != name)
    {
        _serviceName = name;
        emit serviceNameChanged();
    }
}

int QRviServiceInterface::getAssociatedConnection() const
{
    return _associatedConnection;
}

void QRviServiceInterface::setSocketDescriptor(int socket)
{
    if (_associatedConnection != socket)
    {
        _associatedConnection = socket;
        emit socketChanged();
    }
}
