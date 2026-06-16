/*
===============================================================================
    Copyright (C) 2026 Ilya Lyakhovets

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

#ifndef PROFILE_H
#define PROFILE_H

#include <QWidget>
#include <QComboBox>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QTextEdit>

class QCheckBox;
class QVBoxLayout;
class ModelProvider;
class QLabel;
class QKeySequenceEdit;

/*
===========================================================

    Profile

===========================================================
*/
class Profile : public QWidget
{
    Q_OBJECT

public:

    Profile(QWidget* parent = nullptr);

    void setProviders(QMap<QString, ModelProvider> &providers);
    void retranslate();
    void readSettings();
    void writeSettings() const;

    inline QKeySequence keySequence() const { return shortcutEdit->keySequence(); }
    inline QPair<QString, ModelProvider *> currentProvider() const { return modelComboBox->currentData().value<QPair<QString, ModelProvider *>>(); }
    inline QString model() const { return modelComboBox->currentText().sliced(currentProvider().first.size() + 1); }
    inline QString voice() const { return voiceComboBox->currentText(); }
    inline QString key() const { return keyLineEdit->text(); }

private Q_SLOTS:

    void enableStateChanged(Qt::CheckState state);
    void keySequenceChanged(const QKeySequence &keySequence);
    void modelChanged();
    void keyChanged(const QString &key);
    void voiceChanged(int index);

private:

    static int nextId;

public:

    QMap<QString, QString> currentKeys;
    QMap<QString, QString> currentVoices;
    int id;
    QCheckBox *enableCheckBox;
    QVBoxLayout *tabLayout;
    QLabel *shortcutLabel;
    QKeySequenceEdit *shortcutEdit;
    QLabel *modelLabel;
    QComboBox *modelComboBox;
    QLabel *voiceLabel;
    QComboBox *voiceComboBox;
    QLabel *keyLabel;
    QLineEdit *keyLineEdit;
};

#endif // PROFILE_H
