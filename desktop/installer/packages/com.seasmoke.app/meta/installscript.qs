function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/appseasmoke.exe",
            "@StartMenuDir@/SeaSmoke.lnk",
            "workingDirectory=@TargetDir@",
            "description=SeaSmoke");

        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/appseasmoke.exe",
            "@DesktopDir@/SeaSmoke.lnk",
            "workingDirectory=@TargetDir@",
            "description=SeaSmoke");
    }
}
