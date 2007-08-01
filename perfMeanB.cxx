#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkSeparableMeanImageFilter.h"
#include "itkMeanImageFilter.h"
#include "itkSeparableImageFilter.h"
#include "itkTimeProbe.h"
#include <vector>
#include <iomanip>


int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  unsigned repeats = (unsigned)atoi(argv[1]);

  // set up the radius
  typedef std::vector<int> IVec;
  
  IVec rads;
  rads.push_back(1);
  rads.push_back(2);
  rads.push_back(3);
  rads.push_back(5);
  rads.push_back(10);
  rads.push_back(15);
  rads.push_back(20);
  rads.push_back(40);

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  std::cout << "radius \tstd \tMW \tSep std\tSep MW" << std::endl;

  for (unsigned r = 0; r < rads.size(); r++)
    {
    itk::TimeProbe SMWTime, StdTime, SStdTime, MWTime;
    int radius = rads[r];

    IType::SizeType ThisRadius;
    ThisRadius.Fill(radius);

    typedef itk::MeanImageFilter<IType, IType> MeanFilterType;
    MeanFilterType::Pointer mean = MeanFilterType::New();
    mean->SetInput(reader->GetOutput());
    mean->SetRadius(ThisRadius);
    
    for (unsigned i=0;i<repeats; i++)
      {
      StdTime.Start();
      mean->Modified();
      mean->Update();
      StdTime.Stop();
      }

    typedef itk::Neighborhood<bool, dim> KType;
    KType kernel;
    kernel.SetRadius(radius);
    for( KType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
      {
      *kit=1;
      }

    typedef itk::MovingWindowMeanImageFilter< IType, IType, KType > MWFilterType;
    MWFilterType::Pointer mwfilter = MWFilterType::New();
    mwfilter->SetInput( reader->GetOutput() );
    mwfilter->SetKernel(kernel);
    for (unsigned i=0;i<repeats; i++)
      {
      MWTime.Start();
      mwfilter->Modified();
      mwfilter->Update();
      MWTime.Stop();
      }


    typedef itk::SeparableImageFilter<IType, IType, MeanFilterType> SeparableMeanFilterType;
    SeparableMeanFilterType::Pointer sep_mean = SeparableMeanFilterType::New();
    sep_mean->SetInput(reader->GetOutput());
    sep_mean->SetRadius(ThisRadius);
    
    for (unsigned i=0;i<repeats; i++)
      {
      SStdTime.Start();
      sep_mean->Modified();
      sep_mean->Update();
      SStdTime.Stop();
      }

    typedef itk::SeparableMeanImageFilter< IType, IType > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( reader->GetOutput() );
    filter->SetRadius(ThisRadius);
    for (unsigned i=0;i<repeats; i++)
      {
      SMWTime.Start();
      filter->Modified();
      filter->Update();
      SMWTime.Stop();
      }

    std::cout << std::setprecision(3) << radius << "\t"
	      << StdTime.GetMeanTime() <<"\t" 
	      << MWTime.GetMeanTime() << "\t" 
	      << SStdTime.GetMeanTime() << "\t" 
	      << SMWTime.GetMeanTime() << std::endl;
    }
  return 0;
}

