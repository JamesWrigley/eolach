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

#ifndef DLINEEDIT_H
#define DLINEEDIT_H

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QStringListModel>
#include "DCompleter.h"

/* The layout that holds a QLineEdit and icon (in a QLabel) to use in the AddBookDialog.
   A function is passed in to check the QLineEdit's text and change the icon accordingly. */
class DLineEdit : public QHBoxLayout
{
  Q_OBJECT

  private slots:
    void onTextChanged(QString);

 public:
  bool valid;
  QString text();
  void enable_completion(QStringListModel*);
  DLineEdit(QString, bool (*)(QString), QWidget *parent = 0);

 private:
  QLabel *icon;
  QLineEdit *lineedit;
  DCompleter *completer;
  bool completion_enabled = false;

  bool (*check_function)(QString);
};

#endif // DLINEEDIT_H
