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

#ifndef CESCCONF_H
#define CESCCONF_H

#include "cserialcommunication.h"

class CEscConf : public CSerialCommunication
{
  Q_OBJECT
public:
  explicit CEscConf(QObject *parent = 0);
  
  /// read the configuration from the esc
  /// \param pData [out] read data
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool readConfig(QList<esc::SEEpromData*> * pData);

  /// write the configuration to the esc
  /// \param pData [in] data to write
  /// \returns true if the method was successful
  /// \returns true if the method was not successful
  bool writeConfig(QList<esc::SEEpromData*> * pData);
};

#endif // CESCCONF_H
