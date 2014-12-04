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

#ifndef ADDBOOKDIALOG_H
#define ADDBOOKDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include "DLineEdit.h"

class AddBookDialog : public QDialog
{
  Q_OBJECT

  private slots:
    void check_fields();

 public:
    AddBookDialog(QWidget *parent);
    QString book_key;
    void setup_completions();
    static bool validate_isbn(QString);
    static bool validate_generic_field(QString);
    static bool validate_numeric_field(QString);

 private:
    void add_book();

    DLineEdit *title;
    DLineEdit *author;
    DLineEdit *genre;
    DLineEdit *isbn;
    DLineEdit *publication_date;
    QPushButton *finish_button;
    QVBoxLayout *main_layout;
};

#endif // ADDBOOKDIALOG_H
