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

#ifndef CSERIALCOMMUNICATION_H
#define CSERIALCOMMUNICATION_H

#include "external/qextserialport-1.2beta2/src/qextserialport.h"
#include "common.h"
#include <QObject>

class CSerialCommunication : public QextSerialPort
{
  Q_OBJECT
public:
  explicit CSerialCommunication(QObject *parent = 0);
  
  /// read a block of Data of the device
  /// \param [in] qsLength Length of the Data to read (hex as String 2 digits)
  /// \param [in] qsAdress Start adress of the data to read (hex as String 4 digits)
  /// \param [out] qbaRead Buffer in which the data has to be written to
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialReadBlock(QByteArray qbaLength, QByteArray qbaAdress, QByteArray & qbaRead);

  /// write a block of data to the device (raw)
  /// \param [in] qbaLength Length of the data to write (hex as String)
  /// \param [in] qbaAdress Start Adress of the data to write (hex as String)
  /// \param [in] qbaData Data to write (hex as string)
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialWriteBlock(QByteArray qbaLength, QByteArray qbaAdress, QByteArray qbaData);

  /// write a block of data to the device (intel format)
  /// \param [in] sHexData data to write
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialWriteBlock(mcu::SHexData &hexDataIntel);

  /// Converts a line from an intel hex file to SHexData
  /// \param [in] qbaHexDataLine line to decode (hex as String)
  /// \param [out] sHexData decoded data
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool decodeIntelHexDataLine(QByteArray qbaHexDataLine,mcu::SHexData & sHexData);


  // Lower level functions
  /// opens the serial connection to the programmer and dumps out the Version
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool openProgrammer();

  /// initializes the Programmer
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool initProgrammer();

  /// erases the complete device
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool eraseDevice();

  /// erases one page
  /// \param [in] qsPage page to erase
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool erasePage(QByteArray qbaPage);

  /// read serial data from the device
  /// \param [out] qsRead Buffer in which the data has to be written
  /// \param [in] iTimeOutSecs Sets the timeout.
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  /// \remarks If no data is available after iTimeOutSecs seconds the method will fail
  bool serialRead(QByteArray &qbaRead, int iTimeOutSecs = 5);

  /// read serial data from the device (outputs only to signal_dumpString(...))
  /// \param [in] iTimeOutSecs Sets the timeout.
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  /// \remarks If no data is available after iTimeOutSecs seconds the method will fail.
  bool serialRead(int iTimeOutSecs = 5);

  /// Writes a chunk of data to the serial connection
  /// \param [in] qbaData A chunk of data to write
  /// \param [in] iStep Defines how many bytes are written at once. if iStep == 0 all bytes are written at once
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialWrite(QByteArray qbaData, int iStep = 0);


signals:
  /// Sends out a String used for displaying log- and error- Strings
  /// \param [out] qsMessage Message to send
  void signal_dumpString(QString qsMessage);
};

#endif // CSERIALCOMMUNICATION_H
