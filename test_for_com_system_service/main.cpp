#include "text_editor.h"

int main(int argc, char *argv[]) {
    
    QCoreApplication app(argc, argv);
    QObject parent_object;

    TextEditor dbus_service(&parent_object,{"txt"});

    return app.exec();

}