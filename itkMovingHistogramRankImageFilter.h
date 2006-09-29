/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramRankImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramRankImageFilter_h
#define __itkMovingHistogramRankImageFilter_h

#include "itkImageToImageFilter.h"
#include <list>
#include <map>
#include <set>
#include "itkOffsetLexicographicCompare.h"
#include "itkRankHistogram.h"

namespace itk {

/**
 * \class MovingHistogramRankImageFilter
 * \brief gray scale dilation of an image
 *
 * Dilate an image using grayscale morphology. Dilation takes the
 * maximum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * \sa MorphologyImageFilter, GrayscaleFunctionDilateImageFilter, BinaryDilateImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */

template<class TInputImage, class TOutputImage, class TKernel >
class ITK_EXPORT MovingHistogramRankImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef MovingHistogramRankImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingHistogramRankImageFilter, 
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
  
  /** MovingHistogramRankImageFilterBase need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

protected:
  MovingHistogramRankImageFilter();
  ~MovingHistogramRankImageFilter() {};

  typedef RankHistogram<InputPixelType> HistogramType;
  
  typedef RankHistogramVec<InputPixelType, std::less< InputPixelType> > VHistogram;
  typedef RankHistogramMap<InputPixelType, std::less< InputPixelType>  > MHistogram;
  
  
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


  void pushHistogram(HistogramType *histogram, 
		     const OffsetListType* addedList,
		     const OffsetListType* removedList,
		     const RegionType &inputRegion,
		     const RegionType &kernRegion,
		     const InputImageType* inputImage,
		     const IndexType currentIdx);

  void printHist(const HistogramType *H);

  void GetDirAndOffset(const IndexType LineStart, 
		       const IndexType PrevLineStart,
		       const int ImageDimension,
		       OffsetType &LineOffset,
		       OffsetType &Changes,
		       int &LineDirection);

  bool useVectorBasedHistogram()
  {
    // bool, short and char are acceptable for vector based algorithm: they do not require
    // too much memory. Other types are not usable with that algorithm
    return typeid(InputPixelType) == typeid(unsigned char)
      || typeid(InputPixelType) == typeid(signed char)
      || typeid(InputPixelType) == typeid(unsigned short)
      || typeid(InputPixelType) == typeid(signed short)
      || typeid(InputPixelType) == typeid(bool);
  }


private:
  MovingHistogramRankImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  float m_Rank;

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
#include "itkMovingHistogramRankImageFilter.txx"
#endif

#endif


