#ifndef __itkSeparableImageFilter_txx
#define __itkSeparableImageFilter_txx

#include "itkSeparableImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk {

template <class TImage, class TFilter>
SeparableImageFilter<TImage, TFilter>
::SeparableImageFilter()
{
  m_Radius.Fill(5);  // an arbitary starting point
  for (unsigned i = 0; i < ImageDimension; i++)
    {
    m_Filters[i] = FilterType::New();
    if (i > 0) 
      {
      m_Filters[i]->SetInput(m_Filters[i-1]->GetOutput());
      }
    if (i < ImageDimension - 1) 
      {
      m_Filters[i]->ReleaseDataFlagOn();
      }
    }
}


template<class TImage, class TFilter>
void
SeparableImageFilter<TImage, TFilter>
::Modified() const
{
  Superclass::Modified();
  for (unsigned i = 0; i < ImageDimension; i++)
    {
    m_Filters[i]->Modified();
    }
}


template<class TImage, class TFilter>
void
SeparableImageFilter<TImage, TFilter>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr = 
    const_cast< TImage * >( this->GetInput() );
  
  if ( !inputPtr )
    {
    return;
    }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Radius );

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
}


template <class TImage, class TFilter>
void
SeparableImageFilter<TImage, TFilter>
::SetRadius( const RadiusType radius )
{
  this->m_Radius = radius;
  
  // set up the kernels
  for (unsigned i = 0; i< TImage::ImageDimension; i++)
    {
    RadiusType rad;
    rad.Fill(0);
    rad[i] = m_Radius[i];
    m_kernels[i].SetRadius(rad);
    for( typename KernelType::Iterator kit=m_kernels[i].Begin(); kit!=m_kernels[i].End(); kit++ )
      {
      *kit = true;
      }
    m_Filters[i]->SetKernel(m_kernels[i]);
    }
}


template <class TImage, class TFilter>
void
SeparableImageFilter<TImage, TFilter>
::GenerateData()
{
  this->AllocateOutputs();
  // set up the pipeline
  m_Filters[0]->SetInput( this->GetInput() );

  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  for (unsigned i = 0; i< ImageDimension; i++)
    {
    progress->RegisterInternalFilter(m_Filters[i], 1.0/ImageDimension);
    }

  m_Filters[TImage::ImageDimension - 1]->Update();
  this->GraftOutput(m_Filters[TImage::ImageDimension - 1]->GetOutput());

}

}


#endif
