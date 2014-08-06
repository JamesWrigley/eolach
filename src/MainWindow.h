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


class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  MainWindow();

 private:
  void initialize_ui();
  void center_window();

  QTabWidget *keys_tabwidget;
  QSplitter *splitter;
  QMenu *file_menu;
  QHash<QString, int> books_map;
};

#endif // MAINWINDOW_H
