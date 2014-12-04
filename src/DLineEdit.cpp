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
#include <QCompleter>
#include "DLineEdit.h"

DLineEdit::DLineEdit(QString placeholdertext, bool (*function)(QString), QWidget *parent)
{
  lineedit = new QLineEdit();
  lineedit->setPlaceholderText(placeholdertext);
  connect(lineedit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));

  icon = new QLabel();
  icon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(20));

  check_function = function;

  addWidget(lineedit);
  addWidget(icon);
}

void DLineEdit::enable_completion(QStringList completions)
{
  QCompleter *completer = new QCompleter(completions, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  lineedit->setCompleter(completer);
}

void DLineEdit::onTextChanged(QString field_text)
{
  if (check_function(field_text))
    {
      icon->setPixmap(QIcon::fromTheme("dialog-ok-apply").pixmap(20));
    }
  else
    {
      icon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(20));
    }
}

QString DLineEdit::text()
{
  return lineedit->text();
}
