#ifndef __itkFastApproxMaskRankImageFilter_txx
#define __itkFastApproxMaskRankImageFilter_txx

#include "itkFastApproxMaskRankImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk {

template <class TInputImage, class TMaskImage, class TOutputImage>
FastApproxMaskRankImageFilter<TInputImage, TMaskImage, TOutputImage>
::FastApproxMaskRankImageFilter()
{
  m_Rank = 0.5;
  m_Radius.Fill(5);  // an arbitary starting point
  m_firstFilt = RankType1::New();
  for (unsigned i = 0; i < TInputImage::ImageDimension - 1; i++)
    {
    m_otherFilts[i] = RankType2::New();
    }
}
template<class TInputImage, class TMaskImage, class TOutputImage>
void
FastApproxMaskRankImageFilter<TInputImage, TMaskImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr = 
    const_cast< TInputImage * >( this->GetInput() );
  typename MaskImageType::Pointer  maskPtr = 
    const_cast< TMaskImage * >( this->GetMaskImage() );
  
  if ( !inputPtr )
    {
    return;
    }

  if ( !maskPtr)
    {
    std::cout << "mask null" << std::endl;
    return;
    }
  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  typename TMaskImage::RegionType maskRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();
  maskRequestedRegion = maskPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Radius );
  maskRequestedRegion.PadByRadius( m_Radius );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    OStringStream msg;
    msg << static_cast<const char *>(this->GetNameOfClass())
        << "::GenerateInputRequestedRegion()";
    e.SetLocation(msg.str().c_str());
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
  // crop the input requested region at the input's largest possible region
  if ( maskRequestedRegion.Crop(maskPtr->GetLargestPossibleRegion()) )
    {
    maskPtr->SetRequestedRegion( maskRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    maskPtr->SetRequestedRegion( maskRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    OStringStream msg;
    msg << static_cast<const char *>(this->GetNameOfClass())
        << "::GenerateInputRequestedRegion()";
    e.SetLocation(msg.str().c_str());
    e.SetDescription("Requested (for mask) region is (at least partially) outside the largest possible region.");
    e.SetDataObject(maskPtr);
    throw e;
    }
}

template <class TInputImage, class TMaskImage, class TOutputImage>
void
FastApproxMaskRankImageFilter<TInputImage, TMaskImage, TOutputImage>
::GenerateData()
{
  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  this->AllocateOutputs();
  // set up the pipeline
  for (unsigned i = 0; i < TInputImage::ImageDimension - 1; i++)
    {
    if (i > 0) 
      {
      m_otherFilts[i]->SetInput(m_otherFilts[i-1]->GetOutput());
      m_otherFilts[i]->SetMaskImage(this->GetMaskImage());
      }
    }
  m_firstFilt->SetInput(this->GetInput());
  m_firstFilt->SetMaskImage(this->GetMaskImage());
  m_otherFilts[0]->SetInput( m_firstFilt->GetOutput() );
  m_otherFilts[0]->SetMaskImage(this->GetMaskImage());

  // set up the kernels
  for (unsigned i = 0; i< TInputImage::ImageDimension; i++)
    {
    RadiusType ThisRad;
    ThisRad.Fill(0);
    ThisRad[i] = m_Radius[i];
    m_kernels[i].SetRadius(ThisRad);
    for( typename KernelType::Iterator kit=m_kernels[i].Begin(); kit!=m_kernels[i].End(); kit++ )
      {
      *kit=1;
      }
    if (i==0) 
      {
      m_firstFilt->SetKernel(m_kernels[i]);
      progress->RegisterInternalFilter(m_firstFilt, 1.0/TInputImage::ImageDimension);
      } 
    else
      {
      m_otherFilts[i-1]->SetKernel(m_kernels[i]);
      progress->RegisterInternalFilter(m_otherFilts[i-1], 1.0/TInputImage::ImageDimension);
      }
    }

  m_otherFilts[TInputImage::ImageDimension - 2]->Update();
  this->GraftOutput(m_otherFilts[TInputImage::ImageDimension - 2]->GetOutput());
}

}


#endif