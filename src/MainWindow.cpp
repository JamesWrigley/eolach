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

#include <QMenuBar>
#include <QSqlQuery>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"
#include "miscellanea.h"
#include "AddItemDialog.h"

MainWindow::MainWindow()
{
  // Set up GUI
  keysTabwidget = new QTabWidget();
  infoWidget = new InfoWidget(this);
  booksWidget = new KeysWidget("books", getBookInfo, (QStringList() << "Title" << "Author"
                                                      << "Genre" << "Publication Date" << "ISBN"));
  discsWidget = new KeysWidget("discs", getDiscInfo, (QStringList() << "Title" << "Director/Speaker"
                                                      << "Length" << "Year" << "Type"));
  patronsWidget = new KeysWidget("patrons", getPatronInfo, (QStringList() << "Name" << "Address" <<
                                                            "Mobile No." << "Landline No."));
  splitter = new QSplitter(this);

  keysTabwidget->addTab(booksWidget, "Books");
  keysTabwidget->addTab(discsWidget, "Discs");
  keysTabwidget->addTab(patronsWidget, "Patrons");

  splitter->addWidget(keysTabwidget);
  splitter->addWidget(infoWidget);
  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 1);

  toolbar = addToolBar("");
  fileMenu = menuBar()->addMenu("File");

  exitAction = new QAction("Exit", this);
  exitAction->setShortcut(QKeySequence("Ctrl+Q"));

  addItemAction = new QAction(QIcon(":/new-item-icon"), "", this);
  addItemAction->setToolTip("Add new item");

  fileMenu->addAction(exitAction);
  toolbar->addAction(addItemAction);

  connect(keysTabwidget, SIGNAL(currentChanged(int)), infoWidget, SLOT(changeLayout(int)));

  for (KeysWidget *widget : {booksWidget, discsWidget, patronsWidget})
    {
      connect(widget, SIGNAL(itemRemoved()), this, SLOT(onItemRemoved()));
      connect(widget, SIGNAL(itemSelectionChanged()), this, SLOT(changeItem()), Qt::QueuedConnection);
    }

  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
  connect(addItemAction, SIGNAL(triggered()), this, SLOT(createAddItemDialog()));

  updateStatusbar();
  setCentralWidget(splitter);
  setWindowState(Qt::WindowMaximized);
  centerWindow();
  setWindowTitle("Eolach");
}

MainWindow::~MainWindow()
{
  bookstore.close();
}

/* General functions */

void MainWindow::createAddItemDialog()
{
  AddItemDialog addItemDialog(keysTabwidget->currentIndex());
  if (QDialog::Accepted == addItemDialog.exec())
    {
      if (addItemDialog.getItemKey().endsWith("b"))
	{
	  booksWidget->addItem(addItemDialog.getItemKey());
	}
      else if (addItemDialog.getItemKey().endsWith("d"))
	{
	  discsWidget->addItem(addItemDialog.getItemKey());
	}
      else if (addItemDialog.getItemKey().endsWith("p"))
	{
	  patronsWidget->addItem(addItemDialog.getItemKey());
	}
      updateStatusbar();
    }
}

void MainWindow::centerWindow()
{
  QRect screenGeo = QApplication::desktop()->screenGeometry();
  QWidget *mainWindow = QWidget::window();

  int x = (screenGeo.width() - 800) / 2;
  int y = (screenGeo.height() - 600) / 2;

  mainWindow->move(x,y);
}

/* Calculate the number of items in the DB display them in the statusbar */
void MainWindow::updateStatusbar()
{
  statusBar()->showMessage(QString::number(booksWidget->rowCount()) + " books, " +
			   QString::number(discsWidget->rowCount()) + " discs, " +
			   QString::number(patronsWidget->rowCount()) + " patrons.");
}

/* Slots */

void MainWindow::changeItem()
{
  KeysWidget* currentTab = static_cast<KeysWidget*>(keysTabwidget->currentWidget());
  if (currentTab->currentRow() > -1)
    {
      infoWidget->setItem(currentTab->currentItem()->data(Qt::UserRole).toString());
    }
}

void MainWindow::onItemRemoved()
{
  KeysWidget* currentTab = static_cast<KeysWidget*>(keysTabwidget->currentWidget());
  if (1 == currentTab->rowCount())
    {
      infoWidget->clear();
    }
  currentTab->removeRow(currentTab->currentRow());
  updateStatusbar();
}

/* Called when an items data is changed from the InfoWidget */
void MainWindow::onFieldChanged(QString dbTable, QString sqlFieldName, QString newText)
{
  KeysWidget* currentTab = static_cast<KeysWidget*>(keysTabwidget->currentWidget());

  // Well, it's supposed to only be called when item data is changed. It also
  // seems to be called when switching tabs, and if they're empty then
  // currentRow() will be -1, causing a segfault when we try to get itemKey
  // later on. Hence the check.
  if (currentTab->currentRow() > -1)
    {
      QString itemKey = currentTab->currentItem()->data(Qt::UserRole).toString();
      QSqlQuery updateBookInfo(bookstore);
      updateBookInfo.prepare(QString("UPDATE %1 SET %2=:newText WHERE key=:bookKey;").arg(dbTable, sqlFieldName));
      updateBookInfo.bindValue(":table", dbTable);
      updateBookInfo.bindValue(":newText", newText);
      updateBookInfo.bindValue(":bookKey", itemKey);
      updateBookInfo.exec();

      infoWidget->setItem(itemKey);
      currentTab->updateItem(currentTab->currentRow(), itemKey);
    }
}
