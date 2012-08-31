#ifndef CESCCONF_H
#define CESCCONF_H

#include "cabstractserialcomm.h"
#include "ccsvparser.h"

class CEscConf : public CAbstractSerialComm
{
  Q_OBJECT
public:
  explicit CEscConf(QObject *parent = 0);

  bool readConfig(QList<sEEpromData*> * pData);
  bool writeConfig(QList<sEEpromData*> * pData);

private:
  bool readEEpromData(sEEpromData* pData);
  
};

#endif // CESCCONF_H
