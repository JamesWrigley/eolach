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

#include <QDir>
#include <QFile>
#include <QMenuBar>
#include <iostream>
#include <QSqlQuery>
#include <QSqlError>
#include <QStatusBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"

MainWindow::MainWindow()
{
  // Set up GUI
  keys_tabwidget = new QTabWidget();
  splitter = new QSplitter(this);
  books_widget = new KeysWidget();
  info_widget = new InfoWidget();

  keys_tabwidget->addTab(books_widget, "Books");

  splitter->addWidget(keys_tabwidget);
  splitter->addWidget(info_widget);
  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 1);

  exit_action = new QAction("Exit", this);
  exit_action->setShortcut(QKeySequence("Ctrl+Q"));
  QObject::connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

  file_menu = menuBar()->addMenu("File");
  file_menu->addAction(exit_action);

  // Set up DB connection
  QString config_dir_path = QDir().homePath() + "/.eolach/";
  QString db_path = config_dir_path + "bookstore.db";

  if (QFile(db_path).exists())
    {
      bookstore = QSqlDatabase::addDatabase("QSQLITE");
      bookstore.setDatabaseName(db_path);
      bookstore.open();

      // Load existing books from the DB
      QSqlQuery get_book_keys(bookstore);
      get_book_keys.exec("SELECT key FROM bookstore;");

      while (get_book_keys.next())
        {
          QString book_key = get_book_keys.value(0).toString();
          key_table.insert(books_widget->rowCount(), book_key);
          books_widget->add_book(book_key);
        }
    }
  else
    {
      // Note: the key is defined as the SHA1 hash of the book data
      QString initialize_db_sql = "CREATE TABLE bookstore ("
        "key TEXT PRIMARY KEY, "
        "isbn TEXT, "
        "title TEXT, "
        "author TEXT, "
        "publication_date TEXT, "
        "genre TEXT);";

      QDir().mkdir(config_dir_path);
      bookstore = QSqlDatabase::addDatabase("QSQLITE");
      bookstore.setDatabaseName(db_path);
      bookstore.open();

      QSqlQuery initialize_db(bookstore);
      initialize_db.exec(initialize_db_sql);

      populate_keys();
    }

  // Set the info_widget to display book info when one is clicked, and to change
  // the info of a book when it's edited.
  QObject::connect(books_widget, SIGNAL(currentCellChanged(int, int, int, int)),
                   this, SLOT(change_book(int)));
  QObject::connect(info_widget, SIGNAL(fieldChanged(QString, QString)), this, SLOT(onFieldChanged(QString, QString)));

  if (books_widget->rowCount() > 0)
    {
      books_widget->selectRow(0);
      info_widget->set_book(key_table.value(books_widget->currentRow()));
    }

  update_statusbar();

  this->setCentralWidget(splitter);
  this->setWindowState(Qt::WindowMaximized);
  this->center_window();
  this->setWindowTitle("Eolach");
}

MainWindow::~MainWindow()
{
  bookstore.close();
}

void MainWindow::add_book(QString isbn, QString title, QString author, QString publication_date, QString genre)
{
  // Generate hash of book data to be used as a key
  QString book_data = isbn + title + author + publication_date + genre;
  QCryptographicHash sha1Hasher(QCryptographicHash::Sha1);
  sha1Hasher.addData(book_data.toUtf8());
  QString book_key = QString(sha1Hasher.result().toHex());

  // Insert new book into DB
  QString insert_sql = "INSERT INTO bookstore (key, isbn, title, author, publication_date, genre) "
    "VALUES ('" + book_key + "', '" + isbn + "', '" + title + "', '" + author +
    "', '" + publication_date + "', '" + genre + "');";
  QSqlQuery insert(bookstore);
  bool success = insert.exec(insert_sql);
  if (!success)
    {
      std::cout << insert_sql.toStdString() << std::endl;
      std::cout << insert.lastError().text().toStdString() << std::endl;
    }

  // Insert the book hash into key_table, add the book to books_widget,
  // and update the statusbar.
  key_table.insert(books_widget->rowCount(), book_key);
  books_widget->add_book(book_key);
  update_statusbar();
}

void MainWindow::change_book(int r)
{
  info_widget->set_book(key_table.value(r));
}

void MainWindow::onFieldChanged(QString field_name, QString text)
{
  QString book_key = key_table.value(books_widget->currentRow());

  QSqlQuery update_book_info(bookstore);
  update_book_info.exec("UPDATE bookstore SET " + field_name + "='" + text +
                        "' WHERE key='" + book_key + "';");

  info_widget->set_book(book_key);
  books_widget->update_book(books_widget->currentRow(), book_key);
}

void MainWindow::update_statusbar()
{
  this->statusBar()->showMessage(QString::number(key_table.size()) + " books.");
}

void MainWindow::center_window()
{
  QRect screen_geo = QApplication::desktop()->screenGeometry();
  QWidget *main_window = QWidget::window();

  int x = (screen_geo.width() - 800) / 2;
  int y = (screen_geo.height() - 600) / 2;

  main_window->move(x,y);
}

void MainWindow::populate_keys()
{
  // Initialize test books
  add_book("9780760714898", "The Mysterious Island", "Jules Verne", "1874", "Science Fiction");
  add_book("9780895263469", "The Count of Monte Cristo", "Alexandre Dumas", "1844", "Fiction");
  add_book("9780573698996", "Emma", "Jane Austen", "1815", "Fiction");
  add_book("9781598898316", "The Invisible Man", "H.G.Wells", "1897", "Fiction");
  add_book("9780143039990", "War and Peace", "Leo Tolstoy", "1869", "War Novel");
}
