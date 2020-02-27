// Here's a more complete example, including strings representing all available components as of 10/10/19:
// https://github.com/appveyor/build-images/blob/615af0becbbc30d3350de9ace7d2095f9e2c6338/scripts/Windows/qt-installer-windows.qs

function Controller() {
    installer.autoRejectMessageBoxes();
    installer.setMessageBoxAutomaticAnswer("installationErrorWithRetry", QMessageBox.Ignore);
    installer.setMessageBoxAutomaticAnswer("installationError", QMessageBox.Ignore);
    installer.installationFinished.connect(function() {
        gui.clickButton(buttons.NextButton);
    });
}

Controller.prototype.WelcomePageCallback = function() {
    // click delay here because the next button is initially disabled for ~1 second
    gui.clickButton(buttons.NextButton, 10000);
}

Controller.prototype.CredentialsPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.DynamicTelemetryPluginFormCallback = function() {
    var widget = gui.currentPageWidget();
    widget.TelemetryPluginForm.statisticGroupBox.disableStatisticRadioButton.checked = true;
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
    var qtPath = undefined;

    switch (installer.value("buildtype")) {
        case "linux": {
            qtPath = "/qt";
            break;
        }

        case "mingw": {
            qtPath = "D:\\Qt";
            break;
        }

        case "msvc": {
            qtPath = "D:\\Qt";
            break;
        }
    }

    gui.currentPageWidget().TargetDirectoryLineEdit.setText(qtPath);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
    // var page = gui.pageWidgetByObjectName("ComponentSelectionPage");

    // var checkBox = gui.findChild(page, "Latest releases");
    // var fetchButton = gui.findChild(page, "FetchCategoryButton");

    // checkBox.click();
    // fetchButton.click();

    var widget = gui.currentPageWidget();

    widget.deselectAll();

    switch (installer.value("buildtype")) {
        case "linux": {
            widget.selectComponent("qt.qt5.5140.gcc_64");
            break;
        }

        case "mingw": {
            widget.selectComponent("qt.qt5.5140.win64_mingw73");
            widget.selectComponent("qt.tools.win64_mingw730");
            break;
        }

        case "msvc": {
            widget.selectComponent("qt.qt5.5140.win64_msvc2017_64");
            break;
        }

        default: break;
    }

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
    gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
    var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm;
    if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
        checkBoxForm.launchQtCreatorCheckBox.checked = false;
    }
    gui.clickButton(buttons.FinishButton);
}
