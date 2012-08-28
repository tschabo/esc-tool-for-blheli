#include "cescconf.h"

#include<QFile>
#include "ccsvparser.h"

CEscConf::CEscConf(QObject *parent) :
  CAbstractSerialComm(parent)
{
}

bool CEscConf::readConfig(QList<sEEpromData *> *pData)
{
  bool bRetVal = false;

  CCsvParser p;
  connect(&p, SIGNAL(signal_dumpString(QString)), this, SIGNAL(signal_dumpString(QString)));
  QString qsResponse;
  sEEpromData * pEEpromMode = NULL;
  int iEnd = 0;

  if(!openProgrammer())
  {
    goto cleanup;
  }
  if(!initProgrammer())
  {
    goto cleanup;
  }
  // 1A02 - Adress of the Revision of the eeprom layout
  if(!serialReadBlock("01", "1A02", qsResponse))
  {
    goto cleanup;
  }

  {
    sEEpromData * pTmp = new sEEpromData;
    pTmp->bReadOnly = true;
    pTmp->iSize = 1;
    pTmp->qsAdress = "1A02";
    pTmp->qsName = "EEprom Layout Revision";
    pTmp->qsReadData  = qsResponse;
    pTmp->iMinVal=0;
    pData->prepend(pTmp);
  }

  if(!p.parseFile(QString("common") + "_" + qsResponse + ".blh", pData))
  {
    goto cleanup;
  }

  for(int i = 0; i < pData->count(); i++)
  {
    if(!readEEpromData(pData->at(i)))
    {
      goto cleanup;
    }
    // MODE
    if(pData->at(i)->qsName == "Mode")
    {
      pEEpromMode = pData->at(i);
    }
  }

  if(pEEpromMode == NULL)
  {
    signal_dumpString(tr("*** Mode not found\n"));
    goto cleanup;
  }

  iEnd = pData->count();

  if(!p.parseFile(pEEpromMode->qsReadData + "_" + qsResponse + ".blh", pData))
  {
    goto cleanup;
  }

  for(;iEnd < pData->count(); iEnd++)
  {
    if(!readEEpromData(pData->at(iEnd)))
    {
      goto cleanup;
    }
  }

  bRetVal = true;

cleanup:
  close();
  return bRetVal;
}

bool CEscConf::writeConfig(QList<sEEpromData *> *pData)
{
  bool bRetVal = false;
  if(!openProgrammer())
  {
    goto cleanup;
  }
  if(!initProgrammer())
  {
    goto cleanup;
  }
  if(!erasePage("0D"))
  {
    goto cleanup;
  }
  for(int i = 0; i < pData->count(); i++)
  {
    if(!serialWriteBlock(pData->at(i)->iSize,
                         pData->at(i)->qsAdress,
                         pData->at(i)->qsReadData))
    {
      goto cleanup;
    }
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** writing config to device failed\n"));
  }
  else
  {
    signal_dumpString(tr("*** success\n"));
  }
  close();
  return bRetVal;
}

bool CEscConf::readEEpromData(sEEpromData *pData)
{
  QString qsLength = QString::number(pData->iSize, 16).toUpper();
  if(qsLength.size() == 1)
  {
    qsLength.prepend("0");
  }
  if(qsLength.size() != 2)
  {
    signal_dumpString(tr("*** readEEpromData(...): size is malformed\n"));
    return false;
  }
  if(!serialReadBlock(qsLength, pData->qsAdress, pData->qsReadData))
  {
    return false;
  }
  return true;
}
