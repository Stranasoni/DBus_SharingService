#include "dbus_service.h"
#include <QtDBus/QDBusConnection>
#include <QtDebug>




DBusService::DBusService(QObject* parent) : QDBusAbstractAdaptor(parent)
{
    QDBusConnection dbus_connection = QDBusConnection::sessionBus();
    if (!dbus_connection.interface()->isServiceRegistered(QStringLiteral("com.system.sharing")))
    {
        dbus_connection.registerService(QStringLiteral("com.system.sharing"));
        dbus_connection.registerObject(QStringLiteral("/"), "com.system.sharing", parent, 
        QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSignals);
        qDebug() << "new service register";
    }
    else 
        qDebug() << "service already exists";
         
}
//вывод ошибок в том же терминале откуда открывалось приложение
void DBusService::RegisterService(const QString& name, const QString& pathToExe, const QStringList& supportedFormats)
{
    //поиск среди зарегистрированных сервисов
    QFile file("../registeredServices.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        qDebug()<<"Не удалось проверить список зарегистрированных сервисов, к сожалению в таком случае продолжить не можем";       
        return;
    }
   
    QTextStream in_reg_file(&file);
    while(!in_reg_file.atEnd())
    {
        QString line = in_reg_file.readLine();
        if(line.trimmed() == name)
        {
            qDebug()<<"Сервис с таким именем уже зарегистрирован";
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
        qDebug()<< "Не удалось зарегистрировать сервис";
        return;
    }
    QTextStream out_conf_file(&file);
    out_conf_file << configure_text;
    file.close();
    
    /*в специальном файле будут храниться имена
    зарегистрированных сервисов-обработчиков*/
    file.setFileName("../registeredServices.txt");
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug()<<"Сторонний сервис зарегистрирован, но информацию о его присутствие не удалось сохранить";
        return;
    }
    QTextStream out_reg_file(&file);
    out_reg_file << name + "\n";
    file.close();
    qDebug()<<"Новый сервис успешно зарегистрирован";
    
}

void DBusService::OpenFileUsingService(const QString &path, const QString &service)
{
    //проверка существования файла
    if (!QFile::exists(path)) {
        qDebug()<<"Файл не существует";
        return;
    }

    QString extension = QFileInfo(path).suffix();

    //проверка поддерживает ли сервис расширение файла
    QString file_path = QString("/usr/share/dbus-1/services/%1.service").arg(service);
    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Не удалось найти сторонний сервис";
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
        qDebug()<<"Сервис не поддерживает данный формат файлов";
        return;
    }
    //запуск сервера если он не запущен
    QDBusConnection o_dbus = QDBusConnection::sessionBus();
    QDBusConnectionInterface* o_interface = o_dbus.interface();
    if(!o_interface->isServiceRegistered(service))
    {
        if(!o_interface->startService(service).isValid())
        {
            qDebug()<<"Не удалось запустить сервис";     
            return;
        }
    }
    //вызов метода открыти файла у сервиса
    QDBusInterface iface(service,"/",service,o_dbus);
    QDBusReply<void> dbus_replay = iface.call("OpenFile",path);
    if(!dbus_replay.isValid())
    {
        qDebug()<<"Не удалось вызвать метод открытия файла: " + dbus_replay.error().message();
    }


    qDebug() << "Файл " << path << " открыт через сервис: " << service;

}

void DBusService::OpenFile(const QString &path)
{
    //проверка существования файла
    if (!QFile::exists(path)) {
        qDebug() << "Файл не существует";
        return;
    }
    QString extension = QFileInfo(path).suffix();

    //поиск зарегистрированных сервисов поддерживающих заданное расширение
    QFile my_reg_file("../registeredServices.txt");
    if (!my_reg_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() <<  "Не удалось проверить список зарегистрированных сервисов";
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
        qDebug() << "Подходящий сервис не найден";
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
           qDebug() << "Не удалось запустить сервис";
           return;

        }
    }
    //вызов метода открыти файла у сервиса
    QDBusInterface iface(selected_service ,"/",selected_service,o_dbus);
    QDBusReply<void> dbus_replay = iface.call("OpenFile",path);
    if(!dbus_replay.isValid())
    {
        qDebug() << "Не удалось вызвать метод открытия файла: " + dbus_replay.error().message();
    }


    qDebug() << "Файл " << path << " открыт через сервис: " << selected_service ;


}














