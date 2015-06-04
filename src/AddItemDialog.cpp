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
  mainLayout = new QVBoxLayout(this);
  stacker = new QStackedWidget();
  itemSelector = new QComboBox();
  selectorHbox  = new QHBoxLayout();

  // Book fields widget
  bookWidget = new QWidget(this);
  QVBoxLayout *bookLayout = new QVBoxLayout();
  title = new DLineEdit("Title", &validateGenericField);
  author = new DLineEdit("Author", &validateGenericField);
  genre = new DLineEdit("Genre", &validateGenericField);
  publicationDate = new DLineEdit("Publication Date", &validateNumericField);
  isbn = new DLineEdit("ISBN", &validateIsbn);
  bookFields = {title, author, genre, publicationDate, isbn};
  for (DLineEdit *field : bookFields) { bookLayout->addLayout(field); }
  bookWidget->setLayout(bookLayout);
  setupCompletion(author, "author");
  setupCompletion(genre, "genre");

  // Disc fields widget
  discWidget = new QWidget(this);
  QVBoxLayout *discLayout = new QVBoxLayout();
  discTitle = new DLineEdit("Title", &validateGenericField);
  directorOrSpeaker = new DLineEdit("Director/Speaker", &validateGenericField);
  length = new DLineEdit("Length", &validateGenericField);
  year = new DLineEdit("Year", &validateNumericField);
  type = new DLineEdit("Type (CD/DVD)", &validateGenericField);
  discFields = {discTitle, directorOrSpeaker, length, year, type};
  for (DLineEdit *field : discFields) { discLayout->addLayout(field); }
  discWidget->setLayout(discLayout);

  // Patron fields widget
  patronWidget = new QWidget(this);
  QVBoxLayout *patronLayout = new QVBoxLayout();
  name = new DLineEdit("Name", &validateGenericField);
  address = new DLineEdit("Address", &validateGenericField);
  mobileNum = new DLineEdit("Mobile No.", &validateNumericField);
  landlineNum = new DLineEdit("Landline No.", &validateNumericField);
  patronFields = {name, address, mobileNum, landlineNum};
  for (DLineEdit *field : patronFields) { patronLayout->addLayout(field); }
  patronWidget->setLayout(patronLayout);

  selectorDescription = new QLabel("Add:");

  stacker->addWidget(bookWidget);
  stacker->addWidget(discWidget);
  stacker->addWidget(patronWidget);
  itemSelector->addItem("Book");
  itemSelector->addItem("Disc");
  itemSelector->addItem("Patron");
  finishButton = new QPushButton("Finish");
  finishButton->setDefault(true);

  connect(finishButton, SIGNAL(clicked(bool)), this, SLOT(checkFields()));
  connect(itemSelector, SIGNAL(currentIndexChanged(int)), stacker, SLOT(setCurrentIndex(int)));

  selectorHbox->addWidget(selectorDescription);
  selectorHbox->addWidget(itemSelector);
  mainLayout->addLayout(selectorHbox);
  mainLayout->addWidget(stacker);
  mainLayout->addStretch();
  mainLayout->addWidget(finishButton);
  setLayout(mainLayout);
}

void AddItemDialog::addBook()
{
  // Generate hash of book data to be used as a key
  std::random_device keyGen;
  itemKey = QString::number(keyGen()) + "b";

  // Sort the authors and genres
  QStringList authorsList = author->text().split(",", QString::SkipEmptyParts);
  for (int i = 0; i < authorsList.size(); ++i) { authorsList[i] = authorsList[i].simplified(); }
  authorsList.sort(Qt::CaseInsensitive);
  QStringList genresList = genre->text().split(",", QString::SkipEmptyParts);
  for (int i = 0; i < genresList.size(); ++i) { genresList[i] = genresList[i].simplified(); }
  genresList.sort(Qt::CaseInsensitive);

  QSqlQuery insert(QSqlDatabase::database());
  insert.prepare("INSERT INTO books (key, isbn, title, author, publication_date, genre) "
		 "VALUES (:key, :isbn, :title, :author, :publication_date, :genre)");
  insert.bindValue(":key", itemKey);
  insert.bindValue(":isbn", isbn->text());
  insert.bindValue(":title", title->text());
  insert.bindValue(":author", authorsList.join(", "));
  insert.bindValue(":publication_date", publicationDate->text());
  insert.bindValue(":genre", genresList.join(", "));
  insert.exec();

  done(QDialog::Accepted);
}

void AddItemDialog::addDisc()
{
  std::random_device keyGen;
  itemKey = QString::number(keyGen()) + "d";

  QSqlQuery insert(QSqlDatabase::database());
  insert.prepare("INSERT INTO discs (key, title, directorOrSpeaker, length, year, type) "
		 "VALUES (:key, :title, :directorOrSpeaker, :length, :year, :type);");
  insert.bindValue(":key", itemKey);
  insert.bindValue(":title", discTitle->text());
  insert.bindValue(":directorOrSpeaker", directorOrSpeaker->text());
  insert.bindValue(":length", length->text());
  insert.bindValue(":year", year->text());
  insert.bindValue(":type", type->text());
  insert.exec();

  done(QDialog::Accepted);
}

void AddItemDialog::addPatron()
{
  std::random_device keyGen;
  itemKey = QString::number(keyGen()) + "p";

  QSqlQuery insert(QSqlDatabase::database());
  insert.prepare("INSERT INTO patrons (key, name, address, mobile_num, landline_num, items) "
		 "VALUES (:key, :name, :address, :mobile_num, :landline_num, :items);");
  insert.bindValue(":key", itemKey);
  insert.bindValue(":name", name->text());
  insert.bindValue(":address", address->text());
  insert.bindValue(":mobile_num", mobileNum->text());
  insert.bindValue(":landline_num", landlineNum->text());
  insert.bindValue(":items", "");
  insert.exec();

  done(QDialog::Accepted);
}

QString AddItemDialog::getItemKey()
{
  return itemKey;
}

void AddItemDialog::checkFields()
{
  QStringList warnings;
  std::vector<std::vector<DLineEdit*>> itemFields = {bookFields, discFields, patronFields};
  for (DLineEdit *field : itemFields[stacker->currentIndex()])
    {
      if (!field->valid) { warnings << field->placeholderText(); }
    }

  if (!warnings.isEmpty())
    {
      QString invalidFields;
      if (warnings.size() == 1)
	{
	  invalidFields = warnings[0];
	}
      else if (warnings.size() == 2)
	{
	  invalidFields = warnings.join(" and ");
	}
      else if (warnings.size() > 2)
	{
	  warnings[warnings.size() - 1].prepend("and ");
	  invalidFields = warnings.join(", ");
	}

      int confirm = QMessageBox::warning(this, "Warning",
					 invalidFields +
					 " invalid, would you like to continue anyway?",
					 QMessageBox::Yes, QMessageBox::No);
      if (QMessageBox::Yes == confirm)
	{
	  if (stacker->currentIndex() == 0) { addBook(); }
	  else if (stacker->currentIndex() == 1) { addDisc(); }
	  else if (stacker->currentIndex() == 2) { addPatron(); }
	}
    }
  else
    {
      if (stacker->currentIndex() == 0) { addBook(); }
      else if (stacker->currentIndex() == 1) { addDisc(); }
      else if (stacker->currentIndex() == 2) { addPatron(); }
    }
}

void AddItemDialog::setupCompletion(DLineEdit *field, QString sqlField)
{
  QStringListModel* completionModel = new QStringListModel(this);
  QStringList completionList;
  QSqlDatabase bookstore = QSqlDatabase::database();

  QSqlQuery getColumn(bookstore);
  getColumn.exec(QString("SELECT %1 FROM bookstore;").arg(sqlField));

  while (getColumn.next())
    {
      completionList << getColumn.value(0).toString().
	split(",", QString::SkipEmptyParts);
    }
  for (int i = 0; i < completionList.size(); ++i)
    {
      completionList[i] = completionList[i].trimmed();
    }

  completionList.removeDuplicates();
  completionModel->setStringList(completionList);
  field->enableCompletion(completionModel);
}
