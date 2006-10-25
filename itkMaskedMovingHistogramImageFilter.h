/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMaskedMovingHistogramImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMaskedMovingHistogramImageFilter_h
#define __itkMaskedMovingHistogramImageFilter_h

#include "itkMovingHistogramImageFilter.h"
#include <list>
#include <map>
#include <set>
#include "itkOffsetLexicographicCompare.h"

namespace itk {

/**
 * \class MaskedMovingHistogramImageFilter
 *
 * \author Richard Beare
 * \author Gaetan Lehmann
 */

template<class TInputImage, class TMaskImage, class TOutputImage, class TKernel, class THistogram >
class ITK_EXPORT MaskedMovingHistogramImageFilter : 
    public MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, THistogram>
{
public:
  /** Standard class typedefs. */
  typedef MaskedMovingHistogramImageFilter Self;
  typedef MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, THistogram>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MaskedMovingHistogramImageFilter, 
               MovingHistogramImageFilter);
  
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
  typedef THistogram HistogramType;

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

  /** MaskedMovingHistogramImageFilterBase need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

  /** Get the modified mask image */
  MaskImageType * GetOutputMask();

  void AllocateOutputs();

  DataObject::Pointer MakeOutput(unsigned int idx);

  itkSetMacro(FillValue, OutputPixelType);
  itkGetMacro(FillValue, OutputPixelType);

  itkSetMacro(MaskValue, MaskPixelType);
  itkGetMacro(MaskValue, MaskPixelType);

  itkSetMacro(BackgroundMaskValue, MaskPixelType);
  itkGetMacro(BackgroundMaskValue, MaskPixelType);

  void SetGenerateOutputMask( bool );
  itkGetMacro(GenerateOutputMask, bool);
//   itkBooleanMacro(GenerateOutputMask);

protected:
  MaskedMovingHistogramImageFilter();
  ~MaskedMovingHistogramImageFilter() {};

  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const OutputImageRegionType& 
                              outputRegionForThread,
                              int threadId) ;
  
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  void pushHistogram(HistogramType *histogram, 
		     const OffsetListType* addedList,
		     const OffsetListType* removedList,
		     const RegionType &inputRegion,
		     const RegionType &kernRegion,
		     const InputImageType* inputImage,
		     const MaskImageType *maskImage,
		     const IndexType currentIdx);

private:
  MaskedMovingHistogramImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_GenerateOutputMask;

  OutputPixelType m_FillValue;

  MaskPixelType m_MaskValue;

  MaskPixelType m_BackgroundMaskValue;

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaskedMovingHistogramImageFilter.txx"
#endif

#endif


