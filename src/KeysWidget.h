#ifndef KEYSWIDGET_H
#define KEYSWIDGET_H

#include <QDate>
#include <QWidget>
#include <QVector>
#include <QStringList>
#include <QVBoxLayout>
#include <QTableWidget>
#include "Book.h"


class KeysWidget : public QWidget
{
  Q_OBJECT

 public:
  KeysWidget(QWidget *parent = 0);

  private slots:
    void selectTableRow(int);

 private:
    void initialize_ui();

    QTableWidget *keys_tablewidget;
    QVector<Book> test_keys;
    QStringList headers;
    QVBoxLayout *main_vbox;

 public:
    void add_book(Book);
};

#endif // KEYSWIDGET_H
