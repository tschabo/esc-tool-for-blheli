#ifndef CCONFIGMODEL_H
#define CCONFIGMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include "ccsvparser.h"

class CConfigModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit CConfigModel(QObject *parent = 0);

  int columnCount(const QModelIndex &parent) const;
  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

  void initSetData(QList<sEEpromData*> * pData);

  enum eHeaderData
  {
    eName,
    eValue,
    eDefaultValue
  };

  enum eRoles
  {
    eEditableRole = Qt::UserRole + 1,
    eOptionsRole = Qt::UserRole + 2,
    eRawValueRole = Qt::UserRole + 3
  };

signals:
  
public slots:

private:
  QList<sEEpromData*> * c_pData;

  QString hexStringToString(QString qsHexString) const;
  
};

#endif // CCONFIGMODEL_H
