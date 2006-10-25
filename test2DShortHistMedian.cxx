#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkRankImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkTimeProbe.h"

int main(int, char * argv[])
{
  const int dim = 3;
  
  typedef short PType;
  typedef itk::Image< PType, dim > IType;
  itk::TimeProbe HTime, TTime;

  unsigned repeats = (unsigned)atoi(argv[1]);

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();
  typedef itk::Neighborhood<bool, dim> KType;


  KType kernel;
  kernel.SetRadius(1);
  for( KType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit=1;
    }

  typedef itk::RankImageFilter< IType, IType, KType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
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
  writer->SetFileName( argv[3] );
  writer->Update();


  typedef itk::MedianImageFilter<IType, IType> MedianFilterType;
  MedianFilterType::Pointer median = MedianFilterType::New();
  median->SetInput(reader->GetOutput());
  median->SetRadius(kernel.GetRadius());
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

