#ifndef CRYPTOAES_H
#define CRYPTOAES_H

#include "QScopedPointer"
#include "QByteArray"

class CryptoAesPrivate;

class CryptoAes {

public:
    CryptoAes();
    ~CryptoAes();

    QByteArray key() const;
    void setKey(QByteArray key);

    QByteArray encrypt(QByteArray in, QByteArray key);
    QByteArray decrypt(QByteArray in, QByteArray key);
    QByteArray generateKey();

private:
    QScopedPointer<CryptoAesPrivate> p;
};

void QByteArrayToUCharArray(QByteArray src, unsigned char *dest);
QByteArray UCharArrayToQByteArray(unsigned char *src, int p_size);


#endif // CRYPTOAES_H
