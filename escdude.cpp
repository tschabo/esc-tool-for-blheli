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

#include "escdude.h"
#include "ui_escdude.h"

#include <QFileDialog>
#include "cprogrammer.h"
#include "cescconf.h"
#include "qextserialenumerator.h"
#include "cconfigmodel.h"
#include "ceditordelegate.h"

escdude::escdude(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::escdude),
  c_pProgrammer(new CProgrammer(parent)),
  c_pConfig(new CEscConf(parent)),
  c_pConfigModel(new CConfigModel(parent)),
  c_pEditorDelegate(new CEditorDelegate(parent))
{
  ui->setupUi(this);

  ui->c_ptvTable->setModel(c_pConfigModel);
  ui->c_ptvTable->setItemDelegate(c_pEditorDelegate);
  ui->c_ptvTable->horizontalHeader()->setStretchLastSection(true);

  foreach(QextPortInfo port, QextSerialEnumerator::getPorts())
    {
  #if defined(Q_OS_WIN)
      ui->c_pcbPort->insertItem(0, port.portName);
  #endif
  #if defined(Q_OS_UNIX)
      ui->c_pcbPort->insertItem(0, port.physName);
  #endif
    }
    ui->c_pcbPort->setCurrentIndex(0);

    connect(this->c_pProgrammer, SIGNAL(signal_dumpString(QString)), this, SLOT(slot_dumpString(QString)));
    connect(ui->c_ppbChooseFile, SIGNAL(clicked()), this, SLOT(slot_chooseHexFile()));
    connect(ui->c_ppbUpload, SIGNAL(clicked()), this, SLOT(slot_writeFlash()));
    connect(ui->c_ppbVerify, SIGNAL(clicked()), this, SLOT(slot_verifyFlash()));
    connect(this->c_pConfig, SIGNAL(signal_dumpString(QString)), this, SLOT(slot_dumpString(QString)));
    connect(ui->c_ppbReadConf, SIGNAL(clicked()), this, SLOT(slot_readConfig()));
    connect(ui->c_ppbWriteConf, SIGNAL(clicked()), this, SLOT(slot_writeConfig()));
}

escdude::~escdude()
{
  delete ui;
}

void escdude::slot_chooseHexFile()
{
  QFileInfo f(ui->c_pleFile->text());
  QString qsFile = QFileDialog::getOpenFileName(this,"",f.absoluteDir().absolutePath());
  if(qsFile.isEmpty())
  {
    return;
  }
  ui->c_pleFile->setText(qsFile);
}

void escdude::slot_writeFlash()
{
  ui->c_ptbDisplay->clear();
  c_pProgrammer->setPortName(ui->c_pcbPort->currentText());
  c_pProgrammer->writeProgramToDevice(ui->c_pleFile->text());
}

void escdude::slot_verifyFlash()
{
  ui->c_ptbDisplay->clear();
  c_pProgrammer->setPortName(ui->c_pcbPort->currentText());
  c_pProgrammer->verifyProgramOnDevice(ui->c_pleFile->text());
}

void escdude::slot_readConfig()
{
  ui->c_ptbDisplay->clear();
  c_pConfig->setPortName(ui->c_pcbPort->currentText());
  if(!c_configList.isEmpty())
  {
    qDeleteAll(c_configList);
    c_configList.clear();
    c_pConfigModel->initSetData(&c_configList);
  }
  c_pConfig->readConfig(&c_configList);
  c_pConfigModel->initSetData(&c_configList);
  ui->c_ptvTable->resizeColumnsToContents();
}

void escdude::slot_writeConfig()
{
  ui->c_ptbDisplay->clear();
  c_pConfig->setPortName(ui->c_pcbPort->currentText());
  if(c_configList.isEmpty())
  {
    slot_dumpString(tr("Nothing to do"));
    return;
  }
  c_pConfig->writeConfig(&c_configList);
}

void escdude::slot_dumpString(QString qsMessage)
{
  ui->c_ptbDisplay->append(qsMessage);
  QApplication::processEvents();
}
