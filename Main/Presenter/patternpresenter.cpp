/*
    Copyright (C) 2020 Joshua Wade

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

#include "patternpresenter.h"
#include <QDebug>

PatternPresenter::PatternPresenter(QObject* parent, IdGenerator* id,
                                   Project* activeProject)
                                    : QObject(parent) {
    this->id = id;
    this->activeProject = activeProject;
}

void PatternPresenter::setActiveProject(Project* project) {
    this->activeProject = project;
}

void PatternPresenter::setActivePattern(Pattern* pattern) {
    this->activePattern = pattern;
}

void PatternPresenter::createPattern(QString name, QColor color) {
    qDebug() << "PatternPresenter: createPattern()";
    activeProject->getSong()->addPattern(name, color);
}
