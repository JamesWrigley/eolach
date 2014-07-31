#include <QtGui>
#include "KeysWidget.h"

KeysWidget::KeysWidget(QWidget *parent) : QWidget(parent)
{
  // Initialize test keys
  test_keys.append(book("The Mysterious Island", "Jules Verne", 1874));
  test_keys.append(book("The Count of Monte Cristo", "Alexandre Dumas", 1844));
  test_keys.append(book("Emma", "Jane Austen", 1815));
  test_keys.append(book("The Invisible Man", "H.G.Wells", 1897));
  test_keys.append(book("War and Peace", "Leo Tolstoy", 1869));

  keys_tablewidget = new QTableWidget(0, 2, this);

  headers << "Title" << "Author";
  keys_tablewidget->setHorizontalHeaderLabels(headers);
  keys_tablewidget->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
  keys_tablewidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
  keys_tablewidget->horizontalHeader()->resizeSection(0, keys_tablewidget->horizontalHeader()->width() * 5);

  for (int n = 0; n < test_keys.size(); ++n)
    {
      keys_tablewidget->insertRow(keys_tablewidget->rowCount());

      QTableWidgetItem *title = new QTableWidgetItem(test_keys[n].title);
      QTableWidgetItem *author = new QTableWidgetItem(test_keys[n].author);

      keys_tablewidget->setItem(keys_tablewidget->rowCount() - 1, 0, title);
      keys_tablewidget->setItem(keys_tablewidget->rowCount() - 1, 1, author);
    }

  main_vbox = new QVBoxLayout(this);
  main_vbox->addWidget(keys_tablewidget);
  this->setLayout(main_vbox);
}
