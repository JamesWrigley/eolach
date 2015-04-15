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
#include "AddItemDialog.h"

AddItemDialog::AddItemDialog(QWidget *parent)
{
  main_layout = new QVBoxLayout(this);
  stacker = new QStackedWidget();
  item_selector = new QComboBox();
  selector_hbox  = new QHBoxLayout();

  // Book fields widget
  book_widget = new QWidget(this);
  QVBoxLayout *book_layout = new QVBoxLayout();
  title = new DLineEdit("Title", &validate_generic_field);
  author = new DLineEdit("Author", &validate_generic_field);
  genre = new DLineEdit("Genre", &validate_generic_field);
  publication_date = new DLineEdit("Publication Date", &validate_numeric_field);
  isbn = new DLineEdit("ISBN", &validate_isbn);
  book_fields = {title, author, genre, publication_date, isbn};
  for (DLineEdit *field : book_fields) { book_layout->addLayout(field); }
  book_widget->setLayout(book_layout);
  // setup_completions();

  // Patron fields widget
  patron_widget = new QWidget(this);
  QVBoxLayout *patron_layout = new QVBoxLayout();
  name = new DLineEdit("Name", &validate_generic_field);
  address = new DLineEdit("Address", &validate_generic_field);
  mobile_num = new DLineEdit("Mobile No.", &validate_numeric_field);
  landline_num = new DLineEdit("Landline No.", &validate_numeric_field);
  patron_fields = {name, address, mobile_num, landline_num};
  for (DLineEdit *field : patron_fields) { patron_layout->addLayout(field); }
  patron_widget->setLayout(patron_layout);

  selector_description = new QLabel("Add:");
  
  stacker->addWidget(book_widget);
  stacker->addWidget(patron_widget);
  item_selector->addItem("Book");
  item_selector->addItem("Patron");
  finish_button = new QPushButton("Finish");
  finish_button->setDefault(true);

  connect(finish_button, SIGNAL(clicked(bool)), this, SLOT(check_fields()));
  connect(item_selector, SIGNAL(currentIndexChanged(int)), stacker, SLOT(setCurrentIndex(int)));

  selector_hbox->addWidget(selector_description);
  selector_hbox->addWidget(item_selector);
  main_layout->addLayout(selector_hbox);
  main_layout->addWidget(stacker);
  main_layout->addStretch();
  main_layout->addWidget(finish_button);
  setLayout(main_layout);
}

void AddItemDialog::check_fields()
{
  QStringList warnings;
  for (DLineEdit *field : stacker->currentIndex() == 0 ? book_fields : patron_fields) {
    if (!field->valid)
      {
	warnings << field->placeholderText();
      }
  }

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

void AddItemDialog::add_book()
{
  // Generate hash of book data to be used as a key
  std::random_device key_gen;
  item_key = QString::number(key_gen()) + "b";

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
  insert.bindValue(":key", item_key);
  insert.bindValue(":isbn", isbn->text());
  insert.bindValue(":title", title->text());
  insert.bindValue(":author", authors_list.join(", "));
  insert.bindValue(":publication_date", publication_date->text());
  insert.bindValue(":genre", genres_list.join(", "));
  insert.exec();

  done(QDialog::Accepted);
}

void AddItemDialog::add_patron()
{
  std::random_device key_gen;
  item_key = QString::number(key_gen()) + "p";

  QSqlQuery insert(QSqlDatabase::database());
  insert.prepare("INSERT INTO patrons (key, name, address, mobile_num, landline_num, items) "
		 "VALUES (:key, :name, :address, :mobile_num, :landline_num, :items);");
  insert.bindValue(":key", item_key);
  insert.bindValue(":name", name->text());
  insert.bindValue(":address", address->text());
  insert.bindValue(":mobile_num", mobile_num->text());
  insert.bindValue(":landline_num", landline_num->text());
  insert.bindValue(":items", "");
  insert.exec();

  done(QDialog::Accepted);
}

void AddItemDialog::setup_completions()
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
