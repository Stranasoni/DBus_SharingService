#include <QtDBus/QtDBus>

class Request
{
public:
    Request(const QDBusMessage &msg, QObject *context);
    void sendErrorResponse(const QString &error) const;
    void sendSuccessResponse() const;
private:
    QDBusMessage m_message;
    QObject *m_context;

};

class SharingService :  public QDBusAbstractAdaptor, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","it.is.imposible")
public:
    using RequestHandler = std::function<void(const QString &path, const Request &request)>;
    explicit SharingService(const QString& serviceName, const QStringList& supportedFormats, RequestHandler handler, QObject* parent=nullptr); 
    bool start();
public slots:
    void OpenFile(const QString& path);
    
private:
    QString m_serviceName;
    QStringList m_supportedFormats;
    RequestHandler m_handler;
    QObject *parent; 
};

