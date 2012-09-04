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

#include "cescconf.h"

#include "ceepromdefinitonfileparser.h"
#include <QtCore/QFile>

CEscConf::CEscConf(QObject *parent) :
  CSerialCommunication(parent)
{
}

bool CEscConf::readConfig(QList<esc::SEEpromData *> *pData)
{
  bool bRetVal = false;
  esc::SEEpromData * pTmp;
  CEEpromDefinitonFileParser p;
  connect(&p, SIGNAL(signal_dumpString(QString)), this, SIGNAL(signal_dumpString(QString)));
  QByteArray qbaResponse;
  esc::SEEpromData * pEEpromMode = NULL;
  int iEnd = 0;
  bool bDamped = false;
  QString qsDampedEscs;
  QStringList qslDampedEscs;
  QByteArray qbaVersion;

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
  // 1A00 - Adress of the Version
  if(!serialReadBlock("02", "1A00", qbaResponse))
  {
    goto cleanup;
  }

  {
    pTmp = new esc::SEEpromData;
    pTmp->bReadOnly = true;
    pTmp->qbaSize = "02";
    pTmp->qbaAdress = "1A00";
    pTmp->qsName = "Revision";
    if(qbaResponse.split('k').length() != 2)
    {
      pTmp->qbaReadData  = qbaResponse;
    }
    else
    {
      pTmp->qbaReadData = qbaResponse.split('k').at(1);
      qbaVersion = pTmp->qbaReadData;
    }
    pTmp->eDataType = esc::eVersionNumber;
    pData->prepend(pTmp);
  }


  if(!p.parseFile(QString("common") + "_" + qbaVersion + ".blh", pData))
  {
    goto cleanup;
  }

  for(int i = 0; i < pData->count(); i++)
  {
    if(!serialReadBlock(pData->at(i)->qbaSize, pData->at(i)->qbaAdress, qbaResponse))
    {
      goto cleanup;
    }
    if(qbaResponse.split('k').length() != 2)
    {
      pData->at(i)->qbaReadData  = qbaResponse;
    }
    else
    {
      pData->at(i)->qbaReadData = qbaResponse.split('k').at(1);
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
        if(qslDampedEscs.at(j) == CCommon::hexStringToString(pData->at(i)->qbaReadData))
        {
          bDamped = true;
          break;
        }
      }
    }
  }

  if(pEEpromMode == NULL)
  {
    signal_dumpString(tr("*** Mode not found"));
    goto cleanup;
  }

  iEnd = pData->count();

  if(!p.parseFile(pEEpromMode->qbaReadData + "_" + qbaVersion + ".blh", pData, bDamped))
  {
    goto cleanup;
  }

  for(;iEnd < pData->count(); iEnd++)
  {
    if(!serialReadBlock(pData->at(iEnd)->qbaSize, pData->at(iEnd)->qbaAdress, qbaResponse))
    {
      goto cleanup;
    }
    if(qbaResponse.split('k').length() != 2)
    {
      pData->at(iEnd)->qbaReadData  = qbaResponse;
    }
    else
    {
      pData->at(iEnd)->qbaReadData = qbaResponse.split('k').at(1);
    }
  }



  bRetVal = true;

cleanup:
  close();
  return bRetVal;
}

bool CEscConf::writeConfig(QList<esc::SEEpromData *> *pData)
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
    if(!serialWriteBlock(pData->at(i)->qbaSize,
                         pData->at(i)->qbaAdress,
                         pData->at(i)->qbaReadData))
    {
      goto cleanup;
    }
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** writing config to device failed"));
  }
  else
  {
    signal_dumpString(tr("*** success"));
  }
  close();
  return bRetVal;
}
