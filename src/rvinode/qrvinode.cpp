#include "qrvinode.h"

#include "qrvinodemonitor_p.h"

// Qt includes
#include <QDir>
#include <QThreadPool>

#include <QDebug>

/* Public methods */

// Constructor
QRviNode::QRviNode(QObject *parent)
    : QObject(parent), _rviHandle(NULL),
      _confFile(""), _nodePort("9007"),
      _nodeAddress("38.129.64.41"),
      _monitor(Q_NULLPTR)
{
    _monitor = new QRviNodeMonitor(this);
    QThreadPool::globalInstance()->setMaxThreadCount(2);
    setupConnections();
}

/* Functional methods */

// Initializer method
void QRviNode::nodeInit()
{
    // check for path to configuration
    _confFile = QDir::fromNativeSeparators(qgetenv("QT_RVI_NODE_CONFIG_FILE"));
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
    _monitor->stopMonitor();
    QThreadPool::globalInstance()->waitForDone(5000);

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
void QRviNode::nodeConnect(const QString &address, const QString &port)
{
    int fd = 0;

    // did we get new connection info?
    if (!address.isEmpty())
        setNodeAddress(address);
    if (!port.isEmpty())
        setNodePort(port);

    // check the handle is valid
    if (_rviHandle)
    {
        // connect to rvi node
        fd = rviConnect(_rviHandle, _nodeAddress.toStdString().c_str(),
                        _nodePort.toStdString().c_str());
        // check for valid file descriptor
        if (fd < 0)
        {
            qWarning() << "Error: rviConnect failed to return a valid API handle"
                       << "Please check the server address and port"
                       << "Address: " << _nodeAddress << ":" << _nodePort;
            emit remoteConnectionError();
        }
        else if (addNewConnectionDescriptor(fd))
            emit remoteNodeConnected();
    }
    else
    {
        qWarning() << "Error: invalid RviHandle, is the node properly initialized?";
        emit invalidRviHandle();
    }
}

// Rvi Node disconnection method
// checks the list of connections for the descriptor received
// and, if valid, removes and disconnects the specified descriptor
void QRviNode::nodeDisconnect(int fd)
{
    // is this a valid disconnect request?
    if (!_activeConnections.contains(fd))
    {
        qWarning() << "Error: specified connection does not exist in the list"
                   << "of active connections"
                   << "Specified connection: " << fd;
        emit invalidDisconnection();
        return;
    }

    int index = 0;
    int returnVal = 0;

    // find our item's index
    for (int i : _activeConnections)
    {
        if (i == fd)
            break;
        ++index;
    }
    _activeConnections.removeAt(index);

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

// TODO: serviceData param? needs a couple overloads possibly?
void QRviNode::registerService(const QString &serviceName, QRviServiceObject *serviceObject, void *serviceData)
{
    Q_UNUSED(serviceData)

    // save the serviceObject pointer
    _serviceMap[serviceName] = serviceObject;

    // nicely create the char * for service name
    char * sn = new char[serviceName.length() + 1];
    qstrcpy(sn, serviceName.toLatin1().data());

    int result = rviRegisterService(_rviHandle, serviceName.toLocal8Bit().data(), callbackHandler, (void*)sn);
    if (result != 0)
    {
        qWarning() << "Error: unknown failure from rviRegisterService call"
                   << "Error Value: " << result;
        emit registerServiceError(serviceName, result);
        return;
    }
    emit registerServiceSuccess(serviceName);
}

// Returns the reference to this instance of QRviNode
QRviNode * QRviNode::getInstance()
{
    static QRviNode * _instance = new QRviNode();
    return _instance;
}

void QRviNode::callbackHandler(int fd, void *serviceData, const char *parameters)
{
    //currently passing around the serviceName
    QString serviceName((char *)serviceData);
    QRviNode::getInstance()->
            getServiceObjectFromMap(serviceName)->
            rviServiceCallback(
                fd,
                serviceData,
                parameters
                );
}

void QRviNode::invokeService(const QString &serviceName, const QString &parameters)
{
    int result = rviInvokeService(_rviHandle, serviceName.toLocal8Bit().data(), parameters.toLocal8Bit().data());
    if (result != 0)
    {
        qWarning() << "Error: unknown failure from rviInvokeService call"
                   << "Error Value: " << result;
        emit invokeServiceError(serviceName, result);
        return;
    }
    emit invokeServiceSuccess(serviceName, parameters);
}

// QRviNode::processInput receives the fd notified from the monitor thread
// so we know the size of the connectionArray is always 1
void QRviNode::processInput(int fd)
{
    // create int* of lenth = 1
    int * connectionArray = (int*)malloc(sizeof(int *));

    // assign the only element of connectionArray
    connectionArray[0] = fd;

    int result = rviProcessInput(_rviHandle, connectionArray, 1);

    if (result != 0)
    {
        qWarning() << "Unexpected error from rviProcessInput"
                   << "Error value: " << result;
        emit processInputError();
        return;
    }
    emit processInputSuccess(fd);
    free(connectionArray);
}

QRviServiceObject * QRviNode::getServiceObjectFromMap(const QString &serviceName)
{
    if (_serviceMap.contains(serviceName))
        return _serviceMap[serviceName];
    return Q_NULLPTR;
}

QList<int> * QRviNode::activeConnections()
{
    return &_activeConnections;
}

/* Property methods */

// Returns the string of nodePort
QString QRviNode::nodePort() const
{
    return _nodePort;
}

// Sets the node port to the new value if it is actually new
void QRviNode::setNodePort(const QString &port)
{
    if (_nodePort != port)
    {
        _nodePort = port;
        emit nodePortChanged();
    }
}

// Returns the string of nodeAddress
QString QRviNode::nodeAddress() const
{
    return _nodeAddress;
}

// Sets the node address to the new ip if it is actually new
void QRviNode::setNodeAddress(const QString &address)
{
    if (_nodeAddress != address)
    {
        _nodeAddress = address;
        emit nodeAddressChanged();
    }
}

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
    this->nodeCleanup();
}

/* Private methods */

void QRviNode::setupConnections()
{
    connect(this, &QRviNode::nodeMonitorBadPointer,
            this, &QRviNode::handleRviMonitorFatalError);

    connect(_monitor, &QRviNodeMonitor::rviMonitorFatalError,
            this, &QRviNode::handleRviMonitorFatalError);

    connect(_monitor, &QRviNodeMonitor::rviReadyRead,
            this, &QRviNode::processInput, Qt::QueuedConnection);
}

// adds a new descriptor to the self-managed connection list
// checks for duplicate descriptor values, as this should not happen
// return true if unique descriptor added and notifies
// return false if unique descriptor added and notifies
bool QRviNode::addNewConnectionDescriptor(int fd)
{
    // not allowed to have duplicate descriptors
    if (_activeConnections.contains(fd))
    {
        qWarning() << "Error: QRviNode expects new connections to"
                   << "receive a unique integer file descriptor";
        emit addConnectionDuplicateFileDescriptorError();
        return false;
    }

    _activeConnections.append(fd);
    _monitor->addSocketDescriptor(fd);

    // if this is the first connection, start the _monitor
    if (_activeConnections.length() == 1)
        prepareAndRunRviMonitor();

    emit newActiveConnection();
    return true;
}

void QRviNode::handleRviMonitorFatalError(int error)
{
    qWarning() << "QRviNodeMonitor bombed out with the following code..."
               << "Error Value: " << error
               << "Restarting QRviNodeMonitor...";

    // if it still exists, destroy it
    if (_monitor)
    {
        _monitor->stopMonitor();
        delete _monitor;
        _monitor = Q_NULLPTR;
    }

    // recreate a new one
    _monitor = new QRviNodeMonitor(this);

    // add all active connections
    for (int fd : _activeConnections)
        _monitor->addSocketDescriptor(fd);

    _monitor->startMonitor();

    QThreadPool::globalInstance()->start(_monitor);
}

// this method assumes any socket descriptors have already
// been set before running the _monitor
void QRviNode::prepareAndRunRviMonitor()
{
    if (!_monitor)
    {
        qWarning() << "Error: unexpected nullptr returned from QRviNodeMonitor"
                   << "aborting prepareAndRunRviMonitor call...";
        emit nodeMonitorBadPointer(420);
    }
    _monitor->startMonitor();
    QThreadPool::globalInstance()->start(_monitor);
}
