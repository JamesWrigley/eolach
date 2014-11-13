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

#include <QtGui>
#include <QVariant>
#include <QSqlQuery>
#include <QHeaderView>
#include "KeysWidget.h"

KeysWidget::KeysWidget(QWidget *parent) : QTableWidget(parent)
{
  insertColumn(0);
  insertColumn(1);

  headers << "Title" << "Author";
  setHorizontalHeaderLabels(headers);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  this->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void KeysWidget::add_book(QString book_key)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery get_book_info(db);
  get_book_info.exec("SELECT title, author FROM bookstore WHERE key='" + book_key + "';");
  get_book_info.next();

  insertRow(rowCount());

  QTableWidgetItem *title = new QTableWidgetItem(get_book_info.value(0).toString());
  QTableWidgetItem *author = new QTableWidgetItem(get_book_info.value(1).toString());

  setItem(rowCount() - 1, 0, title);
  setItem(rowCount() - 1, 1, author);
}

void KeysWidget::update_book(int row, QString book_key)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery get_book_info(db);
  get_book_info.exec("SELECT title, author FROM bookstore WHERE key='" + book_key + "';");
  get_book_info.next();

  item(row, 0)->setText(get_book_info.value(0).toString());
  item(row, 1)->setText(get_book_info.value(1).toString());
}
