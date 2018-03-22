/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include "qrvinode.h"
#include "qrvisocketnotifier_p.h"

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDebug>

/** rvi_lib callback **/
void callbackHandler(int fd, void *serviceData, const char * parameters);

// Constructor
QRviNode::QRviNode(QObject *parent)
    : QObject(parent), _rviHandle(NULL),
      _confFile(QStringLiteral("")), _serverNodePort(QStringLiteral("")),
      _serverNodeAddress(QStringLiteral(""))
{
    QString port(QLatin1String(qgetenv("RVI_SERVER_PORT")));
    QString address(QLatin1String(qgetenv("RVI_SERVER_ADDRESS")));

    if (address.isEmpty())
        address = "38.129.64.41";
    if (port.isEmpty())
        port = "9007";
}

// Initializer method
void QRviNode::nodeInit()
{
    if (_rviHandle)
    {
        qWarning() << "Error: This QRviNode(" << this << ") was already initialized.";
        return;
    }
    // check for path to configuration
    _confFile = QDir::fromNativeSeparators(QLatin1String(qgetenv("QT_RVI_NODE_CONFIG_FILE")));
    if (_confFile.isEmpty())
    {
        qWarning() << "Error: QT_RVI_NODE_CONFIG_FILE must be set in order to use QRviNode";
        emit noConfigPathSetInEnvironment();
        return;
    }
    // init API and get handle
    _rviHandle = rviInit(_confFile.toLatin1().data());
    if (!_rviHandle)
    {
        qWarning() << "Error: rviInit failed to return a valid API handle";
        emit initError();
        return;
    }
    emit initSuccess();
}

// Cleanup method
void QRviNode::nodeCleanup()
{
    int returnVal = 0;

    //memory and connections to this handle
    //are cleaned up by rvi_lib
    returnVal = rviCleanup(_rviHandle);
    if (returnVal != 0)
    {
        qWarning() << "Error: rviCleanup returned a non-zero value"
                   << "Error Val: " << returnVal;
        emit cleanupFailure(returnVal);
    }
    emit cleanupSuccess();
}

// Rvi Node connection method
// Takes address and port as params but default arg is ""
// uses the existing member values for address/port if no
// new values are passed to the method
int QRviNode::nodeConnect(const QString &address, const QString &port)
{
    int fd = -1;

    QString tempAddress = _serverNodeAddress;
    QString tempPort = _serverNodePort;

    // did we get new connection info?
    if (!address.isEmpty())
        tempAddress = address;
    if (!port.isEmpty())
        tempPort = port;

    // check the handle is valid
    if (_rviHandle)
    {
        // connect to rvi node
        fd = rviConnect(_rviHandle, tempAddress.toStdString().c_str(),
                        tempPort.toStdString().c_str());
        // check for valid file descriptor
        if (fd <= 0)
        {
            qWarning() << "Error: rviConnect failed to return a valid socket descriptor"
                       << "Please check the server address and port"
                       << "Address: " << _serverNodeAddress << ":" << _serverNodePort;
            emit remoteConnectionError();
        }
        else
        {
            if (addNewConnection(fd, tempAddress, tempPort))
                emit remoteNodeConnected();
        }
    }
    else
    {
        qWarning() << "Error: invalid RviHandle, is the node properly initialized?";
        emit invalidRviHandle();
    }
    return fd;
}

// Rvi Node disconnection method
// checks the list of connections for the descriptor received
// and, if valid, removes and disconnects the specified descriptor
void QRviNode::nodeDisconnect(int fd)
{
    int returnVal = 0;

    // is this a valid disconnect request?
    if (!_readerWatchers.contains(fd))
    {
        qWarning() << "Error: specified connection does not exist in the list"
                   << "of active connections"
                   << "Specified connection: " << fd;
        emit invalidDisconnection();
        return;
    }

    // take and cleanup the related monitor thread
    auto * w = _readerWatchers[fd];
    if (w)
    {
        delete w;
        w = Q_NULLPTR;
    }
    // then remove the descriptor map entry
    _readerWatchers.remove(fd);

    // finally tell rvi_lib we no longer need this
    returnVal = rviDisconnect(_rviHandle, fd);

    if (returnVal != 0)
    {
        qWarning() << "Error: unknown failure from rviDisconnect call"
                   << "Error Value: " << returnVal;
        emit unknownErrorDuringDisconnection(returnVal);
        return;
    }
    emit disconnectSuccess(fd);
}

void QRviNode::registerService(const QString &serviceName, QRviServiceInterface *serviceObject)
{
    connect(this, &QRviNode::signalServicesForNodeCleanup,
            serviceObject, &QRviServiceInterface::destroyRviService);

    int result = 0;

    result = rviRegisterService(_rviHandle, serviceName.toLocal8Bit().data(), callbackHandler, serviceObject);

    if (result != 0)
    {
        qWarning() << "Error: unknown failure from rviRegisterService call"
                   << "Error Value: " << result;
        emit registerServiceError(serviceName, result);
        return;
    }
    emit registerServiceSuccess(serviceName);
}


// void *serviceData parameter contains the QRviServiceInterface* to invoke
void callbackHandler(int fd, void *serviceData, const char *parameters)
{
    // TODO: Jack Sanchez 22 May 2017
    // This feels very hacky because it gives the user a pointer to themselves
    // and is something which can be fixed with the C++ reimplementation of:
    // https://github.com/GENIVI/rvi_core/blob/develop/doc/rvi_protocol.md
    QRviServiceInterface * service = (QRviServiceInterface*)serviceData;
    service->rviServiceCallback(fd, serviceData, parameters);
}

void QRviNode::invokeService(const QString &serviceName, const QString &parameters)
{
    if (serviceName.isEmpty())
    {
        qWarning() << "Error: cannot invoke a service without a service name parameter";
        return;
    }

    int result = 0;

    result = rviInvokeService(_rviHandle, serviceName.toLocal8Bit().data(), parameters.toLocal8Bit().data());

    if (result != 0)
    {
        qWarning() << "Error: unknown failure from rviInvokeService call"
                   << "Error Value: " << result;
        emit invokeServiceError(serviceName, result);
        return;
    }
    emit invokeServiceSuccess(serviceName, parameters);
}

void QRviNode::onReadyRead(int socket)
{
    int result = 0;

    // create int* of lenth = 1
    int * connectionArray = (int*)malloc(sizeof(int *));

    // assign the only element of connectionArray
    connectionArray[0] = socket;

    _readerWatchers[socket]->setEnabled(false);
    result = rviProcessInput(_rviHandle, connectionArray, 1);
    _readerWatchers[socket]->setEnabled(true);

    if (result != 0)
    {
        qWarning() << "Unexpected error from rviProcessInput"
                   << "Error value: " << result;
        emit processInputError();
        return;
    }
    emit processInputSuccess(socket);
    free(connectionArray);
}

/* Property methods */

// Returns the string of the config file
QString QRviNode::configFile() const
{
    return _confFile;
}

// Sets the config file to the new value if it is actually new
void QRviNode::setConfigFile(const QString &file)
{
    if (_confFile != file)
    {
        _confFile = file;
        emit configFileChanged();
    }
}

QRviNode::~QRviNode()
{
    // clean all socket notifier memory before the nodeCleanup call
    for (auto * w : _readerWatchers.values())
    {
        if (w)
        {
            delete w;
            w = Q_NULLPTR;
        }
    }

    _readerWatchers.clear();
    this->nodeCleanup();
    emit signalServicesForNodeCleanup();
}

/* Private methods */

// adds a new descriptor to the self-managed connection list
// checks for duplicate descriptor values, as this should not happen
// return true if unique descriptor added and notifies
// return false if unique descriptor added and notifies
bool QRviNode::addNewConnection(int fd, const QString &address, const QString &port)
{
    // not allowed to have duplicate descriptors
    if (_readerWatchers.contains(fd))
    {
        qWarning() << "Error: QRviNode expects new connections to"
                   << "receive a unique integer file descriptor";
        emit addConnectionDuplicateFileDescriptorError();
        return false;
    }

    _readerWatchers.insert(fd, new QRviSocketNotifier(fd, address, port, this));

    auto * notifier = _readerWatchers[fd];
    connect(notifier, &QSocketNotifier::activated, this, &QRviNode::onReadyRead);

    emit newActiveConnection();
    return true;
}

int QRviNode::findAssociatedConnectionId(const QString &address, const QString &port)
{
    // if we only have one active connection, just return that
    if (_readerWatchers.size() == 1)
        return _readerWatchers.firstKey();

    // resolve and save values
    int socket = 0;
    bool noPortParam = port.isEmpty();
    bool noAddressParam = address.isEmpty();

    // user passed no params, defaulting to rvi test server address
    if (noPortParam && noAddressParam)
    {
        for (auto * w : _readerWatchers)
        {// we're just looking for the test server socket, address compare is enough
            if (w->getAddress() == _serverNodeAddress)
            {// found the test server, exit loop
                socket = w->socket();
                break;
            }
        }
    }
    else if (!noPortParam && !noAddressParam)
    {
        QString completeAddress(address + ":" + port);

        for (auto * w : _readerWatchers)
        {
            if (w->getCompleteAddress() == completeAddress)
            {
                socket = w->socket();
                break;
            }
        }
    }
    else
    {
        qWarning() << "Error: provided an incomplete address, cannot use to find a socket";
        return -1;
    }
    // we found no socket, this is unexpected
    if (socket == 0) // return invalid socket descriptor
        return -1;

    return socket;
}
