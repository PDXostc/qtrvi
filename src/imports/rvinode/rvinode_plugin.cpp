/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/


// TODO: Jack Sanchez 22 May 2017
//
// Need to track down the specific modules which provide
// the type registration methods. It seems that they moved in 5.7

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
//    #include <QtQml/qqmlextensionplugin.h>
//    #include <QtQml/QQmlEngine>
//#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    #include <QtQml>
//#endif

#include <QtRviNode/QtRviNode>

QT_BEGIN_NAMESPACE

class RviNodePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("RviNode"));

        qmlRegisterType<QRviNode>(uri, 1, 0, "RviNode");
        qmlRegisterUncreatableType<QRviServiceInterface>(uri, 1, 0, "RviServiceInterface", "Pure virtual interface class");
    }
};

QT_END_NAMESPACE

#include "rvinode_plugin.moc"
