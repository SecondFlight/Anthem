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

#ifndef MODELTESTS_H
#define MODELTESTS_H

#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

#include "Presenter/mainpresenter.h"

#include "Include/rapidjson/pointer.h"

Q_DECLARE_METATYPE(PatchFragment::PatchType);
Q_DECLARE_METATYPE(rapidjson::Value*);

class PresenterEventCounter : public QObject {
Q_OBJECT

public:
    int masterPitchEventCount;
    int mostRecentMasterPitch;
    PresenterEventCounter(QObject* parent) : QObject(parent) {
        masterPitchEventCount = 0;
        mostRecentMasterPitch = 0;
    }
public slots:
    void masterPitchChanged(int pitch) {
        mostRecentMasterPitch = pitch;
        masterPitchEventCount++;
    }
};

class ModelTests : public QObject {
Q_OBJECT

private:
    IdGenerator* id;
    MainPresenter* presenter;
    PresenterEventCounter* eventCounter;
    Project* project;

    rapidjson::Document doc;

    rapidjson::Value basicAddValue;
    rapidjson::Value basicAddValueOld;
    rapidjson::Value basicRemoveValue;
    rapidjson::Value basicRemoveValueOld;
    rapidjson::Value basicReplaceValue;
    rapidjson::Value basicReplaceValueOld;
    rapidjson::Value basicCopyValue;
    rapidjson::Value basicCopyValueOld;
    rapidjson::Value basicMoveValue;
    rapidjson::Value basicMoveValueOld;

private slots:
    void initTestCase() {
        id = new IdGenerator();
        presenter = new MainPresenter(this, id);

        eventCounter = new PresenterEventCounter(this);

        QObject::connect(presenter,    SIGNAL(masterPitchChanged(int)),
                         eventCounter, SLOT(masterPitchChanged(int)));

        project = presenter->getProjectAt(0);

        doc.Parse(
            "{"
            "    \"basic_remove\": \"init value\","
            "    \"basic_replace\": \"init value\","
            "    \"basic_copy_source\": \"init value\","
            "    \"basic_move_source\": \"init value\""
            "}"
        );

        basicAddValue.SetString("add");
        basicAddValueOld.SetNull();
        basicRemoveValue.SetNull();
        basicRemoveValueOld.SetString("init value");
        basicReplaceValue.SetString("replace");
        basicReplaceValueOld.SetString("init value");
        basicCopyValue.SetNull();
        basicCopyValueOld.SetNull();
        basicMoveValue.SetNull();
        basicMoveValueOld.SetNull();
    }

    void emptyProject() {
        qDebug() << "Initial project state";
        QCOMPARE(project->getTransport()->masterPitch->get(), 0.0f);



        qDebug() << "Direct item set";

        // Set the value to -5, but send a live update instead of a patch.
        project->getTransport()->masterPitch->set(-5.0f, false);

        // The control should report the newly set value.
        QCOMPARE(project->getTransport()->masterPitch->get(), -5.0f);

        // The signal for updating the UI should not have fired because the change does
        // not represent a change to the model. Arguably it shouldn't fire at all, but
        // I've made the decision to update the UI on any final model change so I don't
        // need to distinguish between updates coming from the UI and upadtes coming
        // from undo and redo. This reasoning may not hold with models that are managing
        // a higher degree of complexity (as in, the extra update may be unwanted), but
        // for a single value it doesn't seem too bad. We'll see if I change my mind :)
        QCOMPARE(eventCounter->masterPitchEventCount, 0);



        // Set the value to 10, and send a patch (final value in a channge operation).
        project->getTransport()->masterPitch->set(10.0f, true);

        // The control should report the newly set value.
        QCOMPARE(project->getTransport()->masterPitch->get(), 10.0f);

        // This is a final (patch-emitting) change, so the UI should have been notified
        // (see above)
        QCOMPARE(eventCounter->masterPitchEventCount, 1);
        QCOMPARE(eventCounter->mostRecentMasterPitch, 10);



        qDebug() << "Verify correct patch generation via undo history";

        // At this point there should be one patch for 0.0f -> 10.0f
        rapidjson::Value& patch = presenter->getProjectHistoryAt(0)[0]->getPatch();
        QCOMPARE(patch[0]["op"].GetString(), "replace");
        QCOMPARE(patch[0]["path"].GetString(), "/project/transport/master_pitch/initial_value");
        QCOMPARE(patch[0]["value"].GetFloat(), 10.0f);

        // ... and a corresponding undo patch for 10.0f -> 0.0f
        rapidjson::Value& undoPatch = presenter->getProjectHistoryAt(0)[0]->getUndoPatch();
        QCOMPARE(undoPatch[0]["op"].GetString(), "replace");
        QCOMPARE(undoPatch[0]["path"].GetString(), "/project/transport/master_pitch/initial_value");
        QCOMPARE(undoPatch[0]["value"].GetFloat(), 0.0f);
    }

//    void patch_data() {
//        // Note: a document is defined in initTestCase()
//        // that will be used for each of the tests below.

//        QTest::addColumn<PatchFragment::PatchType>("patchType");
//        QTest::addColumn<QString>("from");
//        QTest::addColumn<QString>("path");
//        QTest::addColumn<rapidjson::Value*>("oldValue");
//        QTest::addColumn<rapidjson::Value*>("newValue");

//        QTest::addRow("basic add")         << PatchFragment::PatchType::ADD      << QString()            << "/basic_add"              << &basicAddValue     << &basicAddValueOld;
//        QTest::addRow("basic remove")      << PatchFragment::PatchType::REMOVE   << QString()            << "/basic_remove"           << &basicRemoveValue  << &basicRemoveValueOld;
//        QTest::addRow("basic replace")     << PatchFragment::PatchType::REPLACE  << QString()            << "/basic_replace"          << &basicReplaceValue << &basicReplaceValueOld;
//        QTest::addRow("basic copy")        << PatchFragment::PatchType::COPY     << "/basic_copy_source" << "/basic_copy_destination" << &basicCopyValue    << &basicCopyValueOld;
//        QTest::addRow("basic move")        << PatchFragment::PatchType::MOVE     << "/basic_move_source" << "/basic_move_destination" << &basicMoveValue    << &basicMoveValueOld;
//    }

//    void patch() {
//        QFETCH(PatchFragment::PatchType, patchType);
//        QFETCH(QString, from);
//        QFETCH(QString, path);
//        QFETCH(rapidjson::Value*, oldValue);
//        QFETCH(rapidjson::Value*, newValue);

//        rapidjson::Document startDoc;
//        startDoc.CopyFrom(doc, startDoc.GetAllocator());

//        Patch patch(this, presenter->getProjectAt(0));
//        switch (patchType) {
//        case PatchFragment::PatchType::ADD:
//            patch.patchAdd(path, *newValue);
//            break;
//        case PatchFragment::PatchType::REMOVE:
//            patch.patchRemove(path, *oldValue);
//            break;
//        case PatchFragment::PatchType::REPLACE:
//            patch.patchReplace(path, *oldValue, *newValue);
//            break;
//        case PatchFragment::PatchType::COPY:
//            patch.patchCopy(from, path);
//            break;
//        case PatchFragment::PatchType::MOVE:
//            patch.patchMove(from, path);
//            break;
//        }

//        patch.apply();

//        rapidjson::Document afterPatchDoc;
//        afterPatchDoc.CopyFrom(doc, afterPatchDoc.GetAllocator());

//        switch (patchType) {
//        case PatchFragment::PatchType::ADD:
//            QCOMPARE(rapidjson::Pointer(path.toStdString()).Get(afterPatchDoc)->GetString(), "add");
//            break;
//        case PatchFragment::PatchType::REMOVE:
//            QVERIFY(rapidjson::Pointer(path.toStdString()).GetWithDefault(afterPatchDoc, rapidjson::kNullType).IsNull());
//            break;
//        case PatchFragment::PatchType::REPLACE:
//            QCOMPARE(rapidjson::Pointer(path.toStdString()).Get(afterPatchDoc)->GetString(), "replace");
//            break;
//        case PatchFragment::PatchType::COPY:
//            QCOMPARE(rapidjson::Pointer(from.toStdString()).Get(afterPatchDoc)->GetString(),
//                     rapidjson::Pointer(path.toStdString()).Get(afterPatchDoc)->GetString());
//            break;
//        case PatchFragment::PatchType::MOVE:
//            QCOMPARE(rapidjson::Pointer(from.toStdString()).Get(startDoc)->GetString(),
//                     rapidjson::Pointer(path.toStdString()).Get(afterPatchDoc)->GetString());
//            QVERIFY(rapidjson::Pointer(from.toStdString()).GetWithDefault(afterPatchDoc, rapidjson::kNullType).IsNull());
//            break;
//        }

//        patch.applyUndo();

//        rapidjson::Document afterUndoDoc;
//        afterUndoDoc.CopyFrom(doc, afterUndoDoc.GetAllocator());

//        switch (patchType) {
//        case PatchFragment::PatchType::ADD:
//            QVERIFY(rapidjson::Pointer(path.toStdString()).GetWithDefault(afterUndoDoc, rapidjson::kNullType).IsNull());
//            break;
//        case PatchFragment::PatchType::REMOVE:
//            QCOMPARE(rapidjson::Pointer(path.toStdString()).Get(afterUndoDoc)->GetString(),
//                     rapidjson::Pointer(path.toStdString()).Get(startDoc)->GetString());
//            break;
//        case PatchFragment::PatchType::REPLACE:
//            QCOMPARE(rapidjson::Pointer(path.toStdString()).Get(afterUndoDoc)->GetString(),
//                     rapidjson::Pointer(path.toStdString()).Get(startDoc)->GetString());
//            QVERIFY(rapidjson::Pointer(path.toStdString()).Get(afterUndoDoc)->GetString() !=
//                    rapidjson::Pointer(path.toStdString()).Get(afterPatchDoc)->GetString());
//            break;
//        case PatchFragment::PatchType::COPY:
//            QCOMPARE(rapidjson::Pointer(from.toStdString()).Get(afterUndoDoc)->GetString(),
//                     rapidjson::Pointer(from.toStdString()).Get(startDoc)->GetString());
//            QVERIFY(rapidjson::Pointer(path.toStdString()).GetWithDefault(afterUndoDoc, rapidjson::kNullType).IsNull());
//            break;
//        case PatchFragment::PatchType::MOVE:
//            QCOMPARE(rapidjson::Pointer(from.toStdString()).Get(afterUndoDoc)->GetString(),
//                     rapidjson::Pointer(from.toStdString()).Get(startDoc)->GetString());
//            QVERIFY(rapidjson::Pointer(path.toStdString()).GetWithDefault(afterUndoDoc, rapidjson::kNullType).IsNull());
//            break;
//        }
//    }

    void presenterTests() {
        qDebug() << "Remove the current testing project and open a new one";
        presenter->removeProjectAt(0);
        presenter->newProject();
        delete eventCounter;
        eventCounter = new PresenterEventCounter(this);

        QObject::connect(presenter,    SIGNAL(masterPitchChanged(int)),
                         eventCounter, SLOT(masterPitchChanged(int)));


        qDebug() << "The new project should have no undo history";
        QCOMPARE(presenter->getHistoryPointerAt(0), -1);

        qDebug() << "The new project should not be marked as having unsaved changes";
        QCOMPARE(presenter->projectHasUnsavedChanges(0), false);

        qDebug() << "Undo and redo shouldn't do or break anything";
        presenter->undo();
        QCOMPARE(presenter->getHistoryPointerAt(0), -1);
        QCOMPARE(eventCounter->masterPitchEventCount, 0);
        QCOMPARE(presenter->projectHasUnsavedChanges(0), false);
        presenter->redo();
        QCOMPARE(presenter->getHistoryPointerAt(0), -1);
        QCOMPARE(eventCounter->masterPitchEventCount, 0);
        QCOMPARE(presenter->projectHasUnsavedChanges(0), false);

        qDebug() << "Performing an action should add an undo step";
        presenter->setMasterPitch(3, true);
        QCOMPARE(presenter->getMasterPitch(), 3);
        QCOMPARE(presenter->getHistoryPointerAt(0), 0);
        QCOMPARE(eventCounter->masterPitchEventCount, 1);
        QCOMPARE(presenter->getProjectHistoryAt(0).length(), 1);
        QCOMPARE(presenter->projectHasUnsavedChanges(0), true);
        QCOMPARE(presenter->isProjectSaved(0), false);
        Patch& patch2 = *presenter->getProjectHistoryAt(0)[0];
        QCOMPARE(patch2.getPatch().Size(), 1u);
        QCOMPARE(patch2.getPatch()[0]["op"].GetString(), "replace");
        QCOMPARE(patch2.getUndoPatch()[0]["op"].GetString(), "replace");
        QCOMPARE(patch2.getPatch()[0]["path"].GetString(), "/project/transport/master_pitch/initial_value");
        QCOMPARE(patch2.getUndoPatch()[0]["path"].GetString(), "/project/transport/master_pitch/initial_value");
        QCOMPARE(patch2.getPatch()[0]["value"].GetFloat(), 3.0f);
        QCOMPARE(patch2.getUndoPatch()[0]["value"].GetFloat(), 0.0f);

        qDebug() << "The undo history should not update after an intermediate (live update) change";
        presenter->setMasterPitch(-1, false);
        QCOMPARE(presenter->getHistoryPointerAt(0), 0);
        qDebug() << "... but getMasterPitch should still reflect the proper value";
        QCOMPARE(presenter->getMasterPitch(), -1);

        qDebug() << "The undo history should be accurate after multiple actions";
        presenter->setMasterPitch(1, true);
        presenter->setMasterPitch(2, true);
        presenter->setMasterPitch(3, true);
        presenter->setMasterPitch(4, true);
        presenter->setMasterPitch(5, true);
        QCOMPARE(presenter->getMasterPitch(), 5);
        QCOMPARE(presenter->getHistoryPointerAt(0), 5);
        QCOMPARE(presenter->getProjectHistoryAt(0).length(), 6);
        Patch& patch3 = *presenter->getProjectHistoryAt(0)[5];
        QCOMPARE(patch3.getPatch()[0]["value"].GetFloat(), 5.0f);
        QCOMPARE(patch3.getUndoPatch()[0]["value"].GetFloat(), 4.0f);
        Patch& patch4 = *presenter->getProjectHistoryAt(0)[4];
        QCOMPARE(patch4.getPatch()[0]["value"].GetFloat(), 4.0f);
        QCOMPARE(patch4.getUndoPatch()[0]["value"].GetFloat(), 3.0f);
        Patch& patch5 = *presenter->getProjectHistoryAt(0)[3];
        QCOMPARE(patch5.getPatch()[0]["value"].GetFloat(), 3.0f);
        QCOMPARE(patch5.getUndoPatch()[0]["value"].GetFloat(), 2.0f);
        Patch& patch6 = *presenter->getProjectHistoryAt(0)[2];
        QCOMPARE(patch6.getPatch()[0]["value"].GetFloat(), 2.0f);
        QCOMPARE(patch6.getUndoPatch()[0]["value"].GetFloat(), 1.0f);
        Patch& patch7 = *presenter->getProjectHistoryAt(0)[1];
        QCOMPARE(patch7.getPatch()[0]["value"].GetFloat(), 1.0f);
        QCOMPARE(patch7.getUndoPatch()[0]["value"].GetFloat(), 3.0f);

        qDebug() << "Undo should work as expected";
        presenter->undo();
        presenter->undo();
        QCOMPARE(presenter->getMasterPitch(), 3);
        QCOMPARE(presenter->getHistoryPointerAt(0), 3);

        qDebug() << "Redo should work as expected";
        presenter->redo();
        presenter->redo();
        QCOMPARE(presenter->getMasterPitch(), 5);
        QCOMPARE(presenter->getHistoryPointerAt(0), 5);

        qDebug() << "Undo + change should erase part of the undo history";
        presenter->undo();
        presenter->undo();
        presenter->undo();
        presenter->setMasterPitch(12, true);
        QCOMPARE(presenter->getMasterPitch(), 12);
        QCOMPARE(presenter->getHistoryPointerAt(0), 3);

        qDebug() << "Lots of undos shouldn't break anything";
        presenter->undo();
        presenter->undo();
        presenter->undo();
        presenter->undo();
        presenter->undo();
        presenter->undo();
        QCOMPARE(presenter->getMasterPitch(), 0);
        QCOMPARE(presenter->getHistoryPointerAt(0), -1);

        qDebug() << "Lots of redos shouldn't break anything";
        presenter->redo();
        presenter->redo();
        presenter->redo();
        presenter->redo();
        presenter->redo();
        presenter->redo();
        QCOMPARE(presenter->getMasterPitch(), 12);
        QCOMPARE(presenter->getHistoryPointerAt(0), 3);


        qDebug() << "Creating a new project should work as expected";
        presenter->newProject();
        qDebug() << "Checking for two open projects.";
        presenter->getProjectAt(0);
        presenter->getProjectAt(1);
        presenter->getEngineAt(1);
        presenter->getProjectFileAt(1);
        QCOMPARE(presenter->getHistoryPointerAt(1), -1);
        presenter->getProjectHistoryAt(1);
        QCOMPARE(presenter->activeProjectIndex, 1);
        QCOMPARE(presenter->projectHasUnsavedChanges(0), true);
        QCOMPARE(presenter->isProjectSaved(0), false);
        QCOMPARE(presenter->projectHasUnsavedChanges(1), false);
        QCOMPARE(presenter->isProjectSaved(1), false);

        qDebug() << "We should be able to switch tabs";
        presenter->setMasterPitch(6, true);
        presenter->setMasterPitch(7, true);
        presenter->undo();
        QCOMPARE(presenter->getMasterPitch(), 6);
        QCOMPARE(presenter->getHistoryPointerAt(1), 0);

        presenter->switchActiveProject(0);
        QCOMPARE(presenter->activeProjectIndex, 0);
        presenter->setMasterPitch(6, true);
        presenter->setMasterPitch(7, true);
        QCOMPARE(presenter->getMasterPitch(), 7);
        QCOMPARE(presenter->getHistoryPointerAt(0), 5);

        presenter->switchActiveProject(1);
        QCOMPARE(presenter->activeProjectIndex, 1);
        QCOMPARE(presenter->getHistoryPointerAt(1), 0);
        presenter->redo();
        QCOMPARE(presenter->getHistoryPointerAt(1), 1);
        presenter->setMasterPitch(8, true);
        presenter->setMasterPitch(9, true);
        QCOMPARE(presenter->getMasterPitch(), 9);
        QCOMPARE(presenter->getHistoryPointerAt(1), 3);

        qDebug() << "We should be able to close the first tab";
        presenter->closeProject(0);
        presenter->switchActiveProject(0);
        QCOMPARE(presenter->activeProjectIndex, 0);
        QCOMPARE(presenter->getMasterPitch(), 9);
        QCOMPARE(presenter->getHistoryPointerAt(0), 3);

        qDebug() << "Save and load should work as expected";
        auto path = QDir::currentPath() + "/test.anthem";
        qDebug() << path;
        presenter->setMasterPitch(10, true);
        presenter->saveActiveProjectAs(path);
        QCOMPARE(presenter->projectHasUnsavedChanges(0), false);
        QCOMPARE(presenter->isProjectSaved(0), true);
        presenter->loadProject(path);
        QCOMPARE(presenter->activeProjectIndex, 1);
        QCOMPARE(presenter->getMasterPitch(), 10);
        QCOMPARE(presenter->getHistoryPointerAt(1), -1);
        QCOMPARE(presenter->projectHasUnsavedChanges(1), false);
        QCOMPARE(presenter->isProjectSaved(1), true);
        QCOMPARE(presenter->getProjectAt(presenter->activeProjectIndex)->getSong()->getPatterns().keys().length(), 1);

        presenter->setMasterPitch(-12, true);
        QCOMPARE(presenter->projectHasUnsavedChanges(1), true);
        QCOMPARE(presenter->isProjectSaved(1), true);
        presenter->saveActiveProject();
        presenter->loadProject(path);
        QCOMPARE(presenter->activeProjectIndex, 2);
        QCOMPARE(presenter->getMasterPitch(), -12);
        QCOMPARE(presenter->getHistoryPointerAt(2), -1);
        QCOMPARE(presenter->projectHasUnsavedChanges(1), false);
        QCOMPARE(presenter->isProjectSaved(1), true);
        QCOMPARE(presenter->projectHasUnsavedChanges(2), false);
        QCOMPARE(presenter->isProjectSaved(2), true);
        presenter->closeProject(2);
        presenter->closeProject(1);
        presenter->closeProject(0);
        presenter->newProject();
        presenter->switchActiveProject(0);

        QCOMPARE(presenter->activeProjectIndex, 0);



        qDebug() << "There should be one pattern by default";
        PatternPresenter& patternPresenter = *presenter->getPatternPresenter();
        Song& song = *presenter->getProjectAt(presenter->activeProjectIndex)->getSong();
        QCOMPARE(song.getPatterns().keys().length(), 1);
        QCOMPARE(song.getPatterns()[song.getPatterns().keys()[0]]->getDisplayName(), QString("New pattern"));

        qDebug() << "Pattern delete should work";
        patternPresenter.removePattern(song.getPatterns().keys()[0]);
        QCOMPARE(song.getPatterns().keys().length(), 0);

        qDebug() << "Undo/redo for pattern delete should work";
        presenter->undo();
        QCOMPARE(song.getPatterns().keys().length(), 1);
        presenter->undo();
        QCOMPARE(song.getPatterns().keys().length(), 1);
        QCOMPARE(song.getPatterns()[song.getPatterns().keys()[0]]->getDisplayName(), QString("New pattern"));
        presenter->redo();
        QCOMPARE(song.getPatterns().keys().length(), 0);

        qDebug() << "Pattern create should work";
        patternPresenter.createPattern("Test 1", QColor("#FFFFFF"));
        QCOMPARE(song.getPatterns().keys().length(), 1);
        QCOMPARE(song.getPatterns()[song.getPatterns().keys()[0]]->getDisplayName(), QString("Test 1"));
        QCOMPARE(song.getPatterns()[song.getPatterns().keys()[0]]->getColor(), QColor("#FFFFFF"));
        patternPresenter.createPattern("Test 2", QColor("#FFFFFF"));
        QCOMPARE(song.getPatterns().keys().length(), 2);
        patternPresenter.createPattern("Test 3", QColor("#FFFFFF"));
        QCOMPARE(song.getPatterns().keys().length(), 3);

        qDebug() << "Undo/redo for pattern create should work";
        presenter->undo();
        presenter->undo();
        QCOMPARE(song.getPatterns().keys().length(), 1);
        QCOMPARE(song.getPatterns()[song.getPatterns().keys()[0]]->getDisplayName(), QString("Test 1"));
        QCOMPARE(song.getPatterns()[song.getPatterns().keys()[0]]->getColor(), QColor("#FFFFFF"));
        presenter->undo();
        QCOMPARE(song.getPatterns().keys().length(), 0);
        presenter->redo();
        presenter->redo();
        presenter->redo();
        QCOMPARE(song.getPatterns().keys().length(), 3);
    }

    void cleanupTestCase() {
        auto path = QDir::currentPath() + "/test.anthem";
        QFile file(path);
        file.remove();
    }
};

#endif // MODELTESTS_H
