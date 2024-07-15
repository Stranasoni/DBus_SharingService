#include "SharingService.h"
#include <QtDBus/QDBusConnection>
#include <QtDebug>
#include <filesystem>

SharingService::SharingService(QObject *parent, const QString& serviceName, const QStringList& supportedFormats, RequestHandler handler)
    : QDBusAbstractAdaptor(parent), 
      m_serviceName(serviceName), m_supportedFormats(supportedFormats), 
      m_handler(handler)
{
}

SharingService::~SharingService() { }

bool SharingService::start()
{
    QString global_service = "com.system.sharing";
    QDBusConnection dbus = QDBusConnection::sessionBus();
    QDBusConnectionInterface* interface = dbus.interface();
    if (!interface->isServiceRegistered(global_service))
    {
        if (!interface->startService(global_service).isValid())
        {
            qDebug() << "Не удалось запустить сервис";     
            return false;
        }
    }

    QString executablePath = QString::fromStdString(std::filesystem::canonical("/proc/self/exe").string());
    QDBusInterface iface(global_service, "/", global_service, dbus);
    QDBusReply<void> dbus_reply = iface.call("RegisterService", m_serviceName, executablePath, m_supportedFormats);
    if (!dbus_reply.isValid())
    {
        qDebug() << "Не удалось зарегистрировать сервис: " << dbus_reply.error().message();
        return false;
    }

    if (!dbus.registerService(m_serviceName))
    {
        qDebug() <<"Не удалось зарегистрировать сервис на шине: " << dbus.lastError().message();
        return false;
    }

    if (!dbus.registerObject(QStringLiteral("/"), m_serviceName, parent(), QDBusConnection::ExportAllSlots))
    {
        qDebug() <<"Не удалось зарегистрировать объект: " << dbus.lastError().message();
        return false;
    }

    return true;
}

void SharingService::OpenFile(const QString& path)
{
    QDBusMessage message = QDBusContext::message();
    
    if (m_handler)
    {
        m_handler(path, message);
    }
    else
    {
        QDBusContext::sendErrorReply(QDBusError::Failed, "Обработчик не предоставлен");
    }
}
