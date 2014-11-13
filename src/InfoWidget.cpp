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

#include <iostream>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include "InfoWidget.h"

InfoWidget::InfoWidget()
{
  main_vbox = new QVBoxLayout(this);

  title = new TextField("title", "Title:");
  author = new TextField("author", "Author:");
  publication_date = new TextField("publication_date", "Publication Date:");
  QObject::connect(title, SIGNAL(textChanged(QString, QString)),
                   this, SIGNAL(fieldChanged(QString, QString)));
  QObject::connect(author, SIGNAL(textChanged(QString, QString)),
                   this, SIGNAL(fieldChanged(QString, QString)));
  QObject::connect(publication_date, SIGNAL(textChanged(QString, QString)),
                   this, SIGNAL(fieldChanged(QString, QString)));

  // Packing
  main_vbox->addLayout(title);
  main_vbox->addLayout(author);
  main_vbox->addLayout(publication_date);
  main_vbox->addStretch();

  this->setLayout(main_vbox);
  this->setFrameShape(QFrame::StyledPanel);
  this->setObjectName("MainQFrame");
  this->setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::set_book(QString book_key)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery get_book_info(db);
  get_book_info.exec("SELECT title, author, publication_date FROM bookstore WHERE key='" + book_key + "'");
  get_book_info.next();

  title->set_text(get_book_info.value(0).toString());
  author->set_text(get_book_info.value(1).toString());
  publication_date->set_text(get_book_info.value(2).toString());
}
