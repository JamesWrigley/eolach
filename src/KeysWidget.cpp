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
#include <QHeaderView>
#include <QMessageBox>
#include "KeysWidget.h"

KeysWidget::KeysWidget(QWidget *parent)
{
  // Set up headers
  setColumnCount(5);
  hideColumn(4);
  hideColumn(3);
  hideColumn(2);
  visible_column_count = 2;

  headers << "Title" << "Author" << "Genre" << "Publication Date" << "ISBN";
  setHorizontalHeaderLabels(headers);
  header_context_menu = new QMenu(this);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(create_header_context_menu(QPoint)));

  for (QString text : headers)
    {
      QAction *action = new QAction(text, this);
      action->setCheckable(true);

      if (text == "Title" or text == "Author")
        {
          action->setChecked(true);
        }

      header_context_menu->addAction(action);
      connect(action, SIGNAL(toggled(bool)), this, SLOT(modify_header(bool)));
    }

  // Set up cell context menus
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(create_item_context_menu(QPoint)));

  item_context_menu = new QMenu(this);
  remove_book_action = new QAction("Remove", this);
  item_context_menu->addAction(remove_book_action);
  connect(remove_book_action, SIGNAL(triggered()), this, SLOT(removeBook()));

  // Miscellanea

  bookstore = QSqlDatabase::database();
  load_items();

  setSortingEnabled(true);
  sortByColumn(0, Qt::AscendingOrder);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

/* General functions */

void KeysWidget::add_book(QString book_key)
{
  QSqlQuery get_book_info(bookstore);
  get_book_info.prepare("SELECT title, author, genre, publication_date, isbn FROM bookstore WHERE key=:key;");
  get_book_info.bindValue(":key", book_key);
  get_book_info.exec();
  get_book_info.next();

  insertRow(rowCount());
  // We add the items backwards because the rows are sorted by the first cell by
  // default, and it causes indexing complications if the row is sorted before
  // we're finished populating its cells.
  for (int i = headers.length() - 1; i >= 0; --i)
    {
      QTableWidgetItem *item = new QTableWidgetItem(get_book_info.value(i).toString());
      item->setData(Qt::UserRole, QVariant(book_key));
      setItem(rowCount() - 1, i, item);
    }
}

void KeysWidget::load_items()
{
  QSqlQuery get_book_keys(bookstore);
  get_book_keys.exec("SELECT key FROM bookstore;");

  while (get_book_keys.next())
    {
      add_book(get_book_keys.value(0).toString());
    }
}

void KeysWidget::update_book(int row, QString book_key)
{
  QSqlQuery get_book_info(bookstore);
  get_book_info.exec("SELECT title, author, genre, publication_date, isbn FROM bookstore WHERE key='" + book_key + "';");
  get_book_info.next();

  for (int i = 0; i < headers.length(); ++i)
    {
      item(row, i)->setText(get_book_info.value(i).toString());
    }
}

/* Slots */

void KeysWidget::create_item_context_menu(QPoint pos)
{
  if (itemAt(pos) != 0)
    {
      item_context_menu->popup(viewport()->mapToGlobal(pos));
    }
}

void KeysWidget::create_header_context_menu(QPoint pos)
{
  header_context_menu->popup(mapToGlobal(pos));
}

void KeysWidget::modify_header(bool checked)
{
  int header_index = headers.indexOf(static_cast<QAction*>(sender())->text());
  if (checked)
    {
      showColumn(header_index);
      ++visible_column_count;
    }
  else if (!checked && visible_column_count > 1)
    {
      hideColumn(header_index);
      --visible_column_count;
    }
  else
    {
      static_cast<QAction*>(sender())->setChecked(true);
      --visible_column_count;
    }
}

void KeysWidget::removeBook()
{
  int confirm = QMessageBox::warning(this, "Confirm", "Are you sure you wish to remove this book?",
                                     QMessageBox::Yes, QMessageBox::No);
  if (QMessageBox::Yes == confirm)
    {
      QString book_key = currentItem()->data(Qt::UserRole).toString();
      QSqlQuery remove_book(bookstore);
      remove_book.exec("DELETE FROM bookstore WHERE key='" + book_key + "';");

      emit bookRemoved();
    }
}
