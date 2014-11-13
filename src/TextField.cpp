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

#include "TextField.h"

TextField::TextField(QString sql_field, QString label_name, QWidget *parent)
{
  field_name = sql_field;
  label = new QLabel(label_name);
  edit_box = new CLineEdit();

  QObject::connect(edit_box, SIGNAL(fieldChanged(QString)), this, SLOT(onTextChanged(QString)));
  label->setMinimumWidth(120);

  this->addWidget(label);
  this->addWidget(edit_box);
}

void TextField::onTextChanged(QString new_text)
{
  emit textChanged(field_name, new_text);
}

void TextField::set_text(QString text)
{
  edit_box->setText(text);
}
