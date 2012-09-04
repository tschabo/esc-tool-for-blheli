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

#ifndef CCONFIGMODEL_H
#define CCONFIGMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include "common.h"

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

  void initSetData(QList<esc::SEEpromData*> * pData);

  enum eHeaderData
  {
    eName,
    eValue,
    eDefaultValue,
    eEditableRole
  };

  enum eRoles
  {
    eDatatypeRole= Qt::UserRole + 1,
    eChoicesRole= Qt::UserRole + 2,
    eRawDataRole= Qt::UserRole + 3
  };
  
private:
  QList<esc::SEEpromData*> * c_pData;

  QVariant decodeDisplayRoleData(esc::SEEpromData * pData) const;
  
};

#endif // CCONFIGMODEL_H
