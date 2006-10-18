#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkMovingWindowMeanImageFilter.h"
#include "itkMeanImageFilter.h"
#include "itkTimeProbe.h"

int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef int PType;
  typedef itk::Image< PType, dim > IType;

  unsigned repeats = (unsigned)atoi(argv[1]);
  itk::TimeProbe HTime, TTime;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();
  typedef itk::Neighborhood<bool, dim> KType;


  KType kernel;
  kernel.SetRadius(10);
  for( KType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit=1;
    }

  typedef itk::MovingWindowMeanImageFilter< IType, IType, KType > FilterType;
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

  typedef itk::MeanImageFilter<IType, IType> MeanFilterType;
  MeanFilterType::Pointer mean = MeanFilterType::New();
  mean->SetInput(reader->GetOutput());
  mean->SetRadius(kernel.GetRadius());

  for (unsigned i=0;i<repeats; i++)
    {
    TTime.Start();
    mean->Modified();
    mean->Update();
    TTime.Stop();
    }

  writer->SetInput( mean->GetOutput() );
  writer->SetFileName( argv[4] );
  writer->Update();
  std::cout << "Done standard" << std::endl;
  std::cout << "Traditional time " << TTime.GetMeanTime() << std::endl;
  std::cout << "Updated time " << HTime.GetMeanTime() << std::endl;
  return 0;
}

