#ifndef __itkFastApproxRankImageFilter_txx
#define __itkFastApproxRankImageFilter_txx

namespace itk {

template <class TInputImage>
FastApproxRankImageFilter<TInputImage>
::FastApproxRankImageFilter()
{
  this->SetRank( 0.5 );
}


template<class TInputImage>
void
FastApproxRankImageFilter<TInputImage>
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
