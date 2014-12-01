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

#include <QMouseEvent>
#include "CLineEdit.h"
#include "AddBookDialog.h"

CLineEdit::CLineEdit(QWidget* parent)
{
  connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
  // We set this now to use in onEditingFinished since the window color will
  // have by then been changed by mouseDoubleClickEvent().
  background_color = palette().color(QPalette::Window).name();
  tooltip = "Double-click to edit";

  setReadOnly(true);
  setFrame(false);
  setStyleSheet("QLineEdit { background: " + background_color + " }");
  setToolTip(tooltip);
  setToolTipDuration(3000);
}

void CLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    {
      current_text = text();
      setReadOnly(false);
      setFrame(true);
      setStyleSheet("QLineEdit { background: #F7F7F7 }");
      setToolTip("");
      selectAll();
    }
}

void CLineEdit::onEditingFinished()
{
  setReadOnly(true);
  setFrame(false);
  setStyleSheet("QLineEdit { background: " + background_color + " }");
  setToolTip(tooltip);
  deselect();

  // Avoid spurious DB calls when the user doesn't actually change the text
  if (current_text != text())
    {
      current_text = text();
      emit textChanged(current_text);
    }
}
