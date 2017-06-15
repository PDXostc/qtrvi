/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include "rviservicepong.h"

#include <QtCore/QDebug>

RviServicePong::RviServicePong(QObject * parent)
    : QRviServiceInterface(parent)
{

}

RviServicePong::RviServicePong(int socketDescriptor, const QString &name, QObject * parent)
: QRviServiceInterface(socketDescriptor, name, parent)
{

}

RviServicePong::~RviServicePong()
{

}

void RviServicePong::rviServiceCallback(int fd, void *serviceData, const char *parameters)
{
    //Temp note-to-self: i think serviceData * should be the same address as this
    qDebug() << "void *serviceData&: " << serviceData;
    qDebug() << "this: " << this;

    qDebug() << "Success!";
    qDebug() << "Pong service was invoked!";
    qDebug() << "Calling socket: " << fd;
    qDebug() << "Parameters: " << parameters;
}
