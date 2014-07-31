#ifndef BOOK_H
#define BOOK_H

#include <QDate>
#include <QString>

// The datatype for a book
struct book
{
  QString title;
  QString synopsis;
  QString author;
  bool available;
  QDate publication_date;

  book() {};
  book(QString t, QString a, unsigned int pd)
  {
    title = t;
    publication_date = QDate(pd, 1, 1);
    author = a;
  }
};

#endif // BOOK_H
