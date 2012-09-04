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

#ifndef CPROGRAMMER_H
#define CPROGRAMMER_H

#include "cserialcommunication.h"


class CProgrammer : public CSerialCommunication
{
  Q_OBJECT
public:
  explicit CProgrammer(QObject *parent = 0);

  /// writes a hex file to the MCU
  /// \param [in] qsIntelHexFile the path to the hex file (intel format)
  bool writeProgramToDevice(QString qsIntelHexFile);

  /// verifies a hex file against the program in the MCU
  /// \param [in] qsIntelHexFile the path to the hex file (intel format) which should verified against
  bool verifyProgramOnDevice(QString qsIntelHexFile);

};

#endif // CPROGRAMMER_H
