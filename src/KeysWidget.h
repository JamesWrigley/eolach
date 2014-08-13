#ifndef KEYSWIDGET_H
#define KEYSWIDGET_H

#include <QStringList>
#include <QTableWidget>
#include "Book.h"


class KeysWidget : public QTableWidget
{
  Q_OBJECT

 public:
  KeysWidget(QWidget *parent = 0);

 private:
    void initialize_ui();

    QStringList headers;

 public:
    void add_book(Book);
};

#endif // KEYSWIDGET_H
