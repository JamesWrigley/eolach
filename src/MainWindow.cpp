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

#include <iostream>
#include <QMenuBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QStatusBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include "BookItem.h"
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"
#include "AddBookDialog.h"

MainWindow::MainWindow()
{
  // Set up GUI
  keys_tabwidget = new QTabWidget();
  books_widget = new KeysWidget();
  info_widget = new InfoWidget();
  splitter = new QSplitter(this);

  keys_tabwidget->addTab(books_widget, "Books");

  splitter->addWidget(keys_tabwidget);
  splitter->addWidget(info_widget);
  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 1);

  QIcon::setThemeName("oxygen");
  toolbar = addToolBar("");
  file_menu = menuBar()->addMenu("File");

  exit_action = new QAction("Exit", this);
  add_book_action = new QAction(QIcon::fromTheme("list-add"), "", this);
  exit_action->setShortcut(QKeySequence("Ctrl+Q"));

  file_menu->addAction(exit_action);
  toolbar->addAction(add_book_action);  

  books_widget->load_items();
  if (books_widget->rowCount() > 0)
    {
      books_widget->selectRow(0);
      // We need to do a static_cast since currentItem() returns a QTableWidgetItem*
      info_widget->set_book(static_cast<BookItem*>(books_widget->currentItem())->book_key);
    }

  connect(books_widget, SIGNAL(currentCellChanged(int, int, int, int)),
          this, SLOT(change_book()));
  connect(info_widget, SIGNAL(fieldChanged(QString, QString)),
          this, SLOT(onFieldChanged(QString, QString)));
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));
  connect(add_book_action, SIGNAL(triggered()), this, SLOT(create_add_book_dialog()));

  update_statusbar();
  setCentralWidget(splitter);
  setWindowState(Qt::WindowMaximized);
  center_window();
  setWindowTitle("Eolach");
}

MainWindow::~MainWindow()
{
  bookstore.close();
}

/* General functions */

void MainWindow::create_add_book_dialog()
{
  AddBookDialog add_book_dialog(this);
  if (QDialog::Accepted == add_book_dialog.exec())
    {
      books_widget->add_book(add_book_dialog.book_key);
      update_statusbar();
    }
}

void MainWindow::center_window()
{
  QRect screen_geo = QApplication::desktop()->screenGeometry();
  QWidget *main_window = QWidget::window();

  int x = (screen_geo.width() - 800) / 2;
  int y = (screen_geo.height() - 600) / 2;

  main_window->move(x,y);
}

void MainWindow::update_statusbar()
{
  statusBar()->showMessage(QString::number(books_widget->rowCount()) + " books.");
}

/* Slots */

void MainWindow::change_book()
{
  info_widget->set_book(static_cast<BookItem*>(books_widget->currentItem())->book_key);
}

void MainWindow::onFieldChanged(QString sql_field_name, QString new_text)
{
  QString book_key = static_cast<BookItem*>(books_widget->currentItem())->book_key;

  QSqlQuery update_book_info(bookstore);
  update_book_info.exec("UPDATE bookstore SET " + sql_field_name + "='" + new_text +
                        "' WHERE key='" + book_key + "';");

  info_widget->set_book(book_key);
  books_widget->update_book(books_widget->currentRow(), book_key);
}
