#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "httphandler.h"
#include <QObject>
#include <memory>
#include <qjsonobject.h>
#include <qobject.h>

class authmanager : public QObject
{
    Q_OBJECT
public:
    explicit authmanager(QObject *parent = nullptr);


    void login(const QString &name, const QString &password);
    void login_token(const QString &auth_token);

    void reg(const QString &name, const QString &password);

    void login_success();

    std::unique_ptr<HttpHandler> handler;

signals:
    void auth_attempt(bool success);
};

#endif // AUTHMANAGER_H
