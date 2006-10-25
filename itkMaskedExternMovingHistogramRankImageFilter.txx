/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMaskedExternMovingHistogramRankImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMaskedExternMovingHistogramRankImageFilter_txx
#define __itkMaskedExternMovingHistogramRankImageFilter_txx

#include "itkMaskedExternMovingHistogramRankImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"


namespace itk {


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::MaskedExternMovingHistogramRankImageFilter()
  : m_Kernel()
{
  m_PixelsPerTranslation = 0;
  m_Rank = 0.5; 
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 2 );
#if 0
  typename MaskImageType::Pointer maskout = static_cast<MaskImageType * >(this->MakeOutput(1).GetPointer());
  typename OutputImageType::Pointer outimage = static_cast<OutputImageType*>(this->MakeOutput(0).GetPointer());
#else
  typename MaskImageType::Pointer maskout = TMaskImage::New();
  typename OutputImageType::Pointer outimage = TOutputImage::New();
#endif
  this->SetNthOutput( 1, maskout.GetPointer() );
  this->SetNthOutput( 0, outimage.GetPointer());
  m_FillValue = 0;
}

#if 1
template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::AllocateOutputs()
{
  //Superclass::AllocateOutputs();
  // Allocate the output image.
  typename TOutputImage::Pointer output = this->GetOutputImage();
  output->SetBufferedRegion( output->GetRequestedRegion() );
  output->Allocate();
//   // Allocate the output mask image.
  typename TMaskImage::Pointer mask = this->GetOutputMask();
  mask->SetBufferedRegion( mask->GetRequestedRegion() );
  mask->Allocate();
}

#endif

#if 1
template <class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
DataObject::Pointer
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
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
#endif
template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
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
    std::cout << "MEM: mask null" << std::endl;
    return;
    }
  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  typename TMaskImage::RegionType maskRequestedRegion;
  
  inputRequestedRegion = inputPtr->GetRequestedRegion();
  maskRequestedRegion = maskPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Kernel.GetRadius() );
  maskRequestedRegion.PadByRadius( m_Kernel.GetRadius() );

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

template <class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
typename MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>::MaskImageType *
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::GetOutputMask( void )
{
  
  typename MaskImageType::Pointer res = dynamic_cast<MaskImageType *>(this->ProcessObject::GetOutput( 1 ));
  return res;
}

template <class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
typename MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>::OutputImageType *
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::GetOutputImage( void )
{
  typename OutputImageType::Pointer res = static_cast<OutputImageType *>(this->GetOutput( 0 ) );
  return res;
}

template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::SetKernel( const KernelType& kernel )
{
  // first, build the list of offsets of added and removed pixels when the 
  // structuring element move of 1 pixel on 1 axis; do it for the 2 directions
  // on each axes.
  
  // transform the structuring element in an image for an easier
  // access to the data
  typedef Image< bool, TInputImage::ImageDimension > BoolImageType;
  typename BoolImageType::Pointer tmpSEImage = BoolImageType::New();
  tmpSEImage->SetRegions( kernel.GetSize() );
  tmpSEImage->Allocate();
  RegionType tmpSEImageRegion = tmpSEImage->GetRequestedRegion();
  ImageRegionIteratorWithIndex<BoolImageType> kernelImageIt;
  kernelImageIt = ImageRegionIteratorWithIndex<BoolImageType>(tmpSEImage, tmpSEImageRegion);
  kernelImageIt.GoToBegin();
  KernelIteratorType kernel_it = kernel.Begin();
  OffsetListType kernelOffsets;

  // create a center index to compute the offset
  IndexType centerIndex;
  for( unsigned axis=0; axis<ImageDimension; axis++)
    { centerIndex[axis] = kernel.GetSize()[axis] / 2; }
  
  unsigned long count = 0;
  while( !kernelImageIt.IsAtEnd() )
    {
    kernelImageIt.Set( *kernel_it > 0 );
    if( *kernel_it > 0 )
      {
      kernelImageIt.Set( true );
      kernelOffsets.push_front( kernelImageIt.GetIndex() - centerIndex );
      count++;
      }
    else
      { kernelImageIt.Set( false ); }
    ++kernelImageIt;
    ++kernel_it;
    }
    

  // verify that the kernel contain at least one point
  if( count == 0 )
    { itkExceptionMacro( << "The kernel must contain at least one point." ); }

  // no attribute should be modified before here to avoid setting the filter in a bad status
  // store the kernel !!
  m_Kernel = kernel;

  // clear the already stored values
  m_AddedOffsets.clear();
  m_RemovedOffsets.clear();
  //m_Axes

  // store the kernel offset list
  m_KernelOffsets = kernelOffsets;

  typename itk::FixedArray< unsigned long, ImageDimension > axisCount;
  axisCount.Fill( 0 );

  for( unsigned axis=0; axis<ImageDimension; axis++)
    {
    OffsetType refOffset;
    refOffset.Fill( 0 );
    for( int direction=-1; direction<=1; direction +=2)
      {
      refOffset[axis] = direction;
      for( kernelImageIt.GoToBegin(); !kernelImageIt.IsAtEnd(); ++kernelImageIt)
        {
        IndexType idx = kernelImageIt.GetIndex();
        
        if( kernelImageIt.Get() )
          {
          // search for added pixel during a translation
          IndexType nextIdx = idx + refOffset;
          if( tmpSEImageRegion.IsInside( nextIdx ) )
            {
            if( !tmpSEImage->GetPixel( nextIdx ) )
              {
                m_AddedOffsets[refOffset].push_front( nextIdx - centerIndex );
                axisCount[axis]++;
              }
            }
          else
            {
              m_AddedOffsets[refOffset].push_front( nextIdx - centerIndex );
              axisCount[axis]++;
            }
          // search for removed pixel during a translation
          IndexType prevIdx = idx - refOffset;
          if( tmpSEImageRegion.IsInside( prevIdx ) )
            {
            if( !tmpSEImage->GetPixel( prevIdx ) )
              {
                m_RemovedOffsets[refOffset].push_front( idx - centerIndex );
                axisCount[axis]++;
              }
            }
          else
            {
              m_RemovedOffsets[refOffset].push_front( idx - centerIndex );
              axisCount[axis]++;
            }

          }
        }
      }
    }
    
    // search for the best axis
    typedef typename std::set<DirectionCost> MapCountType;
    MapCountType invertedCount;
    for( unsigned i=0; i<ImageDimension; i++ )
      {
      invertedCount.insert( DirectionCost( i, axisCount[i] ) );
      }

    int i=0;
    for( typename MapCountType::iterator it=invertedCount.begin(); it!=invertedCount.end(); it++, i++)
      {
      m_Axes[i] = it->m_Dimension;
      }

    m_PixelsPerTranslation = axisCount[m_Axes[ImageDimension - 1]] / 2;  // divided by 2 because there is 2 directions on the axis
}


// a modified version that uses line iterators and only moves the
// histogram in one direction. Hopefully it will be a bit simpler and
// faster due to improved memory access and a tighter loop.
template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId) 
{
  
  // instantiate the histogram
  //HistogramType histogram = this->NewHistogram();
    
  OutputImageType* outputImage = this->GetOutputImage();
  MaskImageType * outputMask = this->GetOutputMask();
  const InputImageType* inputImage = this->GetInput();
  const MaskImageType *maskImage = this->GetMaskImage();

  RegionType inputRegion = inputImage->GetRequestedRegion();

  HistogramType *histogram;

  if (useVectorBasedHistogram())
    {
    histogram = new VHistogram;
    }
  else
    {
    histogram = new MHistogram;
    }

  histogram->SetRank(m_Rank);

  // initialize the histogram
  for( typename OffsetListType::iterator listIt = this->m_KernelOffsets.begin(); 
       listIt != this->m_KernelOffsets.end(); listIt++ )
    {
    IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
    if( inputRegion.IsInside( idx ) && maskImage->GetPixel(idx) )
      {
      histogram->AddPixel( inputImage->GetPixel(idx) );
      }
    else
      {
      histogram->AddBoundary();
      }
    }

  // get the map based version set up properly
  //histogram->Initialize();

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

      //std::cout << currentIdx << std::endl;
      if( maskImage->GetPixel( currentIdx ) && histRef->IsValid() )
	{
	outputImage->SetPixel( currentIdx, static_cast< OutputPixelType >( histRef->GetValue() ) );
	outputMask->SetPixel( currentIdx, 1 );
	}
      else
	{
	outputImage->SetPixel( currentIdx, m_FillValue );
	outputMask->SetPixel( currentIdx, 0 );
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
    GetDirAndOffset(LineStart, PrevLineStart, ImageDimension,
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

template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
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
      MaskPixelType Msk = maskImage->GetPixel(idx);
      if (Msk)
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
      MaskPixelType Msk = maskImage->GetPixel(idx);
      if (Msk)
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
      if( inputRegion.IsInside( idx ) && maskImage->GetPixel(idx) )
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
      if( inputRegion.IsInside( idx ) && maskImage->GetPixel(idx) )
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


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::printHist(const HistogramType *H)
{
/*  std::cout << "Hist = " ;
  typename HistogramType::const_iterator mapIt;
  for (mapIt = H.begin(); mapIt != H.end(); mapIt++) 
    {
    std::cout << "V= " << int(mapIt->first) << " C= " << int(mapIt->second) << " ";
    }
  std::cout << std::endl;*/
}

template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::GetDirAndOffset(const IndexType LineStart, 
		  const IndexType PrevLineStart,
		  const int ImageDimension,
		  OffsetType &LineOffset,
		  OffsetType &Changes,
		  int &LineDirection)
{
  // when moving between lines in the same plane there should be only
  // 1 non zero (positive) entry in LineOffset.
  // When moving between planes there will be some negative ones too.
  LineOffset = Changes = LineStart - PrevLineStart;
  for (int y=0;y<ImageDimension;y++) 
    {
    if (LineOffset[y] > 0)
      {
      LineOffset[y]=1;  // should be 1 anyway
      LineDirection=y;
      }
    else
      {
      LineOffset[y]=0;
      }
    }
}


template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel>
void
MaskedExternMovingHistogramRankImageFilter<TInputImage, TMaskImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
}

}// end namespace itk
#endif
