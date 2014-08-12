#include <QtGui>
#include "KeysWidget.h"


KeysWidget::KeysWidget(QWidget *parent) : QWidget(parent)
{
  initialize_ui();
}

void KeysWidget::selectTableRow(int r)
{
  table_widget->selectRow(r);
}

void KeysWidget::add_book(Book book)
{
  table_widget->insertRow(table_widget->rowCount());

  QTableWidgetItem *title = new QTableWidgetItem(book.title);
  QTableWidgetItem *author = new QTableWidgetItem(book.author);

  table_widget->setItem(table_widget->rowCount() - 1, 0, title);
  table_widget->setItem(table_widget->rowCount() - 1, 1, author);
}

void KeysWidget::initialize_ui()
{
  main_vbox = new QVBoxLayout(this);

  table_widget = new QTableWidget(0, 2, this);
  QObject::connect(table_widget, SIGNAL(cellPressed(int, int)),
                   this, SLOT(selectTableRow(int)));

  headers << "Title" << "Author";
  table_widget->setHorizontalHeaderLabels(headers);
  table_widget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  table_widget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

  main_vbox->addWidget(table_widget);

  this->setLayout(main_vbox);
}
