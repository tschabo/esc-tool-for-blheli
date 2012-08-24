#include "ceditordelegate.h"

#include "cconfigmodel.h"

#include <QComboBox>
#include <QLineEdit>

CEditorDelegate::CEditorDelegate(QObject *parent) :
  QItemDelegate(parent)
{
}

QWidget *CEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(option);

  if(index.data(CConfigModel::eRawValueRole).toString().length() == 32)
  {
    QLineEdit * pEditor = new QLineEdit(parent);
    pEditor->setMaxLength(16);
    return pEditor;
  }

  QComboBox * pEditor = new QComboBox(parent);

  QStringList qslOptions = index.data(CConfigModel::eOptionsRole).toStringList();

  int iVal = index.data(CConfigModel::eRawValueRole).toString().toInt(0, 16);

  for(int i = 0; i < qslOptions.count(); i++)
  {
    pEditor->addItem(QString::number(i+1) + " : " + qslOptions.at(i));
  }

  pEditor->setCurrentIndex(iVal);

  return pEditor;
}

void CEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  if(index.data(CConfigModel::eRawValueRole).toString().length() == 32)
  {
    QLineEdit * pEditor = static_cast<QLineEdit*>(editor);
    pEditor->setText(index.data().toString());
  }
  else
  {
    QComboBox * pEditor = static_cast<QComboBox*>(editor);
    int iVal = index.data(CConfigModel::eRawValueRole).toString().toInt(0, 16) - 1;
    pEditor->setCurrentIndex(iVal);
  }
}

void CEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  if(index.data(CConfigModel::eRawValueRole).toString().length() == 32)
  {
    QLineEdit * pEditor = static_cast<QLineEdit*>(editor);
    QString qsText = pEditor->text();
    if(qsText.length() > 16)
    {
      return;
    }
    while(qsText.length() < 16)
    {
      qsText.append(" ");
    }
    model->setData(index, qsText);
  }
  else
  {
    QComboBox * pEditor = static_cast<QComboBox*>(editor);
    model->setData(index, pEditor->currentIndex()+1);
  }
}
