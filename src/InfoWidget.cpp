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

#include <QVariant>
#include <QSqlQuery>
#include <QVBoxLayout>

#include "utils.h"
#include "InfoWidget.h"
#include "MainWindow.h"

QSqlRecord InfoWidget::currentRecord = QSqlRecord();

InfoWidget::InfoWidget()
{
    stacker = new QStackedWidget();
    QVBoxLayout* mainVbox = new QVBoxLayout(this);

    // Book fields widget
    QVBoxLayout* bookLayout = new QVBoxLayout();
    QWidget* bookWidget = new QWidget();
    bookIsbn = new TextField("isbn", "ISBN:", &validateIsbn);
    bookTitle = new TextField("title", "Title:", &validateGenericField);
    bookGenre = new TextField("genre", "Genre:", &validateGenericField);
    bookAuthor = new TextField("author", "Author:", &validateGenericField);
    bookYear = new TextField("publication_date", "Publication Date:", &validateNumericField);
    // Careful here, order is important
    for (TextField* field : {bookTitle, bookGenre, bookAuthor, bookYear, bookIsbn}) {
        bookFields.push_back(field);
        bookLayout->addLayout(field);
    }

    bookLayout->addStretch();
    bookWidget->setLayout(bookLayout);

    // Disc fields widget
    QVBoxLayout* discLayout = new QVBoxLayout();
    QWidget* discWidget = new QWidget();
    discTitle = new TextField("title", "Title:", &validateGenericField);
    discDirectorOrSpeaker = new TextField("directorOrSpeaker", "Director/Speaker:", &validateGenericField);
    discLength = new TextField("length", "Length:", &validateGenericField);
    discYear = new TextField("year", "Year:", &validateNumericField);
    discType = new TextField("type", "Type:", &validateGenericField);
    for (TextField* field : {discTitle, discDirectorOrSpeaker, discLength, discYear, discType}) {
        discFields.push_back(field);
        discLayout->addLayout(field);
    }

    discLayout->addStretch();
    discWidget->setLayout(discLayout);

    // Patron fields widget
    QVBoxLayout* patronLayout = new QVBoxLayout();
    QWidget* patronWidget = new QWidget();
    patronName = new TextField("name", "Name:", &validateGenericField);
    patronAddress = new TextField("address", "Address:", &validateGenericField);
    patronMobile = new TextField("mobile_num", "Mobile No.", &validateNumericField);
    patronLandline = new TextField("landline_num", "Landline No.", &validateNumericField);
    for (TextField* field : {patronName, patronAddress, patronMobile, patronLandline}) {
        patronFields.push_back(field);
        patronLayout->addLayout(field);
    }

    history = new PatronHistory();
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    patronLayout->addSpacing(30);
    patronLayout->addWidget(separator);
    patronLayout->addLayout(history);
    patronWidget->setLayout(patronLayout);

    connect(&MainWindow::signaller, &SignalSingleton::itemSelected,
            [this] (QSqlRecord r) {
                currentRecord = r;
            });

    stacker->addWidget(bookWidget);
    stacker->addWidget(discWidget);
    stacker->addWidget(patronWidget);

    mainVbox->addWidget(stacker);
    mainVbox->addStretch();
    setLayout(mainVbox);
    setFrameShape(QFrame::StyledPanel);
    setObjectName("MainQFrame");
    setStyleSheet("QFrame#MainQFrame {border: 8px solid #909090; border-radius: 7px;}");
}

void InfoWidget::changeLayout(int index)
{
    // Reload the patrons widget in case an item has been deleted that
    // someone had borrowed (who knows why that would happen, but better
    // safe than sorry).
    if (index == 2) {
        history->reload();
    }

    stacker->setCurrentIndex(index);
}

void InfoWidget::clear()
{
    for (auto &itemFields : {bookFields, discFields, patronFields}) {
        for (TextField *field : itemFields) {
            field->setText("");
        }
    }
}
