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
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class AddBookDialog : public QDialog
{
  Q_OBJECT

  private slots:
    void check_fields();

 public:
    AddBookDialog(QWidget *parent);
    QString book_key;
    static bool validate_isbn(QString);

 private:
    void add_book();

    QLineEdit *title;
    QLineEdit *author;
    QLineEdit *genre;
    QLineEdit *isbn;
    QLineEdit *publication_date;
    QPushButton *finish_button;
    QVBoxLayout *main_layout;
};

#endif // ADDBOOKDIALOG_H
