#include <QtGui>
#include "KeysWidget.h"


KeysWidget::KeysWidget(QWidget *parent) : QWidget(parent)
{
  initialize_ui();

  for (int n = 0; n < test_keys.size(); ++n)
    {
      add_book(test_keys[n]);
    }

  main_vbox->addWidget(keys_tablewidget);

  this->setLayout(main_vbox);
}

void KeysWidget::selectTableRow(int r)
{
  keys_tablewidget->selectRow(r);
}

void KeysWidget::add_book(Book book)
{
  keys_tablewidget->insertRow(keys_tablewidget->rowCount());

  QTableWidgetItem *title = new QTableWidgetItem(book.title);
  QTableWidgetItem *author = new QTableWidgetItem(book.author);

  keys_tablewidget->setItem(keys_tablewidget->rowCount() - 1, 0, title);
  keys_tablewidget->setItem(keys_tablewidget->rowCount() - 1, 1, author);
}

void KeysWidget::initialize_ui()
{
  main_vbox = new QVBoxLayout(this);

  keys_tablewidget = new QTableWidget(0, 2, this);
  QObject::connect(keys_tablewidget, SIGNAL(cellPressed(int, int)),
                   this, SLOT(selectTableRow(int)));

  headers << "Title" << "Author";
  keys_tablewidget->setHorizontalHeaderLabels(headers);
  keys_tablewidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  keys_tablewidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

}
