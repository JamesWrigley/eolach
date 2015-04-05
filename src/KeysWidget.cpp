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
#include "miscellanea.h"

KeysWidget::KeysWidget(QString datatype, QString table, QStringList header_list, QWidget *parent)
{
  data_type = datatype;
  db_table = table;
  headers = header_list;

  setColumnCount(headers.length());
  visible_column_count = headers.length();
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
      action->setChecked(true);

      header_context_menu->addAction(action);
      connect(action, SIGNAL(toggled(bool)), this, SLOT(modify_header(bool)));
    }

  // Set up cell context menus
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(create_item_context_menu(QPoint)));

  item_context_menu = new QMenu(this);
  remove_item_action = new QAction("Remove", this);
  item_context_menu->addAction(remove_item_action);
  connect(remove_item_action, SIGNAL(triggered()), this, SLOT(removeItem()));

  // Miscellanea
  bookstore = QSqlDatabase::database();
  enable_sorting(0, Qt::AscendingOrder);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  load_items();
}

/* General functions */

void KeysWidget::add_item(QString item_key)
{
  QSqlQuery get_item_info(bookstore);
  get_item_info.prepare(data_type == "book" ? get_book_info : get_patron_info);
  get_item_info.bindValue(":key", item_key);
  get_item_info.exec();
  get_item_info.next();

  insertRow(rowCount());
  // We temporarily disable sorting because it causes indexing complications if
  // the row is sorted before we're finished populating its cells.
  std::pair<int, Qt::SortOrder> sorting_info(disable_sorting());
  for (int i = headers.length() - 1; i >= 0; --i)
    {
      QTableWidgetItem *item = new QTableWidgetItem(get_item_info.value(i).toString());
      item->setData(Qt::UserRole, QVariant(item_key));
      setItem(rowCount() - 1, i, item);
    }
  enable_sorting(sorting_info.first, sorting_info.second);
}

std::pair<int, Qt::SortOrder> KeysWidget::disable_sorting()
{
  std::pair<int, Qt::SortOrder> sorting_info(horizontalHeader()->sortIndicatorSection(),
					     horizontalHeader()->sortIndicatorOrder());
  setSortingEnabled(false);
  return(sorting_info);
}

void KeysWidget::enable_sorting(int sort_column, Qt::SortOrder sort_order)
{
  setSortingEnabled(true);
  sortByColumn(sort_column, sort_order);
}

void KeysWidget::load_items()
{
  QSqlQuery get_item_keys(bookstore);
  get_item_keys.exec(QString("SELECT key FROM %1;").arg(db_table));

  while (get_item_keys.next())
    {
      add_item(get_item_keys.value(0).toString());
    }
}

void KeysWidget::update_item(int row, QString item_key)
{
  QSqlQuery get_item_info(bookstore);
  get_item_info.prepare(data_type == "book" ? get_book_info : get_patron_info);
  get_item_info.bindValue(":key", item_key);
  get_item_info.exec();
  get_item_info.next();

  std::pair<int, Qt::SortOrder> sorting_info(disable_sorting());
  for (int i = 0; i < headers.length(); ++i)
    {
      item(row, i)->setText(get_item_info.value(i).toString());
    }
  enable_sorting(sorting_info.first, sorting_info.second);
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

void KeysWidget::removeItem()
{
  int confirm = QMessageBox::warning(this, "Confirm",
				     QString("Are you sure you wish to remove this %1?").arg(data_type),
                                     QMessageBox::Yes, QMessageBox::No);
  if (QMessageBox::Yes == confirm)
    {
      QString item_key = currentItem()->data(Qt::UserRole).toString();
      QSqlQuery remove_item(bookstore);
      remove_item.prepare(QString("DELETE FROM %1 WHERE key=:key;").arg(db_table));
      remove_item.bindValue(":key", item_key);
      remove_item.exec();

      emit itemRemoved();
    }
}
