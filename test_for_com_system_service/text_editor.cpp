#include "text_editor.h"
#include <QtDBus/QDBusConnection>
#include <QtDebug>

TextEditor::TextEditor(QObject* parent, const QStringList& supported_format) 
    : supported_format(supported_format), QDBusAbstractAdaptor(parent)
{
    QDBusConnection dbus_connection = QDBusConnection::sessionBus();
    if (!dbus_connection.interface()->isServiceRegistered(QStringLiteral("com.example.texteditor")))
    {
        dbus_connection.registerService(QStringLiteral("com.example.texteditor"));
        dbus_connection.registerObject(QStringLiteral("/"), "com.example.texteditor", parent, 
        QDBusConnection::ExportAdaptors);
        qDebug() << "new service register";
    }
    else 
        qDebug() << "service already exists";
}
    
void TextEditor::OpenFile(const QString& path)
{
    if(!QProcess::startDetached("xdg-open", QStringList()<< path))
    {
          qDebug() << "Failed to open file with text editor.";
    } else {
        qDebug() << "File opened:" << path;  
    }
}