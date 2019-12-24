/*
    Copyright (C) 2019 Joshua Wade

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

pragma Singleton

import QtQuick 2.14

QtObject {
    property FontLoader notoSansRegular: FontLoader {
        source: "../Fonts/NotoSans-Regular.ttf"
        name: "Noto Sans Regular"
    }

    property FontLoader sourceCodeProSemiBold: FontLoader {
        source: "../Fonts/SourceCodePro-SemiBold.ttf"
        name: "Source Code Pro Semi Bold"
    }
}
