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
#include <QString>
#include <QLineEdit>
#include <QHBoxLayout>
#include "CLineEdit.h"

// A class that will represent each field displayed in the info widget
class TextField : public QHBoxLayout
{
  Q_OBJECT

 public:
  // First argument: the SQL field name, second argument: the text to display in edit_box
  TextField(QString, QString, QWidget *parent = 0);
  void set_text(QString);

 signals:
  // First argument: the SQL field name, second argument: the changed text (from edit_box)
  void textChanged(QString, QString);

 private:
  QLabel *label;
  QString field_name;
  QAction *edit_text;
  CLineEdit *edit_box;

  private slots:
    void onTextChanged(QString);
};

#endif // TEXTFIELD_H
