/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include <QtGui/QGuiApplication>
#include <QtQml>

#include "rviserviceping.h"

int main(int argc, char * argv[])
{
    QGuiApplication app(argc, argv);

    qputenv("QT_RVI_NODE_CONFIG_FILE", "./conf.json");
    qmlRegisterType<RviServicePing>("com.jlr.rviserviceping", 1, 0, "RviServicePing");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
