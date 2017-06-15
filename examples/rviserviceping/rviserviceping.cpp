/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include "rviserviceping.h"

#include <QtCore/QDebug>

RviServicePing::RviServicePing(QObject * parent)
    : QRviServiceInterface(parent)
{

}

RviServicePing::RviServicePing(int socketDescriptor, const QString &name, QObject * parent)
: QRviServiceInterface(socketDescriptor, name, parent)
{

}

RviServicePing::~RviServicePing()
{

}

void RviServicePing::rviServiceCallback(int fd, void *serviceData, const char *parameters)
{
    qDebug() << "Ping service invoked!";
    qDebug() << "Parameters: " << parameters;
}
