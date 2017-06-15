/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include "qrvisocketnotifier_p.h"

QRviSocketNotifier::QRviSocketNotifier(int fd, const QString &address, const QString &port, QObject * parent)
    : QSocketNotifier(fd, QSocketNotifier::Read, parent), _socketAddress(address), _socketPort(port)
{
}

QString QRviSocketNotifier::getCompleteAddress()
{
    return QString(_socketAddress + ":" + _socketPort);
}

QString QRviSocketNotifier::getAddress()
{
    return _socketAddress;
}

QString QRviSocketNotifier::getPort()
{
    return _socketPort;
}
