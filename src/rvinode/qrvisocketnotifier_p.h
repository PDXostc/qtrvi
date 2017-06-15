/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#ifndef QRVISOCKETNOTIFIER_H
#define QRVISOCKETNOTIFIER_H

#include <QtCore/QString>
#include <QtCore/QSocketNotifier>

class QRviSocketNotifier : public QSocketNotifier
{
    Q_OBJECT

public:
    QRviSocketNotifier(int fd, const QString &address, const QString &port, QObject * parent);

    QString getCompleteAddress();
    QString getAddress();
    QString getPort();

private:
    QString _socketAddress;
    QString _socketPort;
};

#endif // QRVISOCKETNOTIFIER_H
