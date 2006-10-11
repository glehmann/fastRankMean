#ifndef __itkFastApproxMaskRankImageFilter_h
#define __itkFastApproxMaskRankImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkMaskedMovingHistogramRankImageFilter.h"
#include "itkMaskedExternMovingHistogramRankImageFilter.h"
#include "itkNotImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include "itkImageFileWriter.h"

namespace itk {

/**
 * \class FastApproxMaskRankImageFilter
 * \brief A separable rank filter
 * 
 * Medians aren't separable, but if you want a large robust smoother
 * to be relatively quick then it is worthwhile pretending that they
 * are.
 *
 * \author Richard Beare
 */

template<class TInputImage, class TMaskImage, class TOutputImage>
class ITK_EXPORT FastApproxMaskRankImageFilter : 
public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FastApproxMaskRankImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(FastApproxMaskRankImageFilter,
               ImageToImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef TMaskImage MaskImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
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

  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

  itkSetMacro(Radius, RadiusType);
  itkGetMacro(Radius, RadiusType);
  itkSetMacro(Rank, float);
  itkGetMacro(Rank, float);

  itkSetMacro(WriteInsideMask, bool);
  itkGetMacro(WriteInsideMask, bool);

  void GenerateInputRequestedRegion() ;

protected:
  FastApproxMaskRankImageFilter();
  ~FastApproxMaskRankImageFilter() {};

  void GenerateData();

private:
  FastApproxMaskRankImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  float m_Rank;
  RadiusType m_Radius;

  bool m_WriteInsideMask;

  typedef typename itk::Neighborhood<bool, TInputImage::ImageDimension> KernelType;

  KernelType m_kernels[TInputImage::ImageDimension];
  
  typedef typename itk::MaskedMovingHistogramRankImageFilter<TInputImage, 
							     TMaskImage, 
							     TOutputImage, 
							     KernelType> RankType1;
  typedef typename itk::MaskedMovingHistogramRankImageFilter<TOutputImage, 
							     TMaskImage, 
							     TOutputImage, 
							     KernelType> RankType2;
  typename RankType1::Pointer m_firstFilt;
  typename RankType2::Pointer m_otherFilts[TInputImage::ImageDimension - 1];

  // the stuff for filling holes
  typedef typename itk::MaskedExternMovingHistogramRankImageFilter<TInputImage, 
								   TMaskImage, 
								   TInputImage, 
								   KernelType> ERankType1;

  typename ERankType1::Pointer m_EFilts[TInputImage::ImageDimension];

  typedef typename itk::MaskNegatedImageFilter<TInputImage, TMaskImage, TOutputImage> MaskType;

  typedef typename itk::ImageFileWriter<TMaskImage> WriterType;
  typename WriterType::Pointer m_Writer;
  typename MaskType::Pointer m_MaskFilt;
  
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastApproxMaskRankImageFilter.txx"
#endif

#endif
