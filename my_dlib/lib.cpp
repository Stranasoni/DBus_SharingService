#include "lib.h"
#include <QtDBus/QDBusConnection>
#include <QtDebug>
#include <filesystem>

SharingService::SharingService(const QString& serviceName, const QStringList& supportedFormats, RequestHandler handler,QObject* parent)
    : QDBusAbstractAdaptor(parent),
      m_serviceName(serviceName), m_supportedFormats(supportedFormats), 
      m_handler(handler), parent(parent)
{}

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
    qDebug()<<executablePath;
   
    QDBusInterface iface(global_service,"/",global_service,dbus);

    
    QDBusReply<void> dbus_reply = iface.call("RegisterService", m_serviceName, executablePath, m_supportedFormats);
    if (!dbus_reply.isValid())
    {
        qDebug() << "Не удалось зарегистрировать сервис в com.system.sharing: " << dbus_reply.error().message();
        return false;
    }

    if (!dbus.registerService(m_serviceName))
    {
        qDebug() <<"Не удалось зарегистрировать сервис на шине: " << dbus.lastError().message();
        return false;
    }
   
    if (!dbus.registerObject(QStringLiteral("/"), m_serviceName, parent, QDBusConnection::ExportAdaptors))
    {
        qDebug() <<"Не удалось зарегистрировать объект: " << dbus.lastError().message();
        return false;
    }

    qDebug() << "Сервис успешно запущен";
    return true;


}

void SharingService::OpenFile(const QString &path)
{

    QDBusMessage message = QDBusMessage().createReply("success");
    

    if (m_handler)
    {    
        Request request(message, this);    
        m_handler(path, request);
        qDebug() << "Обработчик вызван";
    }
    else
    {
        sendErrorReply(QDBusError::Failed, "Обработчик не предоставлен");
    }


}


Request::Request(const QDBusMessage &msg, QObject *context)
    : m_message(msg), m_context(context) {}

void Request::sendErrorResponse(const QString &error) const
{
    if (m_context)
    {
        QDBusContext *dbusContext = dynamic_cast<QDBusContext*>(m_context);
        if (dbusContext)
        {
            dbusContext->sendErrorReply(QDBusError::Failed, error);
        }
    }
}

void Request::sendSuccessResponse() const
{
    QDBusConnection::sessionBus().send(m_message.createReply());
}


