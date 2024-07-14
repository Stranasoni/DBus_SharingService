#include <QtDBus/QtDBus>

class TextEditor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.example.texteditor")
public:
    explicit TextEditor(QObject* parent, const QStringList& supported_format);
public slots:
    void OpenFile(const QString& path);

private:
    const QStringList supported_format;

};