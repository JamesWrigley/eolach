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
    void changeItem();
    void onItemRemoved();
    void onTabChanged(int);
    void createAddItemDialog();
    void onFieldChanged(QString, QString, QString);

 public:
    MainWindow();
    ~MainWindow();

 private:
    void centerWindow();
    void updateStatusbar();
    void createInfoWidget();

    QMenu *fileMenu;
    QAction *addItemAction;
    QAction *exitAction;
    QToolBar *toolbar;
    QSplitter *splitter;
    KeysWidget *booksWidget;
    KeysWidget *discsWidget;
    KeysWidget *patronsWidget;
    InfoWidget *infoWidget;
    QTabWidget *keysTabwidget;
    QSqlDatabase bookstore;

    TextField *title;
    TextField *genre;
    TextField *author;
    TextField *publicationDate;

    TextField *isbn;
    TextField *name;
    TextField *items;
    TextField *address;
    TextField *mobileNum;
    TextField *landlineNum;

    TextField *year;
    TextField *type;
    TextField *length;
    TextField *discTitle;
    TextField *directorOrSpeaker;

    std::vector<TextField*> bookFields;
    std::vector<TextField*> discFields;
    std::vector<TextField*> patronFields;
};

#endif // MAINWINDOW_H
