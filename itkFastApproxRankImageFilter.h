#ifndef __itkFastApproxRankImageFilter_h
#define __itkFastApproxRankImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkRankImageFilter.h"

namespace itk {

/**
 * \class FastApproxRankImageFilter
 * \brief A separable rank filter
 * 
 * Medians aren't separable, but if you want a large robust smoother
 * to be relatively quick then it is worthwhile pretending that they
 * are.
 *
 * \author Richard Beare
 */

template<class TInputImage, class TOutputImage>
class ITK_EXPORT FastApproxRankImageFilter : 
public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FastApproxRankImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(FastApproxRankImageFilter,
               ImageToImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename TInputImage::PixelType InputPixelType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

  itkSetMacro(Radius, RadiusType);
  itkGetMacro(Radius, RadiusType);
  itkSetMacro(Rank, float);
  itkGetMacro(Rank, float);

  void GenerateInputRequestedRegion() ;

  virtual void Modified() const;

protected:
  FastApproxRankImageFilter();
  ~FastApproxRankImageFilter() {};

  void GenerateData();

private:
  FastApproxRankImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  float m_Rank;
  RadiusType m_Radius;

  typedef typename itk::Neighborhood<bool, TInputImage::ImageDimension> KernelType;

  KernelType m_kernels[TInputImage::ImageDimension];

  typedef typename itk::RankImageFilter<TInputImage, 
						       TOutputImage, 
						       KernelType> RankType1;
  typedef typename itk::RankImageFilter<TOutputImage, 
						       TOutputImage, 
						       KernelType> RankType2;
  typename RankType1::Pointer m_firstFilt;
  typename RankType2::Pointer m_otherFilts[TInputImage::ImageDimension - 1];
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastApproxRankImageFilter.txx"
#endif

#endif
