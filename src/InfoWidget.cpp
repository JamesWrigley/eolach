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

InfoWidget::InfoWidget()
{
  main_vbox = new QVBoxLayout(this);
  main_vbox->addStretch();

  setLayout(main_vbox);
  setFrameShape(QFrame::StyledPanel);
  setObjectName("MainQFrame");
  setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::add_field(TextField *field)
{
  fields.push_back(field);
  main_vbox->insertLayout(main_vbox->count() - 1, field);
}

void InfoWidget::clear()
{
  for (unsigned int i = 0; i < fields.size(); ++i)
    {
      fields[i]->set_text("");
    }
}

void InfoWidget::set_book(QString book_key)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery get_book_info(db);
  get_book_info.prepare("SELECT title, author, genre, publication_date, isbn FROM bookstore "
			"WHERE key=:book_key;");
  get_book_info.bindValue(":book_key", book_key);
  get_book_info.exec();
  get_book_info.next();

  for (unsigned int i = 0; i < fields.size(); ++i)
    {
      fields[i]->set_text(get_book_info.value(i).toString());
    }
}
