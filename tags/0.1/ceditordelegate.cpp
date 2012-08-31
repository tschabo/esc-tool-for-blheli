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
  int iMinVal = index.data(CConfigModel::eMinValRole).toInt();

  for(int i = 0; i < qslOptions.count(); i++)
  {
    pEditor->addItem(QString::number(i + iMinVal) + " : " + qslOptions.at(i));
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
    int iMinVal = index.data(CConfigModel::eMinValRole).toInt();
    QComboBox * pEditor = static_cast<QComboBox*>(editor);
    int iVal = index.data(CConfigModel::eRawValueRole).toString().toInt(0, 16) - iMinVal;
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
    int iMinVal = index.data(CConfigModel::eMinValRole).toInt();
    QComboBox * pEditor = static_cast<QComboBox*>(editor);
    model->setData(index, pEditor->currentIndex()+iMinVal);
  }
}
