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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QToolBar>
#include <QSplitter>
#include <QTabWidget>
#include <QMainWindow>
#include <QSqlDatabase>
#include "KeysWidget.h"
#include "InfoWidget.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

  private slots:
    void create_add_item_dialog();
    void change_item();
    void onFieldChanged(QString, QString, QString);
    void onItemRemoved();
    void onTabChanged(int);

 public:
    MainWindow();
    ~MainWindow();

 private:
    void center_window();
    void create_info_widget();
    void update_statusbar();

    QSqlDatabase bookstore;
    KeysWidget *books_widget;
    KeysWidget *patrons_widget;
    QMenu *file_menu;
    QSplitter *splitter;
    QTabWidget *keys_tabwidget;
    QToolBar *toolbar;
    QAction *add_item_action;
    QAction *exit_action;

    InfoWidget *info_widget;
    TextField *title;
    TextField *author;
    TextField *genre;
    TextField *publication_date;
    TextField *isbn;
    TextField *name;
    TextField *address;
    TextField *mobile_num;
    TextField *landline_num;
    TextField *items;

    std::vector<TextField*> book_fields;
    std::vector<TextField*> patron_fields;
};

#endif // MAINWINDOW_H
