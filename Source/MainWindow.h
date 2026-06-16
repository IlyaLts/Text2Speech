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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Application.h"
#include "NativeEventFilter.h"
#include "Profile.h"
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSoundEffect>
#include <QTimer>

static constexpr quint64 SmoothTypingDelay = 250;
static constexpr quint64 MaxTimeout = 30000;
static constexpr int NumberOfProfiles = 4;

extern const char *defaultPrompt;
extern const char *hiddenPrompt;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UnhidableMenu;

struct ModelProvider
{
    QString name;
    QString url;
    QSet<QString> models;
    QSet<QString> voices;
    QString defaultVoice;
};

/*
===========================================================

    MainWindow

===========================================================
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void text2Speech(int id);
    QKeySequence keySequence(int id) const { return profiles[id]->keySequence(); };

public Q_SLOTS:

    void show();

protected:

    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:

    void quit();
    void setTrayVisible(bool visible);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void switchLanguage(QLocale::Language language);
    void toggleNotificationSound();
    void toggleSmoothTyping();
    void toggleLaunchOnStartup();
    void toggleShowInTray();
    void openConfig();

private:

    void setupMenus();
    void readSettings();
    void writeSettings() const;
    void retranslate();
    void restoreClipboard(const QString &savedClipboard);
    void message(const QString &message);
    QByteArray synthesizeSpeechOpenai(Profile &profile, const QString &input);
    QByteArray synthesizeSpeechGemini(Profile &profile, const QString &input);

    NativeEventFilter filter;

    Ui::MainWindow *ui;

    QIcon iconMain;
    QIcon iconSettings;

    Profile *profiles[NumberOfProfiles];
    QMap<QString, ModelProvider> providers;
    QList<QAction *> languageActions;
    QAction *notificationSoundAction;
    QAction *launchOnStartupAction;
    QAction *showInTrayAction;
    QAction *openConfigAction;
    QAction *showAction;
    QAction *quitAction;
    QAction *reportBugAction;
    QAction *version;

    QSystemTrayIcon *trayIcon;
    UnhidableMenu *trayIconMenu;
    UnhidableMenu *settingsMenu;
    UnhidableMenu *languageMenu;

    QSoundEffect clock;
    QLocale::Language language;
    bool appInitiated = false;
    int smoothTypingDelay = SmoothTypingDelay;
    int32_t maxTimeout = 30000;
};

#endif // MAINWINDOW_H
