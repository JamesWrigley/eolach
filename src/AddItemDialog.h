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
    void checkFields();

 public:
    AddItemDialog(QWidget *parent);
    QString getItemKey();
    void setupCompletion(DLineEdit*, QString);

 private:
    void addBook();
    void addDisc();
    void addPatron();
    QLabel *selectorDescription;
    QString itemKey;
    QComboBox *itemSelector;
    QVBoxLayout *mainLayout;
    QHBoxLayout *selectorHbox;
    QPushButton *finishButton;
    QStackedWidget *stacker;

    QWidget *bookWidget;
    DLineEdit *isbn;
    DLineEdit *title;
    DLineEdit *genre;
    DLineEdit *author;
    DLineEdit *publicationDate;

    QWidget *discWidget;
    DLineEdit *year;
    DLineEdit *type;
    DLineEdit *length;
    DLineEdit *discTitle;
    DLineEdit *directorOrSpeaker;

    QWidget *patronWidget;
    DLineEdit *name;
    DLineEdit *address;
    DLineEdit *mobileNum;
    DLineEdit *landlineNum;

    std::vector<DLineEdit*> bookFields;
    std::vector<DLineEdit*> discFields;
    std::vector<DLineEdit*> patronFields;
};

#endif // ADDITEMDIALOG_H
