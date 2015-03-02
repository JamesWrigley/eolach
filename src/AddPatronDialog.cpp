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

#include <random>
#include <QSqlQuery>
#include <QMessageBox>
#include "miscellanea.h"
#include "AddPatronDialog.h"

AddPatronDialog::AddPatronDialog(QWidget *parent)
{
  main_layout = new QVBoxLayout(this);

  name = new DLineEdit("Name", &validate_generic_field);
  address = new DLineEdit("Address", &validate_generic_field);
  mobile_num = new DLineEdit("Mobile No.", &validate_generic_field);
  landline_num = new DLineEdit("Landline No.", &validate_generic_field);

  finish_button = new QPushButton("Finish");
  finish_button->setDefault(true);

  connect(finish_button, SIGNAL(clicked(bool)), this, SLOT(check_fields()));

  main_layout->addLayout(name);
  main_layout->addLayout(address);
  main_layout->addLayout(mobile_num);
  main_layout->addLayout(landline_num);
  main_layout->addStretch();
  main_layout->addWidget(finish_button);
  main_layout->addStretch();

  setLayout(main_layout);
}

void AddPatronDialog::check_fields()
{
  QStringList warnings;
  if (!name->valid)
      warnings << "Name";
  if (!address->valid)
      warnings << "Address";
  if (!mobile_num->valid)
    warnings << "Mobile No.";
  if (!landline_num->valid)
    warnings << "Landline No.";

  if (!warnings.isEmpty())
    {
      QString invalid_fields;
      if (warnings.size() == 1)
	{
	  invalid_fields = warnings[0];
	}
      else if (warnings.size() == 2)
	{
	  invalid_fields = warnings.join(" and ");
	}
      else if (warnings.size() > 2)
	{
	  warnings[warnings.size() - 1].prepend("and ");
	  invalid_fields = warnings.join(", ");
	}

      int confirm = QMessageBox::warning(this, "Warning",
					 invalid_fields +
					 " incomplete, would you like to continue anyway?",
					 QMessageBox::Yes, QMessageBox::No);
      if (QMessageBox::Yes == confirm)
	{
	  add_patron();
	}
    }
  else
    {
      add_patron();
    }
}

void AddPatronDialog::add_patron()
{
  std::random_device key_gen;
  patron_key = QString::number(key_gen()) + "p";

  QSqlQuery insert(QSqlDatabase::database());
  insert.prepare("INSERT INTO patrons (key, name, address, mobile_num, landline_num, items) "
		 "VALUES (:key, :name, :address, :mobile_num, :landline_num, :items);");
  insert.bindValue(":key", patron_key);
  insert.bindValue(":name", name->text());
  insert.bindValue(":address", address->text());
  insert.bindValue(":mobile_num", mobile_num->text());
  insert.bindValue(":landline_num", landline_num->text());
  insert.bindValue(":items", "");
  insert.exec();

  done(QDialog::Accepted);
}
