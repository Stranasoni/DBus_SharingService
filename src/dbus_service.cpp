#include "dbus_service.h"
#include <QtDBus/QDBusConnection>
#include <QtDebug>

DBusService::DBusService(QObject* parent) : QDBusAbstractAdaptor(parent)
{
    QDBusConnection dbus_connection = QDBusConnection::sessionBus();
    if (!dbus_connection.interface()->isServiceRegistered(QStringLiteral("com.system.sharing")))
    {
        dbus_connection.registerObject(QStringLiteral("com/system/sharing"), parent);
        dbus_connection.registerService(QStringLiteral("com.system.sharing"));
        qDebug() << "new service register";
    }
    else 
        qDebug() << "service already exists";
         
}