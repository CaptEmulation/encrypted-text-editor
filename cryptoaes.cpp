#include "cryptoaes.h"

#include <string>
using std::string;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::ArraySink;
using CryptoPP::StringSource;
using CryptoPP::ArraySource;
using CryptoPP::StreamTransformationFilter;

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "aes.h"
using CryptoPP::AES;

#include "ccm.h"
using CryptoPP::CBC_Mode;
using CryptoPP::CBC_Mode_ExternalCipher;

#include "modes.h"
#include "QDebug"
#include "QString"

void QByteArrayToUCharArray(QByteArray src, unsigned char *dest)
{
    for (int i = 0; i < src.size(); i++)
    {
        dest[i] = src.at(i);
    }
}

QByteArray UCharArrayToQByteArray(unsigned char *src, int p_size)
{
    QByteArray array((char*)src, p_size);
    return array;
}

class CryptoAesPrivate {
public:
    CryptoPP::AutoSeededRandomPool rnd;
    QByteArray m_key;

    QByteArray key() const {
        return m_key;
    }

    const byte *keyByte() const {
        return (byte*)key().data();
    }

    int keySize() {
        return key().size();
    }

    void setKey(QByteArray key) {
        if (key != m_key) {
            m_key = key;
        }
    }

    QByteArray generateKey() {
        byte key[32];
        rnd.GenerateBlock(key, 32);
        return UCharArrayToQByteArray(key, 32);
    }

    QByteArray encrypt(QByteArray in, QByteArray key) {
        setKey(key);
        return encrypt(in);
    }

    QByteArray encrypt(QByteArray in) {
        byte iv[CryptoPP::AES::BLOCKSIZE];
        rnd.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);
        QByteArray out = QByteArray((char*)iv, CryptoPP::AES::BLOCKSIZE);
        int inputSize = in.size();
        string cipher;

        CBC_Mode<AES>::Encryption aes(keyByte(), keySize(), iv);
        ArraySource((byte *)in.data(), inputSize, true, new StreamTransformationFilter(aes, new StringSink(cipher)));

        QByteArray encryptedBytes = QByteArray(cipher.c_str(), cipher.size());
        out.append(encryptedBytes);
        return out;
    }

    QByteArray decrypt(QByteArray in, QByteArray key) {
        setKey(key);
        return decrypt(in);
    }

    QByteArray decrypt(QByteArray in) {
        QByteArray iv = in.left(AES::BLOCKSIZE);
        in.remove(0, AES::BLOCKSIZE);
        string decrypted;
        QByteArray result;

        try {
            CBC_Mode<AES>::Decryption aes_dec;
            aes_dec.SetKeyWithIV((byte *)key().data(), keySize(), (byte *)iv.data());
            ArraySource((byte *)in.data(), in.size(), true,
                        new StreamTransformationFilter(aes_dec,
                                                   new StringSink(decrypted)));

            result = QByteArray(decrypted.c_str(), in.size());
        } catch (CryptoPP::Exception err) {
            result = "Failed to decrypt";
            qDebug() << QString(err.GetWhat().c_str());
        }

        return result;
    }
};

CryptoAes::CryptoAes() :
    p(new CryptoAesPrivate)
{
}

CryptoAes::~CryptoAes() {

}

QByteArray CryptoAes::key() const
{
    return p->key();
}

void CryptoAes::setKey(QByteArray key)
{
    p->setKey(key);
}

QByteArray CryptoAes::encrypt(QByteArray in, QByteArray key)
{
    return p->encrypt(in, key);
}

QByteArray CryptoAes::decrypt(QByteArray in, QByteArray key)
{
    return p->decrypt(in, key);
}

QByteArray CryptoAes::generateKey()
{
    return p->generateKey();
}
