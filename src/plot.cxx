/** @file plot.cxx
    @brief Plot class implementation
*/

#include <iostream>
#include <TGraph.h>
#include <TH1.h>

#include "common.h"
#include "obj.h"
#include "plot.h"

Plot::Plot()
{
  m_name = Form("plot_%i", number_of_plot);
  m_canvas = new TCanvas(m_name, m_name, 800, 600);

  rebin = 0;
  draw_options = "";

  show_stats = false;
  include_ratio = false;
  include_diff = false;
  do_logx = false;
  do_logy = false;
  do_normalise = false;
  do_normalise_to_first = false;

  number_of_plot++;
}

Plot::~Plot()
{
  if(m_canvas) delete m_canvas;
  if(m_legend) delete m_legend;
}

void Plot::Add(Obj *obj, Color_t colour, bool fill)
{
  obj->SetColor(colour, fill);
  m_list.push_back(obj);
}

void Plot::Save()
{
  if(!m_canvas || !m_canvas->IsOnHeap())
    return;

  m_canvas->Print(m_name+".eps");
}

void Plot::Configure()
{

  //-- Rebin
  if(rebin > 1){
    for(int k=0; k<m_list.size(); k++){
      m_list[k]->Rebin(rebin);
    }
  }

  //-- Axis
  x_min = 1e20;
  x_max = -x_min;
  y_min = 1e20;
  y_max = -y_min;

  for(int k=0; k<m_list.size(); k++){
    if(m_list[k]->GetMinX() < x_min) x_min = m_list[k]->GetMinX();
    if(m_list[k]->GetMaxX() > x_max) x_max = m_list[k]->GetMaxX();
    if(m_list[k]->GetMinY() < y_min) y_min = m_list[k]->GetMinY();
    if(m_list[k]->GetMaxY() > y_max) y_max = m_list[k]->GetMaxY();
  }

  y_max *= 1.1;

  if(m_canvas->GetLogy()){
    if(y_min > 0) y_min *= .9;
    else          y_min = y_max * 1.e-3;
  }
  else if(y_min >= 0) y_min = 1.e-3;

  if(do_normalise || do_normalise_to_first) y_max /= m_list[0]->Integral();

  // Check if all the histos are the same histo (but from different files :P)
  bool same_names = true;
  for(int k=1; k<m_list.size(); k++){
    if(m_list[k]->GetName() != m_list[k]->GetName()){
      same_names = false;
      break;
    }
  }

  m_list[0]->SetRangeY(y_min, y_max);
  if(m_list.size() == 1 || same_names) m_list[0]->SetTitleX(m_list[0]->GetName());

  //-- Configure all objects
  for(int k=0; k<m_list.size(); k++){

    m_list[k]->SetStyle();

    m_list[k]->SetStats(show_stats);

    if(do_normalise && do_normalise_to_first){
      error("I'm confused o.O! Both normalise check buttons are selected. I'll do what I want.");
    }
    else if(do_normalise){
      m_list[k]->NormaliseTo(1.0);
    }
    else if(do_normalise_to_first && m_list.size() != 0){
      m_list[k]->NormaliseTo(m_list[0]->Integral());
    }

  }

}

void Plot::Create()
{
  if(m_list.size() == 0) return;

  // for(int k=0; k<m_list.size(); k++){
  //   if(m_list[k]->GetEntries() == 0) {
  //     error(m_list[k]->GetName() << " is empty.");
  //     continue;
  //   }
  // }

  Configure();

  if(include_ratio){
    TPad *up   = new TPad("upperPad", "upperPad", .001, .29, .999, .999);
    TPad *down = new TPad("lowerPad", "lowerPad", .001, .001,  .999, .28);

    up->SetLeftMargin(0.08);
    up->SetRightMargin(0.05);
    up->SetBottomMargin(0.01);
    up->SetTopMargin(0.06);

    down->SetLeftMargin(0.08);
    down->SetRightMargin(0.05);
    down->SetBottomMargin(0.2);
    down->SetTopMargin(0.01);

    if(do_logx) up->SetLogx();
    if(do_logy) up->SetLogy();

    up->Draw();
    down->Draw();

    up->cd();
    Draw();

    down->cd();
    //DrawRatios();
  }
  else if(include_diff){
    TPad *up   = new TPad("upperPad", "upperPad", .001, .29, .999, .999);
    TPad *down = new TPad("lowerPad", "lowerPad", .001, .001,  .999, .28);

    up->SetLeftMargin(0.08);
    up->SetRightMargin(0.05);
    up->SetBottomMargin(0.01);
    up->SetTopMargin(0.06);

    down->SetLeftMargin(0.08);
    down->SetRightMargin(0.05);
    down->SetBottomMargin(0.2);
    down->SetTopMargin(0.01);

    if(do_logx) up->SetLogx();
    if(do_logy) up->SetLogy();

    up->Draw();
    down->Draw();

    up->cd();
    Draw();

    down->cd();
    //DrawDiffs();
  }
  else {
    if(do_logx) m_canvas->SetLogx();
    if(do_logy) m_canvas->SetLogy();
    Draw();
  }

  return;
}

void Plot::Draw()
{
  m_list[0]->Draw(draw_options);
  for(int i=1; i<m_list.size(); i++){
    m_list[i]->Draw(draw_options+"same");
  }
}

void Plot::DrawRatios()
{
  int n_ratios = m_list.size() - 1;

  if(n_ratios == 0) return;

  Obj *ratio[n_ratios];
  for(int k=0; k< n_ratios; k++){
    ratio[k] = new Obj(m_list[k+1], m_list[0], "ratio");
  }

  if(!ratio[0]) {
    error("There are no ratios to plot.");
    return;
  }

  ratio[0]->SetTitleY("Ratio");
  // ratio[0]->SetStats(0);
  // ratio[0]->GetXaxis()->SetRangeUser(x_min,x_max);
  // ratio[0]->GetYaxis()->CenterTitle();
  // ratio[0]->GetXaxis()->SetTitleSize( 0.08 );
  // ratio[0]->GetXaxis()->SetLabelSize( 0.08 );
  // ratio[0]->GetYaxis()->SetLabelSize( 0.08 );
  // ratio[0]->GetYaxis()->SetTitleSize( 0.08 );
  // ratio[0]->GetYaxis()->SetTitleOffset( 0.4 );
  // ratio[0]->GetXaxis()->SetTitleOffset( 1.1 );

  for(int n=0; n<n_ratios; n++){
    //    ratio[n]->SetColor(m_colours[n+1]);

    if(n==0) ratio[n]->Draw();
    else ratio[n]->Draw("same");
  }
}

/* Draw the relative differences of the selected histograms with respect the first one.
   diff = (hN - h1)/h1 */
// void Plot::DrawDiffs()
// {
//   int n_diffs = m_list.size() - 1;

//   if(n_diffs==0) return;

//   TH1 *diff[n_diffs];
//   for(int k=0; k<n_diffs; k++){
//     diff[k] = CreateRelativeDiff(k+1, 0);
//   }

//   if(!diff[0]) {
//     error("There are no differences to plot.");
//     return;
//   }

//   diff[0]->GetYaxis()->SetTitle("Relative difference");
//   diff[0]->SetStats(0);
//   diff[0]->GetXaxis()->SetRangeUser(x_min,x_max);
//   diff[0]->GetYaxis()->CenterTitle();
//   diff[0]->GetXaxis()->SetTitleSize( 0.08 );
//   diff[0]->GetXaxis()->SetLabelSize( 0.08 );
//   diff[0]->GetYaxis()->SetLabelSize( 0.08 );
//   diff[0]->GetYaxis()->SetTitleSize( 0.08 );
//   diff[0]->GetYaxis()->SetTitleOffset( 0.4 );
//   diff[0]->GetXaxis()->SetTitleOffset( 1.1 );

//   // GetColours();
//   for(int n=0; n<n_diffs; n++){
//     //diff[n]->SetMarkerColor(colours[n+1]);
//     //diff[n]->SetLineColor(colours[n+1]);

//     if(n==0) diff[n]->Draw();
//     else diff[n]->Draw("same");
//   }
// }

void Plot::DrawLegend()
{
  std::vector<TString> legend;

  if(m_list.size() == 1){ //1 solo histo de 1 solo file, return sin legend
    return;
  }

  // Legend config
  //vector<int> hsv = get_number_of_objects_in_each_file();

  // bool mtitle=false; bool mfile=false; bool mtitlefile=false;
  // if( hsv[0]==1 && hsv[1]==1 ){ //1 solo histo de >1 files
  //   mfile=true;
  // }
  // else if(hsv[0] && !hsv[1]){ // >1 histo de 1 solo file
  //   mtitle=true;
  // }
  // else if(hsv[0] && hsv[1] ){ // >1 histo de >1 file
  //   mtitlefile=true;
  // }

  // vector<TString> legtemp;
  // for(unsigned int k=0; k<items_sel.size(); k++){

  //     TString tmp = "";
  //     if(mfile){
  //       tmp = fb[items_sel[k]->get_file()]->get_header_text();
  //     }
  //     else if(mtitle){
  //       items_sel[k]->get_legend_text();
  //     }
  //     else if(mtitlefile){
  //       tmp = " (" + fb[items_sel[k]->get_file()]->get_header_text() + ")";
  //       tmp=items_sel[k]->get_legend_text()+tmp;
  //     }
  //     legend.push_back(tmp);
  //     legtemp.push_back(tmp);
  // }

  // if(type==Plot::Ratio){
  //   for(unsigned int i=1;i<legend.size();i++){
  //     legend[i] += "/";
  //     legend[i] += legend[0];
  //   }
  // }

  // // legend size
  // sort(legtemp.begin(), legtemp.end());
  // reverse(legtemp.begin(),legtemp.end());

  // Double_t maxwidth = legtemp[0].Sizeof() * 0.01;
  // Double_t maxheight = items_sel.size() * 0.035;

  // Double_t xmin, xmax, ymin, ymax;

  // xmax = 0.86; ymax = 0.86;
  // ymin = (ymax - maxheight)>0.2 ? ymax - maxheight : 0.2;
  // xmin = (xmax - maxwidth)>0.2  ? xmax - maxwidth  : 0.2;

  // // Create and plot legend
  // TLegend *leg = new TLegend(xmin, ymin, xmax, ymax);
  // leg->SetFillColor(0);
  // unsigned int begin = type!=Plot::Normal ? 1 : 0;
  // for(unsigned int k=begin; k<items_sel.size(); k++){
  //   leg->AddEntry(plot_list->At(k), legend[k]);
  // }
  // leg->Draw();


  // if(m_macroRecording){
  //   macro->add_legend(legend);
  // }

  //   lnk = (TObjOptLink*)lnk->Next();
  //   k++;


//   return;
}

void Plot::DrawEfficiency()
{
  Obj *eff = new Obj(m_list[0], m_list[1], "efficiency");

  eff->SetTitleX("");
  eff->SetTitleY("Efficiency");

  eff->SetColor(1, false);
  eff->SetStyle();

  // gr->Draw("PAZT");
}

void Plot::Dump()
{
  //   //HistoInfo *temp = new HistoInfo(items_sel[k]->GetFile(), items_sel[k]->GetName(), items_sel[k]->GetText());
  //     temp->SetDrawOptions(lnk->GetOption());
  //     temp->SetColour(colours[k]);
  //     Int_t rebin = nentryRebin->GetIntNumber();
  //     if(rebin > 1) temp->SetRebinNumber(nentryRebin->GetIntNumber());
  //     if(checkNormalise->GetState() ) temp->SetScaleFactor(1/h->Integral());
  //     if(checkNormalise2->GetState()) temp->SetScaleFactor(((TH1*)plot_list->At(0))->Integral()/h->Integral());
  //     //        macro->AddHisto(temp);
}

int Plot::number_of_plot = 0;
