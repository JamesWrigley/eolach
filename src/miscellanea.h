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

#ifndef MISCELLANEA_H
#define MISCELLANEA_H

#include <QString>

/* Here we define some things that don't quite fit anywhere else */

// SQL queries for getting all the columns for a row from a table.
const QString getBookInfo = "SELECT title, author, genre, publication_date, isbn FROM bookstore WHERE key=:key;";
const QString getDiscInfo = "SELECT title, directorOrSpeaker, length, year, type FROM discs WHERE key=:key;";
const QString getPatronInfo = "SELECT name, address, mobile_num, landline_num, items FROM patrons WHERE key=:key;";

// Validation functions
bool validateIsbn(QString);
bool validateGenericField(QString);
bool validateNumericField(QString);

#endif // MISCELLANEA_H
