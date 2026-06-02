function Controller()
{
    if (installer.value("os") == "win")
    {
        installer.setValue("ProductUUID", "Text2Speech");

        var previous = installer.value("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Text2Speech\\InstallLocation");
        if(previous !== "")
        {
            installer.setValue("TargetDir", previous);
            installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
        }
    }
}

Controller.prototype.IntroductionPageCallback = function()
{
    gui.clickButton(buttons.NextButton);
}
