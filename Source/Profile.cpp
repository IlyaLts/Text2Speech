/*
===============================================================================
    Copyright (C) 2025 Ilya Lyakhovets

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/

#include "Profile.h"
#include "Common.h"
#include "MainWindow.h"
#include <QSettings>
#include <QStandardPaths>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>

int Profile::nextId = 0;

/*
===================
Profile::Profile
===================
*/
Profile::Profile(QWidget* parent) : QWidget(parent)
{
    id = nextId;
    nextId++;

    tabLayout = new QVBoxLayout;

    enableCheckBox = new QCheckBox;
    shortcutLabel = new QLabel;
    shortcutEdit = new QKeySequenceEdit;
    modelLabel = new QLabel;
    modelComboBox = new QComboBox;
    voiceLabel = new QLabel;
    voiceComboBox = new QComboBox;
    keyLabel = new QLabel;
    keyLineEdit = new QLineEdit;

    tabLayout->addWidget(enableCheckBox);
    tabLayout->addWidget(shortcutLabel);
    tabLayout->addWidget(shortcutEdit);
    tabLayout->addWidget(modelLabel);
    tabLayout->addWidget(modelComboBox);
    tabLayout->addWidget(voiceLabel);
    tabLayout->addWidget(voiceComboBox);
    tabLayout->addWidget(keyLabel);
    tabLayout->addWidget(keyLineEdit);

    enableCheckBox->setChecked(true);
    shortcutEdit->setClearButtonEnabled(true);
    shortcutEdit->setMaximumSequenceLength(4);
    modelComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);
    voiceComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);
    keyLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    setLayout(tabLayout);

    connect(enableCheckBox, &QCheckBox::checkStateChanged, this, &Profile::enableStateChanged);
    connect(shortcutEdit, &QKeySequenceEdit::keySequenceChanged, this, &Profile::keySequenceChanged);
    connect(keyLineEdit, &QLineEdit::textChanged, this, &Profile::keyChanged);
    connect(modelComboBox, &QComboBox::activated, this, &Profile::modelChanged);
    connect(voiceComboBox, &QComboBox::activated, this, &Profile::voiceChanged);
}

/*
===================
Profile::setProviders
===================
*/
void Profile::setProviders(QMap<QString, ModelProvider> &providers)
{
    modelComboBox->clear();

    for (auto it = providers.begin(); it != providers.end(); it++)
        for (auto &model : it.value().models)
            modelComboBox->addItem(it.key() + "/" + model, QVariant::fromValue(QPair(it.key(), &it.value())));
}

/*
===================
Profile::retranslate
===================
*/
void Profile::retranslate()
{
    enableCheckBox->setText(QCoreApplication::translate("MainWindow", "Enable"));
    shortcutLabel->setText(QCoreApplication::translate("MainWindow", "Shortcut:"));
    modelLabel->setText(QCoreApplication::translate("MainWindow", "Model:"));
    voiceLabel->setText(QCoreApplication::translate("MainWindow", "Voice:"));
    keyLabel->setText(QCoreApplication::translate("MainWindow", "API key:"));

    if (QLineEdit *lineEdit = shortcutEdit->findChild<QLineEdit *>("qt_keysequenceedit_lineedit"))
        lineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Press shortcut"));
}

/*
===================
Profile::readSettings
===================
*/
void Profile::readSettings()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + SETTINGS_FILENAME, QSettings::IniFormat);
    ModelProvider *provider = modelComboBox->currentData().value<QPair<QString, ModelProvider *>>().second;

    settings.beginGroup("Profile" + QString::number(id));

    shortcutEdit->setKeySequence(QKeySequence(settings.value("Shortcut").toString()));

    if (modelComboBox->count())
    {
        // Loads the last-used model
        QString currentModel = settings.value("CurrentModel").toString();
        int index = modelComboBox->findText(currentModel);

        if (index >= 0)
            modelComboBox->setCurrentIndex(index);

        for (auto &voice : provider->voices)
            voiceComboBox->addItem(voice);

        index = voiceComboBox->findText(settings.value("CurrentVoice", voice()).toString());

        if (index >= 0)
            voiceComboBox->setCurrentIndex(index);
    }

    if (voiceComboBox->count())
    {
        // Loads the last-used model
        QString currentVoice = settings.value("CurrentVoice", provider->defaultVoice).toString();
        int index = voiceComboBox->findText(currentVoice);

        if (index >= 0)
            voiceComboBox->setCurrentIndex(index);
    }

    enableCheckBox->setChecked(settings.value(QString("Profile") + "_Enabled", true).toBool());
    settings.endGroup();
}

/*
===================
Profile::writeSettings
===================
*/
void Profile::writeSettings() const
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + SETTINGS_FILENAME, QSettings::IniFormat);

    settings.beginGroup("Profile" + QString::number(id));
    settings.setValue("Shortcut", shortcutEdit->keySequence());

    if (modelComboBox->count())
        settings.setValue("CurrentModel", modelComboBox->currentText());

    settings.setValue(QString("Profile") + "_Enabled", enableCheckBox->isChecked());
    settings.endGroup();
}

/*
===================
Profile::enableStateChanged
===================
*/
void Profile::enableStateChanged(Qt::CheckState state)
{
    bool enable = false;

    int offset = id * NumberOfProfiles;

    if (state == Qt::Unchecked)
    {
        enable = false;

        for (int i = 0; i < 4; i++)
            unregisterShortcut(offset + i);
    }
    else if (state == Qt::Checked)
    {
        enable = true;

        for (int i = 0; i < shortcutEdit->keySequence().count(); i++)
            registerShortcut(offset + i, shortcutEdit->keySequence()[i]);
    }

    shortcutLabel->setEnabled(enable);
    shortcutEdit->setEnabled(enable);
    modelLabel->setEnabled(enable);
    modelComboBox->setEnabled(enable);
    voiceLabel->setEnabled(enable);
    voiceComboBox->setEnabled(enable);
    keyLabel->setEnabled(enable);
    keyLineEdit->setEnabled(enable);
}

/*
===================
Profile::keySequenceChanged
===================
*/
void Profile::keySequenceChanged(const QKeySequence &keySequence)
{
    // Translates shortcut field placeholder
    if (keySequence.isEmpty())
        retranslate();

    int offset = id * NumberOfProfiles;

    for (int i = 0; i < 4; i++)
        unregisterShortcut(offset + i);

    for (int i = 0; i < keySequence.count(); i++)
        registerShortcut(offset + i, keySequence[i]);

    shortcutEdit->clearFocus();
}

/*
===================
Profile::modelChanged
===================
*/
void Profile::modelChanged()
{
    if (!modelComboBox->count())
        return;

    voiceComboBox->clear();
    voiceComboBox->addItems(QList<QString>(currentProvider().second->voices.begin(), currentProvider().second->voices.end()));

    int index = voiceComboBox->findText(currentVoices[currentProvider().first]);

    if (index >= 0)
        voiceComboBox->setCurrentIndex(index);

    keyLineEdit->setText(currentKeys[currentProvider().first]);
}

/*
===================
Profile::voiceChanged
===================
*/
void Profile::voiceChanged(int index)
{
    if (!modelComboBox->count())
        return;

    currentVoices[currentProvider().first] = voiceComboBox->itemText(index);
}

/*
===================
Profile::keyChanged
===================
*/
void Profile::keyChanged(const QString &key)
{
    if (!modelComboBox->count())
        return;

    currentKeys[currentProvider().first] = key;
}
