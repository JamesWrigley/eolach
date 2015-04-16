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

KeysWidget::KeysWidget(QString table, QStringList headerList, QWidget *parent)
{
  dbTable = table;
  headers = headerList;

  setColumnCount(headers.length());
  visibleColumnCount = headers.length();
  setHorizontalHeaderLabels(headers);
  headerContextMenu = new QMenu(this);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
	  this, SLOT(createHeaderContextMenu(QPoint)));

  for (QString text : headers)
    {
      QAction *action = new QAction(text, this);
      action->setCheckable(true);
      action->setChecked(true);

      headerContextMenu->addAction(action);
      connect(action, SIGNAL(toggled(bool)), this, SLOT(modifyHeader(bool)));
    }

  // Set up cell context menus
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createItemContextMenu(QPoint)));

  itemContextMenu = new QMenu(this);
  removeItemAction = new QAction("Remove", this);
  itemContextMenu->addAction(removeItemAction);
  connect(removeItemAction, SIGNAL(triggered()), this, SLOT(removeItem()));

  // Miscellanea
  bookstore = QSqlDatabase::database();
  enableSorting(0, Qt::AscendingOrder);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  loadItems();
}

/* General functions */

void KeysWidget::addItem(QString itemKey)
{
  QSqlQuery getItemInfo(bookstore);

  if (itemKey.endsWith("b")) { getItemInfo.prepare(getBookInfo); }
  else if (itemKey.endsWith("d")) { getItemInfo.prepare(getDiscInfo); }
  else if (itemKey.endsWith("p")) { getItemInfo.prepare(getPatronInfo); }

  getItemInfo.bindValue(":key", itemKey);
  getItemInfo.exec();
  getItemInfo.next();

  insertRow(rowCount());
  // We temporarily disable sorting because it causes indexing complications if
  // the row is sorted before we're finished populating its cells.
  std::pair<int, Qt::SortOrder> sortingInfo(disableSorting());
  for (int i = headers.length() - 1; i >= 0; --i)
    {
      QTableWidgetItem *item = new QTableWidgetItem(getItemInfo.value(i).toString());
      item->setData(Qt::UserRole, QVariant(itemKey));
      setItem(rowCount() - 1, i, item);
    }
  enableSorting(sortingInfo.first, sortingInfo.second);
}

std::pair<int, Qt::SortOrder> KeysWidget::disableSorting()
{
  std::pair<int, Qt::SortOrder> sortingInfo(horizontalHeader()->sortIndicatorSection(),
					     horizontalHeader()->sortIndicatorOrder());
  setSortingEnabled(false);
  return(sortingInfo);
}

void KeysWidget::enableSorting(int sortColumn, Qt::SortOrder sortOrder)
{
  setSortingEnabled(true);
  sortByColumn(sortColumn, sortOrder);
}

void KeysWidget::loadItems()
{
  QSqlQuery getItemKeys(bookstore);
  getItemKeys.exec(QString("SELECT key FROM %1;").arg(dbTable));

  while (getItemKeys.next())
    {
      addItem(getItemKeys.value(0).toString());
    }
}

void KeysWidget::updateItem(int row, QString itemKey)
{
  QSqlQuery getItemInfo(bookstore);

  if (itemKey.endsWith("b")) { getItemInfo.prepare(getBookInfo); }
  else if (itemKey.endsWith("d")) { getItemInfo.prepare(getDiscInfo); }
  else if (itemKey.endsWith("p")) { getItemInfo.prepare(getPatronInfo); }

  getItemInfo.bindValue(":key", itemKey);
  getItemInfo.exec();
  getItemInfo.next();

  std::pair<int, Qt::SortOrder> sortingInfo(disableSorting());
  for (int i = 0; i < headers.length(); ++i)
    {
      item(row, i)->setText(getItemInfo.value(i).toString());
    }
  enableSorting(sortingInfo.first, sortingInfo.second);
}

/* Slots */

void KeysWidget::createItemContextMenu(QPoint pos)
{
  if (itemAt(pos) != 0)
    {
      itemContextMenu->popup(viewport()->mapToGlobal(pos));
    }
}

void KeysWidget::createHeaderContextMenu(QPoint pos)
{
  headerContextMenu->popup(mapToGlobal(pos));
}

void KeysWidget::modifyHeader(bool checked)
{
  int headerIndex = headers.indexOf(static_cast<QAction*>(sender())->text());
  if (checked)
    {
      showColumn(headerIndex);
      ++visibleColumnCount;
    }
  else if (!checked && visibleColumnCount > 1)
    {
      hideColumn(headerIndex);
      --visibleColumnCount;
    }
  else
    {
      static_cast<QAction*>(sender())->setChecked(true);
      --visibleColumnCount;
    }
}

void KeysWidget::removeItem()
{
  int confirm = QMessageBox::warning(this, "Confirm",
				     QString("Are you sure you wish to remove this item?"),
				     QMessageBox::Yes, QMessageBox::No);
  if (QMessageBox::Yes == confirm)
    {
      QString itemKey = currentItem()->data(Qt::UserRole).toString();
      QSqlQuery removeItem(bookstore);
      removeItem.prepare(QString("DELETE FROM %1 WHERE key=:key;").arg(dbTable));
      removeItem.bindValue(":key", itemKey);
      removeItem.exec();

      emit itemRemoved();
    }
}
