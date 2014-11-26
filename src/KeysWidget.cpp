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

#include <QMenu>
#include <QAction>
#include <QVariant>
#include <QSqlQuery>
#include <QStringList>
#include <QHeaderView>
#include "KeysWidget.h"

KeysWidget::KeysWidget(QWidget *parent)
{
  insertColumn(0);
  insertColumn(1);

  QStringList headers = (QStringList() << "Title" << "Author");
  setHorizontalHeaderLabels(headers);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  setEditTriggers(QAbstractItemView::NoEditTriggers);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(create_context_menu(QPoint)));

  context_menu = new QMenu(this);
  remove_book_action = new QAction("Remove", this);
  context_menu->addAction(remove_book_action);

  connect(remove_book_action, SIGNAL(triggered()), this, SLOT(removeBook()));
  bookstore = QSqlDatabase::database();
  load_items();
  setSortingEnabled(true);
  sortByColumn(0, Qt::AscendingOrder);
}

/* General functions */

void KeysWidget::add_book(QString book_key)
{
  QSqlQuery get_book_info(bookstore);
  get_book_info.exec("SELECT title, author FROM bookstore WHERE key='" + book_key + "';");
  get_book_info.next();

  QTableWidgetItem *title = new QTableWidgetItem(get_book_info.value(0).toString());
  QTableWidgetItem *author = new QTableWidgetItem(get_book_info.value(1).toString());
  title->setData(Qt::UserRole, QVariant(book_key));
  author->setData(Qt::UserRole, QVariant(book_key));

  insertRow(rowCount());
  setItem(rowCount() - 1, 0, title);
  setCurrentItem(title);
  setItem(currentRow(), 1, author);
}

void KeysWidget::load_items()
{
  QSqlQuery get_book_keys(bookstore);
  get_book_keys.exec("SELECT key FROM bookstore;");

  while (get_book_keys.next())
    {
      QString book_key = get_book_keys.value(0).toString();
      add_book(book_key);
    }
}

void KeysWidget::update_book(int row, QString book_key)
{
  QSqlQuery get_book_info(bookstore);
  get_book_info.exec("SELECT title, author FROM bookstore WHERE key='" + book_key + "';");
  get_book_info.next();

  item(row, 0)->setText(get_book_info.value(0).toString());
  item(row, 1)->setText(get_book_info.value(1).toString());
}

/* Slots */

void KeysWidget::create_context_menu(QPoint pos)
{
  if (this->itemAt(pos) != 0)
    {
      context_menu->popup(this->viewport()->mapToGlobal(pos));
    }
}

void KeysWidget::removeBook()
{
  QString book_key = currentItem()->data(Qt::UserRole).toString();
  QSqlQuery remove_book(bookstore);
  remove_book.exec("DELETE FROM bookstore WHERE key='" + book_key + "';");

  emit bookRemoved();
}
