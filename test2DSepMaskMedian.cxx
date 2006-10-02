#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkFastApproxMaskRankImageFilter.h"

#include "itkTimeProbe.h"

int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef unsigned short MPType;
  typedef itk::Image< MPType, dim > MType;

  int repeats = atoi(argv[1]);
  itk::TimeProbe HTime, TTime;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  typedef itk::ImageFileReader< MType > MaskReaderType;
  MaskReaderType::Pointer mreader = MaskReaderType::New();
  mreader->SetFileName( argv[3] );
  mreader->Update();

  IType::SizeType Radius;
  Radius.Fill(5);


  typedef itk::FastApproxMaskRankImageFilter< IType, MType, IType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetMaskImage(mreader->GetOutput());
  filter->SetRadius(Radius);
  for (unsigned i=0;i<repeats; i++)
    {
    HTime.Start();
    filter->Modified();
    filter->Update();
    HTime.Stop();
    }
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[4] );
  writer->Update();

  return 0;
}

