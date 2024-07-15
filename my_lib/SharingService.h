#pragma once
#include <QtDBus/QtDBus>
#include <QObject>
#include <QString>
#include <QStringList>
#include <functional>


class SharingService : public QDBusAbstractAdaptor, public QDBusContext
{
    Q_OBJECT
    //Q_CLASSINFO("D-Bus Interface","com.system.sharing")
public:
    using RequestHandler = std::function<void(const QString& path, const QDBusMessage& message)>;

    explicit SharingService(QObject *parent, const QString& serviceName, const QStringList& supportedFormats, RequestHandler handler);
    virtual ~SharingService();

    bool start();

public slots:
    void OpenFile(const QString& path);

private:
    QString m_serviceName;
    QStringList m_supportedFormats;
    RequestHandler m_handler;
};
