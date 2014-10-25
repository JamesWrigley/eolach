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

#include <QtGui>
#include <QFile>
#include <QMenuBar>
#include <iostream>
#include <QSqlQuery>
#include <QStatusBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"

MainWindow::MainWindow()
{
  initialize();
  populate_keys();

  // Set the info_widget to display the books info when a key is clicked,
  // and to change the info of a book when its key data is changed.
  QObject::connect(keys_widget, SIGNAL(cellClicked(int, int)),
                   this, SLOT(change_book_on_click(int)));
  //  QObject::connect(keys_widget, SIGNAL(itemChanged(QTableWidgetItem *)),
  //                   this, SLOT(on_cell_changed(QTableWidgetItem *)));

  // We need to do this after the table is populated, otherwise weird stuff happens
  keys_widget->selectRow(0);
  info_widget->set_book(key_table.value(keys_widget->currentRow()));

  this->center_window();
  this->setWindowTitle("Eolach");
}

void MainWindow::change_book_on_click(int r)
{
  info_widget->set_book(key_table.value(r));
}

/*
void MainWindow::on_cell_changed(QTableWidgetItem *item)
{
  QString new_data = item->text();
  Book *current_book = &key_table[keys_widget->model()->data(keys_widget->model()->index(item->row(), 0)).toString()];

  if (item->column() == 0)
    {
      current_book->title = new_data;
    }
  else if (item->column() == 1)
    {
      current_book->author = new_data;
    }

  info_widget->set_book(key_table[current_book->title]);
}
*/

void MainWindow::populate_keys()
{
  // Initialize test books
  Book TMI = Book("9780760714898", "The Mysterious Island", "Jules Verne", "1874", "Science Fiction");
  Book TCOMC = Book("9780895263469", "The Count of Monte Cristo", "Alexandre Dumas", "1844", "Fiction");
  Book E = Book("9780573698996", "Emma", "Jane Austen", "1815", "Fiction");
  Book TIM = Book("9781598898316", "The Invisible Man", "H.G.Wells", "1897", "Fiction");
  Book WAP = Book("9780143039990", "War and Peace", "Leo Tolstoy", "1869", "War Novel");

  insert_book(TMI);
  insert_book(TCOMC);
  insert_book(E);
  insert_book(TIM);
  insert_book(WAP);
}

void MainWindow::insert_book(Book book)
{
  // Generate hash of book data to be used as a key
  QString book_data = book.isbn + book.title + book.author + book.publication_date + book.genre;
  QCryptographicHash sha1Hasher(QCryptographicHash::Sha1);
  sha1Hasher.addData(book_data.toUtf8());
  QString book_key = QString(sha1Hasher.result().toHex());

  QString insert_sql = "INSERT INTO bookstore (key, isbn, title, author, publication_date, genre, available)"
    "VALUES (" + book_key + "," + book.isbn + "," + book.title + "," + book.author +
    "," + book.publication_date + "," + book.genre + "," + book.synopsis + "," +
    book.available + ")";

  QSqlQuery insert;
  insert.exec(insert_sql);

  key_table.insert(keys_widget->rowCount(), book_key);
}

void MainWindow::initialize()
{
  // Set up DB connection
  QString config_dir_path = QDir().homePath() + "/.eolach/";
  QString db_path = config_dir_path + "bookstore.db";

  if (QFile(db_path).exists())
    {
      bookstore = QSqlDatabase::addDatabase("QSQLITE");
      bookstore.open();
    }
  else
    {
      // Note: the key is defined as the SHA1 hash of the book data
      QString initialize_db_sql = "CREATE TABLE bookstore ("
        "key TEXT PRIMARY KEY,"
        "isbn TEXT,"
        "title TEXT,"
        "author TEXT,"
        "publication_date TEXT,"
        "genre TEXT,"
        "synopsis TEXT,"
        "available BOOLEAN"
        "isbn TEXT);";

      QDir().mkdir(config_dir_path);

      bookstore = QSqlDatabase::addDatabase("QSQLITE");
      bookstore.setDatabaseName(db_path);
      bookstore.open();

      QSqlQuery initialize_db;
      initialize_db.exec(initialize_db_sql);
    }

  // Set up GUI
  keys_tabwidget = new QTabWidget(this);
  splitter = new QSplitter(this);
  keys_widget = new KeysWidget();
  info_widget = new InfoWidget();

  keys_tabwidget->addTab(keys_widget, "Books");

  splitter->addWidget(keys_tabwidget);
  splitter->addWidget(info_widget);

  QAction *exit_action = new QAction("Exit", this);
  exit_action->setShortcut(QKeySequence("Ctrl+Q"));
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

  file_menu = menuBar()->addMenu("File");
  file_menu->addAction(exit_action);

  this->statusBar()->showMessage("Ready");
  this->setCentralWidget(splitter);
  this->setWindowState(Qt::WindowMaximized);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  bookstore.close();
}

void MainWindow::center_window()
{
  QRect screen_geo = QApplication::desktop()->screenGeometry();
  QWidget *main_window = QWidget::window();

  int x = (screen_geo.width() - 800) / 2;
  int y = (screen_geo.height() - 600) / 2;

  main_window->move(x,y);
}
