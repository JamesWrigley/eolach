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

#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QFrame>
#include <QStackedWidget>

#include "TextField.h"
#include "PatronHistory.h"

/* The widget that holds all the item info. Basically a QFrame with a bunch of
   labels for each info field. */
class InfoWidget : public QFrame
{
  Q_OBJECT

  public slots:
      void changeLayout(int);
  
  public:
      InfoWidget(QWidget*);
      void clear();
      void setItem(QString);

  private:
      QStackedWidget* stacker;
      PatronHistory* history;

      std::vector<TextField*> bookFields;
      std::vector<TextField*> discFields;
      std::vector<TextField*> patronFields;
};

#endif // INFOWIDGET_H
