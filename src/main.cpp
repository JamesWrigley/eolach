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
#include <QSqlQuery>
#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
  // Set up DB connection
  QSqlDatabase bookstore;
  QString config_dir_path = QDir().homePath() + "/.eolach/";
  QString db_path = config_dir_path + "bookstore.db";

  if (QFile(db_path).exists())
    {
      bookstore = QSqlDatabase::addDatabase("QSQLITE");
      bookstore.setDatabaseName(db_path);
      bookstore.open();
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
    }

  // Start the application
  QApplication app(argc, argv);
  QApplication::setStyle(QStyleFactory::create("gtk"));
  MainWindow mainwindow;
  mainwindow.show();

  return app.exec();
}
