/** @file macro.cxx
 */

#include "macro.h"

HistoInfo::HistoInfo(Int_t file, TString name, TString title) :
  m_file(file),
  m_name(name),
  m_title(title),
  m_drawoption(""),
  m_rebin(1),
  m_colour(1),
  m_scale_factor(1.0)
{

}

TString HistoInfo::GetMacroName()
{
  TString macroname = Form("h_file%i_%s", m_file, m_name.Data());
  return macroname;
}

Bool_t HistoInfo::operator= (HistoInfo* other)
{
  if( m_file == other->GetFile() && m_name == other->GetName())
    return true;
  return false;
}

void CanvasInfo::AddHisto(HistoInfo* histo)
{
  m_histos.push_back(histo);
  return;
}

void Macro::AddFile(TString file)
{
  m_files.push_back(file);
}

void Macro::AddCanvas(TString name)
{
  CanvasInfo *c = new CanvasInfo(name);
  m_canvases.push_back(c);
  canvas_counter = m_canvases.size()-1;
}

void Macro::AddHisto(HistoInfo* h)
{
  m_canvases[canvas_counter]->AddHisto(h);
}

void Macro::AddLegend(std::vector<TString> legend)
{
  for(int index=0; index<m_canvases[canvas_counter]->GetNumberOfHistos(); index++){
    m_canvases[canvas_counter]->GetHisto(index)->SetLegendText(legend[index]);
  }
}

void Macro::Reset()
{
  m_canvases.clear();
}

void Macro::SaveMacro(TString name, OutputFormat type)
{
  // Int_t n_canvas = m_canvases.size();
  // Int_t n_files  = m_files.size();


  // // Add all histos and remove repeated
  // Int_t all_histos_size = 0;
  // for(unsigned int i=0; i< m_canvases.size(); i++){
  //   all_histos_size += m_canvases[i]->Histos().size();
  // }

  // vector<HistoInfo*> all_histos;
  // all_histos.reserve( all_histos_size ); // preallocate memory
  // for(unsigned int i=0; i<canvases.size(); i++){
  //   for(unsigned int j=0; j<canvases[i]->Histos().size(); j++){
  //     all_histos.push_back( canvases[i]->Histo(j) );
  //   }
  // }

  // for(int k=0; k<all_histos_size; k++){
  //    sort(all_histos.begin(), all_histos.end());
  //    all_histos.erase(unique(all_histos.begin(), all_histos.end()), all_histos.end());
  // }




  // // Write Macro
  // ofstream macrofile(name.Data());


  // // Root macro
  // if(type == MRoot){

  //   macrofile << "// ROOT macro creado con plotter */  \n" << endl;
  //   macrofile << "{ \n" << endl;

  //   macrofile << "// Files" << endl;
  //   for(int i=0;i<n_files;i++){
  //     macrofile << "TFile *file" << i << " = TFile(\"" << files[i]  << "\", \"open\");" << endl;
  //   }

  //   macrofile << "\n" << endl;

  //   macrofile << "// Load Histos" << endl;
  //   for(int j=0; j<all_histos_size; j++){
  //     macrofile << "TH1 *h_file" << all_histos[j]->File()  << "_" << all_histos[j]->Name() << " = (TH1*) file" << all_histos[j]->File() << "->Get(\"" << all_histos[j]->Name()  << "\");" << endl;
  //   }

  //   macrofile << "\n" << endl;

  //   macrofile << "// Plots" << endl;
  //   for(int i=0; i<n_canvas; i++){
  //     macrofile << "TCanvas *canvas" << i << " = new TCanvas();" << endl;
  //     for(int j=0; j < canvases[i]->NumberOfHistos(); j++){
  //   if(j==0)   macrofile << canvases[i]->Histo(j)->Name() << "->SetStats(0);" << endl;
  //   macrofile << canvases[i]->Histo(j)->Name() << "->SetLineColor(" << canvases[i]->Histo(j)->Colour() << ");" << endl;
  //   macrofile << canvases[i]->Histo(j)->Name() << "->SetMarkerColor(" << canvases[i]->Histo(j)->Colour() << ");" << endl;
  //     }
  //     for(int j=0; j < canvases[i]->NumberOfHistos(); j++){
  //       macrofile << canvases[i]->Histo(j)->Name() << "->Draw(\"" << canvases[i]->Histo(j)->DrawOption() <<"\");" << endl;
  //     }


  //     if(canvases[i]->NumberOfHistos() > 1){
  //   macrofile << "\n" << endl;
  //   macrofile << "TLegend *legend" << i << " = new TLegend(0.7,0.7,0.8,0.8);" << endl;
  //   macrofile << "legend" << i << "->SetFillColor(0);" << endl;
  //   macrofile << "legend" << i << "->SetBorderSize(0);" << endl;
  //   for(int j=0; j<canvases[i]->NumberOfHistos(); j++){
  //     macrofile << "legend" << i << "->AddEntry(" << canvases[i]->Histo(j)->MacroName() << ",\"" << canvases[i]->Histo(j)->LegendText() << "\");" << endl;
  //   }
  //   macrofile << "legend" << i << "->Draw();" << endl;
  //   macrofile << "\n" << endl;
  //     }

  //     macrofile << "canvas" << i << "->Print(\"canvas" << i << ".eps\");" << endl;
  //     macrofile << "\n" << endl;
  //   }


  //   macrofile << "}" << endl;
  // }





  // // Python macro
  // if(type == MPython){
  //   macrofile << "#! /usr/bin/env python" << endl;
  //   macrofile << "# Macro creado con plotter" <<  endl;
  //   macrofile << " " << endl;
  //   macrofile << "from ROOT import * \n" << endl;

  //   macrofile << "# Files" << endl;
  //   for(int i=0; i<n_files; i++){
  //     macrofile << "file" << i << " = TFile(\"" << files[i]  << "\", \"open\" )" << endl;
  //   }

  //   macrofile << "\n" << endl;

  //   macrofile << "# Load Histos" << endl;
  //   for(int j=0; j<all_histos_size; j++){
  //     macrofile << "h_file" << all_histos[j]->File()  << "_" << all_histos[j]->Name() << " = file" << all_histos[j]->File() << ".Get(\"" << all_histos[j]->Name()  << "\")" << endl;
  //   }

  //   macrofile << "\n" << endl;

  //   macrofile << "# Configure histos" << endl;
  //   macrofile << "def configHisto(h, colour):" << endl;
  //   macrofile << "    h.SetLineColor(colour)" <<endl;
  //   macrofile << "    h.SetMarkerColor(colour)" <<endl;
  //   macrofile << "    h.SetLineWidth(2)" <<endl;
  //   macrofile << "    h.SetMarkerSize(0.8)" <<endl;
  //   macrofile << "    h.SetTitle(\"\")" <<endl;
  //   macrofile << "    h.SetStats(0)" <<endl;

  //   macrofile << "\n" << endl;

  //   for(int j=0; j<all_histos_size; j++){
  //     macrofile << "configHisto(" << all_histos[j]->MacroName() << "," << all_histos[j]->Colour() << ")" << endl;
  //     if(all_histos[j]->RebinNumber()>1) macrofile << all_histos[j]->MacroName() << ".Rebin(" << all_histos[j]->RebinNumber() << ")" << endl;
  //     if(all_histos[j]->ScaleFactor()!=1) macrofile << all_histos[j]->MacroName() << ".Scale(" << all_histos[j]->ScaleFactor() << ")" << endl;
  //   }

  //   macrofile << "\n" << endl;

  //   macrofile << "# Plots" << endl;
  //   for(int i=0; i<n_canvas; i++){

  //     macrofile << "\n" << endl;

  //     macrofile << "# Canvas" << i << endl;
  //     macrofile << "canvas" << i << " = TCanvas()" << endl;
  //     for(int j=0; j < canvases[i]->NumberOfHistos(); j++){
  //       macrofile << all_histos[j]->MacroName() << ".Draw(\"" << canvases[i]->Histo(j)->DrawOption() <<"\")" << endl;
  //     }

  //     if(canvases[i]->NumberOfHistos() > 1){
  //   macrofile << "\n" << endl;
  //   macrofile << "legend" << i << " = TLegend(0.7,0.7,0.8,0.8)" << endl;
  //   macrofile << "legend" << i << ".SetFillColor(0)" << endl;
  //   macrofile << "legend" << i << ".SetBorderSize(0)" << endl;
  //   for(int j=0; j<canvases[i]->NumberOfHistos(); j++){
  //     macrofile << "legend" << i << ".AddEntry(" << canvases[i]->Histo(j)->MacroName() << ",\"" << canvases[i]->Histo(j)->LegendText() << "\")" << endl;
  //   }
  //   macrofile << "legend" << i << ".Draw()" << endl;
  //   macrofile << "\n" << endl;
  //     }

  //     macrofile << "canvas" << i << ".Print(\"canvas" << i << ".eps\")" << endl;
  //   }

  // }//end python macro

}
