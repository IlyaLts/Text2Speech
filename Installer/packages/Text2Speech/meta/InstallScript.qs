function Component()
{
    installer.setDefaultPageVisible(QInstaller.Introduction, false);
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, true);
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
    
    installer.currentPageChanged.connect(this, Component.prototype.currentPageChanged);
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    
    if (installer.isInstaller())
    {
        var targetDirectory = component.userInterface("targetDirectoryForm");

        if (installer.value("os") == "win")
        {
            if (targetDirectory && targetDirectory.createShortcutOnDesktopCheckBox.checked)
            {
                component.addOperation("CreateShortcut", "@TargetDir@/Text2Speech.exe",
                                       "@DesktopDir@/Text2Speech.lnk",
                                       "workingDirectory=@TargetDir@",
                                       "iconPath=@TargetDir@/Text2Speech.exe",
                                       "iconId=0", "description=Text2Speech");
            }

            component.addOperation("CreateShortcut", "@TargetDir@/Text2Speech.exe",
                                   "@UserStartMenuProgramsPath@/@StartMenuDir@/Text2Speech.lnk",
                                   "workingDirectory=@TargetDir@",
                                   "iconPath=@TargetDir@/Text2Speech.exe",
                                   "iconId=0", "description=Text2Speech");

            component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe",
                                   "@UserStartMenuProgramsPath@/@StartMenuDir@/Maintenance Tool.lnk",
                                   "workingDirectory=@TargetDir@",
                                   "iconPath=@TargetDir@/Text2Speech.exe",
                                   "iconId=0", "description=Text2Speech");
        }
        else if (installer.value("os") == "x11")
        {
            if (targetDirectory && targetDirectory.createShortcutOnDesktopCheckBox.checked)
            {
                component.addElevatedOperation("CreateDesktopEntry",
                                               "@HomeDir@/Desktop/Text2Speech.desktop",
                                               "Version=1.0\nType=Application\nName=Text2Speech\nTerminal=false\nExec=@TargetDir@/Text2Speech\nIcon=@TargetDir@/Icon.png\nCategories=Utilities");
            }

            component.addElevatedOperation("CreateDesktopEntry",
                                           "/usr/share/applications/Text2Speech.desktop",
                                           "Version=1.0\nType=Application\nName=Text2Speech\nTerminal=false\nExec=@TargetDir@/Text2Speech\nIcon=@TargetDir@/Icon.png\nCategories=Utilities");
        }
    }
}

Component.prototype.currentPageChanged = function(page)
{
    try
    {
        if (installer.isInstaller() && installer.value("os") == "win")
        {
            if (page == QInstaller.TargetDirectory)
                installer.addWizardPageItem(component, "targetDirectoryForm", QInstaller.TargetDirectory);
        }
    }
    catch(e)
    {
        console.log(e);
    }
}
