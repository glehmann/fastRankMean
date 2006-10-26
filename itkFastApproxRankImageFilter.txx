#ifndef __itkFastApproxRankImageFilter_txx
#define __itkFastApproxRankImageFilter_txx

namespace itk {

template <class TInputImage, class TOutputImage>
FastApproxRankImageFilter<TInputImage, TOutputImage>
::FastApproxRankImageFilter()
{
  this->SetRank( 0.5 );
}


template<class TInputImage, class TOutputImage>
void
FastApproxRankImageFilter<TInputImage, TOutputImage>
::SetRank( float rank )
{
  if( m_Rank != rank )
    {
    m_Rank = rank;
    for (unsigned i = 0; i < TInputImage::ImageDimension - 1; i++)
      {
      this->m_Filters[i]->SetRank( m_Rank );
      }
    this->Modified();
    }
}

}


#endif
