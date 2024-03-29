/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMaskedMovingHistogramImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even 
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
    PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMaskedMovingHistogramImageFilter_txx
#define __itkMaskedMovingHistogramImageFilter_txx

#include "itkMaskedMovingHistogramImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"


namespace itk {


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::MaskedMovingHistogramImageFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->m_FillValue = NumericTraits< OutputPixelType >::Zero;
  this->m_MaskValue = NumericTraits< MaskPixelType >::max();
  this->m_BackgroundMaskValue = NumericTraits< MaskPixelType >::Zero;
  this->SetGenerateOutputMask( false );
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
THistogram *
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::NewHistogram()
{
  return new THistogram();
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
void
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::SetGenerateOutputMask( bool generateOutputMask )
{
  if( generateOutputMask != this->m_GenerateOutputMask )
  {
    this->m_GenerateOutputMask = generateOutputMask;
    if( generateOutputMask )
      {
      this->SetNumberOfRequiredOutputs( 2 );
//       typename OutputImageType::Pointer outimage = TOutputImage::New();
//       this->SetNthOutput( 0, outimage.GetPointer());
      typename MaskImageType::Pointer maskout = TMaskImage::New();
      this->SetNthOutput( 1, maskout.GetPointer() );
      }
    else
      {
      this->SetNumberOfRequiredOutputs( 1 );
      this->SetNthOutput( 1, NULL );
      }
    }
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
void
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::AllocateOutputs()
{
  if( this->m_GenerateOutputMask )
    {
    // Allocate the output image.
    typename TOutputImage::Pointer output = this->GetOutput();
    output->SetBufferedRegion( output->GetRequestedRegion() );
    output->Allocate();
    // Allocate the output mask image.
    typename TMaskImage::Pointer mask = this->GetOutputMask();
    mask->SetBufferedRegion( mask->GetRequestedRegion() );
    mask->Allocate();
    }
  else
    {
    Superclass::AllocateOutputs();
    }
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
DataObject::Pointer
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::MakeOutput(unsigned int idx)
{
  DataObject::Pointer output;
  switch( idx )
    {
    case 0:
      output = (TOutputImage::New()).GetPointer();
      break;
    case 1:
      output = (TMaskImage::New()).GetPointer();
      break;
    }
  return output.GetPointer();
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
typename MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>::MaskImageType *
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::GetOutputMask( void )
{
  typename MaskImageType::Pointer res = dynamic_cast<MaskImageType *>(this->ProcessObject::GetOutput( 1 ));
  return res;
}


// a modified version that uses line iterators and only moves the
// histogram in one direction. Hopefully it will be a bit simpler and
// faster due to improved memory access and a tighter loop.
template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
void
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                      int threadId) 
{
  
  // instantiate the histogram
  HistogramType * histogram = this->NewHistogram();
    
  OutputImageType* outputImage = this->GetOutput();
  MaskImageType * outputMask = this->GetOutputMask();
  const InputImageType* inputImage = this->GetInput();
  const MaskImageType *maskImage = this->GetMaskImage();

  RegionType inputRegion = inputImage->GetRequestedRegion();

  // initialize the histogram
  for( typename OffsetListType::iterator listIt = this->m_KernelOffsets.begin(); 
      listIt != this->m_KernelOffsets.end(); listIt++ )
    {
    IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
    if( inputRegion.IsInside( idx ) && maskImage->GetPixel(idx) == m_MaskValue )
      {
      histogram->AddPixel( inputImage->GetPixel(idx) );
      }
    else
      {
      histogram->AddBoundary();
      }
    }

  // now move the histogram
  itk::FixedArray<short, ImageDimension> direction;
  direction.Fill(1);
  IndexType currentIdx = outputRegionForThread.GetIndex();
  int axis = ImageDimension - 1;
  OffsetType offset;
  offset.Fill( 0 );
  RegionType stRegion;
  stRegion.SetSize( this->m_Kernel.GetSize() );
  stRegion.PadByRadius( 1 ); // must pad the region by one because of the translation
  
  OffsetType centerOffset;
  for( unsigned axis=0; axis<ImageDimension; axis++)
    { centerOffset[axis] = stRegion.GetSize()[axis] / 2; }
  
  int BestDirection = this->m_Axes[axis];
  int LineLength = inputRegion.GetSize()[BestDirection];
  
  // Report progress every line instead of every pixel
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels()/outputRegionForThread.GetSize()[BestDirection]);
  // init the offset and get the lists for the best axis
  offset[BestDirection] = direction[BestDirection];
  // it's very important for performances to get a pointer and not a copy
  const OffsetListType* addedList = &this->m_AddedOffsets[offset];;
  const OffsetListType* removedList = &this->m_RemovedOffsets[offset];
  
  typedef typename itk::ImageLinearConstIteratorWithIndex<InputImageType> InputLineIteratorType;
  InputLineIteratorType InLineIt(inputImage, outputRegionForThread);
  InLineIt.SetDirection(BestDirection);
  
  typedef typename itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
  InLineIt.GoToBegin();
  IndexType LineStart;
  InLineIt.GoToBegin();
  
  typedef typename std::vector<HistogramType *> HistVecType;
  HistVecType HistVec(ImageDimension);
  typedef typename std::vector<IndexType> IndexVecType;
  IndexVecType PrevLineStartVec(ImageDimension);

  // Steps is used to keep track of the order in which the line
  // iterator passes over the various dimensions.
  int *Steps = new int[ImageDimension];
  
  for (unsigned i=0;i<ImageDimension;i++)
    {
    HistVec[i] = histogram->Clone();
    PrevLineStartVec[i] = InLineIt.GetIndex();
    Steps[i]=0;
    }

  while(!InLineIt.IsAtEnd())
    {
    HistogramType *histRef = HistVec[BestDirection];
    IndexType PrevLineStart = InLineIt.GetIndex();
    for (InLineIt.GoToBeginOfLine(); !InLineIt.IsAtEndOfLine(); ++InLineIt)
      {
      
      // Update the histogram
      IndexType currentIdx = InLineIt.GetIndex();

      if( maskImage->GetPixel(currentIdx) == m_MaskValue && histRef->IsValid() ) 
        {		
        outputImage->SetPixel( currentIdx,
                              static_cast< OutputPixelType >( histRef->GetValue( inputImage->GetPixel(currentIdx) ) ) );
        if( this->m_GenerateOutputMask )
          {
          outputMask->SetPixel( currentIdx, m_MaskValue );
          }
        }	
      else
        {	
        outputImage->SetPixel( currentIdx, m_FillValue );
        if( this->m_GenerateOutputMask )
          {
          outputMask->SetPixel( currentIdx, m_BackgroundMaskValue );
          }
        }
      stRegion.SetIndex( currentIdx - centerOffset );
      pushHistogram(histRef, addedList, removedList, inputRegion, 
                    stRegion, inputImage, maskImage, currentIdx);

      }
    Steps[BestDirection] += LineLength;
    InLineIt.NextLine();
    if (InLineIt.IsAtEnd())
      {
      break;
      }
    LineStart = InLineIt.GetIndex();
    // This section updates the histogram for the next line
    // Since we aren't zig zagging we need to figure out which
    // histogram to update and the direction in which to push
    // it. Then we need to copy that histogram to the relevant
    // places
    OffsetType LineOffset, Changes;
    // Figure out which stored histogram to move and in
    // which direction
    int LineDirection;
    // This function deals with changing planes etc
    this->GetDirAndOffset(LineStart, PrevLineStart, ImageDimension,
                    LineOffset, Changes, LineDirection);
    ++(Steps[LineDirection]);
    IndexType PrevLineStartHist = LineStart - LineOffset;
    const OffsetListType* addedListLine = &this->m_AddedOffsets[LineOffset];;
    const OffsetListType* removedListLine = &this->m_RemovedOffsets[LineOffset];
    HistogramType *tmpHist = HistVec[LineDirection];
    stRegion.SetIndex(PrevLineStart - centerOffset);
    // Now move the histogram
    pushHistogram(tmpHist, addedListLine, removedListLine, inputRegion, 
                  stRegion, inputImage, maskImage, PrevLineStartHist);
    
    //PrevLineStartVec[LineDirection] = LineStart;
    // copy the updated histogram and line start entries to the
    // relevant directions. When updating direction 2, for example,
    // new copies of directions 0 and 1 should be made.
    for (unsigned i=0;i<ImageDimension;i++) 
      {
      if (Steps[i] > Steps[LineDirection])
        {
        // make sure this is the right thing to do
        delete(HistVec[i]);
        HistVec[i] = HistVec[LineDirection]->Clone();
        }
      }
    progress.CompletedPixel();
    }
  for (unsigned i=0;i<ImageDimension;i++) 
    {
    delete(HistVec[i]);
    }
  delete(histogram);
  delete [] Steps;
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>
void
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::pushHistogram(HistogramType *histogram, 
                const OffsetListType* addedList,
                const OffsetListType* removedList,
                const RegionType &inputRegion,
                const RegionType &kernRegion,
                const InputImageType* inputImage,
                const MaskImageType *maskImage,
                const IndexType currentIdx)
{

  if( inputRegion.IsInside( kernRegion ) )
    {
    // update the histogram
    for( typename OffsetListType::const_iterator addedIt = addedList->begin(); 
        addedIt != addedList->end(); addedIt++ )
      { 
      typename InputImageType::IndexType idx = currentIdx + (*addedIt);
      if( maskImage->GetPixel(idx) == m_MaskValue )
        {
        histogram->AddPixel( inputImage->GetPixel( idx ) ); 
        }
      else
        {
        histogram->AddBoundary();
        }
      }
    for( typename OffsetListType::const_iterator removedIt = removedList->begin(); 
        removedIt != removedList->end(); removedIt++ )
      { 
      typename InputImageType::IndexType idx = currentIdx + (*removedIt);
      if( maskImage->GetPixel(idx) == m_MaskValue )
        {
        histogram->RemovePixel( inputImage->GetPixel( idx ) ); 
        }
      else
        {
        histogram->RemoveBoundary();
        }
      }
    }
  else
    {
    // update the histogram
    for( typename OffsetListType::const_iterator addedIt = addedList->begin(); 
        addedIt != addedList->end(); addedIt++ )
      {
      IndexType idx = currentIdx + (*addedIt);
      if( inputRegion.IsInside( idx ) && maskImage->GetPixel(idx) == m_MaskValue )
        {
        histogram->AddPixel( inputImage->GetPixel( idx ) ); 
        }
      else
        { 
        histogram->AddBoundary(); 
        }
      }
    for( typename OffsetListType::const_iterator removedIt = removedList->begin(); 
        removedIt != removedList->end(); removedIt++ )
      {
      IndexType idx = currentIdx + (*removedIt);
      if( inputRegion.IsInside( idx ) && maskImage->GetPixel(idx) == m_MaskValue )
        { 
        histogram->RemovePixel( inputImage->GetPixel( idx ) ); 
        }
      else
        { 
        histogram->RemoveBoundary(); 
        }
      }
    }
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram>

void
MaskedMovingHistogramImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel, THistogram>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "GenerateOutputMask: "  << m_GenerateOutputMask << std::endl;
  os << indent << "FillValue: "  << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_FillValue) << std::endl;
  os << indent << "MaskValue: "  << static_cast<typename NumericTraits<MaskPixelType>::PrintType>(m_MaskValue) << std::endl;
  os << indent << "BackgroundMaskValue: "  << static_cast<typename NumericTraits<MaskPixelType>::PrintType>(m_BackgroundMaskValue) << std::endl;
}

}// end namespace itk
#endif
