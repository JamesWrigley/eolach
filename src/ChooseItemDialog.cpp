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
#include <QHBoxLayout>
#include <QStringList>
#include <QVBoxLayout>
#include "DCompleter.h"
#include "ChooseItemDialog.h"

ChooseItemDialog::ChooseItemDialog(QString patron_key)
{
  loadItems();
  patronKey = patron_key;
  list = new QListWidget();
  textBox = new QLineEdit();
  completerItems = QStringList(itemMap.keys());
  removeButton = new QPushButton(QIcon(":/remove-icon"), "");
  QHBoxLayout* topHbox = new QHBoxLayout();
  QVBoxLayout* mainVbox = new QVBoxLayout(this);
  DCompleter* completer = new DCompleter(completerItems);
  QPushButton* finishButton = new QPushButton("Finish");

  completer->setCaseSensitivity(Qt::CaseInsensitive);
  textBox->setCompleter(completer);
  textBox->setPlaceholderText("Item Name");
  removeButton->setDisabled(true);
  finishButton->setDefault(true);

  // The Qt::QueuedConnection bit is to ensure that the textBox is cleared
  connect(textBox->completer(), SIGNAL(activated(QString)), this,
          SLOT(addToList(QString)), Qt::QueuedConnection);
  connect(list, &QListWidget::itemSelectionChanged, [=] () { removeButton->setDisabled(false); });
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeFromList()));
  connect(finishButton, SIGNAL(clicked()), this, SLOT(applyItems()));

  topHbox->addWidget(textBox);
  topHbox->addWidget(removeButton);

  mainVbox->addLayout(topHbox);
  mainVbox->addWidget(list);
  mainVbox->addWidget(finishButton);

  setLayout(mainVbox);
  setWindowTitle("Modify History");
}

/* Add the text in textBox as an item in the list, and remove from the list of
   completions */
void ChooseItemDialog::addToList(QString item)
{
  if (completerItems.contains(item))
    {
      list->addItem(item);
      completerItems.removeAll(item);
      resetCompleter();
      textBox->clear();
    }
}

void ChooseItemDialog::applyItems()
{
  for (int i = 0; i < list->count(); ++i)
    {
      QString key = itemMap.value(list->item(i)->text());
      QString table = key.endsWith("b") ? "books" : "discs";

      // Update the items 'onLoan' status in its table
      QSqlQuery updateStatus(QSqlDatabase::database());
      updateStatus.prepare(QString("UPDATE %1 SET onLoan = 1 WHERE key = :key").arg(table));
      updateStatus.bindValue(":key", key);
      updateStatus.exec();

      // Create an entry for it in the borrowed table
      QSqlQuery createRecord(QSqlDatabase::database());
      createRecord.prepare("INSERT INTO borrowed (Pkey, Ikey) "
                           "VALUES (:pkey, :ikey);");
      createRecord.bindValue(":pkey", patronKey);
      createRecord.bindValue(":ikey", key);
      createRecord.exec();
    }

  done(QDialog::Accepted);
}

void ChooseItemDialog::loadItems()
{
  QSqlQuery getBooks("SELECT key, title FROM books WHERE onLoan = 0;", QSqlDatabase::database());
  QSqlQuery getDiscs("SELECT key, title FROM discs WHERE onLoan = 0;", QSqlDatabase::database());

  while (getBooks.next())
    {
      itemMap.insert(getBooks.value(1).toString(), getBooks.value(0).toString());
    }
  while (getDiscs.next())
    {
      itemMap.insert(getDiscs.value(1).toString(), getDiscs.value(0).toString());
    }

  completerItems = QStringList(itemMap.keys());
}

void ChooseItemDialog::removeFromList()
{
  removeButton->setDisabled(false);
  completerItems << list->currentItem()->text();
  resetCompleter();
  // We need to delete the item manually, since items removed from a QListWidget
  // will not be managed by Qt.
  delete list->takeItem(list->currentRow());
}

void ChooseItemDialog::resetCompleter()
{
  // We need a cast to DCompleter to get access to its overload of setModel(QStringList)
  DCompleter* completer = static_cast<DCompleter*>(textBox->completer());
  completer->setModel(completerItems);
}
