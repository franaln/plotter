/** @file obj.cxx
    @brief Obj class implementation
*/

#include <iostream>

#include "obj.h"

Obj::Obj(Obj *obj1, Obj *obj2, std::string operation)
{
  if(operation == "ratio"){
    TH1 *h_ratio = (TH1*)obj1->GetHist()->Clone("h_ratio");
    h_ratio->Divide(obj2->GetHist());

    m_type = Hist;
    m_hist = h_ratio;
    m_opts = "";
  }
  else if(operation == "difference"){
    TH1 *h_diff = (TH1*)obj1->GetHist()->Clone("h_diff");
    h_diff->Add(obj2->GetHist(), -1.0);
    h_diff->Divide(obj1->GetHist());

    m_type = Hist;
    m_hist = h_diff;
    m_opts = "";
  }
  else if(operation == "efficiency"){
    TGraphAsymmErrors *gr = new TGraphAsymmErrors();

    gr->Divide(obj2->GetHist(), obj1->GetHist(), "cl=0.683 b(0.5,0.5) mode");

    m_type = Graph;
    m_graph = gr;
    m_opts = "";
  }
}

TString Obj::GetName()
{
  if(m_type == Hist)
    return m_hist->GetName();
  return m_graph->GetName();
}

double Obj::GetMinX()
{
  if(m_type == Hist)
    return (double)m_hist->GetXaxis()->GetXmin();
  else
    return 0.0;
}

double Obj::GetMaxX()
{
  if(m_type == Hist)
    return (double)m_hist->GetXaxis()->GetXmax();
  else
    return 0.0;
}

double Obj::GetMinY()
{
  if(m_type == Hist)
    return (double)m_hist->GetMinimum();
  else
    return 0.0;
}

double Obj::GetMaxY()
{
  if(m_type == Hist)
    return (double)m_hist->GetMaximum();
  else
    return 0.0;
}

void Obj::SetTitleX(TString title)
{
  if(m_type == Hist)
    m_hist->GetXaxis()->SetTitle(title);
}

void Obj::SetTitleY(TString title)
{
  if(m_type == Hist)
    m_hist->GetYaxis()->SetTitle(title);
}

void Obj::SetRangeX(double min, double max)
{
  if(m_type == Hist)
    m_hist->GetXaxis()->SetRangeUser(min, max);
}

void Obj::SetRangeY(double min, double max)
{
  if(m_type == Hist)
    m_hist->GetYaxis()->SetRangeUser(min, max);
}

void Obj::SetStyle()
{
  if(m_type == Hist){
    m_hist->SetMarkerStyle(20);
    m_hist->SetMarkerSize(0.8);
    m_hist->SetLineWidth(2);
  }
  else if(m_type == Graph){
    m_hist->SetMarkerStyle(20);
    m_hist->SetMarkerSize(0.8);
    m_opts += "PZT";
  }

}

void Obj::SetColor(Color_t colour, bool fill)
{
  if(m_type == Hist) {
    m_hist->SetLineColor(colour);
    m_hist->SetMarkerColor(colour);
    if(fill) m_hist->SetFillColor(colour);
  }
  else if(m_type == Graph) {
    m_graph->SetLineColor(colour);
    m_graph->SetMarkerColor(colour);
   }
}

void Obj::Rebin(int group)
{
  if(m_type == Hist) m_hist->Rebin(group);
  else return;
}

double Obj::Integral()
{
  if(m_type == Hist) return m_hist->Integral();
  else return 0.0;
}

void Obj::NormaliseTo(double norm)
{
  if(m_type == Graph) return;

  m_hist->Scale(norm/m_hist->Integral());
}

void Obj::Draw(TString options)
{
  if(m_type == Hist) {
    m_hist->Draw(options+m_opts);
  }
  else if(m_type == Graph) {
    m_graph->Draw(options+m_opts);
  }
}
