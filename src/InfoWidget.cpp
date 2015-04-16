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
#include "InfoWidget.h"
#include "miscellanea.h"

InfoWidget::InfoWidget()
{
  mainVbox = new QVBoxLayout(this);
  mainVbox->addStretch();

  setLayout(mainVbox);
  setFrameShape(QFrame::StyledPanel);
  setObjectName("MainQFrame");
  setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::addField(TextField *field, QString type)
{
  if (type == "b") { bookFields.push_back(field); }
  else if (type == "d") { discFields.push_back(field); }
  else if (type == "p") { patronFields.push_back(field); }

  mainVbox->insertLayout(mainVbox->count() - 1, field);
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

void InfoWidget::removeField(TextField *field)
{
  mainVbox->removeItem(field);
}

void InfoWidget::setItem(QString itemKey)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery getItemInfo(db);
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
