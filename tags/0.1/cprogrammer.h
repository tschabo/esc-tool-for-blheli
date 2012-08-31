#ifndef CPROGRAMMER_H
#define CPROGRAMMER_H

#include "cabstractserialcomm.h"

class CProgrammer : public CAbstractSerialComm
{
  Q_OBJECT

public:
  CProgrammer(QObject * parent);

  bool writeProgramToDevice(QString qsIntelHexFile);
  bool verifyProgramOnDevice(QString qsIntelHexFile);

};

#endif // CPROGRAMMER_H
