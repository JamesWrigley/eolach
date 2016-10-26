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

#include <QIcon>
#include <QMouseEvent>
#include <QApplication>

#include "TextField.h"
#include "MainWindow.h"

TextField::TextField(QString tableIdentifier, QString sqlField,
                     QString labelName, bool (*function)(QString))
{
    fieldName = sqlField;
    checkFunction = function;
    label = new QLabel(labelName);
    this->tableIdentifier = tableIdentifier;

    icon = new QLabel();
    icon->setPixmap(QIcon(":/invalid-icon").pixmap(20));
    editBox = new CLineEdit();
    connect(editBox, &CLineEdit::doubleClicked, this, &TextField::onDoubleClicked);
    connect(editBox, &CLineEdit::textChanged, this, &TextField::onTextChanged);
    connect(editBox, &CLineEdit::textModified, this, &TextField::onTextModified);
    connect(&MainWindow::signaller, &SignalSingleton::itemSelected,
            this, &TextField::onItemSelected);

    label->setMinimumWidth(120);
    addWidget(label);
    addWidget(editBox);
    addWidget(icon);
}

void TextField::enterEditMode()
{
    QMouseEvent doubleClick(QEvent::MouseButtonDblClick, QPointF(), Qt::LeftButton, 0, 0);
    QApplication::sendEvent(editBox, &doubleClick);
}

void TextField::onItemSelected(QSqlRecord record)
{
    QString tableIdentifier = record.value("key").toString().right(1);

    if (!record.isNull(fieldName) && tableIdentifier == this->tableIdentifier) {
        setText(record.value(fieldName).toString());
    }    
}

void TextField::onTextChanged(QString newText)
{
    if (checkFunction(newText)) {
        icon->setPixmap(QIcon(":/valid-icon").pixmap(20));
    } else {
        icon->setPixmap(QIcon(":/invalid-icon").pixmap(20));
    }
}

void TextField::onTextModified(QString newText)
{
    if (checkFunction(newText)) {
        icon->hide();
    } else {
        icon->show();
    }

    InfoWidget::currentRecord.setValue(fieldName, newText);
    emit MainWindow::signaller.itemChanged(InfoWidget::currentRecord);
}

void TextField::onDoubleClicked()
{
    icon->show();
}

void TextField::setText(QString newText)
{
    editBox->setText(newText);
    onTextChanged(newText);
    if (checkFunction(newText)) {
        icon->hide();
    } else {
        icon->show();
    }
}
