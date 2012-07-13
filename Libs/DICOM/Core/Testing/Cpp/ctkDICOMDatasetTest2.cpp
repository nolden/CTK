/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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


// ctkDICOMCore includes
#include "ctkDICOMDataset.h"

// DCMTK includes
#include "dcvrda.h"

// STD includes
#include <iostream>



void printTagValueMap( const ctkDICOMDataset::TagValueMap& map, int indent);

int ctkDICOMDatasetTest2( int argc, char * argv [] )
{
  ctkDICOMDataset dataset;
  dataset.InitializeFromFile( QString(argv[1]) );

  DcmDate date(DcmTag(0x0008,0x0020));
  date.setCurrentDate();

  QVariant result1 = ctkDICOMDataset::ElementToQVariant(&date);
  if ( result1.type() != QVariant::Date )
  {
      std::cout << "Date conversion failed!" << std::endl;
      return EXIT_FAILURE;
  }
  ctkDICOMDataset::TagValueMap elements;
  ctkDICOMDataset::TagValueMap result = dataset.GetElements(elements);

  printTagValueMap(result,0);
  return EXIT_SUCCESS;
}

void printTagValueMap( const ctkDICOMDataset::TagValueMap& map, int indent)
{

  ctkDICOMDataset::TagValueMap::const_iterator i = map.constBegin();

  while (i != map.constEnd() )
  {
    // const DcmTagKey& tag = i.key();
    QString tagName = i.key();

    QVariant value = i.value();
    QString valueString = value.toString();
    QString valueType = value.typeName();

    if ( value.type() == QVariant::Map )
    {
        std::cout << "======" << std::endl;

      printTagValueMap(value.toMap(),indent+3);
    }
    else
    {
        for (int ii = 0 ; ii < indent ; ii++)
          {
            std::cout << "-";
          }
        std::cout << qPrintable(tagName) << " | " << qPrintable(valueType) << " | " << qPrintable(valueString) << std::endl;
    }
    i++;
  }
}
