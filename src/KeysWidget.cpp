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
#include <QSqlRecord>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlDatabase>
#include "KeysWidget.h"
#include "miscellanea.h"

KeysWidget::KeysWidget(QString table, QString get_item_info_query, QStringList header_list)
{
    dbTable = table;
    headers = header_list;
    getItemInfoQuery = get_item_info_query;

    setColumnCount(headers.length());
    visibleColumnCount = headers.length();
    setHorizontalHeaderLabels(headers);
    headerContextMenu = new QMenu(this);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(createHeaderContextMenu(QPoint)));

    for (QString text : headers) {
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
    QStringList itemInfo = getItemInfo(itemKey);

    insertRow(rowCount());
    // We temporarily disable sorting because it causes indexing complications if
    // the row is sorted before we're finished populating its cells.
    std::pair<int, Qt::SortOrder> sortingInfo(disableSorting());
    for (int i = headers.length() - 1; i >= 0; --i) {
        QTableWidgetItem *item = new QTableWidgetItem(itemInfo.at(i));
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

QStringList KeysWidget::getItemInfo(QString itemKey)
{
    QSqlQuery getInfo(QSqlDatabase::database());
    getInfo.prepare(getItemInfoQuery);
    getInfo.bindValue(":key", itemKey);
    getInfo.exec();
    getInfo.next();

    QStringList itemInfo;
    for (int i = 0; i < getInfo.record().count(); ++i) {
        itemInfo << getInfo.value(i).toString();
    }

    return itemInfo;
}

void KeysWidget::loadItems()
{
    QSqlQuery getItemKeys(QSqlDatabase::database());
    getItemKeys.exec(QString("SELECT key FROM %1;").arg(dbTable));

    while (getItemKeys.next()) {
        addItem(getItemKeys.value(0).toString());
    }
}

void KeysWidget::updateItem(int row, QString itemKey)
{
    QStringList itemInfo = getItemInfo(itemKey);

    std::pair<int, Qt::SortOrder> sortingInfo(disableSorting());
    for (int i = 0; i < headers.length(); ++i) {
        item(row, i)->setText(itemInfo.at(i));
    }
    enableSorting(sortingInfo.first, sortingInfo.second);
}

/* Slots */

void KeysWidget::createItemContextMenu(QPoint pos)
{
    if (itemAt(pos) != 0) {
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
    if (checked) {
        showColumn(headerIndex);
        ++visibleColumnCount;
    } else if (!checked && visibleColumnCount > 1) {
        hideColumn(headerIndex);
        --visibleColumnCount;
    } else {
        static_cast<QAction*>(sender())->setChecked(true);
        --visibleColumnCount;
    }
}

void KeysWidget::removeItem()
{
    int confirm = QMessageBox::warning(this, "Confirm",
                                       QString("Are you sure you wish to remove this item?"),
                                       QMessageBox::Yes, QMessageBox::No);
    if (QMessageBox::Yes == confirm) {
        QString itemKey = currentItem()->data(Qt::UserRole).toString();
        QSqlQuery removeItem(QSqlDatabase::database());
        removeItem.prepare(QString("DELETE FROM %1 WHERE key=:key;").arg(dbTable));
        removeItem.bindValue(":key", itemKey);
        removeItem.exec();

        QSqlQuery removeFromBorrowed(QSqlDatabase::database());
        removeFromBorrowed.prepare("DELETE FROM currentBorrowed WHERE Ikey = :key;");
        removeFromBorrowed.bindValue(":key", itemKey);
        removeFromBorrowed.exec();

        emit itemRemoved();
    }
}
