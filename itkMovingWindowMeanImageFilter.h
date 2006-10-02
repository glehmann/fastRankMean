/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingWindowMeanImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingWindowMeanImageFilter_h
#define __itkMovingWindowMeanImageFilter_h

#include "itkImageToImageFilter.h"
#include <list>
#include <map>
#include <set>
#include "itkOffsetLexicographicCompare.h"

namespace itk {

/**
 * \class MovingWindowMeanImageFilter
 * \brief Mean filter
 * Linear filter where each output pixel is the mean of pixels in an
 * input neighborhood.
 *
 * The boundary conditions of this filter are different to
 * itkMeanImageFilter. In this filter the neighborhood is cropped at
 * the border and therefore becomes smaller.
 * 
 * This filter employs a recursive implementation based on the sliding
 * window code from consolidatedMorphology, and is therefore usually a
 * lot faster than the direct implementation.
 */

template<class TInputImage, class TOutputImage, class TKernel >
class ITK_EXPORT MovingWindowMeanImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef MovingWindowMeanImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingWindowMeanImageFilter, 
               ImageToImageFilter);
  
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename TOutputImage::PixelType OutputPixelType ;
  typedef typename TInputImage::PixelType InputPixelType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      
  /** Kernel typedef. */
  typedef TKernel KernelType;
  
  /** Kernel (structuring element) iterator. */
  typedef typename KernelType::ConstIterator KernelIteratorType ;
  
  /** n-dimensional Kernel radius. */
  typedef typename KernelType::SizeType RadiusType ;

  typedef typename std::list< OffsetType > OffsetListType;

  typedef typename std::map< OffsetType, OffsetListType, typename Functor::OffsetLexicographicCompare<ImageDimension> > OffsetMapType;

  /** Set kernel (structuring element). */
  void SetKernel( const KernelType& kernel );

  /** Get the kernel (structuring element). */
  itkGetConstReferenceMacro(Kernel, KernelType);
  
  itkGetMacro(PixelsPerTranslation, unsigned long);
  
  /** MovingWindowMeanImageFilterBase need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

protected:
  MovingWindowMeanImageFilter();
  ~MovingWindowMeanImageFilter() {};
  
  
  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const OutputImageRegionType& 
                              outputRegionForThread,
                              int threadId) ;
  
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** kernel or structuring element to use. */
  KernelType m_Kernel ;
  
  // store the added and removed pixel offset in a list
  OffsetMapType m_AddedOffsets;
  OffsetMapType m_RemovedOffsets;

  // store the offset of the kernel to initialize the histogram
  OffsetListType m_KernelOffsets;

  typename itk::FixedArray< int, ImageDimension > m_Axes;

  unsigned long m_PixelsPerTranslation;


  void pushHistogram(double &Sum,
		     unsigned int &Count,
		     const OffsetListType* addedList,
		     const OffsetListType* removedList,
		     const RegionType &inputRegion,
		     const RegionType &kernRegion,
		     const InputImageType* inputImage,
		     const IndexType currentIdx);


  void GetDirAndOffset(const IndexType LineStart, 
		       const IndexType PrevLineStart,
		       const int ImageDimension,
		       OffsetType &LineOffset,
		       OffsetType &Changes,
		       int &LineDirection);

private:
  MovingWindowMeanImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  class DirectionCost {
    public :
    DirectionCost( int dimension, int count )
      {
      m_Dimension = dimension;
      m_Count = count;
      }
    
    /**
     * return true if the object is worst choice for the best axis
     * than the object in parameter
     */
    inline bool operator< ( const DirectionCost &dc ) const
      {
      if( m_Count > dc.m_Count )
        { return true; }
      else if( m_Count < dc.m_Count )
	{ return false; }
      else //if (m_Count == dc.m_Count) 
	{ return m_Dimension > dc.m_Dimension; }
      }

    int m_Dimension;
    int m_Count;
  };

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMovingWindowMeanImageFilter.txx"
#endif

#endif


