/*=========================================================================

  Library:   CTK

  Copyright (c) 2010

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkDICOMDatastore_h
#define __ctkDICOMDatastore_h

#include <QObject>

#include "ctkDICOMCoreExport.h"

class DcmDataset;

/// Interface for classes that are capable of storing
/// DICOM objects 
/// 
class CTK_DICOM_CORE_EXPORT ctkDICOMDatastore : public QObject 
{
  Q_OBJECT
  public slots:
    virtual void insert ( DcmDataset *dataset ) = 0;
};

#endif
