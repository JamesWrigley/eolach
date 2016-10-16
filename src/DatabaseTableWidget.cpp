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

#include <QHeaderView>
#include <QSizePolicy>

#include "MainWindow.h"
#include "DatabaseTableWidget.h"

DatabaseTableWidget::DatabaseTableWidget(QString table, std::unordered_map<int, QString> headers)
{
    // Create model and set headers
    model = new QSqlTableModel(this, QSqlDatabase::database());
    model->setTable(table);
    model->select();
    for (auto& h : headers) {
        model->setHeaderData(h.first, Qt::Horizontal, h.second);
    }

    view = new QTableView(this);
    view->setModel(model);

    // Hide Key, ISBN, genre, and loan columns
    view->hideColumn(0);
    view->hideColumn(1);
    view->hideColumn(5);
    view->hideColumn(6);

    // Configure view
    view->setSortingEnabled(true);
    view->sortByColumn(2, Qt::AscendingOrder); // Sort by title
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged,
            [](const QModelIndex& current, const QModelIndex&) {
                auto key = current.sibling(current.row(), 0);
                emit MainWindow::signaller->itemSelected(key.data().toString());
            });

    // Preserve selection after sorting
    connect(view->horizontalHeader(), &QHeaderView::sectionPressed,
            [&] (int) {
                auto selectedIndexes = view->selectionModel()->selectedIndexes();
                if (!selectedIndexes.isEmpty()) {
                    selected = selectedIndexes.first().data().toString();
                }
            });
    connect(view->horizontalHeader(), &QHeaderView::sectionClicked,
            [&] (int) {
                if (!selected.isEmpty() && !view->selectionModel()->currentIndex().isValid()) {
                    for (int row = 0; row < model->rowCount(); ++row) {
                        auto key = model->index(row, 0);
                        if (key.data().toString() == selected) {
                            view->selectRow(row);
                            break;
                        }
                    }
                }
            });

    // Set up cell context menus
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->connect(view, &QTableView::customContextMenuRequested,
                  this, &DatabaseTableWidget::createItemContextMenu);

    itemContextMenu = new QMenu(this);
    removeItemAction = new QAction("Remove", this);
    itemContextMenu->addAction(removeItemAction);
    view->connect(removeItemAction, &QAction::triggered,
                  this, &DatabaseTableWidget::removeItem);

    // Create layout
    layout = new QVBoxLayout();
    layout->addWidget(view);
    setLayout(layout);
}

void DatabaseTableWidget::createItemContextMenu(QPoint pos)
{
    if (view->indexAt(pos).isValid()) {
        itemContextMenu->popup(view->viewport()->mapToGlobal(pos));
    }
}

void DatabaseTableWidget::createHeaderContextMenu(QPoint pos)
{
    headerContextMenu->popup(mapToGlobal(pos));
}

void DatabaseTableWidget::removeItem() { }

void DatabaseTableWidget::addItem(QString itemKey) { }

unsigned int DatabaseTableWidget::rowCount()
{
    return model->rowCount();
}
