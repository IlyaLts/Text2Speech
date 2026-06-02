TEMPLATE = aux

INSTALLER = Text2SpeechInstaller

INPUT = $$PWD/config/config.xml $$PWD/packages

installer.input = INPUT
installer.output = $$INSTALLER
installer.commands = binarycreator -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
installer.CONFIG += target_predeps no_link combine

QMAKE_EXTRA_COMPILERS += installer

DISTFILES += \
    config/ControlScript.qs \
    packages/Text2Speech/data/ChangeLog.txt \
    packages/Text2Speech/meta/InstallScript.qs \
    packages/Text2Speech/meta/package.xml

FORMS += \
    packages/Text2Speech/meta/TargetDirectory.ui
