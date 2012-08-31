#ifndef CABSTRACTSERIALCOMM_H
#define CABSTRACTSERIALCOMM_H

#include "qextserialport.h"

struct SHexData
{
  QString qsBytecount;
  QString qsAdress;
  QString qsType;
  QString qsData;
  QString qsCheckSum;
};

class CAbstractSerialComm : public QextSerialPort
{
  Q_OBJECT
public:
  explicit CAbstractSerialComm(QObject *parent = 0);

protected:
  /// read a block of Data of the device
  /// \param [in] qsLength Length of the Data to read as one byte hex String
  /// \param [in] qsAdress Start adress of the data to read
  /// \param [out] qsRead Buffer in which the data has to be written to
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialReadBlock(QString qsLength, QString qsAdress, QString & qsRead);

  /// write a block of data to the device (raw)
  /// \param [in] iLength Length of the data to write
  /// \param [in] qsAdress Start Adress of the data to write
  /// \param [in] qsData Data to write
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialWriteBlock(int iLength, QString qsAdress, QString qsData);

  /// write a block of data to the device (intel format)
  /// \param [in] sHexData data to write
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialWriteBlock(SHexData sHexDataIntel);

  /// initializes the Programmer
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool initProgrammer();

  /// Converts a string representation of a hex encoded byte to an integer
  /// \param [in] qsByte Byte to convert
  /// \param [out] iInt converted value
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool stringHexByteToInt(QString qsByte, int & iInt);

  /// Converts an integer to a string representation of a hex encoded byte
  /// \param [in] iInt integer to convert
  /// \param [out] qsString converted value
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool intToStringHexByte(int iInt, QString & qsByte);

  /// Converts a line from an intel hex file to SHexData
  /// \param [in] qsHexDataLine line to decode
  /// \param [out] sHexData decoded data
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool decodeIntelHexDataLine(QString qsHexDataLine, SHexData & sHexData);

  /// erases the complete device
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool eraseDevice();

  /// erases one page
  /// \param [in] qsPage page to erase
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool erasePage(QString qsPage);

  // explicit serial functions

  /// Writes a chunk of data to the serial connection
  /// \param [in] qsData A chunk of data to write
  /// \param [in] iStep Defines how many bytes are written at once
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool serialWrite(QString qsData, int iStep);

  /// read serial data from the device
  /// \param [out] qsRead Buffer in which the data has to be written
  /// \param [in] iTimeOutSecs Sets the timeout.
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  /// \remarks If no data is available after iTimeOutSecs seconds the method will fail
  bool serialRead(QString & qsRead, int iTimeOutSecs = 5);

  /// read serial data from the device (outputs only to signal_dumpString(...))
  /// \param [in] iTimeOutSecs Sets the timeout.
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  /// \remarks If no data is available after iTimeOutSecs seconds the method will fail.
  bool serialRead(int iTimeOutSecs = 5);

  /// opens the serial connection to the programmer and dumps out the Version
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool openProgrammer();

signals:
  void signal_dumpString(QString qsMessage);
  
public slots:

private:
  
};

#endif // CABSTRACTSERIALCOMM_H
