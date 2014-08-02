#include <QtGui>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
  initialize_ui();

  this->setLayout(main_vbox);
  this->center_window();
  this->setWindowTitle("Eolach");
  this->resize(800, 600);
}

void MainWindow::initialize_ui()
{
  keys_tabwidget = new QTabWidget(this);
  splitter = new QSplitter(this);

  keys_tabwidget->addTab(new KeysWidget(), "Books");

  splitter->addWidget(keys_tabwidget);
  splitter->addWidget(new InfoWidget(Book("War and Peace", "Leo Tolstoy", 1869)));

  main_vbox = new QVBoxLayout(this);
  main_vbox->addWidget(splitter);

  this->setLayout(main_vbox);
  this->setWindowState(Qt::WindowMaximized);
}

void MainWindow::center_window()
{
  QRect screen_geo = QApplication::desktop()->screenGeometry();
  QWidget *main_window = QWidget::window();

  int x = (screen_geo.width() - 800) / 2;
  int y = (screen_geo.height() - 600) / 2;

  main_window->move(x,y);
}
