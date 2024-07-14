#include "dbus_service.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QDBusAbstractAdaptor::QObject parent_object;

    DBusService dbus_service(&parent_object);

    return app.exec();

}