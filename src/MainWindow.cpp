/*************************** Copyrights and License ******************************
 *                                                                                *
 * This file is part of Eolach. http://github.com/JamesWrigley/eolach/            *
 *                                                                                *
 * Eolach is free software: you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software  *
 * Foundation, either version 3 of the License, or (at your option) any later     *
 * version.                                                                       *
 *                                                                                *
 * Eolach is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS      *
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. *
 *                                                                                *
 * You should have received a copy of the GNU General Public License along with   *
 * Eolach. If not, see <http://www.gnu.org/licenses/>.                            *
 *                                                                                *
 *********************************************************************************/

#include <QMenuBar>
#include <QSqlQuery>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"
#include "miscellanea.h"
#include "AddBookDialog.h"
#include "AddPatronDialog.h"

MainWindow::MainWindow()
{
  // Set up GUI
  keys_tabwidget = new QTabWidget();
  create_info_widget();
  books_widget = new KeysWidget("book", "bookstore", (QStringList() << "Title" << "Author"
						      << "Genre" << "Publication Date" << "ISBN"));
  patrons_widget = new KeysWidget("patron", "patrons", (QStringList() << "Name" << "Address" <<
							"Mobile No." << "Landline No."));
  splitter = new QSplitter(this);

  keys_tabwidget->addTab(books_widget, "Books");
  keys_tabwidget->addTab(patrons_widget, "Patrons");

  splitter->addWidget(keys_tabwidget);
  splitter->addWidget(info_widget);
  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 1);

  toolbar = addToolBar("");
  file_menu = menuBar()->addMenu("File");

  exit_action = new QAction("Exit", this);
  exit_action->setShortcut(QKeySequence("Ctrl+Q"));

  add_book_action = new QAction(QIcon(":/new-book-icon"), "", this);
  add_patron_action = new QAction(QIcon(":/new-patron-icon"), "", this);

  file_menu->addAction(exit_action);
  toolbar->addAction(add_book_action);
  toolbar->addAction(add_patron_action);

  if (books_widget->rowCount() > 0)
    {
      books_widget->selectRow(0);
      info_widget->set_item(books_widget->currentItem()->data(Qt::UserRole).toString());
    }

  connect(keys_tabwidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
  connect(books_widget, SIGNAL(currentCellChanged(int, int, int, int)),
          this, SLOT(change_book()));
  connect(books_widget, SIGNAL(itemRemoved()), this, SLOT(onBookRemoved()));
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));
  connect(add_book_action, SIGNAL(triggered()), this, SLOT(create_add_book_dialog()));
  connect(add_patron_action, SIGNAL(triggered()), this, SLOT(create_add_patron_dialog()));

  update_statusbar();
  setCentralWidget(splitter);
  setWindowState(Qt::WindowMaximized);
  center_window();
  setWindowTitle("Eolach");
}

MainWindow::~MainWindow()
{
  bookstore.close();
}

/* General functions */

void MainWindow::create_add_book_dialog()
{
  AddBookDialog add_book_dialog(this);
  if (QDialog::Accepted == add_book_dialog.exec())
    {
      books_widget->add_item(add_book_dialog.book_key);
      update_statusbar();
    }
}

void MainWindow::create_add_patron_dialog()
{
  AddPatronDialog add_patron_dialog(this);
  if (QDialog::Accepted == add_patron_dialog.exec())
    {
      patrons_widget->add_item(add_patron_dialog.patron_key);
      update_statusbar();
    }
}

void MainWindow::create_info_widget()
{
  info_widget = new InfoWidget();
  
  isbn = new TextField("bookstore", "isbn", "ISBN:", &validate_isbn);
  title = new TextField("bookstore", "title", "Title:", &validate_generic_field);
  genre = new TextField("bookstore", "genre", "Genre:", &validate_generic_field);
  author = new TextField("bookstore", "author", "Author:", &validate_generic_field);
  publication_date = new TextField("bookstore", "publication_date", "Publication Date:",
				   &validate_numeric_field);
  book_fields = {title, author, genre, publication_date, isbn};

  name = new TextField("patrons", "name", "Name:", &validate_generic_field);
  address = new TextField("patrons", "address", "Address:", &validate_generic_field);
  items = new TextField("patrons", "items", "Borrowed items:", &validate_generic_field);
  mobile_num = new TextField("patrons", "mobile_num", "Mobile No.", &validate_generic_field);
  landline_num = new TextField("patrons", "landline_num", "Landline No.", &validate_generic_field);
  patron_fields = {name, address, mobile_num, landline_num, items};

  for (TextField* field : {title, author, genre, publication_date, isbn,
	                   name, address, mobile_num, landline_num, items})
    {
      connect(field, SIGNAL(fieldChanged(QString, QString, QString)),
              this, SLOT(onFieldChanged(QString, QString, QString)));
      info_widget->add_field(field);
    }

  // We hide the patron fields because the default tab is for the books
  for (TextField* field : patron_fields)
    {
      field->hide();
    }
}

void MainWindow::center_window()
{
  QRect screen_geo = QApplication::desktop()->screenGeometry();
  QWidget *main_window = QWidget::window();

  int x = (screen_geo.width() - 800) / 2;
  int y = (screen_geo.height() - 600) / 2;

  main_window->move(x,y);
}

/* Calculate the number of books in the DB (by counting the rows in books_widget)
   and displays them in the statusbar */
void MainWindow::update_statusbar()
{
  statusBar()->showMessage(QString::number(books_widget->rowCount()) + " books, " +
			   QString::number(patrons_widget->rowCount()) + " patrons.");
}

/* Slots */

void MainWindow::change_book()
{
  if (books_widget->currentRow() > -1)
    {
      info_widget->set_item(books_widget->currentItem()->data(Qt::UserRole).toString());
    }
}

void MainWindow::onBookRemoved()
{
  if (1 == books_widget->rowCount())
    {
      info_widget->clear();
    }
  books_widget->removeRow(books_widget->currentRow());
  update_statusbar();
}

/* Called when a books data is changed from the info_widget */
void MainWindow::onFieldChanged(QString db_table, QString sql_field_name, QString new_text)
{
  KeysWidget* current_tab = static_cast<KeysWidget*>(keys_tabwidget->currentWidget());
  QString item_key = current_tab->currentItem()->data(Qt::UserRole).toString();
  QSqlQuery update_book_info(bookstore);
  update_book_info.prepare(QString("UPDATE %1 SET %2=:new_text WHERE key=:book_key;").arg(db_table, sql_field_name));
  update_book_info.bindValue(":table", db_table);
  update_book_info.bindValue(":new_text", new_text);
  update_book_info.bindValue(":book_key", item_key);
  update_book_info.exec();

  info_widget->set_item(item_key);
  current_tab->update_item(books_widget->currentRow(), item_key);
}

void MainWindow::onTabChanged(int index)
{
  if (index == 0)
    {
      for (TextField* field : patron_fields)
	  field->hide();
      for (TextField* field : book_fields)
	  field->show();
    }
  else if (index == 1)
    {
      for (TextField* field : book_fields)
	  field->hide();
      for (TextField* field : patron_fields)
	  field->show();
    }
}
