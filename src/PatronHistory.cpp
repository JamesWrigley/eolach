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

#include <QAction>
#include <QToolBar>
#include <QSqlQuery>
#include "PatronHistory.h"
#include "ChooseItemDialog.h"

PatronHistory::PatronHistory()
{
  // Set up the toolbar
  QToolBar* toolbar = new QToolBar();
  QAction* addItemAction = new QAction(QIcon(":/add-icon"), "", this);
  QAction* removeItemAction = new QAction(QIcon(":/remove-icon"), "", this);
  connect(addItemAction, SIGNAL(triggered()), this, SLOT(addItem()));
  toolbar->addAction(addItemAction);
  toolbar->addAction(removeItemAction);

  // Set up the tabwidget
  tabWidget = new QTabWidget();
  pastBorrowedList = new QListWidget();
  currentBorrowedList = new QListWidget();
  tabWidget->addTab(currentBorrowedList, "Current Items");
  tabWidget->addTab(pastBorrowedList, "Past Items");

  addWidget(toolbar);
  addWidget(tabWidget);
}

void PatronHistory::addItem()
{
  ChooseItemDialog chooseItemDialog;
  chooseItemDialog.exec();
}
