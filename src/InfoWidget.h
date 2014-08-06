#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Book.h"


class InfoWidget : public QFrame
{
  Q_OBJECT

 public:
  InfoWidget(Book, QWidget *parent = 0);

 private:
  void initialize_ui();
  void set_book(Book);

  QLabel *title_label;
  QLabel *author_label;
  QLabel *publication_date_label;
  QLabel *synopsis__label;
  QLabel *available_label;

  QLabel *title;
  QLabel *author;
  QLabel *publication_date;
  QLabel *synopsis;
  QLabel *available;

  QHBoxLayout *labels_holder_hbox;
  QVBoxLayout *labels_vbox;
  QVBoxLayout *data_vbox;
  QVBoxLayout *main_vbox;
};

#endif // INFOWIDGET_H
