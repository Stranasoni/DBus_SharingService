#include <QtDBus/QtDBus>

class DBusService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","com.system.sharing")
public:
        explicit DBusService(QObject* parent);
public slots:
    Q_INVOKABLE void RegisterService(const QString& name, const QString& pathToExe, const QStringList& supportedFormats);
    Q_INVOKABLE void OpenFile(const QString &path);
    Q_INVOKABLE void OpenFileUsingService(const QString &path, const QString &service);

};

