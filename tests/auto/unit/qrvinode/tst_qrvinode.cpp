/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

#include <QtTest/QtTest>
#include <QtRviNode/QtRviNode>

// TODO: finish testing public API
class TestQRviNode : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();
    void cleanup();

    // init interface tests
    void testRviNodeInitWithNoConfigFilePath();
    void testRviNodeInitWithBadConfigFilePath();
    void testRviNodeInitWithGoodConfigFilePath();

    // connect interface tests
    void testRviNodeConnectWithBadHandle();
    void testRviNodeConnectWithBadAddress();

    void testRviNodeConnectSuccessWithDefaultTestServerAddress();

private:
};

void TestQRviNode::initTestCase()
{
}

void TestQRviNode::init()
{
}

void TestQRviNode::cleanupTestCase()
{

}

void TestQRviNode::cleanup()
{
}

void TestQRviNode::testRviNodeInitWithGoodConfigFilePath()
{
    // construct node object
    QRviNode node;

    QSignalSpy initSuccessSpy(&node, &QRviNode::initSuccess);
    QSignalSpy initErrorSpy(&node, &QRviNode::initError);
    QSignalSpy noConfigPathSetSpy(&node, &QRviNode::noConfigPathSetInEnvironment);

    qputenv("QT_RVI_NODE_CONFIG_FILE", "./conf.json");

    node.nodeInit();

    // initSuccess signal should only be emitted once
    QCOMPARE(initSuccessSpy.count(), 1);

    // initError should not be emitted
    QCOMPARE(initErrorSpy.count(), 0);

    // we should not see the no config path error
    QCOMPARE(noConfigPathSetSpy.count(), 0);
}

void TestQRviNode::testRviNodeInitWithBadConfigFilePath()
{
    // construct node object
    QRviNode node;

    QSignalSpy initSuccessSpy(&node, &QRviNode::initSuccess);
    QSignalSpy initErrorSpy(&node, &QRviNode::initError);
    QSignalSpy noConfigPathSetSpy(&node, &QRviNode::noConfigPathSetInEnvironment);

    // specify some path instead of the file
    qputenv("QT_RVI_NODE_CONFIG_FILE", ".");

    node.nodeInit();

    // initSuccess should not be emitted
    QCOMPARE(initSuccessSpy.count(), 0);

    // initError should be emitted only once
    QCOMPARE(initErrorSpy.count(), 1);

    // we should not see the no config path error
    QCOMPARE(noConfigPathSetSpy.count(), 0);
}

void TestQRviNode::testRviNodeInitWithNoConfigFilePath()
{
    // construct node object
    QRviNode node;

    QSignalSpy initSuccessSpy(&node, &QRviNode::initSuccess);
    QSignalSpy initErrorSpy(&node, &QRviNode::initError);
    QSignalSpy noConfigPathSetSpy(&node, &QRviNode::noConfigPathSetInEnvironment);

    // no environment variable for QT_RVI_NODE_CONFIG_FILE is set here

    node.nodeInit();

    // initSuccess should not be emitted
    QCOMPARE(initSuccessSpy.count(), 0);

    // initError should not be emitted
    QCOMPARE(initErrorSpy.count(), 0);

    // we should only see the no config path error once
    QCOMPARE(noConfigPathSetSpy.count(), 1);
}

void TestQRviNode::testRviNodeConnectWithBadHandle()
{
    // construct node object
    QRviNode node;

    QSignalSpy connectSuccessSpy(&node, &QRviNode::remoteNodeConnected);
    QSignalSpy connectErrorSpy(&node, &QRviNode::remoteConnectionError);
    QSignalSpy invalidHandleSpy(&node, &QRviNode::invalidRviHandle);

    // not making call to node init to instigate bad handle scenario

    // call connect with default test server parameters
    node.nodeConnect();

    QCOMPARE(connectSuccessSpy.count(), 0);

    QCOMPARE(connectErrorSpy.count(), 0);

    QCOMPARE(invalidHandleSpy.count(), 1);
}

void TestQRviNode::testRviNodeConnectWithBadAddress()
{
    // construct node object
    QRviNode node;

    QSignalSpy connectSuccessSpy(&node, &QRviNode::remoteNodeConnected);
    QSignalSpy connectErrorSpy(&node, &QRviNode::remoteConnectionError);
    QSignalSpy invalidHandleSpy(&node, &QRviNode::invalidRviHandle);

    // call init to prepare the rvi node
    node.nodeInit();

    // call connect with an address which does not serve an rvi node connection
    node.nodeConnect("127.0.0.1", "9999");

    QCOMPARE(connectSuccessSpy.count(), 0);

    QCOMPARE(connectErrorSpy.count(), 1);

    QCOMPARE(invalidHandleSpy.count(), 0);
}

// Note: See TODO at declaration
void TestQRviNode::testRviNodeConnectSuccessWithDefaultTestServerAddress()
{
    // construct node object
    QRviNode node;

    QSignalSpy connectSuccessSpy(&node, &QRviNode::remoteNodeConnected);
    QSignalSpy connectErrorSpy(&node, &QRviNode::remoteConnectionError);
    QSignalSpy invalidHandleSpy(&node, &QRviNode::invalidRviHandle);

    // not making call to node init to instigate bad handle scenario
    node.nodeInit();

    // call connect with default test server parameters
    node.nodeConnect();

    QCOMPARE(connectSuccessSpy.count(), 1);

    QCOMPARE(connectErrorSpy.count(), 0);

    QCOMPARE(invalidHandleSpy.count(), 0);
}

QTEST_MAIN(TestQRviNode)
#include "tst_qrvinode.moc"
