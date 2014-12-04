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
#include <QMessageBox>
#include <QCryptographicHash>
#include "AddBookDialog.h"

AddBookDialog::AddBookDialog(QWidget *parent)
{
  main_layout = new QVBoxLayout(this);

  title = new DLineEdit("Title", &validate_generic_field);
  author = new DLineEdit("Author", &validate_generic_field);
  genre = new DLineEdit("Genre", &validate_generic_field);
  publication_date = new DLineEdit("Publication Date", &validate_numeric_field);
  isbn = new DLineEdit("ISBN", &validate_isbn);
  setup_completions();

  finish_button = new QPushButton("Finish");
  finish_button->setDefault(true);

  connect(finish_button, SIGNAL(clicked(bool)), this, SLOT(check_fields()));

  main_layout->addLayout(title);
  main_layout->addLayout(author);
  main_layout->addLayout(genre);
  main_layout->addLayout(isbn);
  main_layout->addLayout(publication_date);
  main_layout->addStretch();
  main_layout->addWidget(finish_button);
  main_layout->addStretch();

  setLayout(main_layout);
}

void AddBookDialog::check_fields()
{
  if (!validate_isbn(isbn->text()))
    {
      int confirm = QMessageBox::warning(this, "Warning",
                                         "ISBN invalid, would you like to continue anyway?",
                                         QMessageBox::Yes, QMessageBox::No);
      if (QMessageBox::Yes == confirm)
        {
          add_book();
        }
    }
  else
    {
      add_book();
    }
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

void AddBookDialog::setup_completions()
{
  std::vector<QString> sql_fields = {"author", "genre"};
  std::vector<DLineEdit*> lineedits = {author, genre};

  for (unsigned int i = 0; i < sql_fields.size(); ++i)
    {
      QStringList completions;
      QSqlDatabase bookstore = QSqlDatabase::database();
      QSqlQuery get_column(bookstore);
      get_column.exec("SELECT " + sql_fields[i] + " FROM bookstore;");

      while (get_column.next())
        {
          completions << get_column.value(0).toString();
        }

      completions.removeDuplicates();
      lineedits[i]->enable_completion(completions);
    }
}

bool AddBookDialog::validate_generic_field(QString field_text)
{
  return field_text.length() > 0;
}

bool AddBookDialog::validate_numeric_field(QString field_text)
{
  bool success = false;
  field_text.toFloat(&success);
  return success;
}

/* Implements the checksumming algorithm for 10 and 13 digit ISBN's */
bool AddBookDialog::validate_isbn(QString isbn_value)
{
  int sum = 0;
  int check_digit = 10;

  if (10 == isbn_value.length())
    {
      for (int i = 0; i < isbn_value.length() - 1; ++i)
        {
          sum += isbn_value[i].digitValue() * (10 - i);
        }
      check_digit = (11 - (sum % 11)) % 11;
    }
  else if (13 == isbn_value.length())
    {
      for (int i = 1; i < isbn_value.length(); i += 2)
        {
          sum += isbn_value[i].digitValue() * 3;
          sum += isbn_value[i - 1].digitValue();
        }
      check_digit = (10 - (sum % 10)) % 10;
    }

  return isbn_value.endsWith(QString::number(check_digit));
}
