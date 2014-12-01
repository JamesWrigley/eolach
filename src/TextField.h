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
#include <QHBoxLayout>
#include "CLineEdit.h"

/* A class that will represent each field displayed in the info widget */
class TextField : public QHBoxLayout
{
  Q_OBJECT

  private slots:
    void onTextChanged(QString);

 signals:
    // Arguments: the SQL field name and the changed text (from edit_box)
    void fieldChanged(QString, QString);

 public:
    // Constructor arguments: the SQL field name and the text to display in edit_box
    TextField(QString, QString, QWidget *parent = 0);
    void set_text(QString);
    void enterEditMode();

 private:
    QLabel *label;
    QString field_name;
    QAction *edit_text;
    CLineEdit *edit_box;
};

#endif // TEXTFIELD_H
