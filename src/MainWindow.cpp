#include <QtGui>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"

MainWindow::MainWindow()
{
  initialize_ui();
  populate_keys();

  keys_widget->selectRow(0);
  info_widget->set_book(books_map[keys_widget->model()->data(keys_widget->model()->index(0,0)).toString()]);

  this->center_window();
  this->setWindowTitle("Eolach");
}

void MainWindow::populate_keys()
{
  // Initialize test books
  Book TMI = Book("The Mysterious Island", "Jules Verne", 1874);
  Book TCOMC = Book("The Count of Monte Cristo", "Alexandre Dumas", 1844);
  Book E = Book("Emma", "Jane Austen", 1815);
  Book TIM = Book("The Invisible Man", "H.G.Wells", 1897);
  Book WAP = Book("War and Peace", "Leo Tolstoy", 1869);
  books_map[TMI.title] = TMI;
  books_map[TCOMC.title] = TCOMC;
  books_map[E.title] = E;
  books_map[TIM.title] = TIM;
  books_map[WAP.title] = WAP;

  for (QHash<QString, Book>::iterator i = books_map.begin(); i != books_map.end(); ++i)
    {
      keys_widget->add_book(i.value());
    }
}

void MainWindow::initialize_ui()
{
  keys_tabwidget = new QTabWidget(this);
  splitter = new QSplitter(this);
  keys_widget = new KeysWidget();
  info_widget = new InfoWidget();

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
