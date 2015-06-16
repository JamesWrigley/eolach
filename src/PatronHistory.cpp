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
#include <QStringList>
#include "PatronHistory.h"
#include "ChooseItemDialog.h"

PatronHistory::PatronHistory()
{
  // Set up the toolbar
  toolbar = new QToolBar();
  addItemAction = new QAction(QIcon(":/add-icon"), "", this);
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
  connect(removeItemAction, SIGNAL(triggered()), this, SLOT(setItemReturned()));
  connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));

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

QListWidgetItem* PatronHistory::createListWidgetItem(QString key)
{
  QString table = key.endsWith("b") ? "books" : "discs";
  QSqlQuery getItemTitle(QSqlDatabase::database());
  getItemTitle.prepare(QString("SELECT title FROM %1 WHERE key = :key;").arg(table));
  getItemTitle.bindValue(":key", key);
  getItemTitle.exec();
  getItemTitle.next();

  QListWidgetItem* item = new QListWidgetItem(getItemTitle.value(0).toString());
  item->setData(Qt::UserRole, QVariant(key));
  return item;
}

void PatronHistory::onTabChanged(int index)
{
  if (index == 0)
    {
      addItemAction->setDisabled(false);
      removeItemAction->setDisabled(true);
      currentBorrowedList->setCurrentRow(-1);
    }
  else
    {
      addItemAction->setDisabled(true);
      removeItemAction->setDisabled(true);
    }
}

void PatronHistory::reload()
{
  if (!currentPatron.isEmpty())
    {
      pastBorrowedList->clear();
      currentBorrowedList->clear();

      // Get the keys of all items borrowed by the patron (past and present)
      QStringList pastItemKeys;
      QStringList currentItemKeys;
      QSqlQuery getPastKeys(QSqlDatabase::database());
      QSqlQuery getCurrentKeys(QSqlDatabase::database());
      getPastKeys.prepare("SELECT Ikey FROM pastBorrowed WHERE Pkey = :key;");
      getCurrentKeys.prepare("SELECT Ikey FROM currentBorrowed WHERE Pkey = :key;");
      getCurrentKeys.bindValue(":key", currentPatron);
      getPastKeys.bindValue(":key", currentPatron);
      getCurrentKeys.exec();
      getPastKeys.exec();

      while (getPastKeys.next()) { pastItemKeys << getPastKeys.value(0).toString(); }
      while (getCurrentKeys.next()) { currentItemKeys << getCurrentKeys.value(0).toString(); }

      // Load items into the QListWidget's
      for (int i = 0; i < currentItemKeys.size(); ++i)
        {
          currentBorrowedList->addItem(createListWidgetItem(currentItemKeys.at(i)));
        }

      for (int i = 0; i < pastItemKeys.size(); ++i)
        {
          pastBorrowedList->addItem(createListWidgetItem(pastItemKeys.at(i)));
        }
    }
}

void PatronHistory::setItemReturned()
{
  QListWidgetItem* item = currentBorrowedList->takeItem(currentBorrowedList->currentRow());
  QString key = item->data(Qt::UserRole).toString();
  QString table = key.endsWith("b") ? "books" : "discs";

  // Remove from currentBorrowed
  QSqlQuery removeFromCurrentBorrowed(QSqlDatabase::database());
  removeFromCurrentBorrowed.prepare("DELETE FROM currentBorrowed WHERE Ikey = :key;");
  removeFromCurrentBorrowed.bindValue(":key", key);
  removeFromCurrentBorrowed.exec();

  // Mark the item as available in it's table
  QSqlQuery updateStatus(QSqlDatabase::database());
  updateStatus.prepare(QString("UPDATE %1 SET onLoan = 0 WHERE key = :key;").arg(table));
  updateStatus.bindValue(":key", key);
  updateStatus.exec();

  // Check if the item was taken out previously by the same person,
  // and create/update the record as needed.
  QSqlQuery checkExists(QSqlDatabase::database());
  checkExists.prepare("SELECT COUNT(*) FROM pastBorrowed WHERE Pkey = :pkey AND Ikey = :ikey");
  checkExists.bindValue(":pkey", currentPatron);
  checkExists.bindValue(":ikey", key);
  checkExists.exec();
  checkExists.next();

  if (checkExists.value(0).toInt() == 0)
    {
      QSqlQuery createEntry(QSqlDatabase::database());
      createEntry.prepare("INSERT INTO pastBorrowed (Pkey, Ikey, borrowedCount) "
                          "VALUES(:pkey, :ikey, 1);");
      createEntry.bindValue(":pkey", currentPatron);
      createEntry.bindValue(":ikey", key);
      createEntry.exec();
    }
  else
    {
      QSqlQuery updateEntry(QSqlDatabase::database());
      updateEntry.prepare("UPDATE pastBorrowed SET borrowedCount = borrowedCount + 1"
                          "WHERE Pkey = :pkey AND Ikey = :ikey;");
      updateEntry.bindValue(":pkey", currentPatron);
      updateEntry.bindValue(":ikey", key);
      updateEntry.exec();
    }

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
