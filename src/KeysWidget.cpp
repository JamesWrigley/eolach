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
#include <QHeaderView>
#include "KeysWidget.h"

KeysWidget::KeysWidget(QWidget *parent) : QTableWidget(parent)
{
  initialize_ui();

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
}

void KeysWidget::add_book(Book book)
{
  insertRow(rowCount());

  QTableWidgetItem *title = new QTableWidgetItem(book.title);
  QTableWidgetItem *author = new QTableWidgetItem(book.author);

  setItem(rowCount() - 1, 0, title);
  setItem(rowCount() - 1, 1, author);
}

void KeysWidget::initialize_ui()
{
  insertColumn(0);
  insertColumn(1);

  headers << "Title" << "Author";
  setHorizontalHeaderLabels(headers);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  //  horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
}
