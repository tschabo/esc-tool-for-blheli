#include "cescconf.h"

#include <QFile>
#include "ccsvparser.h"

QString hexStringToString(QString qsHexString)
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
  bool bDamped = false;
  QString qsDampedEscs;
  QStringList qslDampedEscs;

  QFile fDamped("damped_esc.list");
  if(!fDamped.exists())
  {
    goto cleanup;
  }
  if(!fDamped.open(QIODevice::ReadOnly))
  {
    goto cleanup;
  }
  qsDampedEscs = fDamped.readAll();
  qslDampedEscs = qsDampedEscs.split("\n", QString::SkipEmptyParts);

  if(!openProgrammer())
  {
    goto cleanup;
  }
  if(!initProgrammer())
  {
    goto cleanup;
  }
  // 1A00 - Adress of the Revision of the eeprom layout
  if(!serialReadBlock("02", "1A00", qsResponse))
  {
    goto cleanup;
  }

  {
    sEEpromData * pTmp = new sEEpromData;
    pTmp->bReadOnly = true;
    pTmp->iSize = 2;
    pTmp->qsAdress = "1A00";
    pTmp->qsName = "Revision";
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
    if(pData->at(i)->qsName == "BESC")
    {
      for(int j = 0; j < qslDampedEscs.count(); j++)
      {
        if(qslDampedEscs.at(j) == hexStringToString(pData->at(i)->qsReadData))
        {
          bDamped = true;
          break;
        }
      }
    }
  }

  if(pEEpromMode == NULL)
  {
    signal_dumpString(tr("*** Mode not found\n"));
    goto cleanup;
  }

  iEnd = pData->count();

  if(!p.parseFile(pEEpromMode->qsReadData + "_" + qsResponse + ".blh", pData, bDamped))
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