#include "secure.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>

// Useless destructor definition
SecureKeyCallback::~SecureKeyCallback(){}

class SecurePrivate {
public:
    Secure *self;

    QNetworkAccessManager m_network;
    SecureKeyCallback *m_delegate;
    QUrl m_url;

    SecureKeyCallback* delegate() const
    {
        return m_delegate;
    }

    void setDelegate(SecureKeyCallback* delegate)
    {
        if (m_delegate != delegate) {
            m_delegate = delegate;
            emit self->delegateChanged(delegate);
        }
    }

    QUrl url() const
    {
        return m_url;
    }

    void setUrl(QUrl url)
    {
        if (m_url != url) {
            m_url = url;
            emit self->urlChanged(url);
        }
    }

    void request() {
        QUrl requestUrl = url();
        //requestUrl.addQueryItem();
        QNetworkRequest request(requestUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        m_network.get(request);
    }

    void responseReceived(QNetworkReply *reply) {
        QByteArray bytes(reply->readAll());
        QString text(QString::fromUtf8(bytes));
        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        QByteArray key(QByteArray::fromBase64(doc.object().value("key").toVariant().toString().toUtf8()));
        m_delegate->acceptKey(key);
    }
};

Secure::Secure(QObject *parent) :
    QObject(parent),
    p(new SecurePrivate)
{
    p->self = this;
    QObject::connect(&p->m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseReceived(QNetworkReply*)));
}

Secure::~Secure()
{

}

SecureKeyCallback *Secure::delegate() const
{
    return p->delegate();
}

QUrl Secure::url() const
{
    return p->url();
}

void Secure::request()
{
    p->request();
}

void Secure::responseReceived(QNetworkReply *reply)
{
    p->responseReceived(reply);
}

void Secure::setDelegate(SecureKeyCallback *delegate)
{
    p->setDelegate(delegate);
}

void Secure::setUrl(QUrl url)
{
    p->setUrl(url);
}



