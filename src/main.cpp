#include "dbus_service.h"
#include "iostream"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QObject parent_object;

    DBusService dbus_service(&parent_object);

    return app.exec();

}