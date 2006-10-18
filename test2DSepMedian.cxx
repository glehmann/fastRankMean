#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkFastApproxRankImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkTimeProbe.h"

int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  unsigned repeats = (unsigned)atoi(argv[1]);
  itk::TimeProbe HTime, TTime;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  IType::SizeType Radius;
  Radius.Fill(5);

  typedef itk::FastApproxRankImageFilter< IType, IType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
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
  writer->SetFileName( argv[3] );
  writer->Update();

  typedef itk::MedianImageFilter<IType, IType> MedianFilterType;
  MedianFilterType::Pointer median = MedianFilterType::New();
  median->SetInput(reader->GetOutput());
  median->SetRadius(Radius);

  for (unsigned i=0;i<repeats; i++)
    {
    TTime.Start();
    median->Modified();
    median->Update();
    TTime.Stop();
    }

  writer->SetInput( median->GetOutput() );
  writer->SetFileName( argv[4] );
  writer->Update();

  std::cout << "Traditional time " << TTime.GetMeanTime() << std::endl;
  std::cout << "Huang time " << HTime.GetMeanTime() << std::endl;
  return 0;
}

