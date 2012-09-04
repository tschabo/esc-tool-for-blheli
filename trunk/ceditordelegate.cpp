/****************************************************************************
** Copyright (c) 2012 Sebastian Schmitt
** All right reserved.
** Web: http://code.google.com/p/esc-tool-for-blheli/
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#include "ceditordelegate.h"
#include "cconfigmodel.h"

#include <QLineEdit>
#include <QComboBox>


CEditorDelegate::CEditorDelegate(QObject *parent) :
  QItemDelegate(parent)
{
}

QWidget *CEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(option);
  QWidget * pEditor = NULL;
  switch(index.data(CConfigModel::eDatatypeRole).toInt())
  {
    case esc::eNumber:
      pEditor = new QComboBox(parent);
      for(int i = 0; i < index.data(CConfigModel::eChoicesRole).toStringList().length(); i++)
      {
        QByteArray qbaTmp;
        QString qsTmp;
        if(!CCommon::decodeChoice(index.data(CConfigModel::eChoicesRole).toStringList().at(i), qbaTmp, qsTmp))
        {
          continue;
        }
        ((QComboBox*) pEditor)->addItem(QString::number(qbaTmp.toInt(NULL, 16)) + " - " + qsTmp, qbaTmp);
      }
      return pEditor;
    case esc::eString:
      pEditor = new QLineEdit(parent);
      ((QLineEdit*) pEditor)->setMaxLength(16);
      ((QLineEdit*) pEditor)->setText(index.data(Qt::DisplayRole).toByteArray());
      return pEditor;
  }
  pEditor = new QLineEdit(parent);
  ((QLineEdit*) pEditor)->setMaxLength(0);
  return pEditor;
}

void CEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  switch(index.data(CConfigModel::eDatatypeRole).toInt())
  {
    case esc::eNumber:
      for(int i = 0; i < ((QComboBox*) editor)->count(); i++)
      {
        if(((QComboBox*) editor)->itemData(i).toByteArray() == index.data(CConfigModel::eRawDataRole))
        {
          ((QComboBox*) editor)->setCurrentIndex(i);
          return;
        }
      }
      break;
    default:
      ((QLineEdit*) editor)->setText(index.data(Qt::DisplayRole).toByteArray());
      return;
  }
  return;
}

void CEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  switch(index.data(CConfigModel::eDatatypeRole).toInt())
  {
    case esc::eNumber:
      model->setData(index, ((QComboBox*) editor)->itemData(((QComboBox*) editor)->currentIndex()).toByteArray());
      break;
    default:
      QString qsTmp = ((QLineEdit*) editor)->text();
      for(int i = qsTmp.count(); i < 16; i++)
      {
        qsTmp.append(" ");
      }
      model->setData(index, CCommon::stringToHexByteArray(qsTmp));
      break;
  }
  return;
}
