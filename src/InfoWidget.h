#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Book.h"


class InfoWidget : public QWidget
{
  Q_OBJECT

 public:
  InfoWidget(QWidget *parent = 0);

 private:
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

  QHBoxLayout *labels_holder;
  QVBoxLayout *labels;
  QVBoxLayout *data;
  QVBoxLayout *main_vbox;
};

#endif // INFOWIDGET_H
