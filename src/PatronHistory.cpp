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

#include <QSqlQuery>
#include "PatronHistory.h"
#include "ChooseItemDialog.h"

PatronHistory::PatronHistory()
{
  // Set up the toolbar
  toolbar = new QToolBar();
  QAction* addItemAction = new QAction(QIcon(":/add-icon"), "", this);
  removeItemAction = new QAction(QIcon(":/remove-icon"), "", this);
  removeItemAction->setDisabled(true);
  toolbar->addAction(addItemAction);
  toolbar->addAction(removeItemAction);
  toolbar->setDisabled(true);

  // Set up the tabwidget
  tabWidget = new QTabWidget();
  pastBorrowedList = new QListWidget();
  currentBorrowedList = new QListWidget();
  tabWidget->addTab(currentBorrowedList, "Current Items");
  tabWidget->addTab(pastBorrowedList, "Past Items");
  tabWidget->setDisabled(true);

  connect(addItemAction, SIGNAL(triggered()), this, SLOT(addItem()));
  connect(removeItemAction, SIGNAL(triggered()), this, SLOT(removeItem()));

  // This sucks.
  connect(currentBorrowedList->model(), &QAbstractItemModel::rowsInserted,
          [=] { removeItemAction->setDisabled(true); });
  connect(currentBorrowedList->model(), &QAbstractItemModel::rowsRemoved,
          [=] { removeItemAction->setDisabled(true); });
  connect(currentBorrowedList, &QListWidget::itemClicked,
          [=] { removeItemAction->setDisabled(false); });

  addWidget(toolbar);
  addWidget(tabWidget);
}

void PatronHistory::addItem()
{
  ChooseItemDialog chooseItemDialog(currentPatron);
  if (chooseItemDialog.exec() == QDialog::Accepted) { reload(); }
}

void PatronHistory::reload()
{
  currentBorrowedList->clear();

  // Get the keys of all items borrowed by the patron
  QStringList itemKeys;
  QSqlQuery getKeys(QSqlDatabase::database());
  getKeys.prepare("SELECT Ikey FROM borrowed WHERE Pkey = :key;");
  getKeys.bindValue(":key", currentPatron);
  getKeys.exec();
  while (getKeys.next()) { itemKeys << getKeys.value(0).toString(); }

  // Load items into the currentBorrowedList widget
  for (int i = 0; i < itemKeys.size(); ++i)
    {
      QString key = itemKeys.at(i);
      QString table = key.endsWith("b") ? "books" : "discs";
      QSqlQuery getItemTitle(QSqlDatabase::database());
      getItemTitle.prepare(QString("SELECT title FROM %1 WHERE key = :key;").arg(table));
      getItemTitle.bindValue(":key", key);
      getItemTitle.exec();
      getItemTitle.next();

      QListWidgetItem* item = new QListWidgetItem(getItemTitle.value(0).toString());
      item->setData(Qt::UserRole, QVariant(key));
      currentBorrowedList->addItem(item);
    }
}

void PatronHistory::removeItem()
{
  QString key = currentBorrowedList->currentItem()->data(Qt::UserRole).toString();
  QString table = key.endsWith("b") ? "books" : "discs";

  // Update database
  QSqlQuery removeFromBorrowed(QSqlDatabase::database());
  removeFromBorrowed.prepare("DELETE FROM borrowed WHERE Ikey = :key;");
  removeFromBorrowed.bindValue(":key", key);
  removeFromBorrowed.exec();

  QSqlQuery updateStatus(QSqlDatabase::database());
  updateStatus.prepare(QString("UPDATE %1 SET onLoan = 0 WHERE key = :key;").arg(table));
  updateStatus.bindValue(":key", key);
  updateStatus.exec();

  delete currentBorrowedList->takeItem(currentBorrowedList->currentRow());
  reload();
}

void PatronHistory::setPatron(QString patron_key)
{
  tabWidget->setDisabled(false);
  toolbar->setDisabled(false);
  currentPatron = patron_key;
  reload();

  if (currentBorrowedList->count() == 0) { removeItemAction->setDisabled(true); }
}
