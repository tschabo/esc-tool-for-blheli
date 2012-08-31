#include "cabstractserialcomm.h"

#include <QtCore/QTime>

CAbstractSerialComm::CAbstractSerialComm(QObject *parent) :
  QextSerialPort(QextSerialPort::Polling, parent)
{
  setDataBits(DATA_8);
  setBaudRate(BAUD38400);
}

bool CAbstractSerialComm::serialReadBlock(QString qsLength, QString qsAdress, QString &qsRead)
{
  bool bRetVal = false;

  if(!write("br", 2))
  {
    goto cleanup;
  }
  if(!write(qsLength.toUtf8(), 2))
  {
    goto cleanup;
  }
  if(!write(qsAdress.toUtf8(), 4))
  {
    goto cleanup;
  }
  if(!serialRead(qsRead))
  {
    goto cleanup;
  }
  if(qsRead.contains("er"))
  {
    goto cleanup;
  }
  qsRead.remove(QString("br") + qsLength + qsAdress + "ok");
  qsRead.remove("\n>");

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** serialReadBlock(...) failed\n"));
  }
  return bRetVal;
}

bool CAbstractSerialComm::serialRead(QString &qsRead, int iTimeOutSecs)
{
  bool bRetVal = false;
  QString tmp;
  QTime qtSaved;

  qsRead.clear();

  qtSaved = QTime::currentTime();
  while(true)
  {
    if(qtSaved.msecsTo(QTime::currentTime()) > iTimeOutSecs * 1000)
    {
      signal_dumpString(tr("*** read operation timed out"));
      goto cleanup;
    }

    tmp = this->readAll();

    if(tmp.count() == 0)
    {
      continue;
    }

    qsRead.append(tmp);

    // termination character
    if(tmp.at(tmp.count()-1) == '>')
    {
      break;
    }

    tmp.clear();
  }

  signal_dumpString(qsRead);

  bRetVal = true;

cleanup:
  return bRetVal;
}

bool CAbstractSerialComm::serialRead(int iTimeOutSecs)
{
  QString qsDummy;
  return serialRead(qsDummy, iTimeOutSecs);
}

bool CAbstractSerialComm::openProgrammer()
{
  if(open(QIODevice::ReadWrite) == -1)
  {
    signal_dumpString(tr("*** opening the serial connection failed"));
    return false;
  }

  // Wait 2 seconds
  QTime qtTmp = QTime::currentTime().addSecs(2);
  while(QTime::currentTime() < qtTmp);

  return true;
}

bool CAbstractSerialComm::stringHexByteToInt(QString qsByte, int &iInt)
{
  bool bRetVal = false;
  iInt = qsByte.toInt(&bRetVal, 16);
  if(!bRetVal)
  {
    signal_dumpString(tr("*** stringHexByteToInt(...) input string is not capable for converting to integer\n"));
  }
  return bRetVal;
}

bool CAbstractSerialComm::serialWrite(QString qsData, int iStep)
{
  if(qsData.count() % iStep > 0)
  {
    signal_dumpString(tr("*** serialWrite(...) failed\n"));
    return false;
  }
  for(int iCount = 0; iCount < qsData.count(); iCount += iStep)
  {
    if(this->write(qsData.mid(iCount, iStep).toUtf8()) == -1)
    {
      signal_dumpString(tr("*** serialWrite(...) failed\n"));
      return false;
    }
    this->flush();
  }
  return true;
}

bool CAbstractSerialComm::decodeIntelHexDataLine(QString qsHexDataLine, SHexData &sHexData)
{
  bool bRetVal = false;

  int iByteCount = -1;
  bool bOk = false;

  if(qsHexDataLine.at(0) != ':')
  {
    goto cleanup;
  }
  sHexData.qsBytecount = qsHexDataLine.mid(1, 2);

  iByteCount = sHexData.qsBytecount.toInt(&bOk, 16);
  if(!bOk)
  {
    goto cleanup;
  }
  if(sHexData.qsBytecount.isEmpty())
  {
    goto cleanup;
  }
  sHexData.qsAdress = qsHexDataLine.mid(3, 4);
  if(sHexData.qsBytecount.isEmpty())
  {
    goto cleanup;
  }
  sHexData.qsType = qsHexDataLine.mid(7, 2);
  if(sHexData.qsType.isEmpty())
  {
    goto cleanup;
  }
  sHexData.qsData = qsHexDataLine.mid(9, iByteCount * 2);
  if(sHexData.qsData.isEmpty() && iByteCount)
  {
    goto cleanup;
  }
  sHexData.qsCheckSum = qsHexDataLine.mid(9 + iByteCount * 2, 2);
  if(sHexData.qsCheckSum.isEmpty())
  {
    goto cleanup;
  }
  if(qsHexDataLine.mid(11 + iByteCount * 2, 1).count() == 1)
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

bool CAbstractSerialComm::eraseDevice()
{
  bool bRetVal = false;

  QString qsResponse;

  if(this->write("e") == -1)
  {
    goto cleanup;
  }
  if(!serialRead(qsResponse))
  {
    goto cleanup;
  }
  if(qsResponse.contains("er"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** eraseDevice() failed\n"));
  }
  return bRetVal;
}

bool CAbstractSerialComm::erasePage(QString qsPage)
{
  bool bRetVal = false;

  QString qsResponse;

  if(this->write("p") == -1)
  {
    goto cleanup;
  }
  if(this->write(qsPage.toUtf8().data()) == -1)
  {
    goto cleanup;
  }
  if(!serialRead(qsResponse))
  {
    goto cleanup;
  }
  if(qsResponse.contains("er"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** erasePage() failed\n"));
  }
  return bRetVal;
}

bool CAbstractSerialComm::intToStringHexByte(int iInt, QString &qsByte)
{
  qsByte = QString::number(iInt, 16);
  if(qsByte.length() == 1)
  {
    qsByte.prepend("0");
  }
  if(qsByte.length() !=2)
  {
    signal_dumpString(tr("*** intToStringHexByte(...) integer value not siutable for conversion to byte hex string"));
    return false;
  }
  return true;
}

bool CAbstractSerialComm::initProgrammer()
{
  QString qsTmp;
  if(this->write("v") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"r\")\n"));
    return false;
  }
  if(!serialRead(qsTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at r serialRead())\n"));
    return false;
  }
  this->flush();
  if(this->write("r") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"r\")\n"));
    return false;
  }
  this->flush();
  if(!serialRead(qsTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at r serialRead())\n"));
    return false;
  }
  if(qsTmp.contains("er"))
  {
    return false;
  }
  if(this->write("d") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"d\")\n"));
    return false;
  }
  this->flush();
  if(!serialRead(qsTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at d serialRead())\n"));
    return false;
  }
  else
  {
    if(qsTmp.contains("er"))
    {
      return false;
    }
    qsTmp.remove("\n>");
    qsTmp.remove("dok");
    if(qsTmp != "0A")
    {
      signal_dumpString(tr("*** wrong deviceId \"%1\"\n").arg(qsTmp));
      return false;
    }
  }
  if(this->write("i") == -1)
  {
    signal_dumpString(tr("*** init programmer failed (at write(\"i\")\n"));
    return false;
  }
  this->flush();
  if(!serialRead(qsTmp))
  {
    signal_dumpString(tr("*** init programmer failed (at i serialRead()\n"));
    return false;
  }
  return true;
}

bool CAbstractSerialComm::serialWriteBlock(SHexData sHexDataIntel)
{
  bool bRetVal = false;

  QString qsDummy;
  if(!write("bw:", 3))
  {
    goto cleanup;
  }
  if(!serialWrite(sHexDataIntel.qsBytecount, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(sHexDataIntel.qsAdress, 4))
  {
    goto cleanup;
  }
  if(!serialWrite(sHexDataIntel.qsType, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(sHexDataIntel.qsData, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(sHexDataIntel.qsCheckSum, 2))
  {
    goto cleanup;
  }
  if(!serialRead(qsDummy))
  {
    goto cleanup;
  }
  if(qsDummy.contains("er"))
  {
    goto cleanup;
  }

  bRetVal = true;

cleanup:
  if(!bRetVal)
  {
    signal_dumpString(tr("*** serialWriteBlock(...) failed\n"));
  }
  return bRetVal;
}

bool CAbstractSerialComm::serialWriteBlock(int iLength, QString qsAdress, QString qsData)
{
  bool bRetVal = false;

  QString qsDummy;

  QString qsLength = QString::number(iLength, 16).toUpper();

  if(qsData.length() != iLength*2)
  {
    signal_dumpString(tr("*** serialWriteBlock(...) data length is malformed\n"));
    goto cleanup;
  }
  if(qsLength.length() == 1)
  {
    qsLength.prepend("0");
  }
  if(qsLength.length() != 2)
  {
    signal_dumpString(tr("*** serialWriteBlock(...) length is malformed\n"));
    goto cleanup;
  }

  if(!serialWrite("bw", 2))
  {
    goto cleanup;
  }
  if(!serialWrite(qsLength, 2))
  {
    goto cleanup;
  }
  if(!serialWrite(qsAdress, 4))
  {
    goto cleanup;
  }
  if(!serialWrite(qsData, 2))
  {
    goto cleanup;
  }
  if(!serialRead(qsDummy))
  {
    goto cleanup;
  }
  if(qsDummy.contains("er"))
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
