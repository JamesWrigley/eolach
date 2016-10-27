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

#include <QDateTime>
#include <QSqlQuery>

#include "PatronHistory.h"
#include "ChooseItemDialog.h"

PatronHistory::PatronHistory()
{
    // Set up the toolbar
    toolbar = new QToolBar();
    addItemAction = new QAction(QIcon(":/add-icon"), "", this);
    addItemAction->setToolTip("Add borrowed item");
    removeItemAction = new QAction(QIcon(":/remove-icon"), "", this);
    removeItemAction->setToolTip("Mark as returned");
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
    if (chooseItemDialog.exec() == QDialog::Accepted) {
        reload();
    }
}

void PatronHistory::onTabChanged(int index)
{
    if (index == 0) {
        addItemAction->setDisabled(false);
        removeItemAction->setDisabled(true);
        currentBorrowedList->setCurrentRow(-1);
    } else {
        addItemAction->setDisabled(true);
        removeItemAction->setDisabled(true);
    }
}

void PatronHistory::reload()
{
    if (!currentPatron.isEmpty()) {
        pastBorrowedList->clear();
        currentBorrowedList->clear();

        // Get the keys of all items borrowed by the patron (past and present)
        QSqlQuery getLoans(QSqlDatabase::database());
        getLoans.prepare("SELECT key, item, return_date FROM loans "
                         "WHERE patron = :patron;");
        getLoans.bindValue(":patron", currentPatron);
        getLoans.exec();

        while (getLoans.next()) {
            QString item_key = getLoans.value(1).toString();
            QString table = item_key.endsWith("b") ? "books" : "discs";
            QSqlQuery getItemTitle(QSqlDatabase::database());
            getItemTitle.prepare(QString("SELECT title FROM %1 WHERE key = :item;").arg(table));
            getItemTitle.bindValue(":item", item_key);
            getItemTitle.exec();
            getItemTitle.next();

            QListWidgetItem* item = new QListWidgetItem(getItemTitle.value(0).toString());
            // Set the user role to be the loan key
            item->setData(Qt::UserRole, QVariant(getLoans.value(0).toString()));

            if (getLoans.value(2).toString().isEmpty()) { // Open loan
                currentBorrowedList->addItem(item);
            } else { // Closed loan
                pastBorrowedList->addItem(item);
            }
        }
    }
}

void PatronHistory::setItemReturned()
{
    QListWidgetItem* item = currentBorrowedList->takeItem(currentBorrowedList->currentRow());
    QString loan_key = item->data(Qt::UserRole).toString();

    // Set return date on loan record
    QSqlQuery updateLoan(QSqlDatabase::database());
    updateLoan.prepare(QString("UPDATE loans SET return_date = :rd "
                               "WHERE key = :lkey;"));
    updateLoan.bindValue(":rd", QDateTime::currentDateTime());
    updateLoan.bindValue(":lkey", loan_key);
    updateLoan.exec();

    // Mark the item as available in it's table
    QSqlQuery getItemKey(QSqlDatabase::database());
    getItemKey.prepare(QString("SELECT item FROM loans WHERE key = :lkey;"));
    getItemKey.bindValue(":lkey", loan_key);
    getItemKey.exec();
    getItemKey.next();

    QString item_key = getItemKey.value(0).toString();
    QString table = item_key.endsWith("b") ? "books" : "discs";
    QSqlQuery updateItemStatus(QSqlDatabase::database());
    updateItemStatus.prepare(QString("UPDATE %1 SET onLoan = 0 "
                                     "WHERE key = :item_key;").arg(table));
    updateItemStatus.bindValue(":item_key", item_key);
    updateItemStatus.exec();

    reload();
}

void PatronHistory::setPatron(QString patron_key)
{
    tabWidget->setDisabled(false);
    toolbar->setDisabled(false);
    currentPatron = patron_key;
    reload();

    if (currentBorrowedList->count() == 0) {
        removeItemAction->setDisabled(true);
    }
}
