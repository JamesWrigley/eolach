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

#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include <QLabel>
#include <QAction>
#include <QLineEdit>
#include <QSqlRecord>
#include <QHBoxLayout>

#include "CLineEdit.h"

/* A class that will represent each field displayed in the info widget */
class TextField : public QHBoxLayout
{
  Q_OBJECT

  private slots:
      void onDoubleClicked();
      void onTextChanged(QString);
      void onTextModified(QString);
      void onItemSelected(QSqlRecord);

  signals:
      // Arguments: the SQL field name and the changed text (from editBox)
      void fieldChanged(QString, QString, QString);

  public:
      // Constructor arguments: the database table identifier, the field name
      // the text to display in editBox, and a validator function.
      TextField(QString, QString, QString, bool (*)(QString));
      void enterEditMode();
      void setText(QString);

  private:
      bool (*checkFunction)(QString);

      QLabel *icon;
      QLabel *label;
      QString fieldName;
      QAction *editText;
      CLineEdit *editBox;
      QString tableIdentifier;
};

#endif // TEXTFIELD_H
