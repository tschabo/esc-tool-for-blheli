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

#include "cconfigmodel.h"

CConfigModel::CConfigModel(QObject *parent) :
  QAbstractTableModel(parent),
  c_pData(NULL)
{
}

int CConfigModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 3;
}

int CConfigModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  if(c_pData == NULL)
  {
    return 0;
  }
  return c_pData->count();
}

QVariant CConfigModel::data(const QModelIndex &index, int role) const
{
  if(c_pData == NULL)
  {
    return QVariant();
  }
  if(!index.isValid())
  {
    return QVariant();
  }

  switch(role)
  {
    case Qt::DisplayRole:
      switch(index.column())
      {
        case eName:
          return c_pData->at(index.row())->qsName;
        case eValue:
          return decodeDisplayRoleData(c_pData->at(index.row()));
        case eDefaultValue:
          if(c_pData->at(index.row())->qbaDefaultValue.length() != 2)
          {
            return QVariant();
          }
          return c_pData->at(index.row())->qbaDefaultValue.toInt(NULL, 16);
      }
      return QVariant();

    case eEditableRole:
      return !c_pData->at(index.row())->bReadOnly;

    case eDatatypeRole:
      return c_pData->at(index.row())->eDataType;

    case eChoicesRole:
      return c_pData->at(index.row())->qslChoices;

    case eRawDataRole:
      return c_pData->at(index.row())->qbaReadData;
  }

  return QVariant();
}

QVariant CConfigModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal)
  {
    return QVariant();
  }

  if(role != Qt::DisplayRole)
  {
    return QVariant();
  }

  switch(section)
  {
    case eName:
      return tr("Name");
    case eValue:
      return tr("Value");
    case eDefaultValue:
      return tr("Default");
  }

  return QVariant();
}

Qt::ItemFlags CConfigModel::flags(const QModelIndex &index) const
{
  if(index.column() == eName || index.column() == eDefaultValue)
  {
    return Qt::NoItemFlags;
  }

  if(index.data(eEditableRole).toBool())
  {
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
  }
  return Qt::NoItemFlags;
}

bool CConfigModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(!index.isValid())
  {
    return false;
  }
  if(role != Qt::EditRole)
  {
    return false;
  }
  if(index.column() != 1)
  {
    return false;
  }

//  QString qsValue;

//  switch(index.data(eDatatypeRole).toInt())
//  {
//    case esc::eNumber:
//      if(index.data(Qt::DisplayRole).toByteArray().split('-').length() < 1)
//      {
//        return false;
//      }
//      qsValue = index.data(Qt::DisplayRole).toByteArray().split('-').at(0);
//      qsValue.remove(' ');
//      qsValue = QString::number(qsValue.toInt(), 16);
//      if(qsValue.length() == 1)
//      {
//        qsValue.prepend("0");
//      }
//      if(qsValue.length() > 2)
//      {
//        return false;
//      }
//      break;
//    case esc::eString:
//      qsValue = CCommon::stringToHexByteArray(index.data(Qt::DisplayRole).toByteArray());
//      break;
//    case esc::eVersionNumber:
//      // readonly value
//      return false;
//      break;
//    case esc::ePercent:
//      // readonly value
//      return false;
//      break;
//    case esc::eMode:
//      // readonly value
//      return false;
//      break;
//    case esc::eRaw:
//      qsValue = index.data(Qt::DisplayRole).toByteArray();
//      break;
//    default:
//      return false;
//  }

//  c_pData->at(index.row())->qbaReadData  = qsValue.toAscii();
  c_pData->at(index.row())->qbaReadData = value.toByteArray();
  dataChanged(index, index);
  return true;
}

void CConfigModel::initSetData(QList<esc::SEEpromData *> *pData)
{
  c_pData = pData;
  reset();
}

QVariant CConfigModel::decodeDisplayRoleData(esc::SEEpromData *pData) const
{
  switch(pData->eDataType)
  {
    case esc::eNumber:
    {
      int iNum = pData->qbaReadData.toInt(NULL,16);
      QString qsTextforNumber;
      for(int i = 0; i < pData->qslChoices.length(); i++)
      {
        QStringList qslTmp = pData->qslChoices.at(i).split('~');
        if(qslTmp.length() != 2)
        {
          continue;
        }
        if(qslTmp.at(0).toInt(NULL, 16) == iNum)
        {
          qsTextforNumber = qslTmp.at(1);
          break;
        }
      }
      return QString("%1 - %2").arg(iNum).arg(qsTextforNumber);
    }
    case esc::eString:
      return CCommon::hexStringToString(pData->qbaReadData);
    case esc::eVersionNumber:
      if(pData->qbaReadData.length() != 4)
      {
        // Fallback
        return pData->qbaReadData;
      }
      return QString("%1.%2").arg(pData->qbaReadData.mid(0,2).toInt(NULL,16)).arg(pData->qbaReadData.mid(2,2).toInt(NULL,16));
    case esc::ePercent:
      //      return QString::number(((double)(100*QByteArray::number(pData->qbaReadData)))/255.0);
    case esc::eMode:
      if(pData->qbaReadData == "A55A")
      {
        return "Main";
      }
      if(pData->qbaReadData == "5AA5")
      {
        return "Tail";
      }
      if(pData->qbaReadData == "55AA")
      {
        return "Multi";
      }
      return pData->qbaReadData;
    case esc::eRaw:
      return pData->qbaReadData;
  }
  return QVariant();
}
