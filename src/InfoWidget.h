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

#include <vector>

#include <QFrame>
#include <QSqlRecord>
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
      InfoWidget();
      void clear();

      static QSqlRecord currentRecord;

  private:
      QStackedWidget* stacker;
      PatronHistory* history;

      // Book fields
      TextField* bookIsbn;
      TextField* bookGenre;
      TextField* bookAuthor;
      TextField* bookTitle;
      TextField* bookYear;
      std::vector<TextField*> bookFields;

      // Disc fields
      TextField* discType;
      TextField* discYear;
      TextField* discLength;
      TextField* discTitle;
      TextField* discDirectorOrSpeaker;
      std::vector<TextField*> discFields;

      // Patron fields
      TextField* patronName;
      TextField* patronAddress;
      TextField* patronMobile;
      TextField* patronLandline;
      std::vector<TextField*> patronFields;
};

#endif // INFOWIDGET_H
