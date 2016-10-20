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

#include "utils.h"
#include "MainWindow.h"
#include "KeysWidget.h"
#include "InfoWidget.h"
#include "AddItemDialog.h"
#include "DatabaseTableWidget.h"

SignalSingleton MainWindow::signaller;

MainWindow::MainWindow()
{
    // Set up GUI
    keysTabwidget = new QTabWidget();
    infoWidget = new InfoWidget();

    booksWidget = new DatabaseTableWidget("books", {{2, "Title"}, {3, "Author"},
                                                    {5, "Genre"}, {4, "Publication Date"},
                                                    {1, "ISBN"}});
    discsWidget = new KeysWidget("discs", getDiscInfo, {"Title", "Director/Speaker",
                                                        "Length", "Year", "Type"});
    patronsWidget = new KeysWidget("patrons", getPatronInfo, {"Name", "Address",
                                                              "Mobile No.", "Landline No."});
    splitter = new QSplitter(this);

    keysTabwidget->addTab(booksWidget, "Books");
    keysTabwidget->addTab(discsWidget, "Discs");
    keysTabwidget->addTab(patronsWidget, "Patrons");
    booksWidget->show();

    splitter->addWidget(keysTabwidget);
    splitter->addWidget(infoWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    toolbar = addToolBar("");
    fileMenu = menuBar()->addMenu("File");

    exitAction = new QAction("Exit", this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));

    addItemAction = new QAction(QIcon(":/new-item-icon"), "", this);
    addItemAction->setToolTip("Add new item");

    fileMenu->addAction(exitAction);
    toolbar->addAction(addItemAction);

    connect(keysTabwidget, &QTabWidget::currentChanged, infoWidget, &InfoWidget::changeLayout);

    connect(&signaller, &SignalSingleton::itemRemoved, this, &MainWindow::onItemRemoved);
    connect(&signaller, &SignalSingleton::itemChanged, this, &MainWindow::onFieldChanged);

    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(addItemAction, &QAction::triggered, this, &MainWindow::createAddItemDialog);

    updateStatusbar();
    setCentralWidget(splitter);
    setWindowState(Qt::WindowMaximized);
    centerWindow();
    setWindowTitle("Eolach");
}

MainWindow::~MainWindow()
{
    bookstore.close();
}

/* General functions */

void MainWindow::createAddItemDialog()
{
    AddItemDialog addItemDialog(keysTabwidget->currentIndex());
    if (QDialog::Accepted == addItemDialog.exec()) {
        if (addItemDialog.getItemKey().endsWith("b")) {
            booksWidget->addItem(addItemDialog.getItemKey());
	} else if (addItemDialog.getItemKey().endsWith("d")) {
            discsWidget->addItem(addItemDialog.getItemKey());
	} else if (addItemDialog.getItemKey().endsWith("p")) {
            patronsWidget->addItem(addItemDialog.getItemKey());
	}
        updateStatusbar();
    }
}

void MainWindow::centerWindow()
{
    QRect screenGeo = QApplication::desktop()->screenGeometry();
    QWidget *mainWindow = QWidget::window();

    int x = (screenGeo.width() - 800) / 2;
    int y = (screenGeo.height() - 600) / 2;

    mainWindow->move(x,y);
}

/* Calculate the number of items in the DB display them in the statusbar */
void MainWindow::updateStatusbar()
{
    statusBar()->showMessage(QString::number(booksWidget->rowCount()) + " books, " +
                             QString::number(discsWidget->rowCount()) + " discs, " +
                             QString::number(patronsWidget->rowCount()) + " patrons.");
}

/* Slots */

void MainWindow::onItemRemoved()
{
    // KeysWidget* currentTab = static_cast<KeysWidget*>(keysTabwidget->currentWidget());
    // if (1 == currentTab->rowCount()) {
    //     infoWidget->clear();
    // }

    // currentTab->removeRow(currentTab->currentRow());
    updateStatusbar();
}

/* Called when an items data is changed from the InfoWidget */
void MainWindow::onFieldChanged(QSqlRecord record)
{
    DatabaseTableWidget* widget = NULL;
    QString tableIdentifier = record.value("key").toString().right(1);

    if (tableIdentifier == "b") { // Book record
        widget = booksWidget;
    } /* else if (tableIdentifier == "d") { // Disc record
        widget = discsWidget;
    } else if (tableIdentifier == "p") { // Patron record
        widget = patronWidget;
    } */

    int row = widget->getCurrentRow();
    widget->getModel()->setRecord(row, record);
    widget->getModel()->submitAll();
    // We need to manually reselect the row because the model will reload
    widget->selectRow(row);
}
