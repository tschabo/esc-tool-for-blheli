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
        case CConfigModel::eName:
          return c_pData->at(index.row())->qsName;
        case CConfigModel::eValue:
          if(c_pData->at(index.row())->qsReadData.count() == 32)
          {
            return hexStringToString(c_pData->at(index.row())->qsReadData);
          }
          if(c_pData->at(index.row())->qsReadData.count() == 2)
          {
            bool bOk = true;
            int iTmp = c_pData->at(index.row())->qsReadData.toInt(&bOk, 16);
            if(!bOk)
            {
              return QString("decode error: ").append(c_pData->at(index.row())->qsReadData);
            }
            QString qsOut;
            if((iTmp <= c_pData->at(index.row())->qslChoices.count()))
            {
              return qsOut + QString::number(iTmp) + " : " + c_pData->at(index.row())->qslChoices.at(iTmp-c_pData->at(index.row())->iMinVal);
            }
            return iTmp;
          }
          return c_pData->at(index.row())->qsReadData;
        case CConfigModel::eDefaultValue:
          return c_pData->at(index.row())->qsDefaultVal;
        default:
          return QVariant();
      }
      break;


    case CConfigModel::eEditableRole:
      if(index.row() != 1)
      {
//        return 1;
      }
      if(c_pData->at(index.row())->bReadOnly)
      {
        return 1;
      }
      return 0;
      break;

    case CConfigModel::eOptionsRole:
      return c_pData->at(index.row())->qslChoices;
      break;

    case CConfigModel::eRawValueRole:
      return c_pData->at(index.row())->qsReadData;

    case CConfigModel::eMinValRole:
      return c_pData->at(index.row())->iMinVal;

    default:
      return QVariant();
  }
}

QVariant CConfigModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Vertical)
  {
    return QVariant();
  }
  if(role == Qt::DisplayRole)
  {
    switch(section)
    {
      case CConfigModel::eName:
        return tr("Name");
        break;
      case CConfigModel::eValue:
        return tr("Value");
        break;
      case CConfigModel::eDefaultValue:
        return tr("Default value");
        break;
      default:
        return "ERR";
    }
  }
  return QVariant();
}

bool CConfigModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(role != Qt::EditRole)
  {
    return false;
  }
  if(index.column() != 1)
  {
    return false;
  }

  if(value.toString().length() == 16)
  {
    QString qsTmp;
    for(int i = 0; i < value.toString().length(); i++)
    {
      QString qsX = QString::number((int)value.toString().at(i).toAscii(), 16);
      if(qsX.count() == 1)
      {
        qsX.prepend("0");
      }
      qsTmp.append(qsX);
    }
    if(qsTmp.count() != 32)
    {
      return false;
    }
    c_pData->at(index.row())->qsReadData = qsTmp;
    dataChanged(index,index);
    return true;
  }

  QString qsData = QString::number(value.toInt(), 16);
  if(qsData.length() == 1)
  {
    qsData.prepend("0");
  }
  if(qsData.length() == 2)
  {
    c_pData->at(index.row())->qsReadData = qsData;
    dataChanged(index,index);
    return true;
  }
  return false;
}

Qt::ItemFlags CConfigModel::flags(const QModelIndex &index) const
{
  if(index.column() != 1)
  {
    return Qt::NoItemFlags;
  }
  bool bOk = true;
  if(index.data(CConfigModel::eEditableRole).toInt(&bOk) == 0)
  {
    if(!bOk)
    {
      return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
  }
  return Qt::NoItemFlags;
}

void CConfigModel::initSetData(QList<sEEpromData *> *pData)
{
  c_pData = pData;
  reset();
}

QString CConfigModel::hexStringToString(QString qsHexString) const
{
  if(qsHexString.count() % 2 == 1)
  {
    return QString("decode error: ").append(qsHexString);
  }
  QString qsTmp;
  bool bOk = true;
  for(int i = 0; i < qsHexString.count(); i+=2)
  {
    qsTmp.append(qsHexString.mid(i, 2).toShort(&bOk, 16));
    if(!bOk)
    {
      return QString("decode error: ").append(qsHexString);
    }
  }
  return qsTmp;
}
