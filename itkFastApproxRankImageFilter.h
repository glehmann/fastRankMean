#ifndef __itkFastApproxRankImageFilter_h
#define __itkFastApproxRankImageFilter_h

#include "itkSeparableImageFilter.h"
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
public SeparableImageFilter<TInputImage, TOutputImage, RankImageFilter<TInputImage, TInputImage, Neighborhood<bool, TInputImage::ImageDimension> > >
{
public:
  /** Standard class typedefs. */
  typedef FastApproxRankImageFilter Self;
  typedef SeparableImageFilter<TInputImage, TOutputImage, RankImageFilter<TInputImage, TInputImage, Neighborhood<bool, TInputImage::ImageDimension> > >  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(FastApproxRankImageFilter,
               SeparableImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::FilterType FilterType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

  void SetRank( float );
  itkGetMacro(Rank, float);

protected:
  FastApproxRankImageFilter();
  ~FastApproxRankImageFilter() {};

private:
  FastApproxRankImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  float m_Rank;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastApproxRankImageFilter.txx"
#endif

#endif
