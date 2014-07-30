#ifndef KEYSWIDGET_H
#define KEYSWIDGET_H

#include <QDate>
#include <QWidget>
#include <QVector>
#include <QStringList>
#include <QVBoxLayout>
#include <QTableWidget>


struct book
{
  QString title;
  QString blurb;
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

class KeysWidget : public QWidget
{
  Q_OBJECT

 public:
  KeysWidget(QWidget *parent = 0);

 private:
  QTableWidget *keys_tablewidget;

  QVector<book> test_keys;
  QStringList headers;

  QVBoxLayout *main_vbox;
};

#endif // KEYSWIDGET_H
