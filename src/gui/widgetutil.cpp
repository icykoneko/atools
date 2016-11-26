/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "widgetutil.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QAction>
#include <QTextEdit>
#include <QScrollBar>

namespace atools {
namespace gui {
namespace util {

bool canTextEditUpdate(const QTextEdit *textEdit)
{
  // Do not update if scrollbar is clicked
  return !textEdit->verticalScrollBar()->isSliderDown() &&
         !textEdit->horizontalScrollBar()->isSliderDown();
}

void updateTextEdit(QTextEdit *textEdit, const QString& text)
{
  // Remember cursor position
  QTextCursor cursor = textEdit->textCursor();
  int pos = cursor.position();
  int anchor = cursor.anchor();

  // Remember scrollbar position
  int vScrollPos = textEdit->verticalScrollBar()->value();
  int hScrollPos = textEdit->horizontalScrollBar()->value();
  textEdit->setText(text);

  if(anchor != pos)
  {
    // There is a selection - Reset cursor
    int maxPos = textEdit->document()->characterCount() - 1;

    // Probably the document changed its size
    anchor = std::min(maxPos, anchor);
    pos = std::min(maxPos, pos);

    // Create selection again
    cursor.setPosition(anchor, QTextCursor::MoveAnchor);
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    textEdit->setTextCursor(cursor);
  }

  // Reset scroll bars
  textEdit->verticalScrollBar()->setValue(vScrollPos);
  textEdit->horizontalScrollBar()->setValue(hScrollPos);
}

void showHideLayoutElements(const QList<QLayout *> layouts, bool visible,
                            const QList<QWidget *>& otherWidgets)
{
  for(QWidget *w : otherWidgets)
    w->setVisible(visible);

  for(QLayout *layout : layouts)
    for(int i = 0; i < layout->count(); i++)
      layout->itemAt(i)->widget()->setVisible(visible);
}

bool anyWidgetChanged(const QList<const QObject *>& widgets)
{
  bool changed = false;
  for(const QObject *widget : widgets)
  {
    if(const QLayout * layout = dynamic_cast<const QLayout *>(widget))
      for(int i = 0; i < layout->count(); i++)
        changed |= anyWidgetChanged({layout->itemAt(i)->widget()});
    else if(const QCheckBox * cbx = dynamic_cast<const QCheckBox *>(widget))
      changed |= cbx->isTristate() ?
                 cbx->checkState() != Qt::PartiallyChecked :
                 cbx->checkState() == Qt::Checked;
    else if(const QAction * a = dynamic_cast<const QAction *>(widget))
      changed |= a->isCheckable() && !a->isChecked();
    else if(const QAbstractButton * b = dynamic_cast<const QAbstractButton *>(widget))
      changed |= b->isCheckable() && !b->isChecked();
    else if(const QLineEdit * le = dynamic_cast<const QLineEdit *>(widget))
      changed |= !le->text().isEmpty();
    else if(const QTextEdit * te = dynamic_cast<const QTextEdit *>(widget))
      changed |= !te->document()->isEmpty();
    else if(const QSpinBox * sb = dynamic_cast<const QSpinBox *>(widget))
      changed |= sb->value() != sb->maximum() && sb->value() != sb->minimum();
    else if(const QDoubleSpinBox * dsb = dynamic_cast<const QDoubleSpinBox *>(widget))
      changed |= dsb->value() != dsb->maximum() && dsb->value() != dsb->minimum();
    else if(const QComboBox * cb = dynamic_cast<const QComboBox *>(widget))
      changed |= cb->currentIndex() != 0;
  }
  return changed;
}

bool allChecked(const QList<const QAction *>& actions)
{
  bool notChecked = false;
  for(const QAction *action : actions)
    notChecked |= action->isCheckable() && !action->isChecked();
  return !notChecked;
}

bool noneChecked(const QList<const QAction *>& actions)
{
  bool checked = false;
  for(const QAction *action : actions)
    checked |= action->isCheckable() && action->isChecked();
  return !checked;
}

void changeStarIndication(QAction *action, bool changed)
{
  if(changed)
  {
    if(!action->text().startsWith("* "))
      action->setText("* " + action->text());
  }
  else
  {
    if(action->text().startsWith("* "))
      action->setText(action->text().remove(0, 2));
  }
}

} // namespace util
} // namespace gui
} // namespace atools