#ifndef SECURE_H
#define SECURE_H

#include <QObject>
#include <QNetworkReply>

class SecurePrivate;

class SecureKeyCallback {
public:
    ~SecureKeyCallback();

    virtual void acceptKey(QByteArray key) = 0;
};

class Secure : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SecureKeyCallback* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit Secure(QObject *parent = 0);
    ~Secure();

    SecureKeyCallback* delegate() const;

    QUrl url() const;

signals:
    void delegateChanged(SecureKeyCallback* delegate);

    void urlChanged(QUrl arg);

public slots:
    void request();
    void responseReceived(QNetworkReply *reply);
    void setDelegate(SecureKeyCallback* delegate);

    void setUrl(QUrl url);

private:
    QScopedPointer<SecurePrivate> p;
    SecureKeyCallback* m_delegate;
    QUrl m_url;
};

#endif // SECURE_H
