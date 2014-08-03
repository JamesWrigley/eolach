#include "InfoWidget.h"


InfoWidget::InfoWidget(Book book, QWidget *parent) : QFrame(parent)
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

  title = new QLabel(book.title, this);
  author = new QLabel(book.author, this);
  publication_date = new QLabel(QString::number(book.publication_date.year()), this);

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
  this->setStyleSheet("QFrame#MainQFrame {border: 8px solid #494949; border-radius: 7px;}");
}
