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

#include <QVariant>
#include <QSqlQuery>
#include "InfoWidget.h"

InfoWidget::InfoWidget(QWidget *parent) : QFrame(parent)
{
  labels_holder_hbox = new QHBoxLayout();
  labels_vbox = new QVBoxLayout();
  data_vbox = new QVBoxLayout();
  main_vbox = new QVBoxLayout();

  title_label = new QLabel("Title:", this);
  author_label = new QLabel("Author:", this);
  publication_date_label = new QLabel("Publication Date:", this);
  // synopsis_label = new QLabel("Synopsis:", this);
  // available_label = new QLabel("Available:", this);

  title = new QLabel(this);
  author = new QLabel(this);
  publication_date = new QLabel(this);

  // Packing
  labels_vbox->addWidget(title_label);
  labels_vbox->addWidget(author_label);
  labels_vbox->addWidget(publication_date_label);
  labels_vbox->addStretch();

  data_vbox->addWidget(title);
  data_vbox->addWidget(author);
  data_vbox->addWidget(publication_date);
  data_vbox->addStretch();

  labels_holder_hbox->addLayout(labels_vbox);
  labels_holder_hbox->addLayout(data_vbox);

  main_vbox->addLayout(labels_holder_hbox);

  this->setLayout(main_vbox);
  this->setFrameShape(QFrame::StyledPanel);
  this->setObjectName("MainQFrame");
  this->setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::set_book(QString book_key)
{
  QSqlQuery get_book_info;
  get_book_info.exec("SELECT title, author, publication_date FROM bookstore WHERE key = '" + book_key + "'");
  get_book_info.next();

  title->setText(get_book_info.value(0).toString());
  author->setText(get_book_info.value(1).toString());
  publication_date->setText(get_book_info.value(2).toString());
}
