#include "cprogrammer.h"

#include <QFile>
#include <QStringList>

CProgrammer::CProgrammer(QObject * parent):
  CAbstractSerialComm(parent)
{
}

bool CProgrammer::writeProgramToDevice(QString qsIntelHexFile)
{
  bool bRetVal = false;

  QStringList qslFileContents;
  SHexData convertedData;
  QString qsFileContents;

  QFile f(qsIntelHexFile);
  if(!f.exists())
  {
    signal_dumpString(tr("*** file \"%1\" does not exist\n").arg(qsIntelHexFile));
    goto cleanup;
  }
  if(!f.open(QIODevice::ReadOnly))
  {
    signal_dumpString(tr("*** failed to open file \"%1\"\n").arg(qsIntelHexFile));
    goto cleanup;
  }
  qsFileContents = f.readAll();
  if(!openProgrammer())
  {
    goto cleanup;
  }
  if(!initProgrammer())
  {
    goto cleanup;
  }
  if(!eraseDevice())
  {
    goto cleanup;
  }
  qslFileContents = qsFileContents.split('\n', QString::SkipEmptyParts);
  for(int i = 0; i < qslFileContents.count(); i++)
  {
    if(qslFileContents.at(i) == ":00000001FF")
    {
      if((qslFileContents.count()-1) == i)
      {
        bRetVal = true;
      }
      goto cleanup;
    }
    if(!decodeIntelHexDataLine(qslFileContents.at(i), convertedData))
    {
      goto cleanup;
    }
    if(!serialWriteBlock(convertedData))
    {
      goto cleanup;
    }
  }

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** writing the file to the device failed\n"));
  }
  else
  {
    signal_dumpString(tr("success\n"));
  }
  close();
  return bRetVal;
}

bool CProgrammer::verifyProgramOnDevice(QString qsIntelHexFile)
{
  bool bRetVal = false;
  QFile f(qsIntelHexFile);
  if(!f.exists())
  {
    signal_dumpString(tr("*** file \"%1\" does not exist\n").arg(qsIntelHexFile));
    return false;
  }
  if(!f.open(QIODevice::ReadOnly))
  {
    signal_dumpString(tr("*** opening\"%1\" failed\n").arg(qsIntelHexFile));
  }
  QString qsFileContents = f.readAll();
  QStringList qslFileContents = qsFileContents.split("\n", QString::SkipEmptyParts);
  SHexData dataDecoded;
  QString qsResponse;
  if(!openProgrammer())
  {
    goto cleanup;
  }
  if(!initProgrammer())
  {
    goto cleanup;
  }
  for(int i = 0; i < qslFileContents.count(); i++)
  {
    if(!decodeIntelHexDataLine(qslFileContents.at(i), dataDecoded))
    {
      goto cleanup;
    }
    signal_dumpString(QString("DATA: " + qsResponse + "\n"));
    if(!serialReadBlock(dataDecoded.qsBytecount, dataDecoded.qsAdress, qsResponse))
    {
      goto cleanup;
    }
    if(qsResponse != dataDecoded.qsData)
    {
      goto cleanup;
    }
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** verifiying failed}n"));
  }
  else
  {
    signal_dumpString(tr("success\n"));
  }
  close();
  return bRetVal;

}
