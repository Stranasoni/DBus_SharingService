#include <QtDBus/QtDBus>

class DBusService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("sharing_dbus_service","com.system.sharing")
public:
        explicit DBusService(QObject* parent);
public slots:
    
    void RegisterService(const QString &name, const QStringList &supportedFormats);
    void OpenFile(const QString &path);
    void OpenFileUsingService(const QString &path, const QString &service);

private:
    QMap<QString, QStringList> registeredServices;
};

