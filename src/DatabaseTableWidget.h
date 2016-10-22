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

#ifndef DATABASETABLEWIDGET_H
#define DATABASETABLEWIDGET_H

#include <unordered_map>

#include <QMenu>
#include <QAction>
#include <QTableView>
#include <QVBoxLayout>
#include <QSqlTableModel>

class DatabaseTableWidget : public QWidget
{
  Q_OBJECT

  public:
      DatabaseTableWidget(QString, std::unordered_map<int, QString>);
      int getCurrentRow();
      unsigned int rowCount();
      QSqlTableModel* getModel();
      void selectRow(int);

  private slots:
      void removeItem();
      void storeSelection(int);
      void restoreSelection(int);
      void createItemContextMenu(QPoint);
      void createHeaderContextMenu(QPoint);

  private:
      void setSort(int, Qt::SortOrder);

      QString selected;

      QTableView* view;
      QVBoxLayout* layout;
      QSqlTableModel* model;
      QMenu *itemContextMenu;
      QMenu *headerContextMenu;
      QAction *removeItemAction;
};

#endif
