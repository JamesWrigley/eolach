#include <QtGui>
#include "KeysWidget.h"


KeysWidget::KeysWidget(QWidget *parent) : QTableWidget(parent)
{
  initialize_ui();
}

void KeysWidget::selectTableRow(int r)
{
  selectRow(r);
}

void KeysWidget::add_book(Book book)
{
  insertRow(rowCount());

  QTableWidgetItem *title = new QTableWidgetItem(book.title);
  QTableWidgetItem *author = new QTableWidgetItem(book.author);

  setItem(rowCount() - 1, 0, title);
  setItem(rowCount() - 1, 1, author);
}

void KeysWidget::initialize_ui()
{
  insertColumn(0);
  insertColumn(1);

  QObject::connect(this, SIGNAL(cellPressed(int, int)),
                   this, SLOT(selectTableRow(int)));

  headers << "Title" << "Author";
  setHorizontalHeaderLabels(headers);
  horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
}
