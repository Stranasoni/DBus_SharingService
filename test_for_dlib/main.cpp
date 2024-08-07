#include "../my_dlib/lib.h"
#include <QCoreApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString serviceName = "com.example.SharingService";
    QStringList supportedFormats = {"txt", "pdf","jpg","mp3","zip"};
    
     //Обработчик OpenFile
     auto onOpenFile = [](const QString &path, const Request &req) {
        if (!QFileInfo::exists(path))
        {
            req.sendErrorResponse("File doesn't exist");
        }
        else
        {
            // Открытие при помощи xdg-open
            if (!QProcess::startDetached("xdg-open", QStringList() << path))
            {
                req.sendErrorResponse("Failed to start process");
            }
            else
            {
                req.sendSuccessResponse();
            }
        }
    };

    QObject parent;
    SharingService service(serviceName, supportedFormats, onOpenFile,&parent);

    if (!service.start()) {
        qCritical() << "Не удалось запустить сервис";
         return 1;
     }

    return app.exec();
}
