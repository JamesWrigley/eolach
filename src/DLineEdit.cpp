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
#include "DLineEdit.h"

DLineEdit::DLineEdit(QString placeholdertext, bool (*function)(QString))
{
  lineedit = new QLineEdit();
  lineedit->setPlaceholderText(placeholdertext);
  connect(lineedit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));

  valid = false;
  icon = new QLabel();
  icon->setPixmap(QIcon(":/invalid-icon").pixmap(20));

  checkFunction = function;

  addWidget(lineedit);
  addWidget(icon);
}

void DLineEdit::enableCompletion(QStringListModel* completions)
{
  completer = new DCompleter(completions, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  lineedit->setCompleter(completer);
  completionEnabled = true;
}

void DLineEdit::onTextChanged(QString fieldText)
{
  if (checkFunction(fieldText))
    {
      icon->setPixmap(QIcon(":/valid-icon").pixmap(20));
      valid = true;
    }
  else
    {
      icon->setPixmap(QIcon(":/invalid-icon").pixmap(20));
      valid = false;
    }
}

QString DLineEdit::placeholderText()
{
  return lineedit->placeholderText();
}

void DLineEdit::setFocus()
{
  lineedit->setFocus();
}

QString DLineEdit::text()
{
  return lineedit->text();
}
