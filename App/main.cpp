// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QString>
#include <QtCore/qcoreevent.h>

#include "autogen/environment.h"
#include "squeezeserver.h"

int main(int argc, char *argv[])
{
    set_qt_environment();
    QGuiApplication app(argc, argv);
    app.setOrganizationName("GeorgeGalt");
    app.setOrganizationDomain("georgegalt.com");
    app.setApplicationName("Tiny2SqueezeApp");

    QQmlApplicationEngine engine;
    const QUrl url(mainQmlFile);

    QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    SqueezeServer *squeezeServer = new SqueezeServer(&app, &engine);
    qmlRegisterSingletonInstance("com.georgegalt.SqueezeServer", 1, 0, "SqueezeServer", squeezeServer);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *qmlWindow = engine.rootObjects().at(0);

    QMetaObject::invokeMethod(qmlWindow, "setupServer");

    return app.exec();
}
