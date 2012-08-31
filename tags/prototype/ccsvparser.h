#ifndef CCSVPARSER_H
#define CCSVPARSER_H

#include <QObject>
#include <QList>
#include <QStringList>

enum eEEpromDataDataType
{
  eName = 0,
  eAdress = 1,
  eSize = 2,
  eDefaultValue = 3,
  eChoices = 4,
  eReadonly = 5,
  eMinVal = 6
};

struct sEEpromData
{
  QString qsName;
  QString qsAdress;
  int iSize;
  QString qsDefaultVal;
  QStringList qslChoices;
  bool bReadOnly;
  int iMinVal;

  QString qsReadData;
};

class CCsvParser : public QObject
{
  Q_OBJECT
public:
  explicit CCsvParser(QObject *parent = 0);

  bool parseDoc(QString & qsDoc, QList<sEEpromData*> *qlData, bool bDampedModeAvailable = false);
  bool parseFile(QString qsFile, QList<sEEpromData *> *qlData, bool bDampedModeAvailable = false);
  
private:
  void extractChoices(QString qsChoices, QStringList & qslChoices, bool bDampedModeAvailable);

signals:
  void signal_dumpString(QString qsMessage);
  
public slots:
  
};

#endif // CCSVPARSER_H
