/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMaskedMovingHistogramRankImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMaskedExternMovingHistogramRankImageFilter_h
#define __itkMaskedExternMovingHistogramRankImageFilter_h

#include "itkImageToImageFilter.h"
#include <list>
#include <map>
#include <set>
#include "itkOffsetLexicographicCompare.h"
#include "itkRankHistogramMask.h"

namespace itk {

/**
 * \class MaskedExternMovingHistogramRankImageFilter
 * \brief Rank filter of a greyscale image restricted to a mask
 *
 * This is a version that is meant to expand a region defined by a
 * mask. It returns an updated mask for use in a separable filter.
 *
 * \author Richard Beare
 */

template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel >
class ITK_EXPORT MaskedExternMovingHistogramRankImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef MaskedExternMovingHistogramRankImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MaskedExternMovingHistogramRankImageFilter, 
               ImageToImageFilter);
  
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef TMaskImage MaskImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename TOutputImage::PixelType OutputPixelType ;
  typedef typename TInputImage::PixelType InputPixelType ;
  typedef typename MaskImageType::PixelType MaskPixelType;

     /** Set the marker image */
  void SetMaskImage(MaskImageType *input)
     {
     // Process object is not const-correct so the const casting is required.
     this->SetNthInput( 1, const_cast<TMaskImage *>(input) );
     }

  /** Get the marker image */
  MaskImageType * GetMaskImage()
    {
      return static_cast<MaskImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
    }


   /** Set the input image */
  void SetInput1(InputImageType *input)
     {
     this->SetInput( input );
     }

   /** Set the marker image */
  void SetInput2(MaskImageType *input)
     {
     this->SetMaskImage( input );
     }

  /** Get the modified mask image */
  MaskImageType * GetOutputMask();

  /** Get the result image */
  OutputImageType * GetOutputImage();
  

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
  itkSetMacro(Rank, float);
  itkGetMacro(Rank, float);

  /** MaskedExternMovingHistogramRankImageFilterBase need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;
  void AllocateOutputs();

#if 1
  /**  Create the Output */
  DataObject::Pointer MakeOutput(unsigned int idx);
#endif

protected:
  MaskedExternMovingHistogramRankImageFilter();
  ~MaskedExternMovingHistogramRankImageFilter() {};

  typedef RankHistogramMask<InputPixelType> HistogramType;
  
  typedef RankHistogramMaskVec<InputPixelType, std::less< InputPixelType> > VHistogram;
  typedef RankHistogramMaskMap<InputPixelType, std::less< InputPixelType>  > MHistogram;
  
  
  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const OutputImageRegionType& 
                              outputRegionForThread,
                              int threadId) ;
  
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** kernel or structuring element to use. */
  KernelType m_Kernel ;
  OutputPixelType m_FillValue;

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
		     const MaskImageType *maskImage,
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
//       || typeid(InputPixelType) == typeid(unsigned short)
//       || typeid(InputPixelType) == typeid(signed short)
      || typeid(InputPixelType) == typeid(bool);
  }


private:
  MaskedExternMovingHistogramRankImageFilter(const Self&); //purposely not implemented
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
#include "itkMaskedExternMovingHistogramRankImageFilter.txx"
#endif

#endif


