/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "mdichild.h"
#include "cryptoaes.h"
#include "passworddialog.h"
#include "finddialog.h"
#include "finddialog.h"
#include "cryptoaes.h"
#include "sha.h"
using CryptoPP::SHA256;
#include "aes.h"
using CryptoPP::AES;

class MdiChildPrivate {
public:
    MdiChild *self;

    QString m_fileName;
    PasswordDialog *m_passwordDialog;
    FindDialog *m_findDialog;
    QByteArray m_key;
    QTextCursor m_lastFindCursor;
    QString m_lastFindText;

    QString fileName() const {
        return m_fileName;
    }

    void setFileName(QString fileName) {
        if (fileName != m_fileName) {
            m_fileName = fileName;
        }
    }

    PasswordDialog *passwordDialog() const {
        return m_passwordDialog;
    }

    void setPasswordDialog(PasswordDialog *passwordDialog) {
        if (passwordDialog != m_passwordDialog) {
            m_passwordDialog = passwordDialog;
        }
    }

    FindDialog *findDialog() const {
        return m_findDialog;
    }

    void setFindDialog(FindDialog *findDialog) {
        if (findDialog != m_findDialog) {
            m_findDialog = findDialog;
        }
    }


    QByteArray key() {
        return m_key;
    }

    void setKey(QByteArray key) {
        if (m_key != key) {
            m_key = key;
        }
    }

    QTextCursor lastFindCursor() {
        return m_lastFindCursor;
    }

    void setLastFindCursor(QTextCursor cursor) {
        if (m_lastFindCursor != cursor) {
            m_lastFindCursor = cursor;
        }
    }

    QString lastFindText() {
        return m_lastFindText;
    }

    void setLastFindText(QString lastFindText) {
        if (m_lastFindText != lastFindText) {
            m_lastFindText = lastFindText;
        }
    }

    void createKey() {
        QString passwordText = passwordDialog()->password();
        byte out[SHA256::DIGESTSIZE];
        SHA256().CalculateDigest(out, (byte*)passwordText.toUtf8().data(), passwordText.size());
        setKey(UCharArrayToQByteArray(out, SHA256::DIGESTSIZE));
        qDebug() << "Key length" << key().length();
    }

    void destroyPasswordDialog() {
        passwordDialog()->close();
        passwordDialog()->deleteLater();
        setPasswordDialog(0);
    }

    void destroyFindFialog() {
        if (findDialog()) {
            findDialog()->close();
            findDialog()->deleteLater();
            setFindDialog(0);
        }
    }

    void findAccept() {
        setLastFindText(findDialog()->findText());
        findNext();
    }

    void findNext() {
        if (lastFindText().length()) {
            setLastFindCursor(self->document()->find(lastFindText(), lastFindCursor()));
            self->setTextCursor(lastFindCursor());
        }
        destroyFindFialog();
    }

    void findReject() {
        destroyFindFialog();
    }

    void find() {
        createFindDialog();
    }

    void createFindDialog() {
        setFindDialog(new FindDialog(self));
        QObject::connect(findDialog(), SIGNAL(accepted()), self, SLOT(findAccept()));
        QObject::connect(findDialog(), SIGNAL(rejected()), self, SLOT(findReject()));
        findDialog()->show();
    }

    void loadAccept() {
        createKey();
        self->loadFile(fileName());
        destroyPasswordDialog();
    }

    void loadReject() {
        destroyPasswordDialog();
    }

    void saveAccept() {
        createKey();
        self->saveFile(fileName());
        destroyPasswordDialog();
    }

    void saveReject() {
        destroyPasswordDialog();
    }

    void createPasswordDialog() {
        setPasswordDialog(new PasswordDialog(self));
        QObject::connect(passwordDialog(), SIGNAL(accepted()), self, SLOT(loadAccept()));
        QObject::connect(passwordDialog(), SIGNAL(rejected()), self, SLOT(loadReject()));
        passwordDialog()->show();
    }

    void openPasswordDialogForLoad(QString fileName) {
        if (!passwordDialog()) {
            setFileName(fileName);
            createPasswordDialog();
        } else if (key().length()) {
            self->loadFile(fileName);
        }
    }

    void openPasswordDialogForSave(QString fileName) {
        if (!passwordDialog()) {
            setFileName(fileName);
            createPasswordDialog();
        } else if (key().length()) {
            self->saveFile(fileName);
        }
    }
};

MdiChild::MdiChild() :
    p(new MdiChildPrivate)
{
    p->self = this;
    p->m_passwordDialog = 0;
    p->m_findDialog = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

MdiChild::~MdiChild()
{

}

void MdiChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool MdiChild::load(const QString &fileName)
{
        p->openPasswordDialogForLoad(fileName);
        return true;
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    setHtml(QString::fromUtf8(CryptoAes().decrypt(file.readAll().remove(0, 4), p->key())));
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    return true;
}

bool MdiChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MdiChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    p->openPasswordDialogForSave(fileName);
    return true;
}

void MdiChild::find()
{
    p->find();
}

void MdiChild::findNext()
{
    p->findNext();
}

bool MdiChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QByteArray scrambled(CryptoAes().encrypt(toHtml().toUtf8(), p->key()));
    out << scrambled;
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

void MdiChild::loadAccept()
{
    p->loadAccept();
}

void MdiChild::loadReject()
{
    p->loadReject();
}

void MdiChild::saveAccept()
{
    p->saveAccept();
}

void MdiChild::saveReject()
{
    p->saveReject();;
}

void MdiChild::findAccept()
{
    p->findAccept();
}

void MdiChild::findReject()
{
    p->findReject();
}

bool MdiChild::maybeSave()
{
    if (document()->isModified()) {
	QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
		     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

