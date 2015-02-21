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
#include "AddBookDialog.h"

MainWindow::MainWindow()
{
  // Set up GUI
  keys_tabwidget = new QTabWidget();
  create_info_widget();
  books_widget = new KeysWidget();
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

  if (books_widget->rowCount() > 0)
    {
      books_widget->selectRow(0);
      info_widget->set_book(books_widget->currentItem()->data(Qt::UserRole).toString());
    }

  connect(books_widget, SIGNAL(currentCellChanged(int, int, int, int)),
          this, SLOT(change_book()));
  connect(books_widget, SIGNAL(bookRemoved()), this, SLOT(onBookRemoved()));
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

void MainWindow::create_info_widget()
{
  info_widget = new InfoWidget();
  
  title = new TextField("title", "Title:", &AddBookDialog::validate_generic_field);
  author = new TextField("author", "Author:", &AddBookDialog::validate_generic_field);
  genre = new TextField("genre", "Genre:", &AddBookDialog::validate_generic_field);
  publication_date = new TextField("publication_date", "Publication Date:",
				   &AddBookDialog::validate_numeric_field);
  isbn = new TextField("isbn", "ISBN:", &AddBookDialog::validate_isbn);

  for (TextField* field : {title, author, genre, publication_date, isbn})
    {
      connect(field, SIGNAL(fieldChanged(QString, QString)),
              this, SLOT(onFieldChanged(QString, QString)));
      info_widget->add_field(field);
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

/* Calculate the number of books in the DB (by counting the rows in books_widget)
   and displays them in the statusbar */
void MainWindow::update_statusbar()
{
  statusBar()->showMessage(QString::number(books_widget->rowCount()) + " books.");
}

/* Slots */

void MainWindow::change_book()
{
  if (books_widget->currentRow() > -1)
    {
      info_widget->set_book(books_widget->currentItem()->data(Qt::UserRole).toString());
    }
}

void MainWindow::onBookRemoved()
{
  if (1 == books_widget->rowCount())
    {
      info_widget->clear();
    }
  books_widget->removeRow(books_widget->currentRow());
  update_statusbar();
}

/* Called when a books data is changed from the info_widget */
void MainWindow::onFieldChanged(QString sql_field_name, QString new_text)
{
  // Warn the user if the ISBN is invalid
  if ("isbn" == sql_field_name && !AddBookDialog::validate_isbn(new_text))
    {
      int confirm = QMessageBox::warning(this, "Warning",
                                         "ISBN invalid, would you like to continue anyway?",
                                         QMessageBox::Yes, QMessageBox::No);
      if (QMessageBox::No == confirm)
        {
          isbn->enterEditMode();
          return;
        }
    }

  QString book_key = books_widget->currentItem()->data(Qt::UserRole).toString();
  QSqlQuery update_book_info(bookstore);
  update_book_info.prepare(QString("UPDATE bookstore SET %1=:new_text WHERE key=:book_key;").arg(sql_field_name));
  update_book_info.bindValue(":new_text", new_text);
  update_book_info.bindValue(":book_key", book_key);
  update_book_info.exec();

  info_widget->set_book(book_key);
  books_widget->update_book(books_widget->currentRow(), book_key);
}
