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

#ifndef KEYSWIDGET_H
#define KEYSWIDGET_H

#include <utility>
#include <QSqlDatabase>
#include <QTableWidget>

/* The QTableWidget subclass that displays a list of all the items in a
   table. */
class KeysWidget : public QTableWidget
{
  Q_OBJECT

 signals:
  void itemRemoved();

  private slots:
    void removeItem();
    void modifyHeader(bool);
    void createItemContextMenu(QPoint);
    void createHeaderContextMenu(QPoint);

 public:
    KeysWidget(QString, QString, QStringList, QWidget *parent = 0);
    void loadItems();
    void addItem(QString);
    void updateItem(int, QString);

 private:
    void enableSorting(int, Qt::SortOrder);
    QStringList getItemInfo(QString);
    
    int visibleColumnCount;
    QMenu *itemContextMenu;
    QMenu *headerContextMenu;
    QAction *removeItemAction;
    QString dbTable;
    QString getItemInfoQuery;
    QStringList headers;
    QSqlDatabase database;
    std::pair<int, Qt::SortOrder> disableSorting();
};

#endif // KEYSWIDGET_H
