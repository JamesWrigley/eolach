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

#include <QSqlQuery>
#include <QMouseEvent>
#include <QSqlDatabase>

#include "CLineEdit.h"

CLineEdit::CLineEdit()
{
    // We set this now to use in onEditingFinished since the window color will
    // have by then been changed by mouseDoubleClickEvent().
    backgroundColor = palette().color(QPalette::Window).name();
    mouseOverColor = palette().color(QPalette::Window).darker(110).name();

    connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));

    setReadOnly(true);
    setFrame(false);
    setStyleSheet("QLineEdit { background: " + backgroundColor + " }");
}

void CLineEdit::enterEvent(QEvent* event)
{
    // We check isReadOnly() because it will be true whenever the user isn't
    // editing, and we don't want to change the background during the edit.
    if (text().length() > 0 && isReadOnly()) {
        setStyleSheet("QLineEdit { background: " + mouseOverColor + " }");
    }
}

void CLineEdit::leaveEvent(QEvent* event)
{
    if (text().length() > 0 && isReadOnly()) {
        setStyleSheet("QLineEdit { background: " + backgroundColor + " }");
    }
}

void CLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSqlDatabase bookstore = QSqlDatabase::database();
    QSqlQuery getRowCount;
    getRowCount.exec("SELECT COUNT(*) FROM books;");
    getRowCount.next();

    emit doubleClicked();
    if (event->button() == Qt::LeftButton && 0 < getRowCount.value(0).toInt()) {
        currentText = text();
        setReadOnly(false);
        setFrame(true);
        setStyleSheet("QLineEdit { background: #F7F7F7 }");
        selectAll();
    }
}

void CLineEdit::onEditingFinished()
{
    setReadOnly(true);
    setFrame(false);
    setStyleSheet("QLineEdit { background: " + backgroundColor + " }");
    deselect();

    currentText = text();
    emit textModified(currentText);
}
