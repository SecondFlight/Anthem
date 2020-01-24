/*
    Copyright (C) 2019, 2020 Joshua Wade

    This file is part of Anthem.

    Anthem is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Anthem is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Anthem. If not, see
                        <https://www.gnu.org/licenses/>.
*/

#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

#include <QObject>
#include <QString>
#include <QVector>

#include "Include/rapidjson/document.h"

#include "Model/project.h"
#include "Utilities/projectfile.h"
#include "Utilities/idgenerator.h"
#include "Core/communicator.h"
#include "Core/engine.h"
#include "Utilities/patch.h"
#include "patternpresenter.h"

class MainPresenter : public Communicator {
    Q_OBJECT
private:
    void emitAllChangeSignals();

    void connectUiUpdateSignals(Project* project);
    void disconnectUiUpdateSignals(Project* project);

    /// If there isn't an active (non-sent) patch in
    /// the list, add one.
    void initializeNewPatchIfNeeded();

    /// Used to track whether the user has 1) opened a
    /// project or 2) modified the blank project since
    /// the software was launched. This determines
    /// whether "File -> Open project" should replace
    /// the current tab or use a new one.
    bool isInInitialState;

    bool isActiveProjectValid;

    /// List of currently open projects
    QVector<Project*> projects;

    /// List of project files (mirrors list of projects)
    QVector<ProjectFile*> projectFiles;

    /// Handles engine lifecycle and communication.
    QVector<Engine*> engines;

    /// Current place in the history
    QVector<int> historyPointers;

    /// List of project histories
    QVector<QVector<Patch*>> projectHistories;
    bool isPatchInProgress;

    /// API for the pattern editor
    PatternPresenter* patternEditor;

    IdGenerator* id;

public:
    explicit MainPresenter(QObject* parent, IdGenerator* id);

    // Implementations of virtual functions in Communicator
    void patchAdd(QString path, rapidjson::Value& value);
    void patchRemove(
        QString path, rapidjson::Value& oldValue
    );
    void patchReplace(
        QString path,
        rapidjson::Value& oldValue,
        rapidjson::Value& newValue
    );
    void patchCopy(QString from, QString path);
    void patchMove(QString from, QString path);
    void sendPatch();

    void liveUpdate(uint64_t controlId, float value);

    /// Project that is currently loaded
    int activeProjectIndex;

    // Used to access and manipulate project data
    void addProject(
        Project* project,
        ProjectFile* projectFile,
        Engine* engine
    );
    void removeProjectAt(int index);
    Project* getProjectAt(int index);
    ProjectFile* getProjectFileAt(int index);
    Engine* getEngineAt(int index);
    QVector<Patch*> getProjectHistoryAt(int index);
    int getHistoryPointerAt(int index);

signals:
    void tabAdd(QString name);
    void tabRename(int index, QString name);
    void tabSelect(int index);
    void tabRemove(int index);

    /// Signal to tell the UI to open a save dialog
    void saveDialogRequest();

    /// Signal to tell the UI to display an information
    /// dialog to the user
    void informationDialogRequest(
        QString title, QString notification
    );

    /// Emitted when a save has been completed.
    void saveCompleted();

    /// Emitted when a save has been cancelled.
    void saveCancelled();

    /// Emitted when a status message should be displayed
    void statusMessageRequest(QString message);


    // Update signals for UI elements
    void masterPitchChanged(int pitch);
    void beatsPerMinuteChanged(float bpm);
    void timeSignatureNumeratorChanged(quint8 numerator);
    void timeSignatureDenominatorChanged(quint8 denominator);

public slots:
    void newProject();
    void loadProject(QString path);
    void saveActiveProject();
    void saveActiveProjectAs(QString path);

    /// Checks if the given project has ever been saved,
    /// or was opened from a file
    bool isProjectSaved(int projectIndex);

    /// Checks if the given project has unsaved changes
    bool projectHasUnsavedChanges(int projectIndex);

    /// Raises a signal to tell the UI to open a save
    /// dialog.
    void openSaveDialog();

    /// Raises a signal notifying listeners that a save
    /// operation has completed successfully.
    void notifySaveCancelled();

    /// Raises a signal notifying listeners that a save
    /// operation was cancelled by the user.
    void notifySaveCompleted();

    int getNumOpenProjects();

    void undo();
    void redo();

    // These functions do not update the tab state
    // in the UI
    void switchActiveProject(int index);
    /// Does not update the active project
    void closeProject(int index);

    /// Tells the UI to display the given string as a
    /// status message.
    void displayStatusMessage(QString message);


    // Getters and setters for model properties
    int getMasterPitch();
    void setMasterPitch(int pitch, bool isFinal);
    float getBeatsPerMinute();
    void setBeatsPerMinute(float bpm, bool isFinal);
    quint8 getTimeSignatureNumerator();
    void setTimeSignatureNumerator(quint8 numerator);
    quint8 getTimeSignatureDenominator();
    void setTimeSignatureDenominator(quint8 denominator);

    // Functions with the ui_ prefix are used as
    // receiver slots for model change signals. Each
    // ui_ function should:
    //     a) always be connected to the relevant
    //        model's update signals, and
    //     b) emit the relevant update signal.
    void ui_updateMasterPitch(float pitch);
    void ui_updateBeatsPerMinute(float bpm);
    void ui_updateTimeSignatureNumerator(quint8 numerator);
    void ui_updateTimeSignatureDenominator(quint8 denominator);
};

#endif // MAINPRESENTER_H
