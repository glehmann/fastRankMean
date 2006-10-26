#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkNeighborhood.h"
#include "itkFastApproxRankImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkTimeProbe.h"
#include <vector>
#include <iomanip>


int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef int PType;
  typedef itk::Image< PType, dim > IType;

  unsigned repeats = (unsigned)atoi(argv[1]);

  // set up the radius
  typedef std::vector<int> IVec;
  
  IVec rads;
  rads.push_back(1);
  rads.push_back(5);
  rads.push_back(10);
  rads.push_back(15);
  rads.push_back(20);
  rads.push_back(40);

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  std::cout << "radius \t  Traditional time \t Separable time" << std::endl;

  for (unsigned r = 0; r < rads.size(); r++)
    {
    itk::TimeProbe HTime, TTime;
    int radius = rads[r];

    IType::SizeType ThisRadius;
    ThisRadius.Fill(radius);

    typedef itk::FastApproxRankImageFilter< IType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( reader->GetOutput() );
    filter->SetRadius(ThisRadius);
    for (unsigned i=0;i<repeats; i++)
      {
      HTime.Start();
      filter->Modified();
      filter->Update();
      HTime.Stop();
      }

    typedef itk::MedianImageFilter<IType, IType> MedianFilterType;
    MedianFilterType::Pointer median = MedianFilterType::New();
    median->SetInput(reader->GetOutput());
    median->SetRadius(ThisRadius);
    
    for (unsigned i=0;i<repeats; i++)
      {
      TTime.Start();
      median->Modified();
      median->Update();
      TTime.Stop();
      }

    std::cout << std::setprecision(3) << radius << "\t"
	      << TTime.GetMeanTime() <<"\t" 
	      << HTime.GetMeanTime() << std::endl;
    }
  return 0;
}

