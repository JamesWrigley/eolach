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

#include <QVariant>
#include <QSqlQuery>
#include "InfoWidget.h"
#include "miscellanea.h"

InfoWidget::InfoWidget()
{
  main_vbox = new QVBoxLayout(this);
  main_vbox->addStretch();

  setLayout(main_vbox);
  setFrameShape(QFrame::StyledPanel);
  setObjectName("MainQFrame");
  setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::add_field(TextField *field, QString type)
{
  if (type == "b") { book_fields.push_back(field); }
  else if (type == "d") { disc_fields.push_back(field); }
  else if (type == "p") { patron_fields.push_back(field); }

  main_vbox->insertLayout(main_vbox->count() - 1, field);
}

void InfoWidget::clear()
{
  for (std::vector<TextField*> item_fields : {book_fields, disc_fields, patron_fields})
    {
      for (TextField *field : item_fields)
	{
	  field->set_text("");
	}
    }
}

void InfoWidget::remove_field(TextField *field)
{
  main_vbox->removeItem(field);
}

void InfoWidget::set_item(QString item_key)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery get_item_info(db);
  std::vector<TextField*> item_fields;

  if (item_key.endsWith("b"))
    {
      get_item_info.prepare(get_book_info);
      item_fields = book_fields;
    }
  else if (item_key.endsWith("d"))
    {
      get_item_info.prepare(get_disc_info);
      item_fields = disc_fields;
    }
  else if (item_key.endsWith("p"))
    {
      get_item_info.prepare(get_patron_info);
      item_fields = patron_fields;
    }

  get_item_info.bindValue(":key", item_key);
  get_item_info.exec();
  get_item_info.next();

  for (unsigned int i = 0; i < item_fields.size(); ++i)
    {
      item_fields[i]->set_text(get_item_info.value(i).toString());
    }
}
