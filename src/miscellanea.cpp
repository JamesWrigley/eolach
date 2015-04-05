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

#include "miscellanea.h"

bool validate_generic_field(QString field_text)
{
  return field_text.length() > 0;
}

bool validate_numeric_field(QString field_text)
{
  bool success = false;
  field_text.toFloat(&success);
  return success;
}

bool validate_isbn(QString isbn_value)
{ // Implements the checksumming algorithm for 10 and 13 digit ISBN's
  int sum = 0;
  int check_digit = 10;

  if (10 == isbn_value.length())
    {
      for (int i = 0; i < isbn_value.length() - 1; ++i)
        {
          sum += isbn_value[i].digitValue() * (10 - i);
        }
      check_digit = (11 - (sum % 11)) % 11;
    }
  else if (13 == isbn_value.length())
    {
      for (int i = 1; i < isbn_value.length(); i += 2)
        {
          sum += isbn_value[i].digitValue() * 3;
          sum += isbn_value[i - 1].digitValue();
        }
      check_digit = (10 - (sum % 10)) % 10;
    }

  return isbn_value.endsWith(QString::number(check_digit));
}
