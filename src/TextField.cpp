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

TextField::TextField(QString sql_field, QString label_name, bool (*function)(QString), QWidget *parent)
{
  field_name = sql_field;
  check_function = function;
  label = new QLabel(label_name);
  icon = new QLabel();
  icon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(20));
  edit_box = new CLineEdit();
  connect(edit_box, SIGNAL(textModified(QString)), this, SLOT(onTextModified(QString)));
  connect(edit_box, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
  connect(edit_box, SIGNAL(doubleClicked()), this, SLOT(onDoubleClicked()));

  label->setMinimumWidth(120);
  addWidget(label);
  addWidget(edit_box);
  addWidget(icon);
  icon->hide();
}

void TextField::enterEditMode()
{
  QMouseEvent double_click(QEvent::MouseButtonDblClick, QPointF(), Qt::LeftButton, 0, 0);
  QApplication::sendEvent(edit_box, &double_click);
}

void TextField::onTextChanged(QString new_text)
{
  if (check_function(new_text))
    {
      icon->setPixmap(QIcon::fromTheme("dialog-ok-apply").pixmap(20));
    }
  else
    {
      icon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(20));
    }
}

void TextField::onTextModified(QString new_text)
{
  if (check_function(new_text))
    {
      icon->hide();
    }
  emit fieldChanged(field_name, new_text);
}

void TextField::onDoubleClicked()
{
  icon->show();
}

void TextField::set_text(QString new_text)
{
  edit_box->setText(new_text);
  if (!check_function(new_text))
    {
      icon->show();
    }
  else
    {
      icon->hide();
    }
}
