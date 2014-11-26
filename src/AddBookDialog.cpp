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

#include <QSqlQuery>
#include <QCryptographicHash>
#include "AddBookDialog.h"

AddBookDialog::AddBookDialog(QWidget *parent)
{
  main_layout = new QVBoxLayout(this);
  finish_button = new QPushButton("Finish");
  title = new QLineEdit();
  author = new QLineEdit();
  genre = new QLineEdit();
  isbn = new QLineEdit();
  publication_date = new QLineEdit();

  title->setPlaceholderText("Title");
  author->setPlaceholderText("Author");
  genre->setPlaceholderText("Genre");
  isbn->setPlaceholderText("ISBN");
  publication_date->setPlaceholderText("Publication Date");
  finish_button->setDefault(true);

  connect(finish_button, SIGNAL(clicked(bool)), this, SLOT(add_book()));

  main_layout->addWidget(title);
  main_layout->addWidget(author);
  main_layout->addWidget(genre);
  main_layout->addWidget(isbn);
  main_layout->addWidget(publication_date);
  main_layout->addStretch();
  main_layout->addWidget(finish_button);
  main_layout->addStretch();

  setLayout(main_layout);
}

void AddBookDialog::add_book()
{
  // Generate hash of book data to be used as a key
  QString book_data = title->text() + author->text() + genre->text() + isbn->text() + publication_date->text();
  QCryptographicHash sha1Hasher(QCryptographicHash::Sha1);
  sha1Hasher.addData(book_data.toUtf8());
  book_key = QString(sha1Hasher.result().toHex());

  // Insert new book into DB
  QSqlDatabase bookstore = QSqlDatabase::database();
  QString insert_sql = "INSERT INTO bookstore (key, isbn, title, author, publication_date, genre) "
    "VALUES ('" + book_key + "', '" + isbn->text() + "', '" + title->text() + "', '" + author->text() +
    "', '" + publication_date->text() + "', '" + genre->text() + "');";
  QSqlQuery insert(bookstore);
  insert.exec(insert_sql);

  done(QDialog::Accepted);
}
