/** @file plot.cxx
    @brief Plot class implementation
*/

#include <iostream>
#include <TGraph.h>
#include <TH1.h>

#include "plot.h"

Plot::Plot()
{
  m_name = Form("plot_%i", number_of_plot);
  m_canvas = new TCanvas(m_name, m_name, 800, 600);
  m_list = new TList();

  number_of_plot++;
}

Plot::~Plot()
{
  if(m_canvas) delete m_canvas;
  if(m_legend) delete m_legend;
  if(m_list) delete m_list;
}

void Plot::Add(TObject *obj, Color_t colour)
{
  m_list->Add(obj);
  m_colours.push_back(colour);
}

void Plot::Show()
{
  Create();
  return;
}

void Plot::Save()
{
  //TCanvas *old = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(m_name);
  //if (!old || !old->IsOnHeap())
  //return;
  if(!m_canvas || !m_canvas->IsOnHeap())
    return;

  m_canvas->Print(m_name);
}

void Plot::Create()
{
  if(m_list->GetSize() == 0) return;

  //-- Axis
  x_min = 1e300;
  x_max = -x_min;
  y_min = 1e300;
  y_max = -y_min;

  TIter next(m_list);
  while((TH1 *h = next())) {
    //obj->Draw(next.GetOption());

    // TObjOptLink *lnk = (TObjOptLink*)m_list->FirstLink();
    // while (lnk) {

    //TObject *obj = lnk->GetObject();

    //    if(obj->InheritsFrom("TH1")){
    if (h->GetXaxis()->GetXmin() < x_min) x_min = h->GetXaxis()->GetXmin();
    if (h->GetXaxis()->GetXmax() > x_max) x_max = h->GetXaxis()->GetXmax();
    if (h->GetMinimum() < y_min) y_min = h->GetMinimum();
    if (h->GetMaximum() > y_max) y_max = h->GetMaximum();
    // }
    // else if(obj->InheritsFrom("TGraph")){
    //   TGraph *g = (TGraph*)obj;
    //   if (g->GetXaxis()->GetXmin() < x_min) x_min = g->GetXaxis()->GetXmin();
    //   if (g->GetXaxis()->GetXmax() > x_max) x_max = g->GetXaxis()->GetXmax();

    //   Double_t x = 0; Double_t y = 0;
    //   for(int i=0; i<g->GetN(); i++){
    //     g->GetPoint(i, x, y);
    //     if (y < y_min) y_min = y;
    //     if (y > y_max) y_max = y;
    //   }
    //   delete g;
    // }
    //lnk = (TObjOptLink*)lnk->Next();
  }

  y_max *= 1.1;


  //TH1 *h; TGraph *g;

  //if(do_normalise || do_normalise_to_first) y_max /= h->Integral();

  if(m_canvas->GetLogy()){
    if(y_min > 0)  y_min *= .9;
    else           y_min = y_max * 1.e-3;
  }
  else if(y_min >= 0) y_min = 1.e-3;


  //-- Configure first object
  //bool first_is_histo = m_list->At(0)->InheritsFrom("TH1");
  //bool first_is_graph = m_list->At(0)->InheritsFrom("TGraph");

  // Check if all the histos are the same histo (but from different files :P)
  bool same = true;
  for(int i=1; i<m_list->GetSize(); i++){
    if(m_list->At(i)->GetName() != m_list->At(0)->GetName()){
      same = false;
      break;
    }
  }

  //if(first_is_histo){

  TH1 *h = (TH1*)m_list->At(0);

  // y axis
  h->GetYaxis()->SetRangeUser(y_min, y_max);

  // x axis title
  if(m_list->GetSize() == 1) h->GetXaxis()->SetTitle(h->GetName());
  else if(same) h->GetXaxis()->SetTitle(h->GetName());

  if(show_stats) h->SetStats(1);
  else           h->SetStats(0);

  // }
  // else if(first_is_graph){
  //   g = (TGraph*)m_list->At(0);

  //   // y axis
  //   g->GetYaxis()->SetRangeUser(y_min, y_max);

  //   // x axis title
  //   if(m_list->GetSize() == 1) g->GetXaxis()->SetTitle(m_list->At(0)->GetName());
  //   else if(same) g->GetXaxis()->SetTitle(m_list->At(0)->GetName());
  // }

  TIter next(m_list);
  while((TH1 *h = next())) {
    // bool is_histo = lnk->GetObject()->InheritsFrom("TH1");
    // bool is_graph = lnk->GetObject()->InheritsFrom("TGraph");

    //    if(is_histo){
    h->SetLineColor(m_colours[k]);
    h->SetMarkerColor(m_colours[k]);
    //if(check_fill[k]->GetState()) h->SetFillColor(colours[k]);
    //h->SetMarkerStyle(marker_style);
    //h->SetMarkerSize(marker_size);
    //     h->SetLineWidth(line_width);
    // }
    // else if(is_graph){
    //   g = (TGraph*)obj;
    //   g->SetLineColor(m_colours[k]);
    //   g->SetMarkerColor(m_colours[k]);
    //     g->SetMarkerStyle(marker_style);
    //     g->SetMarkerSize(marker_size);
    //     g->SetLineWidth(line_width);
    //}
  }

  // if(items_sel[k]->IsTypeHist()){

  //   h = (TH1*)GetObject(items_sel[k]);

  //   if( h->GetEntries() == 0 ) {
  //     error(items_sel[k]->GetName() << " is empty.");
  //     continue;
  //   }

  //   if(!h) continue;

  //   h->SetName(items_sel[k]->GetName());
  //   h->SetTitle(items_sel[k]->GetName());

  //   Bool_t is2D = h->IsA()->InheritsFrom(TH2::Class());
  //   Bool_t is3D = h->IsA()->InheritsFrom(TH3::Class());
  //   Bool_t is1D = kFALSE; if(!is2D && !is3D) is1D = kTRUE;

  //   TH2 *h2; TH3 *h3;
  //   if(is2D) h2 = (TH2*) h;
  //   if(is3D) h3 = (TH3*) h;

  //   if(check_normalise->GetState() && check_normalise2->GetState())
  //     error("I'm confused o.O! Both normalise check buttons are selected. I'll do what I want.");
  //   if(check_normalise->GetState())   h->Scale(1/h->Integral());
  //   if(check_normalise2->GetState() && plot_list->GetSize()!=0)
  //     h->Scale( ((TH1*)plot_list->At(0))->Integral()/h->Integral());

  //   Int_t rebin = nentry_rebin->GetIntNumber();
  //   if(rebin > 1){
  //     if(is1D)
  //       h->Rebin(rebin);
  //     else if(is2D){
  //       h2->RebinX(rebin);
  //       h2->RebinY(rebin);
  //     }
  //     else if(is3D){
  //       h3->RebinX(rebin);
  //       h3->RebinY(rebin);
  //       h3->RebinZ(rebin);
  //     }
  //   }

  // Draw options
  //   TString draw_opt = "";
  //   if(check_text->GetState()) draw_opt += "text";

  //   if(is1D){
  //     if(check_hist->GetState()) draw_opt += "hist";
  //     if(check_p->GetState()) draw_opt += "P";
  //     if(check_pie->GetState()) draw_opt += "PIE";
  //   }
  //   else if(is2D){
  //     if(radio_scatter->GetState())  draw_opt += "scat";
  //     else if(radio_box->GetState()) draw_opt += "box";
  //     else  draw_opt += "colz";
  //   }

    //   if(check_text->GetState()) draw_opt += "text";
    //   if(k!=0) draw_opt += "same";

    //   if(is1D) plot_list->Add(h, draw_opt);
    //   else if(is2D) plot_list->Add(h2, draw_opt);
    //   else if(is3D) plot_list->Add(h3, draw_opt);

    // }
    // else if( items_sel[k]->IsTypeGraph() ){

    //   TGraph *g = (TGraph*)GetObject(items_sel[k]);
    //   if(!g) continue;

    //   g->SetName(items_sel[k]->GetName());
    //   g->SetTitle(items_sel[k]->GetName());

    //   TString draw_opt = "";
    //   draw_opt += "PZT";
    //   if(k==0) draw_opt += "A";
    //   else draw_opt += "same";
    //   plot_list->Add(g, draw_opt);
    // }


    // if((type == Plot::Ratio || type == Plot::Efficiency) && items_sel.size()<2) return;

    // Plot order: 1) Selected order (default). 2) Order by file and entry.
    //if(checkOrder->GetState())  sort(items_sel.begin(), items_sel.end(), SortItems);


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
    PlotHistos();

    down->cd();
    PlotRatios();
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
    PlotHistos();

    down->cd();
    PlotRelativeDiffs();
  }
  //else {
  //if(_macroRecording) macro->AddCanvas(canvas[nro]->GetName());
  //if(checkLogX->GetState()) canvas[nro]->SetLogx();
  //       if(checkLogY->GetState()) canvas[nro]->SetLogy();


  TIter next(m_list);
  TObject *obj;
  while((obj = next())){
    obj->Draw(obj->GetOption());
  }


  return;
}

void Plot::CreateLegend()
{
  // vector<TString> legend;

  // if( items_sel.size()==1 ){ //1 solo histo de 1 solo file, return sin legend
  //   return;
  // }

  // // Legend config
  // vector<int> hsv = get_number_of_objects_in_each_file();

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

// bool Plotter::PlotEfficiency()
// {
//   // TGraphAsymmErrors *gr = CreateEfficiency();
//   // if(!gr) { error("No se pudo crear el TGraphAsymmErrors."); return false; }

//   // gr->GetXaxis()->SetTitle("");
//   // gr->GetYaxis()->SetTitle("Efficiency");

//   // GetColours();

//   // gr->SetMarkerColor(colours[0]);
//   // gr->SetLineColor(colours[0]);
//   // gr->SetMarkerStyle(marker_style);
//   // gr->SetMarkerSize(marker_size);
//   // gr->SetLineWidth(line_width);

//   // gr->Draw("PAZT");

//   return true;
// }

// bool Plotter::PlotRatios(bool down)
// {
//   // Int_t nRatios = items_sel.size() - 1;

//   // if(nRatios==0) return false;

//   // TH1 *ratio[nRatios];
//   // for(int k=0; k<nRatios; k++){
//   //   ratio[k] = CreateRatio(k+1, 0, down);
//   // }

//   // if(!ratio[0]) {
//   //   error("There are no ratios to plot.");
//   //   return false;
//   // }

//   // ratio[0]->GetYaxis()->SetTitle("Ratio");
//   // ratio[0]->SetStats(0);
//   // if(down) {
//   //   ratio[0]->GetXaxis()->SetRangeUser(x_min,x_max);
//   //   ratio[0]->GetYaxis()->CenterTitle();
//   //   ratio[0]->GetXaxis()->SetTitleSize( 0.08 );
//   //   ratio[0]->GetXaxis()->SetLabelSize( 0.08 );
//   //   ratio[0]->GetYaxis()->SetLabelSize( 0.08 );
//   //   ratio[0]->GetYaxis()->SetTitleSize( 0.08 );
//   //   ratio[0]->GetYaxis()->SetTitleOffset( 0.4 );
//   //   ratio[0]->GetXaxis()->SetTitleOffset( 1.1 );
//   // }

//   // GetColours();
//   // for(int n=0; n<nRatios; n++){
//   //   if(down){
//   //     ratio[n]->SetMarkerColor(colours[n+1]);
//   //     ratio[n]->SetLineColor(colours[n+1]);
//   //   }
//   //   else{
//   //     ratio[n]->SetMarkerColor(colours[n]);
//   //     ratio[n]->SetLineColor(colours[n]);
//   //   }

//   //   if(n==0) ratio[n]->Draw();
//   //   else ratio[n]->Draw("same");
//   // }

//   //  if(!down) PlotLegend(Plot::Ratio);

//   return true;
// }

 void Plot::Dump()
{
    // if(_macroRecording && isHisto){
    //   //HistoInfo *temp = new HistoInfo(items_sel[k]->GetFile(), items_sel[k]->GetName(), items_sel[k]->GetText());
    //     temp->SetDrawOptions(lnk->GetOption());
    //     temp->SetColour(colours[k]);
    //     Int_t rebin = nentryRebin->GetIntNumber();
    //     if(rebin > 1) temp->SetRebinNumber(nentryRebin->GetIntNumber());
    //     if(checkNormalise->GetState() ) temp->SetScaleFactor(1/h->Integral());
    //     if(checkNormalise2->GetState()) temp->SetScaleFactor(((TH1*)plot_list->At(0))->Integral()/h->Integral());
    //     //        macro->AddHisto(temp);
    // }
}

int Plot::number_of_plot = 0;
