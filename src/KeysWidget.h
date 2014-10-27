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

#include <QStringList>
#include <QTableWidget>
#include "Book.h"

class KeysWidget : public QTableWidget
{
  Q_OBJECT

 public:
  KeysWidget(QWidget *parent = 0);

 private:
    void initialize();

    QStringList headers;

 public:
    void add_book(QString);
};

#endif // KEYSWIDGET_H
