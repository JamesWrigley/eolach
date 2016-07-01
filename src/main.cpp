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

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // Set up DB connection
    QSqlDatabase bookstore;
    QString configDirPath = QDir().homePath() + "/.eolach/";
    QString dbPath = configDirPath + "bookstore.db";

    if (QFile(dbPath).exists()) {
        bookstore = QSqlDatabase::addDatabase("QSQLITE");
        bookstore.setDatabaseName(dbPath);
        bookstore.open();
    } else {
        QDir().mkdir(configDirPath);
        bookstore = QSqlDatabase::addDatabase("QSQLITE");
        bookstore.setDatabaseName(dbPath);
        bookstore.open();

        QSqlQuery createBooksTable(bookstore);
        QSqlQuery createDiscsTable(bookstore);
        QSqlQuery createLoansTable(bookstore);
        QSqlQuery createPatronsTable(bookstore);

        createBooksTable.exec("CREATE TABLE books ("
                              "key TEXT PRIMARY KEY, "
                              "isbn TEXT, "
                              "title TEXT, "
                              "author TEXT, "
                              "publication_date TEXT, "
                              "genre TEXT, "
                              "onLoan BOOLEAN);");

        createDiscsTable.exec("CREATE TABLE discs ("
                              "key TEXT PRIMARY KEY, "
                              "title TEXT, "
                              "directorOrSpeaker TEXT, "
                              "length TEXT, "
                              "year TEXT, "
                              "type TEXT, "
                              "onLoan BOOLEAN);");

        createPatronsTable.exec("CREATE TABLE patrons ("
                                "key TEXT PRIMARY KEY, "
                                "name TEXT, "
                                "address TEXT, "
                                "mobile_num TEXT, "
                                "landline_num TEXT);");        

        createLoansTable.exec("CREATE TABLE loans ("
                              "id INTEGER PRIMARY KEY, "
                              "item TEXT, "
                              "patron TEXT, "
                              "loan_date TEXT, "
                              "due_date TEXT, "
                              "return_date TEXT);");
    }

    // Start the application
    QApplication app(argc, argv);
    MainWindow mainwindow;
    mainwindow.show();

    return app.exec();
}
