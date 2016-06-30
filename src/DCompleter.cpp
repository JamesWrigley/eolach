/*************************** Copyrights and License ******************************
 *                                                                                *
 * This file is part of Eolach. http://github.com/JamesWrigley/eolach/            *
 *                                                                                *
 * Eolach is free software: you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software  *
 * Foundation, either version 3 of the License, or (at your option) any later     *
 * version.                                                                       *
 *                                                                                *
 * Eolach is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS      *
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. *
 *                                                                                *
 * You should have received a copy of the GNU General Public License along with   *
 * Eolach. If not, see <http://www.gnu.org/licenses/>.                            *
 *                                                                                *
 *********************************************************************************/

#include <QStringListModel>

#include "DCompleter.h"

QString DCompleter::pathFromIndex(const QModelIndex& index) const
{
    QString path = QCompleter::pathFromIndex(index);
    QString text = static_cast<QLineEdit*>(widget())->text();

    int pos = text.lastIndexOf(",");
    if (pos >= 0) {
        path = text.left(pos) + ", " + path;
    }

    return path;
}

void DCompleter::removeCompletion(QModelIndex index)
{
    model()->removeRow(index.row());
}

QStringList DCompleter::splitPath(const QString& path) const
{
    int pos = path.lastIndexOf(",") + 1;

    while (pos < path.length() && QString(path.at(pos)) == QString(" ")) {
        pos++;
    }

    QStringList value = QStringList(path.mid(pos));
    return value;
}
