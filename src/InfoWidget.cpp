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

#include <QVariant>
#include <QSqlQuery>
#include <QVBoxLayout>
#include "InfoWidget.h"
#include "miscellanea.h"

InfoWidget::InfoWidget(QWidget* mainWindow)
{
  stacker = new QStackedWidget();
  QVBoxLayout* mainVbox = new QVBoxLayout(this);

  // Book fields widget
  QVBoxLayout* bookLayout = new QVBoxLayout();
  QWidget* bookWidget = new QWidget();
  TextField* isbn = new TextField("books", "isbn", "ISBN:", &validateIsbn);
  TextField* title = new TextField("books", "title", "Title:", &validateGenericField);
  TextField* genre = new TextField("books", "genre", "Genre:", &validateGenericField);
  TextField* author = new TextField("books", "author", "Author:", &validateGenericField);
  TextField* publicationDate = new TextField("books", "publication_date", "Publication Date:", &validateNumericField);
  for (TextField* field : {title, genre, author, publicationDate, isbn}) // Careful here, order is important
    {
      bookFields.push_back(field);
      bookLayout->addLayout(field);
      connect(field, SIGNAL(fieldChanged(QString, QString, QString)),
              mainWindow, SLOT(onFieldChanged(QString, QString, QString)));
    }
  bookLayout->addStretch();
  bookWidget->setLayout(bookLayout);

  // Disc fields widget
  QVBoxLayout* discLayout = new QVBoxLayout();
  QWidget* discWidget = new QWidget();
  TextField* discTitle = new TextField("discs", "title", "Title:", &validateGenericField);
  TextField* directorOrSpeaker = new TextField("discs", "directorOrSpeaker", "Director/Speaker:", &validateGenericField);
  TextField* length = new TextField("discs", "length", "Length:", &validateGenericField);
  TextField* year = new TextField("discs", "year", "Year:", &validateNumericField);
  TextField* type = new TextField("discs", "type", "Type:", &validateGenericField);
  for (TextField* field : {discTitle, directorOrSpeaker, length, year, type})
    {
      discFields.push_back(field);
      discLayout->addLayout(field);
      connect(field, SIGNAL(fieldChanged(QString, QString, QString)),
              mainWindow, SLOT(onFieldChanged(QString, QString, QString)));
    }
  discLayout->addStretch();
  discWidget->setLayout(discLayout);

  // Patron fields widget
  QVBoxLayout* patronLayout = new QVBoxLayout();
  QWidget* patronWidget = new QWidget();
  TextField* name = new TextField("patrons", "name", "Name:", &validateGenericField);
  TextField* address = new TextField("patrons", "address", "Address:", &validateGenericField);
  TextField* mobileNum = new TextField("patrons", "mobile_num", "Mobile No.", &validateNumericField);
  TextField* landlineNum = new TextField("patrons", "landline_num", "Landline No.", &validateNumericField);

  for (TextField* field : {name, address, mobileNum, landlineNum})
    {
      patronFields.push_back(field);
      patronLayout->addLayout(field);
      connect(field, SIGNAL(fieldChanged(QString, QString, QString)),
              mainWindow, SLOT(onFieldChanged(QString, QString, QString)));
    }

  history = new PatronHistory();
  QFrame* separator = new QFrame();
  separator->setFrameShape(QFrame::HLine);
  separator->setFrameShadow(QFrame::Sunken);
  patronLayout->addSpacing(30);
  patronLayout->addWidget(separator);
  patronLayout->addLayout(history);
  patronWidget->setLayout(patronLayout);

  stacker->addWidget(bookWidget);
  stacker->addWidget(discWidget);
  stacker->addWidget(patronWidget);

  mainVbox->addWidget(stacker);
  mainVbox->addStretch();
  setLayout(mainVbox);
  setFrameShape(QFrame::StyledPanel);
  setObjectName("MainQFrame");
  setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::changeLayout(int index)
{
  stacker->setCurrentIndex(index);
}

void InfoWidget::clear()
{
  for (std::vector<TextField*> itemFields : {bookFields, discFields, patronFields})
    {
      for (TextField *field : itemFields)
        {
          field->setText("");
        }
    }
}

void InfoWidget::setItem(QString itemKey)
{
  QSqlQuery getItemInfo(QSqlDatabase::database());
  std::vector<TextField*> itemFields;

  if (itemKey.endsWith("b"))
    {
      getItemInfo.prepare(getBookInfo);
      itemFields = bookFields;
    }
  else if (itemKey.endsWith("d"))
    {
      getItemInfo.prepare(getDiscInfo);
      itemFields = discFields;
    }
  else if (itemKey.endsWith("p"))
    {
      getItemInfo.prepare(getPatronInfo);
      history->setPatron(itemKey);
      itemFields = patronFields;
    }

  getItemInfo.bindValue(":key", itemKey);
  getItemInfo.exec();
  getItemInfo.next();

  for (unsigned int i = 0; i < itemFields.size(); ++i)
    {
      itemFields[i]->setText(getItemInfo.value(i).toString());
    }
}
