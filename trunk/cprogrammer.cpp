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

#include "cprogrammer.h"

#include <QtCore/QFile>

CProgrammer::CProgrammer(QObject *parent) :
  CSerialCommunication(parent)
{
}

bool CProgrammer::writeProgramToDevice(QString qsIntelHexFile)
{
  bool bRetVal = false;

  QList<QByteArray> qlFileContents;
  mcu::SHexData convertedData;
  QByteArray qbaFileContents;

  QFile f(qsIntelHexFile);
  if(!f.exists())
  {
    signal_dumpString(tr("*** file \"%1\" does not exist").arg(qsIntelHexFile));
    goto cleanup;
  }
  if(!f.open(QIODevice::ReadOnly))
  {
    signal_dumpString(tr("*** failed to open file \"%1\"").arg(qsIntelHexFile));
    goto cleanup;
  }
  qbaFileContents = f.readAll();
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
  qlFileContents = qbaFileContents.split('\n');
  for(int i = 0; i < qlFileContents.count(); i++)
  {
    // end signature
    if(qlFileContents.at(i) == ":00000001FF")
    {
      break;
    }
    if(!decodeIntelHexDataLine(qlFileContents.at(i), convertedData))
    {
      goto cleanup;
    }
    if(!serialWriteBlock(convertedData))
    {
      goto cleanup;
    }
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** writing the file to the device failed\n"));
  }
  else
  {
    signal_dumpString(tr("success"));
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
  QByteArray qbaFileContents = f.readAll();
  QList<QByteArray> qlFileContents = qbaFileContents.split('\n');
  mcu::SHexData dataDecoded;
  QByteArray qbaResponse;
  if(!openProgrammer())
  {
    goto cleanup;
  }
  if(!initProgrammer())
  {
    goto cleanup;
  }
  for(int i = 0; i < qlFileContents.count(); i++)
  {
    if(!decodeIntelHexDataLine(qlFileContents.at(i), dataDecoded))
    {
      goto cleanup;
    }
    if(dataDecoded.qbaData == "")
    {
      break;
    }
    if(!serialReadBlock(dataDecoded.qbaBytecount, dataDecoded.qbaAdress, qbaResponse))
    {
      goto cleanup;
    }
    // the k from ok
    // at this point there has to be a ok because serialBlockRead(..)
    // returned true
    QList<QByteArray> lqbaTmp = qbaResponse.split('k');
    if(lqbaTmp.count() != 2)
    {
      goto cleanup;
    }
    if(lqbaTmp.at(1) != dataDecoded.qbaData)
    {
      goto cleanup;
    }
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** verifiying failed"));
  }
  else
  {
    signal_dumpString(tr("success"));
  }
  close();
  return bRetVal;
}
