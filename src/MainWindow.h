#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QHash>
#include <QMenu>
#include <QSplitter>
#include <QTabWidget>
#include <QMainWindow>
#include <QStringList>
#include <QTableWidget>

#include "Book.h"
#include "KeysWidget.h"
#include "InfoWidget.h"


class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  MainWindow();

 private:
  void initialize_ui();
  void populate_keys();
  void center_window();

  KeysWidget *keys_widget;
  InfoWidget* info_widget;

  QTabWidget *keys_tabwidget;
  QSplitter *splitter;
  QMenu *file_menu;
  QHash<QString, Book> books_map;
};

#endif // MAINWINDOW_H
