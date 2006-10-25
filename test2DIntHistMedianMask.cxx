#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkMaskedRankImageFilter.h"

#include "itkTimeProbe.h"

int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef int PType;
  typedef itk::Image< PType, dim > IType;

  typedef unsigned char MPType;
  typedef itk::Image< MPType, dim > MType;

  unsigned repeats = (unsigned)atoi(argv[1]);
  itk::TimeProbe HTime, TTime;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  typedef itk::ImageFileReader< MType > MaskReaderType;
  MaskReaderType::Pointer mreader = MaskReaderType::New();
  mreader->SetFileName( argv[3] );
  mreader->Update();

  typedef itk::Neighborhood<bool, dim> KType;


  KType kernel;
  kernel.SetRadius(10);
  for( KType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit=1;
    }


  typedef itk::MaskedRankImageFilter< IType, MType, IType, KType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetMaskImage(mreader->GetOutput());
  filter->SetKernel(kernel);
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

