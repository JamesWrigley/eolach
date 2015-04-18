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

#include <QMenu>
#include <QAction>
#include <QVariant>
#include <QSqlQuery>
#include <QHeaderView>
#include "KeysWidget.h"
#include "miscellanea.h"

KeysWidget::KeysWidget(QString table, QStringList headerList, QWidget *parent)
{
  dbTableModel = new QSqlTableModel(0, QSqlDatabase::database());
  dbTableModel->setTable(table);
  dbTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
  dbTableModel->select();

  setModel(dbTableModel);
  show();
}

void KeysWidget::addItem(QString itemKey)
{
}

int KeysWidget::currentRow()
{
  return currentIndex().row();
}

QString KeysWidget::currentItemKey()
{
  return dbTableModel->data(dbTableModel->index(currentRow(), 0)).toString();
}

void KeysWidget::createItemContextMenu(QPoint pos)
{
  // if (itemAt(pos) != 0)
  //   {
  //     itemContextMenu->popup(viewport()->mapToGlobal(pos));
  //   }
}

void KeysWidget::createHeaderContextMenu(QPoint pos)
{
  // headerContextMenu->popup(mapToGlobal(pos));
}

int KeysWidget::rowCount()
{
  return dbTableModel->rowCount();
}

void KeysWidget::updateItem(int itemIndex, QString itemKey)
{
}
