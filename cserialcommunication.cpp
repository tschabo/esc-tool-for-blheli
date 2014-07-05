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

#include "cserialcommunication.h"
#include <QtCore/QTime>

CSerialCommunication::CSerialCommunication(QObject *parent) :
  QextSerialPort(QextSerialPort::Polling, parent)
{
  setBaudRate(BAUD38400);
}

bool CSerialCommunication::serialReadBlock(QByteArray qbaLength, QByteArray qbaAdress, QByteArray &qbaRead)
{
  bool bRetVal = false;

  QString qsRead;

  if(qbaLength.length() != 2)
  {
    signal_dumpString(tr("*** serialReadBlock(...) wrong parameter value"));
    goto cleanup;
  }
  if(qbaAdress.length() != 4)
  {
    signal_dumpString(tr("*** serialReadBlock(...) wrong parameter value"));
    goto cleanup;
  }

  // br means block read
  if(!write("br", 2))
  {
    goto cleanup;
  }
  if(!write(qbaLength, 2))
  {
    goto cleanup;
  }
  if(!write(qbaAdress, 4))
  {
    goto cleanup;
  }
  if(!serialRead(qbaRead))
  {
    goto cleanup;
  }
  if(qbaRead.contains("er"))
  {
    goto cleanup;
  }

  qsRead = qbaRead;
  qsRead.remove(QString("br") + QString(qbaLength) + QString(qbaAdress) + "ok");
  qsRead = QString(qbaRead).remove("\n>");

  qbaRead = qsRead.toLocal8Bit();

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** serialReadBlock(...) failed\n"));
  }
  return bRetVal;
}

bool CSerialCommunication::serialWriteBlock(QByteArray qbaLength, QByteArray qbaAdress, QByteArray qbaData)
{
  bool bRetVal = false;

  QByteArray qbaResponse;

  if(qbaLength.length() != 2)
  {
    signal_dumpString(tr("*** serialWriteBlock(...) length is malformed"));
    goto cleanup;
  }
  if(qbaAdress.length() != 4)
  {
    signal_dumpString(tr("*** serialWriteBlock(...) adress is malformed"));
    goto cleanup;
  }

  // bw means block write
  if(!serialWrite("bw", 2))
  {
    goto cleanup;
  }
  if(!serialWrite(qbaLength, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(qbaAdress, 4))
  {
    goto cleanup;
  }
  if(!serialWrite(qbaData, 2))
  {
    goto cleanup;
  }
  if(!serialRead(qbaResponse))
  {
    goto cleanup;
  }
  if(!qbaResponse.contains("ok"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** Writing configuration failed\n"));
  }
  return bRetVal;
}

bool CSerialCommunication::openProgrammer()
{
  if(open(QIODevice::ReadWrite) == -1)
  {
    signal_dumpString(tr("*** opening the serial connection failed"));
    return false;
  }

  // Wait 2 seconds
  // needed since owSilProg V008 firmware
  QTime qtTmp = QTime::currentTime().addSecs(2);
  while(QTime::currentTime() < qtTmp);

  return true;
}

bool CSerialCommunication::initProgrammer()
{
  QByteArray qbaTmp;

  // v means print firmware version
  if(this->write("v") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"r\")"));
    return false;
  }
  if(!serialRead(qbaTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at r serialRead())"));
    return false;
  }
  this->flush();

  // r means reset programmer
  if(this->write("r") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"r\")"));
    return false;
  }
  this->flush();
  if(!serialRead(qbaTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at r serialRead())"));
    return false;
  }
  if(!qbaTmp.contains("ok"))
  {
    return false;
  }

  // d means get device id
  if(this->write("d") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"d\")"));
    return false;
  }
  this->flush();
  if(!serialRead(qbaTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at d serialRead())"));
    return false;
  }
  else
  {
    if(!qbaTmp.contains("ok"))
    {
      return false;
    }
    QString qsTmp(qbaTmp);
    qsTmp.remove("\n>");
    qsTmp.remove("dok");

    // check device id
    if(qsTmp != "0A")
    {
      signal_dumpString(tr("*** wrong deviceId \"%1\"\n").arg(qsTmp));
      return false;
    }
  }

  // i means initialize programmer
  if(this->write("i") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"i\")\n"));
    return false;
  }
  this->flush();
  if(!serialRead(qbaTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at i serialRead()\n"));
    return false;
  }
  if(!qbaTmp.contains("ok"))
  {
    signal_dumpString(tr("*** init programmer failed (at i serialRead())"));
    return false;
  }
  return true;
}

bool CSerialCommunication::eraseDevice()
{
  bool bRetVal = false;

  QByteArray qbaResponse;

  // e means erase device
  if(this->write("e") == -1)
  {
    goto cleanup;
  }
  if(!serialRead(qbaResponse))
  {
    goto cleanup;
  }
  if(!qbaResponse.contains("ok"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** eraseDevice() failed"));
  }
  return bRetVal;
}

bool CSerialCommunication::erasePage(QByteArray qbaPage)
{

  bool bRetVal = false;

  QByteArray qbaResponse;

  // page number is always one byte (as String 2 digits)
  if(qbaPage.length() != 2)
  {
    signal_dumpString(tr("*** erasePage(...) wrong parameter value"));
    goto cleanup;
  }

  // p means erase page
  if(this->write("p") == -1)
  {
    goto cleanup;
  }
  if(this->write(qbaPage) == -1)
  {
    goto cleanup;
  }
  if(!serialRead(qbaResponse))
  {
    goto cleanup;
  }
  if(!qbaResponse.contains("ok"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** erasePage() failed"));
  }
  return bRetVal;
}

bool CSerialCommunication::serialRead(QByteArray &qbaRead, int iTimeOutSecs)
{
  bool bRetVal = false;
  QByteArray qbaTmp;
  QTime qtSaved;

  qbaRead.clear();

  // get the current time for calculating the timeout
  qtSaved = QTime::currentTime();
  while(true)
  {
    // timeout
    if(qtSaved.msecsTo(QTime::currentTime()) > iTimeOutSecs * 1000)
    {
      signal_dumpString(tr("*** read operation timed out"));
      goto cleanup;
    }

    qbaTmp = this->readAll();

    // nothing read - next iteration
    if(qbaTmp.length() == 0)
    {
      continue;
    }

    qbaRead.append(qbaTmp);

    // cancel the loop
    // if the String ends with '>' termination character
    if(qbaTmp.at(qbaTmp.count()-1) == '>')
    {
      break;
    }

    qbaTmp.clear();
  }

  signal_dumpString(QString(qbaRead).remove("\n>"));

  bRetVal = true;

cleanup:
  return bRetVal;
}

bool CSerialCommunication::serialRead(int iTimeOutSecs)
{
  QByteArray qbaDummy;
  return serialRead(qbaDummy, iTimeOutSecs);
}

bool CSerialCommunication::serialWrite(QByteArray qbaData, int iStep)
{
  if(qbaData.length() % iStep > 0)
  {
    signal_dumpString(tr("*** serialWrite(...) failed\n"));
    return false;
  }
  for(int iCount = 0; iCount < qbaData.length(); iCount += iStep)
  {
    if(this->write(qbaData.mid(iCount, iStep)) == -1)
    {
      signal_dumpString(tr("*** serialWrite(...) failed\n"));
      return false;
    }
    this->flush();
  }
  return true;
}

bool CSerialCommunication::serialWriteBlock(mcu::SHexData &hexDataIntel)
{
  bool bRetVal = false;

  QByteArray qbaDummy;

  // bw: means block write (intel format)
  if(!write("bw:", 3))
  {
    goto cleanup;
  }
  if(!serialWrite(hexDataIntel.qbaBytecount, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(hexDataIntel.qbaAdress, 4))
  {
    goto cleanup;
  }
  if(!serialWrite(hexDataIntel.qbaType, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(hexDataIntel.qbaData, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(hexDataIntel.qbaCheckSum, 2))
  {
    goto cleanup;
  }
  if(!serialRead(qbaDummy))
  {
    goto cleanup;
  }
  if(!qbaDummy.contains("ok"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** serialWriteBlock(...) failed"));
  }
  return bRetVal;
}

bool CSerialCommunication::decodeIntelHexDataLine(QByteArray qbaHexDataLine, mcu::SHexData &sHexData)
{
  bool bRetVal = false;

  int iByteCount = -1;
  bool bOk = false;

  if(qbaHexDataLine.at(0) != ':')
  {
    goto cleanup;
  }
  sHexData.qbaBytecount = qbaHexDataLine.mid(1, 2);

  iByteCount = sHexData.qbaBytecount.toInt(&bOk, 16);
  if(!bOk)
  {
    goto cleanup;
  }
  if(sHexData.qbaBytecount.isEmpty())
  {
    goto cleanup;
  }
  sHexData.qbaAdress = qbaHexDataLine.mid(3, 4);
  if(sHexData.qbaBytecount.isEmpty())
  {
    goto cleanup;
  }
  sHexData.qbaType = qbaHexDataLine.mid(7, 2);
  if(sHexData.qbaType.isEmpty())
  {
    goto cleanup;
  }
  sHexData.qbaData = qbaHexDataLine.mid(9, iByteCount * 2);
  if(sHexData.qbaData.isEmpty() && iByteCount)
  {
    goto cleanup;
  }
  sHexData.qbaCheckSum = qbaHexDataLine.mid(9 + iByteCount * 2, 2);
  if(sHexData.qbaCheckSum.isEmpty())
  {
    goto cleanup;
  }

  // check if line is longer as calculated
  if(qbaHexDataLine.mid(11 + iByteCount * 2, 1).count() == 1)
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** decodeIntelHexDataLine(...) wrong format of input string\n"));
  }
  return bRetVal;
}
