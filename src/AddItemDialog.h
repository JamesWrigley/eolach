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

#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QLabel>
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "DLineEdit.h"

class AddItemDialog : public QDialog
{
  Q_OBJECT

  private slots:
    void check_fields();

 public:
    AddItemDialog(QWidget *parent);
    void setup_completions();
    QString getItemKey();

 private:
    void add_book();
    void add_patron();
    QString item_key;
    QLabel *selector_description;
    QStackedWidget *stacker;
    QVBoxLayout *main_layout;
    QHBoxLayout *selector_hbox;
    QPushButton *finish_button;
    QComboBox *item_selector;
    
    DLineEdit *title;
    DLineEdit *author;
    DLineEdit *genre;
    DLineEdit *isbn;
    DLineEdit *publication_date;
    QWidget *book_widget;
    std::vector<DLineEdit*> book_fields;

    DLineEdit *name;
    DLineEdit *address;
    DLineEdit *mobile_num;
    DLineEdit *landline_num;
    QWidget *patron_widget;
    std::vector<DLineEdit*> patron_fields;
};

#endif // ADDITEMDIALOG_H
