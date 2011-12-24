/*
 *  Copyright (C) 2011 Felix Geyer <debfx@fobos.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "KeyOpenDialog.h"
#include "ui_KeyOpenDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "keys/FileKey.h"
#include "keys/PasswordKey.h"

KeyOpenDialog::KeyOpenDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::KeyOpenDialog())
{
    m_ui->setupUi(this);

    m_ui->comboKeyFile->addItem("");
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(m_ui->buttonTogglePassword, SIGNAL(toggled(bool)), SLOT(togglePassword(bool)));
    connect(m_ui->buttonBrowseFile, SIGNAL(clicked()), SLOT(browseKeyFile()));

    connect(m_ui->editPassword, SIGNAL(textChanged(QString)), SLOT(activatePassword()));
    connect(m_ui->comboKeyFile, SIGNAL(editTextChanged(QString)), SLOT(activateKeyFile()));

    connect(m_ui->checkPassword, SIGNAL(toggled(bool)), SLOT(setOkButtonEnabled()));
    connect(m_ui->checkKeyFile, SIGNAL(toggled(bool)), SLOT(setOkButtonEnabled()));
    connect(m_ui->comboKeyFile, SIGNAL(editTextChanged(QString)), SLOT(setOkButtonEnabled()));

    connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(createKey()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

KeyOpenDialog::~KeyOpenDialog()
{
}

CompositeKey KeyOpenDialog::key()
{
    return m_key;
}

void KeyOpenDialog::createKey()
{
    if (m_ui->checkPassword->isChecked()) {
        m_key.addKey(PasswordKey(m_ui->editPassword->text()));
    }

    if (m_ui->checkKeyFile->isChecked()) {
        FileKey key;
        QString errorMsg;
        if (!key.load(m_ui->comboKeyFile->currentText(), &errorMsg)) {
            QMessageBox::warning(this, tr("Error"), tr("Can't open key file:\n%1").arg(errorMsg));
            return;
        }
        m_key.addKey(key);
    }

    accept();
}

void KeyOpenDialog::togglePassword(bool checked)
{
    m_ui->editPassword->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}

void KeyOpenDialog::activatePassword()
{
    m_ui->checkPassword->setChecked(true);
}

void KeyOpenDialog::activateKeyFile()
{
    m_ui->checkKeyFile->setChecked(true);
}

void KeyOpenDialog::setOkButtonEnabled()
{
    bool enable = m_ui->checkPassword->isChecked() || (m_ui->checkKeyFile->isChecked() && !m_ui->comboKeyFile->currentText().isEmpty());

    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

void KeyOpenDialog::browseKeyFile()
{
    QString filters = QString("%1 (*);;%2 (*.key)").arg(tr("All files"), tr("Key files"));
    QString filename = QFileDialog::getOpenFileName(this, "Select key file", QString(), filters);

    if (!filename.isEmpty()) {
        m_ui->comboKeyFile->setItemText(0, filename);
    }
}