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

#ifndef COMMON_H
#define COMMON_H

#include <QtCore/QStringList>

namespace mcu
{
  // Structure for holding data
  // which was read from or
  // which will be written to the programmer
  struct SHexData
  {
    QByteArray qbaBytecount;
    QByteArray qbaAdress;
    QByteArray qbaType;
    QByteArray qbaData;
    QByteArray qbaCheckSum;
  };
}


namespace esc
{
  enum EDataType
  {
    eNumber = 0,
    eString = 1,
    eVersionNumber = 2,
    ePercent = 3,
    eMode = 4,
    eRaw = 5
  };

  // structure which holds conifg data of the esc
  struct SEEpromData
  {
    QString qsName;
    QByteArray qbaAdress;
    QByteArray qbaSize;
    QByteArray qbaDefaultValue;
    QStringList qslChoices;
    esc::EDataType eDataType;
    bool bReadOnly;

    QByteArray qbaReadData;
  };
}

class CCommon
{
public:

  /// Converts a string representation of a hex encoded byte to an integer
  /// \param [in] qbaByte Byte to convert (hex as String 2 digits)
  /// \param [out] iInt converted value
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  static bool stringHexByteToInt(QByteArray qsByte, int & iInt)
  {
    if(qsByte.length() != 2)
    {
      return false;
    }
    bool bRetVal = false;
    iInt = qsByte.toInt(&bRetVal, 16);
    return bRetVal;
  }

  /// Converts an integer to a string representation of a hex encoded byte
  /// \param [in] iInt integer to convert
  /// \param [out] qbaByte converted value (hex as String 2 digits)
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  static bool intToStringHexByte(int iInt, QByteArray & qbaByte)
  {
    qbaByte = QByteArray::number(iInt, 16);
    if(qbaByte.length() == 1)
    {
      qbaByte.prepend("0");
    }
    if(qbaByte.length() !=2)
    {
      return false;
    }
    return true;
  }

  /// decodes a choice (used for SEEpromData)
  /// \param [in] qsChoice the choice to decode
  /// \param [out] qbaEEpromValue the eeprom data value
  /// \param [out] qsReadableValue the string representation of the value
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  /// \remarks delimiter is '~' example for a choice: "01~3.0 V/cell"  - 01 is the hex value of the eeprom Value
  static bool decodeChoice(QString qsChoice, QByteArray &qbaEEpromValue, QString &qsReadableValue)
  {
    QStringList qslTmp = qsChoice.split('~');
    if(qslTmp.length() != 2)
    {
      qbaEEpromValue = "XX";
      qsReadableValue = "ERROR";
      return false;
    }
    if(qslTmp.at(0).length() !=2)
    {
      qbaEEpromValue = "XX";
      qsReadableValue = "ERROR";
      return false;
    }
    qbaEEpromValue = qslTmp.at(0).toAscii();
    qsReadableValue = qslTmp.at(1);
    return true;
  }

  static QString hexStringToString(QString qsHexString)
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

  static QByteArray stringToHexByteArray(QString qsString)
  {

    QByteArray qbaTmp, qbaRet;
    int iTmp = 0;
    for(int i = 0; i < qsString.toAscii().length(); i++)
    {
      iTmp = (int) qsString.toAscii().at(i);
      qbaTmp = QByteArray::number(iTmp, 16);
      if(qbaTmp.length() == 1)
      {
        qbaTmp.prepend('0');
      }
      qbaRet.append(qbaTmp);
    }
    return qbaRet;
  }

private:
  CCommon(){}
  ~CCommon(){}
};

#endif // COMMON_H
