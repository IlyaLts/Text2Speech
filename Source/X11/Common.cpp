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

#include "../Common.h"
#include <QClipboard>
#include <QElapsedTimer>
#include <QKeyCombination>
#include <QDebug>
#include <QApplication>
#include <QGuiApplication>
#include <QMessageBox>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <X11/keysymdef.h>

QMap<int, QKeyCombination> g_activeGrabs;

/*
===================
copyToClipboard
===================
*/
bool copyToClipboard()
{
    Display *display = XOpenDisplay(NULL);
    if (!display)
        return false;

    KeyCode kc_ctrl = XKeysymToKeycode(display, XK_Control_L);
    KeyCode kc_c = XKeysymToKeycode(display, XK_c);

    if (kc_ctrl == 0 || kc_c == 0)
    {
        XCloseDisplay(display);
        return false;
    }

    XTestFakeKeyEvent(display, kc_ctrl, True, CurrentTime);
    XTestFakeKeyEvent(display, kc_c, True, CurrentTime);
    XSync(display, false);

    QElapsedTimer timer;
    timer.start();

    // Waits for content to be cut
    while (!timer.hasExpired(1000))
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    XTestFakeKeyEvent(display, kc_c, False, CurrentTime);
    XTestFakeKeyEvent(display, kc_ctrl, False, CurrentTime);
    XSync(display, false);
    XCloseDisplay(display);

    return true;
}

/*
===================
registerShortcut
===================
*/
void registerShortcut(int id, const QKeyCombination &keyCombination)
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    QGuiApplication *guiApp = qobject_cast<QGuiApplication *>(coreApp);

    if (!guiApp)
        return;

    QNativeInterface::QX11Application *x11App = guiApp->nativeInterface<QNativeInterface::QX11Application>();

    if (!x11App)
    {
        QMessageBox::warning(nullptr, "Cannot register shortcut", "Not running on X11 or native interface not available.");
        qWarning() << "Cannot register shortcut: Not running on X11 or native interface not available.";
        return;
    }

    Display *display = x11App->display();
    Window root = DefaultRootWindow(display);
    int keycode = toNativeKey(keyCombination.key());

    if (keycode == 0)
        return;

    // For some reason, it works only with AnyModifier. So, we should check the modifier manually
    XGrabKey(display, keycode, AnyModifier, root, true, GrabModeAsync, GrabModeAsync);
    XSync(display, False);
    g_activeGrabs.insert(id, keyCombination);
}

/*
===================
unregisterShortcut
===================
*/
void unregisterShortcut(int id)
{
    if (!g_activeGrabs.contains(id))
        return;

    QCoreApplication *coreApp = QCoreApplication::instance();
    QGuiApplication *guiApp = qobject_cast<QGuiApplication *>(coreApp);

    if (!guiApp)
        return;

    QNativeInterface::QX11Application *x11App = guiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11App)
        return;

    Display *display = x11App->display();
    Window root = DefaultRootWindow(display);
    int keycode = toNativeKey(g_activeGrabs.value(id).key());

    XUngrabKey(display, keycode, AnyModifier, root);
    XSync(display, False);
    g_activeGrabs.remove(id);
}

/*
===================
toNativeModifier
===================
*/
unsigned int toNativeModifier(Qt::KeyboardModifiers modifiers)
{
    unsigned int mask = 0;

    if (modifiers.testFlag(Qt::ShiftModifier))
        mask |= ShiftMask;

    if (modifiers.testFlag(Qt::ControlModifier))
        mask |= ControlMask;

    if (modifiers.testFlag(Qt::AltModifier))
        mask |= Mod1Mask; // Alt

    if (modifiers.testFlag(Qt::MetaModifier))
        mask |= Mod4Mask; // Win

    return mask;
}

/*
===================
toNativeKey
===================
*/
unsigned int toNativeKey(Qt::Key key)
{
    QCoreApplication *coreApp = QCoreApplication::instance();

    QGuiApplication *guiApp = qobject_cast<QGuiApplication *>(coreApp);
    if (!guiApp)
        return 0;

    QNativeInterface::QX11Application *x11App = guiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11App)
        return 0;

    KeySym keysym = XStringToKeysym(QKeySequence(key).toString().toUtf8().toUpper().constData());
    return XKeysymToKeycode(x11App->display(), keysym);
}
