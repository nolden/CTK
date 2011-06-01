/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QAbstractItemView>
#include <QCleanlooksStyle>
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QToolBar>

// CTK includes
#include "ctkCheckableComboBox.h"
#include <ctkCheckableModelHelper.h>

//-----------------------------------------------------------------------------
class ctkCheckableComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckableComboBox);
protected:
  ctkCheckableComboBox* const q_ptr;
  QModelIndexList checkedIndexes()const;
public:
  ctkCheckableComboBoxPrivate(ctkCheckableComboBox& object);
  void init();

  void updateCheckedList();
  
  ctkCheckableModelHelper* CheckableModelHelper;
  QModelIndexList          CheckedList;
};

//-----------------------------------------------------------------------------
ctkCheckableComboBoxPrivate::ctkCheckableComboBoxPrivate(ctkCheckableComboBox& object)
  : q_ptr(&object)
{
  this->CheckableModelHelper = 0;
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBoxPrivate::init()
{
  Q_Q(ctkCheckableComboBox);
  this->CheckableModelHelper = new ctkCheckableModelHelper(Qt::Horizontal, q);
  this->CheckableModelHelper->setForceCheckability(true);
  
  q->setCheckableModel(q->model());
  q->view()->installEventFilter(q);
  q->view()->viewport()->installEventFilter(q);
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBoxPrivate::updateCheckedList()
{
  Q_Q(ctkCheckableComboBox);
  QModelIndexList newCheckedList = this->checkedIndexes();
  if (newCheckedList == this->CheckedList)
    {
    return;
    }
  this->CheckedList = newCheckedList;
  qDebug() << "change";
  emit q->checkedIndexesChanged();
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBoxPrivate::checkedIndexes()const
{
  Q_Q(const ctkCheckableComboBox);
  return q->model()->match(
      q->model()->index(0,0), Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchRecursive);
}
 
//-----------------------------------------------------------------------------
ctkCheckableComboBox::ctkCheckableComboBox(QWidget* parentWidget)
  : QComboBox(parentWidget)
  , d_ptr(new ctkCheckableComboBoxPrivate(*this))
{
  Q_D(ctkCheckableComboBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCheckableComboBox::~ctkCheckableComboBox()
{
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBox::eventFilter(QObject *o, QEvent *e)
{
  Q_D(ctkCheckableComboBox);
  switch (e->type())
    {
    case QEvent::MouseButtonRelease:
      {
      QMouseEvent *m = static_cast<QMouseEvent *>(e);
      if (this->view()->isVisible() && 
          this->view()->rect().contains(m->pos()) &&
          this->view()->currentIndex().isValid()
          //&& !blockMouseReleaseTimer.isActive()
          && (this->view()->currentIndex().flags() & Qt::ItemIsEnabled)
          && (this->view()->currentIndex().flags() & Qt::ItemIsSelectable))
        {
        // make the item current, it will then call QComboBox::update (and
        // repaint) when the current index data is changed (checkstate toggled).
        this->setCurrentIndex(this->view()->currentIndex().row());
        d->CheckableModelHelper->toggleCheckState(this->view()->currentIndex());
        return true;
        }
      break;
      } 
    default:
        break;
    }
  return this->QComboBox::eventFilter(o, e);
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::setCheckableModel(QAbstractItemModel* newModel)
{
  Q_D(ctkCheckableComboBox);
  this->disconnect(this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                   this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
  if (newModel != this->model())
    {
    this->setModel(newModel);
    }
  this->connect(this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
  d->CheckableModelHelper->setModel(newModel);
  d->updateCheckedList();
}

//-----------------------------------------------------------------------------
QAbstractItemModel* ctkCheckableComboBox::checkableModel()const
{
  Q_D(const ctkCheckableComboBox);
  return this->model();
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBox::checkedIndexes()const
{
  Q_D(const ctkCheckableComboBox);
  return d->CheckedList;
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBox::allChecked()const
{
  Q_D(const ctkCheckableComboBox);
  return d->CheckableModelHelper->headerCheckState(0) == Qt::Checked;
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBox::noneChecked()const
{
  Q_D(const ctkCheckableComboBox);
  return d->CheckableModelHelper->headerCheckState(0) == Qt::Unchecked;
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::onDataChanged(const QModelIndex& start, const QModelIndex& end)
{
  Q_D(ctkCheckableComboBox);
  Q_UNUSED(start);
  Q_UNUSED(end);
  d->updateCheckedList();
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::paintEvent(QPaintEvent *)
{
  Q_D(ctkCheckableComboBox);
  QStylePainter painter(this);
  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;
  this->initStyleOption(&opt);

  if (this->allChecked())
    {
    opt.currentText = "All";
    opt.currentIcon = QIcon();
    }
  else if (this->noneChecked())
    {
    opt.currentText = "None";
    opt.currentIcon = QIcon();
    }
  else
    {
    //search the checked items
    QModelIndexList indexes = this->checkedIndexes();
    if (indexes.count() == 1)
      {
      opt.currentText = this->model()->data(indexes[0], Qt::DisplayRole).toString();
      opt.currentIcon = qvariant_cast<QIcon>(this->model()->data(indexes[0], Qt::DecorationRole));
      }
    else
      {
      QStringList indexesText;
      foreach(QModelIndex checkedIndex, indexes)
        {
        indexesText << this->model()->data(checkedIndex, Qt::DisplayRole).toString();
        }
      opt.currentText = indexesText.join(", ");
      opt.currentIcon = QIcon();
      }
    }
  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw the icon and text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
