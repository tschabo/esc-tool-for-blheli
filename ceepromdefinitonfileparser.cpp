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

#include "ceepromdefinitonfileparser.h"

#include <QtCore/QFile>

CEEpromDefinitonFileParser::CEEpromDefinitonFileParser(QObject *parent) :
  QObject(parent)
{
}

bool CEEpromDefinitonFileParser::parseDoc(QString &qsDoc, QList<esc::SEEpromData*> *qlData, bool bDampedModeAvailable)
{
  bool bRetVal = false;

  esc::SEEpromData * pTmpEEpromData = NULL;

  QStringList qslDoc = qsDoc.split("\n", QString::SkipEmptyParts);

  QStringList qslTmpPart;

  bool bTmpOk;

  for(int i = 0; i < qslDoc.count(); i++)
  {
    pTmpEEpromData = new esc::SEEpromData;
    qslTmpPart = qslDoc.at(i).split(";");
    if(qslTmpPart.count() != 7)
    {
      signal_dumpString(tr("*** csv parser: wrong format"));
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
          pTmpEEpromData->qbaDefaultValue = qslTmpPart.at(j).toLocal8Bit();
          break;
        case eSize:
          pTmpEEpromData->qbaSize = qslTmpPart.at(j).toLocal8Bit();
          if(!bTmpOk)
          {
            signal_dumpString(tr("*** csv parser: size malformed"));
            goto cleanup;
          }
          if(pTmpEEpromData->qbaSize.length() == 1)
          {
            pTmpEEpromData->qbaSize.prepend('0');
          }
          break;
        case eChoices:
          extractChoices(qslTmpPart.at(j), pTmpEEpromData->qslChoices, bDampedModeAvailable);
          break;
        case eAdress:
          if(qslTmpPart.at(j).length() != 4)
          {
            signal_dumpString(tr("*** csv parser: malformed adress"));
            goto cleanup;
          }
          pTmpEEpromData->qbaAdress = qslTmpPart.at(j).toLocal8Bit();
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
            signal_dumpString(tr("*** csv parser: readonly flag malformed"));
            goto cleanup;
          }
          break;
        case eDataType:
          switch(qslTmpPart.at(j).toInt(&bTmpOk, 10))
          {
            case esc::eNumber:
            case esc::eString:
            case esc::eVersionNumber:
            case esc::ePercent:
            case esc::eRaw:
            case esc::eMode:
              pTmpEEpromData->eDataType = (esc::EDataType) qslTmpPart.at(j).toInt();
              break;
            default:
              signal_dumpString(tr("*** csv parser: datatype malformed"));
              goto cleanup;
          }
          if(!bTmpOk)
          {
            signal_dumpString(tr("*** csv parser: datatype malformed"));
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
    signal_dumpString(tr("*** parsing csv document failed"));
    if(pTmpEEpromData)
    {
      delete pTmpEEpromData;
    }
//    for(int i = 0; i < qlData->count(); i++)
//    {
//      delete qlData->at(i);
//    }
  }
  return bRetVal;
}

bool CEEpromDefinitonFileParser::parseFile(QString qsFile, QList<esc::SEEpromData *> *qlData, bool bDampedModeAvailable)
{
  bool bRetVal = false;

  QString qsRead;

  QFile f(qsFile);
  if(!f.exists())
  {
    signal_dumpString(tr("*** csv parser: file \"%1\" does not exist").arg(qsFile));
    goto cleanup;
  }
  if(!f.open(QIODevice::ReadOnly))
  {
    signal_dumpString(tr("*** csv parser: could not open file \"%1\"").arg(qsFile));
    goto cleanup;
  }

  qsRead = f.readAll();

  if(!parseDoc(qsRead, qlData, bDampedModeAvailable))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  f.close();
  return bRetVal;
}

void CEEpromDefinitonFileParser::extractChoices(QString qsChoices, QStringList &qslChoices, bool bDampedModeAvailable)
{
  // percental
  char tmpByte[3];
  if(qsChoices.startsWith("%"))
  {
    for(int i = 0; i < 256; i++)
    {
      sprintf(tmpByte, "%02X", i);
      qslChoices.append(QString(tmpByte) + "~" + QString::number(((100.0 * (double) i) / 255.0)).append("%"));
    }
    return;
  }

  qslChoices = qsChoices.split("|");

  for(int i = qslChoices.count() - 1; i >= 0; i--)
  {
    if(qslChoices.at(i).startsWith('~'))
    {
      if(!bDampedModeAvailable)
      {
        qslChoices.removeAt(i);
      }
      else
      {
        qslChoices[i].remove('~');
      }
    }
  }
}
