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

void DBusService::RegisterService(const QString& name, const QString& pathToExe, const QStringList& supportedFormats)
{
    //поиск среди зарегистрированных сервисов
    QFile file("registeredServices");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, 
        "Не удалось проверить список зарегистрированных сервисов,\
                    к сожалению в таком случае продолжить не можем");
        QDBusConnection::sessionBus().send(warning);
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(line.trimmed() == name)
        {
            QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, 
                "Сервис с таким именем уже зарегистрирован");
            QDBusConnection::sessionBus().send(warning);
            file.close();
            return;
        }
    }
    file.close();

    //содержимое конф файла
    QString configure_text =QString(
        "[D-BUS Service]\n"
        "Name=%1\n"
        "Exec=%2\n"
        "SupportedFormats=%3\n")
        .arg(name, pathToExe, supportedFormats.join(','));
    
    //запись в файл, нужны root права
    QString path = QString("/usr/share/dbus-1/services/%1.service").arg(name);
    file.setFileName(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, 
            "Не удалось зарегистрировать сервис");
        QDBusConnection::sessionBus().send(warning);    
        return;
    }
    QTextStream out(&file);
    out << configure_text;
    file.close();
    
    /*в специальном файле будут храниться имена
    зарегистрированных сервисов-обработчиков*/
    file.setFileName("registeredServices");
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, 
            "Сторонний сервис зарегистрирован, но информацию о его присутствие не удалось сохранить");
        QDBusConnection::sessionBus().send(warning);    
        return;
    }
    QTextStream out (&file);
    out << name + "\n";
    file.close();
    qDebug()<<"Новый сервис успешно зарегистрирован";

}

void DBusService::OpenFileUsingService(const QString &path, const QString &service)
{
    //проверка существования файла
    if (!QFile::exists(path)) {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, 
        "Файл не существует");
        QDBusConnection::sessionBus().send(warning);
        return;
    }

    QString extension = QFileInfo(path).suffix();

    //проверка поддерживает ли сервис расширение файла
    QString file_path = QString("/usr/share/dbus-1/services/%1.service").arg(service);
    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed,"Не удалось найти сторонний сервис");
        QDBusConnection::sessionBus().send(warning);
        return;
    }
    QTextStream in(&file);
    bool format_supported = false;
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.startsWith("SupportedFormats="))
        {
            QStringList formats_list = line.section('=',1,1).split(',');
            if(formats_list.contains(extension,Qt::CaseInsensitive))
                format_supported = true;
            break;
        }        
    }
    file.close();
    if(!format_supported)
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed,"Сервис не поддерживает данный формат файлов");
        QDBusConnection::sessionBus().send(warning);
        return;
    }
    //запуск сервера если он не запущен
    QDBusConnection o_dbus = QDBusConnection::sessionBus();
    QDBusConnectionInterface* o_interface = o_dbus.interface();
    if(!o_interface->isServiceRegistered(service))
    {
        if(!o_interface->startService(service).isValid())
        {
           QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, "Не удалось запустить сервис");
           QDBusConnection::sessionBus().send(warning);
           return;

        }
    }
    //вызов метода открыти файла у сервиса
    QDBusInterface iface(service,"/",service,o_dbus);
    QDBusReply<void> dbus_replay = iface.call("OpenFile",path);
    if(!dbus_replay.isValid())
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed,
        "Не удалось вызвать метод открытия файла: " + dbus_replay.error().message());
    }


    qDebug() << "Файл " << path << " открыт через сервис: " << service;

}

void OpenFile(const QString &path)
{
    //проверка существования файла
    if (!QFile::exists(path)) {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, 
        "Файл не существует");
        QDBusConnection::sessionBus().send(warning);
        return;
    }
    QString extension = QFileInfo(path).suffix();

    //поиск зарегистрированных сервисов поддерживающих заданное расширение
    QFile my_reg_file("registeredServices");
    if (!my_reg_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, "Не удалось проверить список зарегистрированных сервисов");
        QDBusConnection::sessionBus().send(warning);
        return;
    }
    QTextStream in(&my_reg_file);
    QStringList services;
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if(!line.isEmpty())
        {
            QFile service_file(QString("/usr/share/dbus-1/services/%1.service").arg(line));
            if(!service_file.open(QIODevice::ReadOnly|QIODevice::Text)) continue;

            QTextStream in_service(&service_file);
            while(!in_service.atEnd())
            {
                QString line_service = in_service.readLine().trimmed();
                if(line_service.startsWith("SupportedFormats="))
                {
                    QStringList formats_list = line_service.section('=',1,1).split(',');
                    if(formats_list.contains(extension,Qt::CaseInsensitive))
                        services.append(line);
                }
            }
            service_file.close();
        }
    }
    my_reg_file.close();

    if(services.isEmpty())
    {
        QDBusMessage reply = QDBusMessage::createError(QDBusError::Failed, 
        "Подходящий сервис не найден");
        QDBusConnection::sessionBus().send(reply);
        return;
    }
    //выбор случайного сервиса
    QString selected_service = services.at(QRandomGenerator::global()->bounded(services.size()));

    //запуск сервиса если не запущен
    QDBusConnection o_dbus = QDBusConnection::sessionBus();
    QDBusConnectionInterface* o_interface = o_dbus.interface();
    if(!o_interface->isServiceRegistered(selected_service ))
    {
        if(!o_interface->startService(selected_service ).isValid())
        {
           QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed, "Не удалось запустить сервис");
           QDBusConnection::sessionBus().send(warning);
           return;

        }
    }
    //вызов метода открыти файла у сервиса
    QDBusInterface iface(selected_service ,"/",selected_service,o_dbus);
    QDBusReply<void> dbus_replay = iface.call("OpenFile",path);
    if(!dbus_replay.isValid())
    {
        QDBusMessage warning = QDBusMessage::createError(QDBusError::Failed,
        "Не удалось вызвать метод открытия файла: " + dbus_replay.error().message());
    }


    qDebug() << "Файл " << path << " открыт через сервис: " << selected_service ;


}














