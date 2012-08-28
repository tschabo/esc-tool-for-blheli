#include "ccsvparser.h"

#include <Qt/qfile.h>

CCsvParser::CCsvParser(QObject *parent) :
  QObject(parent)
{
}

bool CCsvParser::parseDoc(QString &qsDoc, QList<sEEpromData *> *qlData)
{
  bool bRetVal = false;

  sEEpromData * pTmpEEpromData = NULL;

  QStringList qslDoc = qsDoc.split("\n", QString::SkipEmptyParts);

  QStringList qslTmpPart;

  bool bTmpOk;

  for(int i = 0; i < qslDoc.count(); i++)
  {
    pTmpEEpromData = new sEEpromData;
    qslTmpPart = qslDoc.at(i).split(";");
    if(qslTmpPart.count() != 7)
    {
      signal_dumpString(tr("*** csv parser: wrong format\n"));
      goto cleanup;
    }
    for(int j = 0; j < qslTmpPart.count(); j++)
    {
      bTmpOk = true;
      switch(j)
      {
        case eName:
          pTmpEEpromData->qsName = qslTmpPart.at(j);
          break;
        case eDefaultValue:
          pTmpEEpromData->qsDefaultVal = qslTmpPart.at(j);
          break;
        case eSize:
          pTmpEEpromData->iSize = qslTmpPart.at(j).toInt(&bTmpOk, 10);
          if(!bTmpOk)
          {
            signal_dumpString(tr("*** csv parser: size malformed\n"));
            goto cleanup;
          }
          break;
        case eChoices:
          extractChoices(qslTmpPart.at(j), pTmpEEpromData->qslChoices);
          break;
        case eAdress:
          if(qslTmpPart.at(j).length() != 4)
          {
            signal_dumpString(tr("*** csv parser: malformed adress\n"));
            goto cleanup;
          }
          pTmpEEpromData->qsAdress = qslTmpPart.at(j);
          break;
        case eReadonly:
          if(qslTmpPart.at(j).toInt(&bTmpOk, 10) == 1)
          {
            pTmpEEpromData->bReadOnly = true;
          }
          if(qslTmpPart.at(j).toInt(&bTmpOk, 10) == 0)
          {
            pTmpEEpromData->bReadOnly = false;
          }
          if(!bTmpOk)
          {
            signal_dumpString(tr("*** csv parser: readonly flag malformed\n"));
            goto cleanup;
          }
          break;
        case eMinVal:
          pTmpEEpromData->iMinVal = qslTmpPart.at(j).toInt(&bTmpOk, 10);
          if(!bTmpOk)
          {
            signal_dumpString(tr("*** csv parser: min value malformed\n"));
            goto cleanup;
          }
          break;
        default:
          // should never be reached
          goto cleanup;
      }
    }
    qlData->append(pTmpEEpromData);
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** parsing csv document failed\n"));
    if(pTmpEEpromData)
    {
      delete pTmpEEpromData;
    }
    for(int i = 0; i < qlData->count(); i++)
    {
      delete qlData->at(i);
    }
  }
  return bRetVal;
}

bool CCsvParser::parseFile(QString qsFile, QList<sEEpromData *> * qlData)
{
  bool bRetVal = false;

  QString qsRead;

  QFile f(qsFile);
  if(!f.exists())
  {
    signal_dumpString(tr("*** csv parser: file \"%1\" does not exist\n").arg(qsFile));
    goto cleanup;
  }
  if(!f.open(QIODevice::ReadOnly))
  {
    signal_dumpString(tr("*** csv parser: could not open file \"%1\"\n").arg(qsFile));
    goto cleanup;
  }

  qsRead = f.readAll();

  if(!parseDoc(qsRead, qlData))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  f.close();
  return bRetVal;

}

void CCsvParser::extractChoices(QString qsChoices, QStringList &qslChoices)
{
  // percental
  if(qsChoices.startsWith("%"))
  {
    for(int i = 0; i < 256; i++)
    {
      qslChoices.append(QString::number(((100.0 * (double) i) / 255.0)).append("%"));
    }
    return;
  }

  qslChoices = qsChoices.split("|");
}
