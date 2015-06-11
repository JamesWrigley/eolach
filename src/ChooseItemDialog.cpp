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

#include <QVBoxLayout>
#include "DCompleter.h"
#include "ChooseItemDialog.h"

ChooseItemDialog::ChooseItemDialog(QStringList completer_items)
{
  list = new QListWidget();
  textBox = new QLineEdit();
  completerItems = completer_items;
  QVBoxLayout* mainVbox = new QVBoxLayout(this);
  DCompleter* completer = new DCompleter(completerItems);

  completer->setCaseSensitivity(Qt::CaseInsensitive);
  textBox->setCompleter(completer);
  textBox->setPlaceholderText("Item Name");
  // The Qt::QueuedConnection bit is to ensure that the textBox is cleared
  connect(textBox->completer(), SIGNAL(activated(QString)), this, SLOT(addToList(QString)), Qt::QueuedConnection);

  mainVbox->addWidget(textBox);
  mainVbox->addWidget(list);

  setLayout(mainVbox);
}

/* Add the text in textBox as an item in the list, and remove from the list of
   completions */
void ChooseItemDialog::addToList(QString item)
{
  if (completerItems.contains(item))
    {
      list->addItem(item);
      completerItems.removeOne(item);

      // We need to cast to get access to the overload of setModel(QString)
      DCompleter* completer = static_cast<DCompleter*>(textBox->completer());
      completer->setModel(completerItems);
      textBox->clear();
    }
}
