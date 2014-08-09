#include <QtGui>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"

MainWindow::MainWindow()
{
  initialize_ui();

  this->center_window();
  this->setWindowTitle("Eolach");
}

void MainWindow::populate_keys()
{
  // Initialize test keys
  // books_map.insert("The Mysterious Island", "Jules Verne", 1874);
  // books_map.insert("The Count of Monte Cristo", "Alexandre Dumas", 1844);
  // books_map.insert("Emma", "Jane Austen", 1815);
  // books_map.insert("The Invisible Man", "H.G.Wells", 1897);
  // books_map.insert("War and Peace", "Leo Tolstoy", 1869);
}
void MainWindow::initialize_ui()
{
  keys_tabwidget = new QTabWidget(this);
  splitter = new QSplitter(this);
  keys_widget = new KeysWidget();
  info_widget = new InfoWidget(Book("War and Peace", "Leo Tolstoy", 1869));

  keys_tabwidget->addTab(keys_widget, "Books");

  splitter->addWidget(keys_tabwidget);
  splitter->addWidget(info_widget);

  file_menu = menuBar()->addMenu("File");

  this->statusBar()->showMessage("Ready");
  this->setCentralWidget(splitter);
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
