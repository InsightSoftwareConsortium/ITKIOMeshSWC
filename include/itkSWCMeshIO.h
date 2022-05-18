/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef itkSWCMeshIO_h
#define itkSWCMeshIO_h
#include "IOMeshSWCExport.h"

#include "itkMeshIOBase.h"
#include "itkNumberToString.h"
#include "itkVectorContainer.h"

#include <fstream>

/** \class SWCMeshIOEnums
 * 
 * \brief enums for the SWCMeshIO class.
 * 
 * \ingroup IOMeshSWC
 */
class SWCMeshIOEnums
{
public:
  /** \class SWCPointData
   * \ingroup IOMeshSWC
   */
  enum class SWCPointData : uint8_t
  {
    SampleIdentifier = 0,
    TypeIdentifier,
    Radius,
    ParentIdentifier
  };
};
extern IOMeshSWC_EXPORT std::ostream &
                        operator<<(std::ostream & out, const SWCMeshIOEnums::SWCPointData value);

namespace itk
{
/**
 *\class SWCMeshIO
 * \brief This class defines how to read and write SWC neuron morphology files.
 *
 * \ingroup IOFilters
 * \ingroup IOMeshSWC
 */

class IOMeshSWC_EXPORT SWCMeshIO : public MeshIOBase
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(SWCMeshIO);

  /** Standard class type aliases. */
  using Self = SWCMeshIO;
  using Superclass = MeshIOBase;
  using ConstPointer = SmartPointer<const Self>;
  using Pointer = SmartPointer<Self>;

  using StreamOffsetType = Superclass::StreamOffsetType;
  using SizeValueType = Superclass::SizeValueType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SWCMeshIO, MeshIOBase);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine if the file can be read with this MeshIO implementation.
   * \param FileNameToRead The name of the file to test for reading.
   * \post Sets classes MeshIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this MeshIO can read the file specified.
   */
  bool
  CanReadFile(const char * fileName) override;

  /** Set the spacing and dimension information for the set filename. */
  void
  ReadMeshInformation() override;

  /** Reads the data from disk into the memory buffer provided. */
  void
  ReadPoints(void * buffer) override;

  void
  ReadCells(void * buffer) override;

  void
  ReadPointData(void * buffer) override;

  void
  ReadCellData(void * buffer) override;

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine if the file can be written with this MeshIO implementation.
   * \param FileNameToWrite The name of the file to test for writing.
   * \post Sets classes MeshIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this MeshIO can write the file specified.
   */
  bool
  CanWriteFile(const char * fileName) override;

  /** Set the spacing and dimension information for the set filename. */
  void
  WriteMeshInformation() override;

  /** Writes the data to disk from the memory buffer provided. */
  void
  WritePoints(void * buffer) override;

  void
  WriteCells(void * buffer) override;

  void
  WritePointData(void * buffer) override;

  void
  WriteCellData(void * buffer) override;

  void
  Write() override;

  using SampleIdentifierType = int16_t;
  using TypeIdentifierType = uint8_t;
  using RadiusType = double;
  using ParentIdentifierType = int16_t;

  using SampleIdentifierContainerType = VectorContainer<IdentifierType, SampleIdentifierType>;
  using TypeIdentifierContainerType = VectorContainer<IdentifierType, TypeIdentifierType>;
  using RadiusContainerType = VectorContainer<IdentifierType, RadiusType>;
  using ParentIdentifierContainerType = VectorContainer<IdentifierType, ParentIdentifierType>;

  /** Set/Get the sample identifiers. */
  void SetSampleIdentifiers(const SampleIdentifierContainerType *);
  const SampleIdentifierContainerType * GetSampleIdentifiers() const;

  /** Set/Get the type identifiers.
   *  0 - undefined
   *  1 - soma
   *  2 - axon
   *  3 - (basal) dendrite
   *  4 - apical dendrite
   *  5 - custom
   *  6 - unspecified neurite
   *  7 - glia processes
   */
  void SetTypeIdentifiers(const TypeIdentifierContainerType *);
  const TypeIdentifierContainerType * GetTypeIdentifiers() const;


  /** Set/Get the Radius in micrometers (half the node thickness). */
  void SetRadii(const RadiusContainerType *);
  const RadiusContainerType * GetRadii() const;

  /** Set/Get the parent sample identifiers. */
  void SetParentIdentifiers(const ParentIdentifierContainerType *);
  const ParentIdentifierContainerType * GetParentIdentifiers() const;

  /** Set/Get the content of the point data on the input/output itk::Mesh. */
  itkGetConstMacro(PointDataContent, SWCMeshIOEnums::SWCPointData);
  itkSetMacro(PointDataContent, SWCMeshIOEnums::SWCPointData);

protected:
  /** Write points to output stream */
  template <typename T>
  void
  WritePoints(T * buffer, std::ofstream & outputFile)
  {
    Indent        indent(1);
    SizeValueType index = itk::NumericTraits<SizeValueType>::ZeroValue();

    for (SizeValueType ii = 0; ii < this->m_NumberOfPoints; ++ii)
    {
      outputFile << indent;
      for (unsigned int jj = 0; jj < this->m_PointDimension; ++jj)
      {
        outputFile << ConvertNumberToString(buffer[index++]) << " ";
      }
      outputFile << '\n';
    }
  }

  template <typename T>
  void
  WriteCells(T * buffer, std::ofstream & outputFile)
  {
    Indent        indent(7);
    SizeValueType index = itk::NumericTraits<SizeValueType>::ZeroValue();

    for (SizeValueType ii = 0; ii < this->m_NumberOfCells; ++ii)
    {
      auto numberOfCellPoints = static_cast<unsigned int>(buffer[++index]);
      index++;
      for (unsigned int jj = 0; jj < numberOfCellPoints - 1; ++jj)
      {
        outputFile << indent << buffer[index++] + 1;
      }

      outputFile << indent << -static_cast<long long>(buffer[index++] + 1) << '\n';
    }
  }

protected:
  SWCMeshIO();
  ~SWCMeshIO() override;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  StreamOffsetType m_FilePosition{ 0 };
  SizeValueType    m_PartId;
  SizeValueType    m_FirstCellId;
  SizeValueType    m_LastCellId;

  SampleIdentifierContainerType::Pointer m_SampleIdentifiers;
  TypeIdentifierContainerType::Pointer m_TypeIdentifiers;
  RadiusContainerType::Pointer m_Radii;
  ParentIdentifierContainerType::Pointer m_ParentIdentifiers;

  SWCMeshIOEnums::SWCPointData m_PointDataContent{ SWCMeshIOEnums::SWCPointData::TypeIdentifier };
};
} // end namespace itk

#endif
