/** @file plot.cxx
    @brief Plot class implementation
*/

#include <iostream>
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

void Plot::Add(TObject *obj)
{
  m_list->Add(obj);
}

void Plot::Show()
{
  Create();
  return;
}

void Plot::Save()
{
  return;
}

void Plot::Create()
{

  //Some checks before creating canvas
  if(m_list->GetSize() == 0) return;
  // if((type == Plot::Ratio || type == Plot::Efficiency) && items_sel.size()<2) return;

  // Plot order: 1) Selected order (default). 2) Order by file and entry.
  //if(checkOrder->GetState())  sort(items_sel.begin(), items_sel.end(), SortItems);


  //     if(checkIncludeRatio->GetState()){

  //       TPad *up   = new TPad("upperPad", "upperPad", .001, .29, .999, .999);
  //       TPad *down = new TPad("lowerPad", "lowerPad", .001, .001,  .999, .28);

  //       up->SetLeftMargin(0.08);
  //       up->SetRightMargin(0.05);
  //       up->SetBottomMargin(0.01);
  //       up->SetTopMargin(0.06);

  //       down->SetLeftMargin(0.08);
  //       down->SetRightMargin(0.05);
  //       down->SetBottomMargin(0.2);
  //       down->SetTopMargin(0.01);

  //       if(checkLogX->GetState()) up->SetLogx();
  //       if(checkLogY->GetState()) up->SetLogy();

  //       up->Draw();
  //       down->Draw();

  //       up->cd();
  //       PlotHistos();

  //       down->cd();
  //       good_plot = PlotRatios(true);
  //     }
  //     else if(checkIncludeDiff->GetState()){

  //       TPad *up   = new TPad("upperPad", "upperPad", .001, .29, .999, .999);
  //       TPad *down = new TPad("lowerPad", "lowerPad", .001, .001,  .999, .28);

  //       up->SetLeftMargin(0.08);
  //       up->SetRightMargin(0.05);
  //       up->SetBottomMargin(0.01);
  //       up->SetTopMargin(0.06);

  //       down->SetLeftMargin(0.08);
  //       down->SetRightMargin(0.05);
  //       down->SetBottomMargin(0.2);
  //       down->SetTopMargin(0.01);

  //       if(checkLogX->GetState()) up->SetLogx();
  //       if(checkLogY->GetState()) up->SetLogy();

  //       up->Draw();
  //       down->Draw();

  //       up->cd();
  //       PlotHistos();

  //       down->cd();
  //       good_plot = PlotRelativeDiffs(true);
  //     }
  //     else {
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

//   return;
}


int Plot::number_of_plot = 0;
