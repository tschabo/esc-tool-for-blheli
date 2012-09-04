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

#ifndef CEEPROMDEFINITONFILEPARSER_H
#define CEEPROMDEFINITONFILEPARSER_H

#include <QObject>
#include "common.h"

class CEEpromDefinitonFileParser : public QObject
{
  Q_OBJECT

public:
  explicit CEEpromDefinitonFileParser(QObject *parent = 0);
  
  bool parseDoc(QString & qsDoc, QList<esc::SEEpromData*> *qlData, bool bDampedModeAvailable = false);
  bool parseFile(QString qsFile, QList<esc::SEEpromData*> *qlData, bool bDampedModeAvailable = false);

private:
  void extractChoices(QString qsChoices, QStringList & qslChoices, bool bDampedModeAvailable);

  enum EDefFileColumns
  {
    eName = 0,
    eAdress = 1,
    eSize = 2,
    eDefaultValue = 3,
    eChoices = 4,
    eReadonly = 5,
    eDataType = 6
  };

signals:
  void signal_dumpString(QString qsMessage);
};

#endif // CEEPROMDEFINITONFILEPARSER_H
