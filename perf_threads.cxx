#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include <vector>
#include <iomanip>
#include "itkNeighborhood.h"

#include "itkFastApproxMaskRankImageFilter.h"
#include "itkFastApproxRankImageFilter.h"
#include "itkMaskedRankImageFilter.h"
#include "itkMovingWindowMeanImageFilter.h"
#include "itkRankImageFilter.h"
#include "itkSeparableMeanImageFilter.h"

int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;
  typedef float AType;
  typedef itk::Image< AType, dim > AccType;

  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  // and the mask image, for the masked filters
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  
  typedef itk::FastApproxMaskRankImageFilter< IType, IType, IType > FastApproxMaskRankType;
  FastApproxMaskRankType::Pointer fastApproxMaskRank = FastApproxMaskRankType::New();
  fastApproxMaskRank->SetInput( reader->GetOutput() );
  fastApproxMaskRank->SetMaskImage( reader2->GetOutput() );
  fastApproxMaskRank->SetRadius( 5 );

  typedef itk::FastApproxRankImageFilter< IType, IType > FastApproxRankType;
  FastApproxRankType::Pointer fastApproxRank = FastApproxRankType::New();
  fastApproxRank->SetInput( reader->GetOutput() );
  fastApproxRank->SetRadius( 5 );

  typedef itk::Neighborhood<bool, dim> KernelType;
  typedef itk::MaskedRankImageFilter< IType, IType, IType, KernelType > MaskedRankType;
  MaskedRankType::Pointer maskedRank = MaskedRankType::New();
  maskedRank->SetInput( reader->GetOutput() );
  maskedRank->SetMaskImage( reader2->GetOutput() );
  maskedRank->SetRadius( 5 );

  typedef itk::MovingWindowMeanImageFilter< IType, IType, KernelType > MovingWindowMeanType;
  MovingWindowMeanType::Pointer movingWindowMean = MovingWindowMeanType::New();
  movingWindowMean->SetInput( reader->GetOutput() );
  movingWindowMean->SetRadius( 5 );

  typedef itk::RankImageFilter< IType, IType, KernelType > RankType;
  RankType::Pointer rank = RankType::New();
  rank->SetInput( reader->GetOutput() );
  rank->SetRadius( 5 );

  typedef itk::SeparableMeanImageFilter< IType, IType > SeparableMeanType;
  SeparableMeanType::Pointer separableMean = SeparableMeanType::New();
  separableMean->SetInput( reader->GetOutput() );
  separableMean->SetRadius( 5 );

  reader->Update();
  reader2->Update();
  
  std::cout << "#nb" << "\t" 
            << "fastApproxMaskRank" << "\t"
            << "fastApproxRank" << "\t"
            << "maskedRank" << "\t"
            << "movingWindowMean" << "\t"
            << "rank" << "\t"
            << "separableMean" << "\t"
            << std::endl;

  for( int t=1; t<=10; t++ )
    {
    itk::TimeProbe fastApproxMaskRankTime;
    itk::TimeProbe fastApproxRankTime;
    itk::TimeProbe maskedRankTime;
    itk::TimeProbe movingWindowMeanTime;
    itk::TimeProbe rankTime;
    itk::TimeProbe separableMeanTime;

    fastApproxMaskRank->SetNumberOfThreads( t );
    fastApproxRank->SetNumberOfThreads( t );
    maskedRank->SetNumberOfThreads( t );
    movingWindowMean->SetNumberOfThreads( t );
    rank->SetNumberOfThreads( t );
    separableMean->SetNumberOfThreads( t );
    
    for( int i=0; i<10; i++ )
      {
      fastApproxMaskRank->Modified();
      fastApproxMaskRankTime.Start();
      fastApproxMaskRank->Update();
      fastApproxMaskRankTime.Stop();

      fastApproxRank->Modified();
      fastApproxRankTime.Start();
      fastApproxRank->Update();
      fastApproxRankTime.Stop();

      maskedRank->Modified();
      maskedRankTime.Start();
      maskedRank->Update();
      maskedRankTime.Stop();

      movingWindowMean->Modified();
      movingWindowMeanTime.Start();
      movingWindowMean->Update();
      movingWindowMeanTime.Stop();

      rank->Modified();
      rankTime.Start();
      rank->Update();
      rankTime.Stop();

      separableMean->Modified();
      separableMeanTime.Start();
      separableMean->Update();
      separableMeanTime.Stop();

      }
      
    std::cout << std::setprecision(3) << t << "\t" 
              << fastApproxMaskRankTime.GetMeanTime() << "\t"
              << fastApproxRankTime.GetMeanTime() << "\t"
              << maskedRankTime.GetMeanTime() << "\t"
              << movingWindowMeanTime.GetMeanTime() << "\t"
              << rankTime.GetMeanTime() << "\t"
              << separableMeanTime.GetMeanTime() << "\t"
              << std::endl;
    }
  
  
  return 0;
}

