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

#include <random>
#include <QRegExp>
#include <QSqlQuery>
#include <QMessageBox>
#include "miscellanea.h"
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
  QStringList warnings;
  if (!title->valid)
      warnings << "Title";
  if (!author->valid)
      warnings << "Author";
  if (!genre->valid)
    warnings << "Genre";
  if (!isbn->valid)
    warnings << "ISBN";
  if (!publication_date->valid)
    warnings << "Publication Date";

  if (!warnings.isEmpty())
    {
      QString invalid_fields;
      if (warnings.size() == 1)
	{
	  invalid_fields = warnings[0];
	}
      else if (warnings.size() == 2)
	{
	  invalid_fields = warnings.join(" and ");
	}
      else if (warnings.size() > 2)
	{
	  warnings[warnings.size() - 1].prepend("and ");
	  invalid_fields = warnings.join(", ");
	}

      int confirm = QMessageBox::warning(this, "Warning",
                                         invalid_fields +
					 " invalid, would you like to continue anyway?",
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
  std::random_device key_gen;
  book_key = QString::number(key_gen()) + "b";

  // Sort the authors and genres
  QStringList authors_list = author->text().split(",", QString::SkipEmptyParts);
  for (int i = 0; i < authors_list.size(); ++i) { authors_list[i] = authors_list[i].simplified(); }
  authors_list.sort(Qt::CaseInsensitive);
  QStringList genres_list = genre->text().split(",", QString::SkipEmptyParts);
  for (int i = 0; i < genres_list.size(); ++i) { genres_list[i] = genres_list[i].simplified(); }
  genres_list.sort(Qt::CaseInsensitive);

  QSqlQuery insert(QSqlDatabase::database());
  insert.prepare("INSERT INTO bookstore (key, isbn, title, author, publication_date, genre) "
		 "VALUES (:key, :isbn, :title, :author, :publication_date, :genre)");
  insert.bindValue(":key", book_key);
  insert.bindValue(":isbn", isbn->text());
  insert.bindValue(":title", title->text());
  insert.bindValue(":author", authors_list.join(", "));
  insert.bindValue(":publication_date", publication_date->text());
  insert.bindValue(":genre", genres_list.join(", "));
  insert.exec();

  done(QDialog::Accepted);
}

void AddBookDialog::setup_completions()
{
  std::vector<QString> sql_fields = {"author", "genre"};
  std::vector<DLineEdit*> lineedits = {author, genre};

  for (unsigned int i = 0; i < sql_fields.size(); ++i)
    {
      QStringListModel* completion_model = new QStringListModel(this);
      QStringList completion_list;
      QSqlDatabase bookstore = QSqlDatabase::database();

      QSqlQuery get_column(bookstore);
      get_column.exec(QString("SELECT %1 FROM bookstore;").arg(sql_fields[i]));

      while (get_column.next())
        {
	  completion_list << get_column.value(0).toString().
	    split(",", QString::SkipEmptyParts);
        }
      for (int i = 0; i < completion_list.size(); ++i)
	{
	  completion_list[i] = completion_list[i].trimmed();
	}

      completion_list.removeDuplicates();
      completion_model->setStringList(completion_list);
      lineedits[i]->enable_completion(completion_model);
    }
}
