/** @file plotobj.cxx
    @brief PlotObj class implementation
*/

#include <iostream>

#include "plotobj.h"

Double_t PlotObj::GetMinX()
{
  if(m_type == Hist)
    return m_hist->GetXaxis()->GetXmin();
  else
    return 0.0;
}

Double_t PlotObj::GetMaxX()
{
  if(m_type == Hist)
    return m_hist->GetXaxis()->GetXmax();
  else
    return 0.0;
}

Double_t PlotObj::GetMinY()
{
  if(m_type == Hist)
    return m_hist->GetMinimum();
  else
    return 0.0;
}

Double_t PlotObj::GetMaxY()
{
  if(m_type == Hist)
    return m_hist->GetMaximum();
  else
    return 0.0;
}

void PlotObj::SetColour(Color_t colour)
{
  if(m_type == Hist) {
    m_hist->SetLineColor(colour);
    m_hist->SetMarkerColor(colour);
  }
  else if(m_type == Graph) {
    m_graph->SetLineColor(colour);
    m_graph->SetMarkerColor(colour);
   }
}

void PlotObj::Draw()
{
  if(m_type == Hist) m_hist->Draw();
  else if(m_type == Graph) m_graph->Draw();
}
