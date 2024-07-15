#include "../my_lib/SharingService.h"
#include <QCoreApplication>
#include <QDebug>
#include "SharingService.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString serviceName = "com.example.SharingService";
    QStringList supportedFormats = {"txt", "pdf"};
    
    SharingService::RequestHandler onOpenFile = [](const QString& path, const QDBusMessage& message) {
        qDebug() << "Файл открыт: " << path;
    };

    SharingService service(nullptr, serviceName, supportedFormats, onOpenFile);

    if (!service.start()) {
        qCritical() << "Не удалось запустить сервис";
        return 1;
    }

    return app.exec();
}
