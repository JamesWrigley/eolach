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

bool validateGenericField(QString fieldText)
{
    return fieldText.length() > 0;
}

bool validateNumericField(QString fieldText)
{
    bool success = false;
    fieldText.toFloat(&success);
    return success;
}

bool validateIsbn(QString isbnValue)
{ // Implements the checksumming algorithm for 10 and 13 digit ISBN's
    int sum = 0;
    int checkDigit = 10;

    if (10 == isbnValue.length()) {
        for (int i = 0; i < isbnValue.length() - 1; ++i) {
            sum += isbnValue[i].digitValue() * (10 - i);
	}
        checkDigit = (11 - (sum % 11)) % 11;
    } else if (13 == isbnValue.length()) {
        for (int i = 1; i < isbnValue.length(); i += 2) {
            sum += isbnValue[i].digitValue() * 3;
            sum += isbnValue[i - 1].digitValue();
	}
        checkDigit = (10 - (sum % 10)) % 10;
    }

    return isbnValue.endsWith(QString::number(checkDigit));
}
