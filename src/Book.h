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

#ifndef BOOK_H
#define BOOK_H

// The datatype for a book
struct Book
{
  QString isbn;
  QString title;
  QString genre;
  QString author;
  QString synopsis;
  bool available;
  QString publication_date;

  Book() {};
  Book(QString i, QString t, QString g, QString a, QString s, bool avlble, QString pd)
  {
    isbn = i;
    title = t;
    genre = g;
    author = a;
    synopsis = s;
    available = avlble;
    publication_date = pd;
  };
};

#endif // BOOK_H
