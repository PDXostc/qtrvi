/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QDebug>

#include <QtRviNode/QRviNode>

#include "rviservicepong.h"

int main(int argc, char * argv[])
{
    int counter = 0;
    QString servicePrefix(QStringLiteral("genivi.org/client/bbfbb478-d628-480a-8528-cff40d73678f/"));
    QCoreApplication app(argc, argv);

    qputenv("QT_RVI_NODE_CONFIG_FILE", "./conf.json");

    QRviNode node;

    // Connect a few lambdas to log events
    QObject::connect(&node, &QRviNode::initSuccess, [](){
        qDebug() << "Successful init!";
    });
    QObject::connect(&node, &QRviNode::remoteNodeConnected, [](){
        qDebug() << "Remote node connected!";
    });
    QObject::connect(&node, &QRviNode::initError, [](){
        qDebug() << "Error init'ing the node!";
    });
    QObject::connect(&node, &QRviNode::remoteConnectionError, [](){
        qDebug() << "Error connecting to specified remote address!";
    });
    QObject::connect(&node, &QRviNode::registerServiceSuccess, [](){
        qDebug() << "Registered a service!";
    });

    node.nodeInit();

    int socket = node.nodeConnect();

    RviServicePong * ponger = new RviServicePong(socket, "ponger");

    // Connect to service invoke service signals
    QObject::connect(&node, &QRviNode::invokeServiceError,
                     []() {
        qDebug() << "Currently, rvi_lib does not support local service invokations."
                 << "This error signal signifies success.";
    });
    QObject::connect(&node, &QRviNode::invokeServiceSuccess,
                     []() {
        qDebug() << "Successfully invoked the pinger service!";
    });

    // Connect to service destroy signals
    QObject::connect(ponger, &RviServicePong::destroyRviService,
                     [&]() {
        delete ponger;
    });

    node.registerService(ponger->serviceName(), ponger);

    node.invokeService(servicePrefix + ponger->serviceName());

    qDebug() << "Program counter: " << counter++;
    // Know when the app is ending to cleanup memory
    QObject::connect(&app, &QCoreApplication::aboutToQuit,
                     [&]() {
        node.nodeDisconnect(socket);
    });

    qDebug() << "Program counter: " << counter++;
    return app.exec();
}
