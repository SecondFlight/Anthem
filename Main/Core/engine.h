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

// This class abstracts lifecycle and communication for
// the actual engine, which lives in a separate process.

#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QProcess>

#include "Include/rapidjson/document.h"
#include "Include/rapidjson/stringbuffer.h"
#include "Include/rapidjson/writer.h"

class Engine : public QObject {
    Q_OBJECT
private:
    QProcess* engine;
    void addRPCHeaders(
        rapidjson::Document& json,
        std::string headers
    );
    void write(rapidjson::Document& json);

public:
    explicit
    Engine(QObject *parent);
    ~Engine();

    void start();
    void stop();

    void sendLiveControlUpdate(
        uint64_t controlId, float value
    );
    void sendMidiNoteEvent(
        uint64_t generatorId,
        uint8_t status,
        uint8_t data1,
        uint8_t data2
    );
    // TODO: Add play, pause, stop, seek, etc.

    void sendPatch(
        QString operation,
        QString from,
        QString path,
        rapidjson::Value &value
    );
    void sendPatchList(rapidjson::Value& patchList);

signals:
    void engineStarted();

public slots:
    void onEngineStart();
    void onEngineMessageChunk();
};

#endif // ENGINE_H
