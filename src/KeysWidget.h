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

/* The QTableWidget subclass that displays a list of all the books currently in
   the database. */
class KeysWidget : public QTableWidget
{
  Q_OBJECT

 signals:
  void bookRemoved();

  private slots:
    void create_item_context_menu(QPoint);
    void create_header_context_menu(QPoint);
    void modify_header(bool);
    void removeBook();

 public:
    KeysWidget(QWidget *parent = 0);
    void add_book(QString);
    void load_items();
    void update_book(int, QString);

 private:
    std::pair<int, Qt::SortOrder> disable_sorting();
    void enable_sorting(int, Qt::SortOrder);
    int visible_column_count;
    QMenu *item_context_menu;
    QMenu *header_context_menu;
    QStringList headers;
    QSqlDatabase bookstore;

    QAction *remove_book_action;
};

#endif // KEYSWIDGET_H
