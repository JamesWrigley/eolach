#include "InfoWidget"


InfoWidget::InfoWidget(QWidget *parent) : QWidget(parent)
{
  labels_holder = new QHBoxLayout();
  labels = new QVBoxLayout();
  data = new QVBoxLayout();
  main_vbox = new QVBoxLayout();

  title_label = new QLabel("Title:", this);
  author_label = new QLabel("Author:", this);
  publication_date_label = new QLabel("Publication Date:", this);
  // synopsis_label = new QLabel("Synopsis:", this);
  // available_label = new QLabel("Available:", this);

  title = new QLabel("The Count of Monte Cristo", this);
  author = new QLabel("Alexandre Dumas", this);
  publication_date = new QLabel("1844", this);

  // Packing
  labels_holder->addWidget(title_label);
  labels_holder->addWidget(author_label);
  labels_holder->addWidget(publication_date_label);
}
