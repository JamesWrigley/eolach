#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QWidget>
#include <QTabWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QTableWidget>


class MainWindow : public QWidget
{
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = 0);

 private:
  void initialize_ui();
  void center_window();

  QTabWidget *keys_tabwidget;

  QTableWidget *keys_tablewidget;

  QVBoxLayout *main_vbox;
};

#endif // MAINWINDOW_H
