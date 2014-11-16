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

#include <QMenu>
#include <QHash>
#include <QAction>
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
    void change_book(int);
    void onFieldChanged(QString, QString);

 public:
    MainWindow();
    ~MainWindow();

 private:
    void add_book(QString, QString, QString, QString, QString);
    void center_window();
    void populate_keys();
    void update_statusbar();

    QSqlDatabase bookstore;
    InfoWidget *info_widget;
    KeysWidget *books_widget;
    QHash<int, QString> key_table;

    QMenu *file_menu;
    QSplitter *splitter;
    QTabWidget *keys_tabwidget;

    QAction *exit_action;
};

#endif // MAINWINDOW_H
