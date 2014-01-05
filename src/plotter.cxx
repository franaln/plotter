/** @file plotter.cxx
*/

#include "plotter.h"

#include <TEnv.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#include <TLegend.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGMsgBox.h>

#include "item.h"
#include "itemsbox.h"

const char* settings_file = ".plotterrc";

const char* default_colours[] = {"kBlack",
                                 "kRed",
                                 "kBlue",
                                 "kGreen",
                                 "kYellow",
                                 "kMagenta",
                                 "kCyan",
                                 "kOrange+7",
                                 "kRed-4",
                                 "kBlue-4",
                                 "kGreen-4",
                                 "kYellow-4",
                                 "kMagenta-4",
                                 "kCyan-4",
                                 "kOrange",
                                 "kRed-9",
                                 "kBlue-9",
                                 "kGreen-9",
                                 "kYellow-9",
                                 "kMagenta-9"};

enum MenuId {
  M_FILE_OPEN,
  M_FILE_SETTINGS,
  M_FILE_SAVE_CANVASES,
  M_FILE_RESET,
  M_FILE_CLOSE,
  M_FILE_EXIT,
  M_CREATE_ADD,
  M_CREATE_SUBSTRACT,
  M_CREATE_EFFICIENCY,
  M_CREATE_RATIO,
  M_CREATE_SCALE,
  M_MACRO_BEGIN,
  M_MACRO_RESET,
  M_MACRO_CREATE_ROOT,
  M_MACRO_CREATE_PYTHON,
  M_VIEW_CUTS,
  M_VIEW_COLOURS,
  RB_COLZ,
  RB_SCATTER,
  RB_BOX
};

bool SortVs(Item *i, Item *j)
{
  if( i->GetFile() < j->GetFile() ) return true;
  if( i->GetFile() == j->GetFile() && i->GetEntry() < j->GetEntry() ) return true;
  return false;
}


ClassImp(Plotter);

Plotter::Plotter(vector<TString> filenames, bool merge) :
  TGMainFrame(gClient->GetRoot(), 800, 500),
  m_file_names(filenames),
  m_merge_mode(merge),
  m_macro_recording(false)
{
  SetCleanup(kLocalCleanup);

  plot_list = 0;

  // Open files
  m_number_of_files = m_file_names.size();
  if(m_number_of_files==0){
    error("There is no file to open");
    ExitError();
  }

  if(m_merge_mode){
    CreateMergedFileBox();
  }
  else{
    //check input files
    TFile *fin;
    for(UInt_t i=0; i<m_number_of_files; i++){
      msg("Loading file: " << m_file_names[i]);

      if(m_file_names[i]!="") fin = new TFile(m_file_names[i].Data(), "open");

      if(fin->IsZombie()) {
        error("El archivo existe? Es un archivo de root?");
        ExitError();
      }
    }
  }

  // Load settings and style. Add AtlasStyle?
  LoadSettings();
  SetStyle();

  // Create Gui
  CreateMainWindow();
  SetWindowName("plotter");
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
  MapSubwindows();
  ShowHideColours();
  ShowHideCuts();
  Resize(GetDefaultSize());
  MapWindow();
}

Plotter::~Plotter()
{
  Cleanup();
  plot_list->Clear();  delete plot_list;
  for(UInt_t i=0; i<canvas.size(); i++ ) delete canvas[i];
}



/* GUI
------*/

void Plotter::CreateMainWindow()
{
  /* Main Window:
     - menu bar
     - main frame (with fileboxes)
     - options frame
     - colours
     - cuts
  */

  CreateMenuBar();
  frame_main = new TGCompositeFrame(this, 1, 1, kHorizontalFrame);
  CreateMainFrame();
  CreateOptionsFrame();
  CreateColoursFrame();
  AddFrame(frame_main, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 2, 0, 2));
  CreateCutsEntry();
  //  CreateStatusBar(); //Not useful now
}

void Plotter::CreateMenuBar()
{
  /* Menu bar
     - File: Save all canvases, Exit
     - View : Colours, Cuts
     - Create:
     - Macro: Begin, Reset, Save ROOT macro, Save python macro
   */

  layout_menu_bar     = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  layout_menu_bar_item = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

  menu_file = new TGPopupMenu(fClient->GetRoot());
  menu_file->AddEntry("Save all canvases ", M_FILE_SAVE_CANVASES);
  menu_file->AddEntry("Settings... ", M_FILE_SETTINGS);
  menu_file->DisableEntry(M_FILE_SETTINGS);
  menu_file->AddSeparator();
  menu_file->AddEntry("Exit", M_FILE_EXIT);
  menu_file->Associate(this);

  menu_view = new TGPopupMenu(fClient->GetRoot());
  menu_view->AddEntry("Colours", M_VIEW_COLOURS);
  menu_view->AddEntry("Cuts", M_VIEW_CUTS);
  menu_view->Associate(this);

  // menu_create = new TGPopupMenu(fClient->GetRoot());
  // menu_create->AddEntry("Add", M_CREATE_ADD);
  // menu_create->AddEntry("Substract", M_CREATE_SUBSTRACT);
  // menu_create->AddEntry("Efficiency", M_CREATE_EFFICIENCY);
  // menu_create->Associate(this);

  menu_macro = new TGPopupMenu(fClient->GetRoot());
  menu_macro->AddEntry("Begin", M_MACRO_BEGIN);
  menu_macro->AddEntry("Reset", M_MACRO_RESET);
  menu_macro->DisableEntry(M_MACRO_RESET);
  menu_macro->AddEntry("Save Root macro...",   M_MACRO_CREATE_ROOT);
  menu_macro->DisableEntry(M_MACRO_CREATE_ROOT);
  menu_macro->AddEntry("Save python macro...", M_MACRO_CREATE_PYTHON);
  menu_macro->DisableEntry(M_MACRO_CREATE_PYTHON);
  menu_macro->Associate(this);

  menu_bar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  menu_bar->AddPopup("&File",   menu_file, layout_menu_bar_item);
  menu_bar->AddPopup("&View",   menu_view, layout_menu_bar_item);
  //menu_bar->AddPopup("&Histos", menu_create, layout_menu_bar_item);
  menu_bar->AddPopup("&Macro",  menu_macro, layout_menu_bar_item);

  AddFrame(menu_bar, layout_menu_bar);
}

void Plotter::CreateMainFrame()
{
  /* Create frame with fileboxes.
     Each filebox represents a file
   */

  UInt_t n_cols = 0;
  UInt_t n_rows = 0;

  if(m_number_of_files <= 5){
    n_rows = 1;
    n_cols = m_number_of_files;
  }
  else if(m_number_of_files > 5 && m_number_of_files <= 10){
    n_rows = 2;
    n_cols = int( floor(m_number_of_files/2. + 0.5) );
  }
  else{
    n_rows = 3;
    n_cols = int( floor(m_number_of_files/3. + 0.5) );
  }

  if(m_merge_mode) {
    m_number_of_files = 1;
    n_rows = 1;
    n_cols = 1;
  }

  // Width and height of the filebox. Depends screen resolution
  Int_t  x, y;
  UInt_t width, height;
  gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),x,y,width,height);

  height = height * 0.75;

  UInt_t h = 600;
  UInt_t w = 200;
  if(n_rows == 1)       h = (height-200);
  else if(n_rows == 2)  h = int(height/2);
  else                 h = int(height/3);

  frame_aux = new TGCompositeFrame(frame_main, 0, 0, kVerticalFrame);

  for(UInt_t i=0; i<n_rows; i++){
    frame_row[i] = new TGHorizontalFrame(frame_aux, 10, 10, kHorizontalFrame);
  }

  // Create file boxes (one for each file)
  for(UInt_t i=0; i<m_number_of_files; i++){
    Int_t row;
    if(i < n_cols) row = 0;
    else if(i>=n_cols && i<2*n_cols) row = 1;
    else row = 2;

    boxes[i] = new ItemsBox(frame_row[row], w, h, i);
    boxes[i]->AddFile(m_file_names[i]);

    boxes[i]->GetContent()->Connect("Selected(Int_t)", "Plotter", this, "OnItemClick(Int_t)");
    boxes[i]->GetContent()->GetContainer()->Connect("DoubleClicked(TGFrame*, Int_t)", "Plotter", this, "OnItemDoubleClick(TGFrame*, Int_t)");

    frame_row[row]->AddFrame(boxes[i], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 2, 0, 2));
  }


  for(UInt_t i=0;i<n_rows;i++){
    frame_aux->AddFrame(frame_row[i], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 2, 0, 2));
  }

  frame_main->AddFrame(frame_aux, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));
}

/** @fn create_options_frame
    @brief creates the frame with all the plot options and the buttons
*/
void Plotter::CreateOptionsFrame()
{
  layout_buttons = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2);
  layout_checks  = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 2);

  frame_options = new TGVerticalFrame(frame_main, 0, 0, kVerticalFrame);

  //-- Buttons
  button_clear_selection = new TGTextButton(frame_options, "Clear selection", 0);
  button_draw            = new TGTextButton(frame_options, "Draw!",           0);
  button_draw_efficiency = new TGTextButton(frame_options, "Draw Efficiency", 0);
  button_draw_ratio      = new TGTextButton(frame_options, "Draw Ratio",      0);
  button_exit            = new TGTextButton(frame_options, "Exit",            0);

  button_clear_selection->SetToolTipText("Clear all selected entries.");
  button_draw->SetToolTipText("Plot the selected histos.");
  //  button_draw_efficiency->SetToolTipText("Plot the efficiency between the two selected histos (hlast/hfirst).");
  //button_draw_ratio->SetToolTipText("Plot the ratio between the selected histos wrt the first one selected (hn/hfirst).");

  button_clear_selection->SetStyle("modern");
  button_draw->SetStyle("modern");
  button_draw_efficiency->SetStyle("modern");
  button_draw_ratio->SetStyle("modern");
  button_exit->SetStyle("modern");

  button_clear_selection->Associate(this);
  button_draw->Associate(this);
  button_draw_efficiency->Associate(this);
  button_draw_ratio->Associate(this);
  button_exit->Associate(this);

  button_clear_selection->Connect("Clicked()", "Plotter", this, "OnButtonClearSelection()");
  button_draw->Connect("Clicked()", "Plotter", this, "OnButtonDraw()");
  //button_draw_efficiency->Connect("Clicked()", "Plotter", this, "draw(\"Plot::Efficiency\")");
  //button_draw_ratio->Connect("Clicked()", "Plotter", this, "draw(\"Plot::Ratio\")");
  button_exit->Connect("Clicked()", "Plotter", this, "OnButtonExit()");


  //-- General Options
  group_options = new TGGroupFrame(frame_options, "Draw Options", kVerticalFrame);

  group_options->AddFrame(check_include_ratio = new TGCheckButton(group_options, "Include Ratio", 0), new TGLayoutHints(kLHintsLeft, 2, 2, 10, 2));
  group_options->AddFrame(check_include_diff  = new TGCheckButton(group_options, "Include Difference", 0), new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  group_options->AddFrame(check_order        = new TGCheckButton(group_options, "Keep file order", 0), new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  check_order->SetToolTipText("Draw the selected histos in the files/entries order instead the selection order.");

  frame_log = new TGCompositeFrame(group_options, 10, 10, kHorizontalFrame);
  frame_log->AddFrame(check_log_x = new TGCheckButton(frame_log, "SetLogX", 0), new TGLayoutHints( kLHintsLeft, 2, 2, 5, 2));
  frame_log->AddFrame(check_log_y = new TGCheckButton(frame_log, "SetLogY", 0), new TGLayoutHints( kLHintsLeft, 10, 2, 5, 2));
  group_options->AddFrame(frame_log, new TGLayoutHints( kLHintsLeft, 0, 0, 0, 0));

  //-- Histos options
  group_hist_options = new TGGroupFrame(frame_options, "Histos", kVerticalFrame);
  group_hist_options->SetTitlePos(TGGroupFrame::kLeft);

  frame_rebin = new TGCompositeFrame(group_hist_options, 0, 0, kHorizontalFrame);
  label_rebin = new TGLabel(frame_rebin, "Rebin");
  nentry_rebin = new TGNumberEntry(frame_rebin, 1, 1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  nentry_rebin->Resize(75,20);
  nentry_rebin->Associate(this);
  frame_rebin->AddFrame(label_rebin, new TGLayoutHints( kLHintsLeft, 2, 2, 5, 2));
  frame_rebin->AddFrame(nentry_rebin, new TGLayoutHints( kLHintsRight, 2, 2, 5, 2));
  group_hist_options->AddFrame(frame_rebin,    new TGLayoutHints(kLHintsLeft, 2, 2, 10, 2));

  group_hist_options->AddFrame(check_normalise  = new TGCheckButton(group_hist_options, "Normalise (1)", 0), new TGLayoutHints(kLHintsLeft, 2, 2, 10, 2));
  group_hist_options->AddFrame(check_normalise2  = new TGCheckButton(group_hist_options, "Normalise (first)", 0), new TGLayoutHints(kLHintsLeft, 2, 2, 10, 2));
  check_normalise->SetToolTipText("Normalise the histograms to 1.");
  check_normalise2->SetToolTipText("Normalise the histograms to the first selected histogram.");

  group_hist_options->AddFrame(check_stats = new TGCheckButton(group_hist_options, "Show Stats", 0), layout_checks);
  check_stats->SetToolTipText("Show the histogram stats.");
  group_hist_options->AddFrame(check_hist = new TGCheckButton(group_hist_options, "Line", 0), layout_checks);
  check_hist->SetToolTipText("Use the \"hist\" option.");
  group_hist_options->AddFrame(check_p = new TGCheckButton(group_hist_options, "Point", 0), layout_checks);
  check_p->SetToolTipText("Use the \"P\" option.");
  group_hist_options->AddFrame(check_text = new TGCheckButton(group_hist_options, "Text", 0), layout_checks);
  check_hist->SetToolTipText("Use the \"text\" option.");
  group_hist_options->AddFrame(check_pie = new TGCheckButton(group_hist_options, "Pie", 0), layout_checks);
  check_pie->SetToolTipText("Use the \"pie\" option.");
  //  group_histosoptions->AddFrame(checkStack       = new TGCheckButton(group_histosoptions, "Stack", 0),       layoutChecks);
  //  checkStack->SetToolTipText("Stack the histos.");

  //-- Histos2D options
  group_hist2D_options = new TGGroupFrame(frame_options, "Histos 2D", kVerticalFrame);
  group_hist2D_options->SetTitlePos(TGGroupFrame::kLeft);

  group_hist2D_options->SetLayoutManager(new TGHorizontalLayout(group_hist2D_options));
  group_hist2D_options->AddFrame(radio_colz = new TGRadioButton(group_hist2D_options, "COLZ", RB_COLZ), new TGLayoutHints( kLHintsLeft, 1, 1, 1, 1));
  group_hist2D_options->AddFrame(radio_scatter = new TGRadioButton(group_hist2D_options, "SCATTER", RB_SCATTER), new TGLayoutHints( kLHintsLeft, 1, 1, 1, 1));
  group_hist2D_options->AddFrame(radio_box = new TGRadioButton(group_hist2D_options, "BOX", RB_BOX), new TGLayoutHints( kLHintsLeft, 1, 1, 1, 1));
  radio_colz->SetState(kButtonDown);
  radio_colz->Associate(this);
  radio_scatter->Associate(this);
  radio_box->Associate(this);

  frame_options->AddFrame(button_clear_selection, new TGLayoutHints(kLHintsExpandX, 2, 2, 29, 2));
  frame_options->AddFrame(button_draw, layout_buttons);
  frame_options->AddFrame(button_draw_efficiency, layout_buttons);
  frame_options->AddFrame(button_draw_ratio,layout_buttons);
  frame_options->AddFrame(button_exit, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 20));
  frame_options->AddFrame(group_options, new TGLayoutHints(kLHintsExpandX, 2, 5, 5, 20));
  frame_options->AddFrame(group_hist_options, new TGLayoutHints(kLHintsExpandX, 2, 5, 5, 20));
  frame_options->AddFrame(group_hist2D_options, new TGLayoutHints(kLHintsExpandX, 2, 5, 2, 20));

  frame_main->AddFrame(frame_options, new TGLayoutHints(kLHintsNormal, 0, 2, 0, 2)); //Add Options frame to fFrame
}

/** creates the frame to choose the plot colours
*/
void Plotter::CreateColoursFrame()
{
    frame_colours = new TGVerticalFrame(frame_main, 0, 0, kVerticalFrame);

  group_colours = new TGGroupFrame(frame_colours, "Colours", kVerticalFrame);
  group_colours->SetLayoutManager(new TGMatrixLayout(group_colours, 0, 2, 6, 6));

  for(UInt_t i=0; i<20; i++){
    colorselect[i] = new TGColorSelect(group_colours, pcolors[i], 50+i);
    group_colours->AddFrame(colorselect[i], new TGLayoutHints(kLHintsNormal | kLHintsExpandY, 10, 2, 2, 2));
    colorselect[i]->Resize(35, 15);
    colorselect[i]->Associate(this);
    group_colours->AddFrame(check_fill[i] = new TGCheckButton(group_colours, "", 0), new TGLayoutHints(kLHintsRight, 30, 2, 2, 2));
    check_fill[i]->SetToolTipText("Fill");
  }
  frame_colours->AddFrame(group_colours, new TGLayoutHints(kLHintsExpandY, 5, 5, 5, 5));

  frame_main->AddFrame(frame_colours, new TGLayoutHints(kLHintsExpandY, 2, 2, 2, 2));
}

void Plotter::CreateCutsEntry()
{
  entry_cuts = new TGTextEntry(this, "Cuts");
  entry_cuts->SetDefaultSize(150, entry_cuts->GetDefaultHeight());
  entry_cuts->SetAlignment(kTextCenterX);
  AddFrame(entry_cuts,   new TGLayoutHints(kLHintsExpandX, 5, 2, 2, 2));
  menu_view->CheckEntry(M_VIEW_CUTS);
}

void Plotter::CreateStatusBar() // Sin uso por ahora :P
{
  status_bar = new TGStatusBar(this, 50, 10, kHorizontalFrame);
  AddFrame(status_bar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX,0,0,2,0));
}

Bool_t Plotter::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  // Handle messages send to Plotter object.

  switch (GET_MSG(msg)) {

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {

    case kCM_MENU:
      switch (parm1) {
      case M_FILE_EXIT:
        {
          Exit();
        }
        break;

      case M_FILE_SAVE_CANVASES:
        {
          SaveCanvases();
        }
        break;

      case M_FILE_SETTINGS:
        //	new SettingsBox(fClient->GetRoot(), this, 400, 200);
        break;

      case M_VIEW_CUTS:
        {
          ShowHideCuts();
        }
        break;

      case M_VIEW_COLOURS:
        {
          ShowHideColours();
        }
        break;

      case M_CREATE_ADD:
        {
          //      CreateOther(0);
          //      AddHistos();
        }
        break;

      case M_MACRO_BEGIN:
        {
          BeginMacro();
        }
        break;

      case M_MACRO_RESET:
        {
          ResetMacro();
        }
        break;

      case M_MACRO_CREATE_ROOT:
        {
          CreateMacro(MRoot);
        }
        break;

      case M_MACRO_CREATE_PYTHON:
        {
          CreateMacro(MPython);
        }
        break;

      default:
        break;
      }

    case kCM_RADIOBUTTON:
      switch (parm1) {
      case RB_COLZ:
        radio_box->SetState(kButtonUp);
        radio_scatter->SetState(kButtonUp);
        break;
      case RB_SCATTER:
        radio_box->SetState(kButtonUp);
        radio_colz->SetState(kButtonUp);
        break;
      case RB_BOX:
        radio_scatter->SetState(kButtonUp);
        radio_colz->SetState(kButtonUp);
        break;
      }
      break;

    default:
      break;

    }

  }

  return kTRUE;
}

void Plotter::CreateMergedFileBox()
{

  Int_t n_files = m_file_names.size();
  TFile* fin[n_files];
  TString tree_name;

  fin[0] = new TFile(m_file_names[0], "open") ;

  TList*   list = fin[0]->GetListOfKeys();
  TKey*    key = (TKey*)list->At(0);
  TObject* obj = key->ReadObj();

  if( obj->InheritsFrom("TTree") )
    tree_name = obj->GetName();
  else{
    error("There is no tree in the input file.");
    ExitError();
  }

  merge_chain = new TChain(tree_name, tree_name);

  for(Int_t k=0; k<n_files; k++){
    msg("Adding file: " << m_file_names[k]);
    merge_chain->Add( m_file_names[k] );
  }

//   CreateGui(tree_name);

//   LoadItems();
}

void Plotter::SetStyle()
{
  // Plots style

  plotter_style = new TStyle("Plotter", "PlotterStyle");

  plotter_style->SetPalette(1); // 2D plots pallete

  plotter_style->SetFrameFillColor(0);
  plotter_style->SetFrameBorderSize(0);
  plotter_style->SetFrameBorderMode(0);
  plotter_style->SetCanvasBorderMode(0);
  plotter_style->SetCanvasColor(0);
  plotter_style->SetPadBorderMode(0);
  plotter_style->SetPadColor(0);
  plotter_style->SetStatColor(0);
  plotter_style->SetTitleBorderSize(0);
  plotter_style->SetTitleFillColor(0);
  plotter_style->SetLegendBorderSize(0);
  plotter_style->SetLegendFillColor(0);
  plotter_style->SetEndErrorSize(0);
  plotter_style->SetOptTitle(0);

  plotter_style->SetStatBorderSize(1);
  plotter_style->SetPaperSize(20,26);
  plotter_style->SetPadTopMargin(0.05);
  plotter_style->SetPadRightMargin(0.05);
  plotter_style->SetPadBottomMargin(0.16);
  plotter_style->SetPadLeftMargin(0.16);
  plotter_style->SetTitleXOffset(1.4);
  plotter_style->SetTitleYOffset(1.4);

  //Fonts
  plotter_style->SetTextFont(42);
  plotter_style->SetTextSize(0.05);
  plotter_style->SetLabelFont(42,"x");
  plotter_style->SetTitleFont(42,"x");
  plotter_style->SetLabelFont(42,"y");
  plotter_style->SetTitleFont(42,"y");
  plotter_style->SetLabelFont(42,"z");
  plotter_style->SetTitleFont(42,"z");
  plotter_style->SetLabelSize(0.05,"x");
  plotter_style->SetTitleSize(0.05,"x");
  plotter_style->SetLabelSize(0.05,"y");
  plotter_style->SetTitleSize(0.05,"y");
  plotter_style->SetLabelSize(0.05,"z");
  plotter_style->SetTitleSize(0.05,"z");

  plotter_style->SetPadTickX(1);
  plotter_style->SetPadTickY(1);

  plotter_style->cd();
  gROOT->ForceStyle();
}


void Plotter::GetColours()
{
  for(int k=0; k<plot_list->GetSize(); k++){
    colours[k] = TColor::GetColor(pcolors[k]);
    if(frame_main->IsVisible(frame_colours))
      colours[k] = TColor::GetColor(colorselect[k]->GetColor());
  }
}

void Plotter::ClearSelection()
{
  /*
     - clear items_sel
     - clear plot_list
     - set items status to false
  */

  items_sel.clear();
  if(plot_list) plot_list->Clear();

  for(UInt_t i=0; i<m_number_of_files; i++){
    boxes[i]->Clear();
  }

  return;
}

void Plotter::Exit()
{
  msg("Chau :)");
  CloseWindow();
}

void Plotter::ExitError()
{
  msg("Chau :(");
  CloseWindow();
}

void Plotter::CloseWindow()
{
  gApplication->Terminate(0);
}



/* Draw
-------*/

TObject* Plotter::GetObject(Item* it)
{

  Int_t nfile  = it->GetFile();
  TString name = it->GetName();

  TObject *obj;

  if(it->IsBranch()){
    TString cut = "";
    cut = TString(entry_cuts->GetText()).EqualTo("Cuts") ? "" : entry_cuts->GetText();

    TTree* tree = boxes[nfile]->GetCurrentTree();
    tree->Draw(name+">>h", cut, "goff");

    obj = (TH1*)gDirectory->Get("h");
  }
  else {
    TDirectory *dir = boxes[nfile]->GetCurrentDir();
    obj = dir->Get(name);
}

  return obj;
}

void Plotter::Draw(Plot::Type type)
{
  /* Main plot function.
   * If Normal: plot the items
   * If Ratio: plot the ratio between the (N-1) last items wrt the first one.
   * If Efficiency: plot the efficiency of the last item wrt the first one.
   */

  //Some checks before creating canvas
  if(items_sel.size()==0) return;
  if((type == Plot::Ratio || type == Plot::Efficiency) && items_sel.size()<2) return;

  // Plot order: 1) Selected order (default). 2) Order by file and entry.
  if(check_order->GetState())  sort(items_sel.begin(), items_sel.end(), SortVs);

  //Create new canvas
  Int_t nro = canvas.size();
  char name[50]; sprintf(name, "canvas%d", nro);

  TCanvas *c = new TCanvas(name, name, 800, 600);

  c->SetLeftMargin(0.08);
  c->SetBottomMargin(0.09);
  c->SetRightMargin(0.06);
  c->SetTopMargin(0.04);  // para que no queden margenes al guardarlo

  canvas.push_back(c);

  bool good_plot = false;

  switch (type)
    {
    case Plot::Normal:

      CreatePlotList();
      ConfigurePlotList();

      if(check_include_ratio->GetState()){

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

        if(check_log_x->GetState()) up->SetLogx();
        if(check_log_y->GetState()) up->SetLogy();

        up->Draw();
        down->Draw();

        up->cd();
        PlotHistos();

        down->cd();
        good_plot = PlotRatios(true);
      }
      else if(check_include_diff->GetState()){

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

        if(check_log_x->GetState()) up->SetLogx();
        if(check_log_y->GetState()) up->SetLogy();

        up->Draw();
        down->Draw();

        up->cd();
        PlotHistos();

        down->cd();
        good_plot = PlotRelativeDiffs(true);
      }
      else {
        //if(_macroRecording) macro->AddCanvas(canvas[nro]->GetName());
        if(check_log_x->GetState()) canvas[nro]->SetLogx();
        if(check_log_y->GetState()) canvas[nro]->SetLogy();
        PlotHistos();
        good_plot = true;

      }
      break;
    case Plot::Efficiency:
      good_plot = PlotEfficiency();
      break;
    case Plot::Ratio:
      good_plot = PlotRatios();
      break;
    default:
      break;
    }

  if(!good_plot) {
    delete canvas[nro];
    canvas.pop_back();
  }

  if(plot_list) plot_list->Clear();
  return;
}

void Plotter::CreatePlotList()
{
  /* Plot list
     - Create list of objtes to plot from items_sel
     - Add draw options
  */

  if(!plot_list)  plot_list = new TList();

  plot_list->Clear();

  //-- Convert the list of items in a TList of hists/graphs with the draw options
  for(UInt_t k=0; k < items_sel.size(); k++){

    if( !items_sel[k]->IsPlotable() ) continue;

    TH1 *h;

    if(items_sel[k]->IsTypeHist()){

      h = (TH1*)GetObject(items_sel[k]);

      if( h->GetEntries() == 0 ) {
        error(items_sel[k]->GetName() << " is empty.");
        continue;
      }

      if(!h) continue;

      h->SetName(items_sel[k]->GetName());
      h->SetTitle(items_sel[k]->GetName());

      Bool_t is2D = h->IsA()->InheritsFrom(TH2::Class());
      Bool_t is3D = h->IsA()->InheritsFrom(TH3::Class());
      Bool_t is1D = kFALSE; if(!is2D && !is3D) is1D = kTRUE;

      TH2 *h2; TH3 *h3;
      if(is2D) h2 = (TH2*) h;
      if(is3D) h3 = (TH3*) h;

      if(check_normalise->GetState() && check_normalise2->GetState())
        error("I'm confused o.O! Both normalise check buttons are selected. I'll do what I want.");
      if(check_normalise->GetState())   h->Scale(1/h->Integral());
      if(check_normalise2->GetState() && plot_list->GetSize()!=0)
        h->Scale( ((TH1*)plot_list->At(0))->Integral()/h->Integral());

      Int_t rebin = nentry_rebin->GetIntNumber();
      if(rebin > 1){
        if(is1D)
          h->Rebin(rebin);
        else if(is2D){
          h2->RebinX(rebin);
          h2->RebinY(rebin);
        }
        else if(is3D){
          h3->RebinX(rebin);
          h3->RebinY(rebin);
          h3->RebinZ(rebin);
        }
      }

      // Draw options
      TString draw_opt = "";
      if(check_text->GetState()) draw_opt += "text";

      if(is1D){
        if(check_hist->GetState()) draw_opt += "hist";
        if(check_p->GetState()) draw_opt += "P";
        if(check_pie->GetState()) draw_opt += "PIE";
      }
      else if(is2D){
        if(radio_scatter->GetState())  draw_opt += "scat";
        else if(radio_box->GetState()) draw_opt += "box";
        else  draw_opt += "colz";
      }

      if(check_text->GetState()) draw_opt += "text";
      if(k!=0) draw_opt += "same";

      if(is1D) plot_list->Add(h, draw_opt);
      else if(is2D) plot_list->Add(h2, draw_opt);
      else if(is3D) plot_list->Add(h3, draw_opt);

    }
    else if( items_sel[k]->IsTypeGraph() ){

      TGraph *g = (TGraph*)GetObject(items_sel[k]);
      if(!g) continue;

      g->SetName(items_sel[k]->GetName());
      g->SetTitle(items_sel[k]->GetName());

      TString draw_opt = "";
      draw_opt += "PZT";
      if(k==0) draw_opt += "A";
      else draw_opt += "same";
      plot_list->Add(g, draw_opt);
    }
  }

}

void Plotter::ConfigurePlotList()
{
  /* Configure the objects in the plot list */

  TH1 *h; TGraph *g;

  //-- Axis
  x_min = 1e300;
  x_max = -x_min;
  y_min = 1e300;
  y_max = -y_min;

  TObjOptLink *lnk = (TObjOptLink*)plot_list->FirstLink();
  while (lnk) {
    TObject *obj = lnk->GetObject();

    Bool_t isHisto = obj->InheritsFrom("TH1");
    Bool_t isGraph = obj->InheritsFrom("TGraph");

    if(isHisto){
      h = (TH1*)obj;
      if (h->GetXaxis()->GetXmin() < x_min) x_min = h->GetXaxis()->GetXmin();
      if (h->GetXaxis()->GetXmax() > x_max) x_max = h->GetXaxis()->GetXmax();
      if (h->GetMinimum() < y_min) y_min = h->GetMinimum();
      if (h->GetMaximum() > y_max) y_max = h->GetMaximum();
    }
    else if(isGraph){
      g = (TGraph*)obj;

      if (g->GetXaxis()->GetXmin() < x_min) x_min = g->GetXaxis()->GetXmin();
      if (g->GetXaxis()->GetXmax() > x_max) x_max = g->GetXaxis()->GetXmax();

      Double_t x = 0; Double_t y = 0;
      for(int i=0; i<g->GetN(); i++){
        g->GetPoint(i, x, y);
        if (y < y_min) y_min = y;
        if (y > y_max) y_max = y;
      }
    }

    lnk = (TObjOptLink*)lnk->Next();
  }

  y_max *= 1.1;

  if(check_normalise->GetState() && check_normalise2->GetState()) y_max /= h->Integral();

  if (gPad->GetLogy()){
    if (y_min > 0)  y_min *= .9;
    else            y_min = y_max * 1.e-3;
  }
  else if (y_min >= 0) y_min = 1.e-3;

  if(plot_list->GetSize()<1) return;

  //-- Configure first object
  Bool_t firstIsHisto = plot_list->At(0)->InheritsFrom("TH1");
  Bool_t firstIsGraph = plot_list->At(0)->InheritsFrom("TGraph");

  // Check if all the histos are the same histo (but from different files :P)
  Bool_t same = true;
  for(int i=1; i<plot_list->GetSize(); i++){
    if(!items_sel[i]->GetName().EqualTo(items_sel[0]->GetName())){
      same = false;
      break;
    }
  }

  if(firstIsHisto){
    h = (TH1*)plot_list->At(0);

    // y axis
    h->GetYaxis()->SetRangeUser(y_min, y_max);

    // x axis title
    if(plot_list->GetSize() == 1) h->GetXaxis()->SetTitle(items_sel[0]->GetName());
    else if(same) h->GetXaxis()->SetTitle(items_sel[0]->GetName());

    if(check_stats->GetState()) h->SetStats(1);
    else                       h->SetStats(0);
  }
  else if(firstIsGraph){
    g = (TGraph*)plot_list->At(0);

    // y axis
    g->GetYaxis()->SetRangeUser(y_min, y_max);

    // x axis title
    if(plot_list->GetSize() == 1) g->GetXaxis()->SetTitle(items_sel[0]->GetName());
    else if(same) g->GetXaxis()->SetTitle(items_sel[0]->GetName());
  }

  GetColours();

  lnk = (TObjOptLink*)plot_list->FirstLink();
  int k = 0;
  while (lnk) {

    TObject *obj = lnk->GetObject();

    Bool_t isHisto = lnk->GetObject()->InheritsFrom("TH1");
    Bool_t isGraph = lnk->GetObject()->InheritsFrom("TGraph");

    if(isHisto){
      h = (TH1*) obj;
      h->SetLineColor(colours[k]);
      h->SetMarkerColor(colours[k]);
      if(check_fill[k]->GetState()) h->SetFillColor(colours[k]);

      h->SetMarkerStyle(marker_style);
      h->SetMarkerSize(marker_size);
      h->SetLineWidth(line_width);
    }
    else if(isGraph){
      g = (TGraph*)obj;
      g->SetLineColor(colours[k]);
      g->SetMarkerColor(colours[k]);

      g->SetMarkerStyle(marker_style);
      g->SetMarkerSize(marker_size);
      g->SetLineWidth(line_width);
    }

    // If Macro:
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

    lnk = (TObjOptLink*)lnk->Next();
    k++;
  }

}

void Plotter::PlotHistos()
{
  TObjOptLink *lnk = (TObjOptLink*)plot_list->FirstLink();
  while (lnk) {
    TObject *obj = lnk->GetObject();
    obj->Draw(lnk->GetOption());
    lnk = (TObjOptLink*)lnk->Next();
  }

  PlotLegend(Plot::Normal);

  return;
}

bool Plotter::PlotEfficiency()
{
  TGraphAsymmErrors *gr = CreateEfficiency();
  if(!gr) { error("No se pudo crear el TGraphAsymmErrors."); return false; }

  gr->GetXaxis()->SetTitle("");
  gr->GetYaxis()->SetTitle("Efficiency");

  GetColours();

  gr->SetMarkerColor(colours[0]);
  gr->SetLineColor(colours[0]);
  gr->SetMarkerStyle(marker_style);
  gr->SetMarkerSize(marker_size);
  gr->SetLineWidth(line_width);

  gr->Draw("PAZT");

  return true;
}

bool Plotter::PlotRatios(bool down)
{
  Int_t nRatios = items_sel.size() - 1;

  if(nRatios==0) return false;

  TH1 *ratio[nRatios];
  for(int k=0; k<nRatios; k++){
    ratio[k] = CreateRatio(k+1, 0, down);
  }

  if(!ratio[0]) {
    error("There are no ratios to plot.");
    return false;
  }

  ratio[0]->GetYaxis()->SetTitle("Ratio");
  ratio[0]->SetStats(0);
  if(down) {
    ratio[0]->GetXaxis()->SetRangeUser(x_min,x_max);
    ratio[0]->GetYaxis()->CenterTitle();
    ratio[0]->GetXaxis()->SetTitleSize( 0.08 );
    ratio[0]->GetXaxis()->SetLabelSize( 0.08 );
    ratio[0]->GetYaxis()->SetLabelSize( 0.08 );
    ratio[0]->GetYaxis()->SetTitleSize( 0.08 );
    ratio[0]->GetYaxis()->SetTitleOffset( 0.4 );
    ratio[0]->GetXaxis()->SetTitleOffset( 1.1 );
  }

  GetColours();
  for(int n=0; n<nRatios; n++){
    if(down){
      ratio[n]->SetMarkerColor(colours[n+1]);
      ratio[n]->SetLineColor(colours[n+1]);
    }
    else{
      ratio[n]->SetMarkerColor(colours[n]);
      ratio[n]->SetLineColor(colours[n]);
    }

    if(n==0) ratio[n]->Draw();
    else ratio[n]->Draw("same");
  }

  if(!down) PlotLegend(Plot::Ratio);

  return true;
}

bool Plotter::PlotRelativeDiffs(bool down)
{
  /* Plot the relative differences of the selected histograms with respect the first one.
     diff = (hN - h1)/h1 */

  Int_t nDiffs = items_sel.size() - 1;

  if(nDiffs==0) return false;

  TH1 *diff[nDiffs];
  for(int k=0; k<nDiffs; k++){
    diff[k] = CreateRelativeDiff(k+1, 0, down);
  }

  if(!diff[0]) {
    error("There are no differences to plot.");
    return false;
  }

  diff[0]->GetYaxis()->SetTitle("Relative difference");
  diff[0]->SetStats(0);
  if(down) {
    diff[0]->GetXaxis()->SetRangeUser(x_min,x_max);
    diff[0]->GetYaxis()->CenterTitle();
    diff[0]->GetXaxis()->SetTitleSize( 0.08 );
    diff[0]->GetXaxis()->SetLabelSize( 0.08 );
    diff[0]->GetYaxis()->SetLabelSize( 0.08 );
    diff[0]->GetYaxis()->SetTitleSize( 0.08 );
    diff[0]->GetYaxis()->SetTitleOffset( 0.4 );
    diff[0]->GetXaxis()->SetTitleOffset( 1.1 );
  }

  GetColours();
  for(int n=0; n<nDiffs; n++){
    if(down){
      diff[n]->SetMarkerColor(colours[n+1]);
      diff[n]->SetLineColor(colours[n+1]);
    }
    else{
      diff[n]->SetMarkerColor(colours[n]);
      diff[n]->SetLineColor(colours[n]);
    }

    if(n==0) diff[n]->Draw();
    else diff[n]->Draw("same");
  }

  return true;
}

void Plotter::PlotLegend(Plot::Type type)
{
  vector<TString> legend;

  if( items_sel.size()==1 ){ //1 solo histo de 1 solo file, return sin legend
    return;
  }

  // Legend config
  vector<int> hsv = GetNumberOfObjectsInEachFile();

  bool mtitle=false; bool mfile=false; bool mtitlefile=false;
  if( hsv[0]==1 && hsv[1]==1 ){ //1 solo histo de >1 files
    mfile=true;
  }
  else if(hsv[0] && !hsv[1]){ // >1 histo de 1 solo file
    mtitle=true;
  }
  else if(hsv[0] && hsv[1] ){ // >1 histo de >1 file
    mtitlefile=true;
  }

  vector<TString> legtemp;
  for(unsigned int k=0; k<items_sel.size(); k++){

      TString tmp = "";
      if(mfile){
        tmp = boxes[items_sel[k]->GetFile()]->GetHeaderText();
      }
      else if(mtitle){
        items_sel[k]->GetLegendText();
      }
      else if(mtitlefile){
        tmp = " (" + boxes[items_sel[k]->GetFile()]->GetHeaderText() + ")";
        tmp=items_sel[k]->GetLegendText()+tmp;
      }
      legend.push_back(tmp);
      legtemp.push_back(tmp);
  }

  if(type==Plot::Ratio){
    for(unsigned int i=1;i<legend.size();i++){
      legend[i] += "/";
      legend[i] += legend[0];
    }
  }

  // legend size
  sort(legtemp.begin(), legtemp.end());
  reverse(legtemp.begin(),legtemp.end());

  Double_t maxwidth = legtemp[0].Sizeof() * 0.01;
  Double_t maxheight = items_sel.size() * 0.035;

  Double_t xmin, xmax, ymin, ymax;

  xmax = 0.86; ymax = 0.86;
  ymin = (ymax - maxheight)>0.2 ? ymax - maxheight : 0.2;
  xmin = (xmax - maxwidth)>0.2  ? xmax - maxwidth  : 0.2;

  // Create and plot legend
  TLegend *leg = new TLegend(xmin, ymin, xmax, ymax);
  leg->SetFillColor(0);
  unsigned int begin = type!=Plot::Normal ? 1 : 0;
  for(unsigned int k=begin; k<items_sel.size(); k++){
    leg->AddEntry(plot_list->At(k), legend[k]);
  }
  leg->Draw();


  //if(_macroRecording){
    //macro->AddLegend(legend);
  //}

  return;
}

vector<int> Plotter::GetNumberOfObjectsInEachFile()
{
  vector<int> hsv;
  for(unsigned int f=0; f<m_number_of_files; f++) hsv.push_back(0);

  for(unsigned int k=0; k<items_sel.size(); k++){
    hsv[items_sel[k]->GetFile()]++;
  }

  sort(hsv.begin(),hsv.end());
  reverse(hsv.begin(),hsv.end());

  return hsv;
}

Int_t Plotter::SaveCanvases()
{
  // Save all open canvases to .eps files.

  Int_t n = 0;

  for(UInt_t k=0; k < canvas.size(); k++){
    char tmp[50]; sprintf(tmp, "canvas%d",k);

    TCanvas *old = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(tmp);
    if (!old || !old->IsOnHeap())
      continue;

    sprintf(tmp, "canvas%d.eps",k);
    canvas[k]->Print(tmp);
    n++;
  }

  return n;
}



/* Create
   ------*/

//void Plotter::CreateOther(Int_t id)
//{

  // if(v_sel.size()==0) return;

  // //  int val;
  // // SetBit(kDontCallClose);
  // // new TGMsgBox(fClient->GetRoot(), this, "Inis", "bla bla bla", kMBIconQuestion, kMBDismiss, &val);
  // // ResetBit(kDontCallClose);


  // v *tmp = new v(-1, vfile.size(), name, title, othersList->GetSize(), V::Histo1D, false);
  // othersvs.push_back(tmp);

//}

// TH1* AddHistos()
// {
  //CreatePlotRatio();

  // for(int i=0; i< plot_list->GetSize(); i++){
  //   if(!plot_list->At(i)->InheritsFrom("TH1")) {
  //     error();
  //     return 0;
  //   }
  // }


  // TH1* h = (TH1*) plot_list->
//}


TGraphAsymmErrors* Plotter::CreateEfficiency()
{
  // Create efficiency if there are two selected histos.

  CreatePlotList();

  if(plot_list->GetSize()!=2) { error("Solo funciona si seleccionas dos histogramas."); return 0; }

  if(!plot_list->At(0)->IsA()->InheritsFrom(TH1::Class()) || !plot_list->At(1)->IsA()->InheritsFrom(TH1::Class())) {
    error("Solo funciona si seleccionas dos histogramas.");
    return 0;
  }

  TH1 *h_numerator, *h_denominator;

  h_numerator   = (TH1*) plot_list->At(1)->Clone("h_numerator");
  h_denominator = (TH1*) plot_list->At(0)->Clone("h_denominator");

  TGraphAsymmErrors *gr = new TGraphAsymmErrors();

  gr->Divide(h_numerator, h_denominator, "cl=0.683 b(0.5,0.5) mode");

  return gr;
}

TH1* Plotter::CreateRatio(int index_first, int index_last, bool down)
{

  if(!down) CreatePlotList();

  if(plot_list->GetSize()!=2) { error("Solo funciona con dos histogramas."); return 0; }

  if(!plot_list->At(index_first)->IsA()->InheritsFrom(TH1::Class()) || !plot_list->At(index_last)->IsA()->InheritsFrom(TH1::Class())) {
    error("Solo funciona si seleccionas dos histogramas."); return 0;
  }

  TH1 *h_numerator, *h_denominator;

  h_numerator   = (TH1*)plot_list->At(index_first)->Clone("h_numerator");
  h_denominator = (TH1*)plot_list->At(index_last)->Clone("h_denominator");

  TH1 *ratio = (TH1*)h_numerator->Clone("ratio");

  ratio->Divide(h_denominator);

  return ratio;
}

TH1* Plotter::CreateRelativeDiff(int index_first, int index_last, bool down)
{
  // Compute the relative difference between two histograms
  // diff = (h2 - h1)/h1

  if(!down) CreatePlotList();

  if(plot_list->GetSize()!=2) { error("Solo con dos histogramas."); return 0; }

  if(!plot_list->At(index_first)->IsA()->InheritsFrom(TH1::Class()) || !plot_list->At(index_last)->IsA()->InheritsFrom(TH1::Class())) {
    error("Solo funciona con dos histogramas."); return 0;
  }

  TH1 *h_first, *h_last;

  h_first = (TH1*)plot_list->At(index_first)->Clone("h_first");
  h_last  = (TH1*)plot_list->At(index_last)->Clone("h_last");

  TH1 *diff = (TH1*)h_first->Clone("diff");

  diff->Add(h_last, -1.0)  ;
  diff->Divide(h_first);

  return diff;
}


/* Macro
   ----- */

void Plotter::BeginMacro()
{
  /* Begin macro recording */

  // macro = new Macro("macro");

  // for(unsigned int i=0; i<number_of_files; i++){
  //   macro->AddFile(file_names[i]);
  // }

  // menuMacro->DisableEntry(M_MACRO_BEGIN);
  // menuMacro->EnableEntry(M_MACRO_RESET);
  // menuMacro->EnableEntry(M_MACRO_CREATE_ROOT);
  // menuMacro->EnableEntry(M_MACRO_CREATE_PYTHON);

  // _macroRecording = true;
  // return;
}

void Plotter::ResetMacro()
{
  // macro->Reset();
  // msg("The macro has been reseted.");
}

void Plotter::CreateMacro(OutputFormat type)
{
  /* End/Create/Save macro. Open a dialog to select the macro name. */

  // static TString dir(".");
  // TGFileInfo fi;
  // fi.fIniDir    = StrDup(dir);
  // new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);
  // if(fi.fFilename) {

  //   macro->SaveMacro(fi.fFilename, type);

  //   msg("The macro " << fi.fFilename << " has been created. ");
  // }

  // return;
}



/* Settings
   --------*/

void Plotter::LoadSettings()
{
  // Load configuration values from .plotterrc if exists, or the default values

  TEnv env(settings_file);

  for(Int_t k=0; k<20; k++){
    char temp[50]; sprintf(temp, "Colour%d", k);
    Color_t ctemp = ConvertStringToColour(env.GetValue(temp, default_colours[k]));
    pcolors[k] = TColor::Number2Pixel(ctemp);
  }

  //Style
  marker_size  = 0.8; //env.GetValue("MarkerSize", 0.8);
  marker_style = 20; //env.GetValue("MarkerStyle", 20);
  line_width   = 1; //env.GetValue("LineWidth", 1);

  return;
}

void Plotter::SaveSettings()
{
  // Save colours in .plotterrc. Por ahora no lo uso

  // TEnv env(settingsFile);
  // for(Int_t k=0; k<20; k++){
  //   pcolors[k] = colorselect[k]->GetColor();
  //   char temp[50]; sprintf(temp, "Colour%d", k);
  //   env.SetValue(temp, TColor::PixelAsHexString(pcolors[k]) );
  // }
  // env.SaveLevel(kEnvUser);

  return;
}

Color_t Plotter::ConvertStringToColour(const char *c)
{
  // Convert string colour to color_t colour. Buscar si hay una forma mejor de hacer esto :/

  TString str(c);
  Color_t colour;

  if(str.Contains("kBlack"))
    colour = kBlack;
  else if(str.Contains("kRed"))
    colour = kRed;
  else if(str.Contains("kBlue"))
    colour = kBlue;
  else if(str.Contains("kGreen"))
    colour = kGreen;
  else if(str.Contains("kYellow"))
    colour = kYellow;
  else if(str.Contains("kMagenta"))
    colour = kMagenta;
  else if(str.Contains("kCyan"))
    colour = kCyan;
  else if(str.Contains("kOrange"))
    colour = kOrange;
  else if(str.Contains("kAzure"))
    colour = kAzure;
  else if(str.Contains("kPink"))
    colour = kPink;
  else if(str.Contains("kSpring"))
    colour = kSpring;
  else if(str.Contains("kTeal"))
    colour = kTeal;
  else if(str.Contains("kViolet"))
    colour = kViolet;
  else if(str.Contains("kGray"))
    colour = kGray;

  if(str.Contains("+")){
    TString index = str(str.Index("+")+1, str.Length());
    colour += index.Atoi();
  }

  if(str.Contains("-")){
    TString index = str(str.Index("-")+1, str.Length());
    colour -= index.Atoi();
  }

  return colour;
}



/* Slots
   -----*/

void Plotter::OnItemClick(Int_t id)
{
  /* Handle click on item
     - Toggle item status
     - If status:
                 true  -> Add item to items_selected depending its status
                 false -> Erase item from items_selected depending its status
  */

  Int_t nentry = id_to_entry(id);
  Int_t nfile  = id_to_file(id);

  Item *it = boxes[nfile]->GetItem(nentry);

  // if folder/tree do nothing and return
  if(!it->IsPlotable()) {
    ClearSelection();
    return;
  }

   // toggle status
  it->ToggleStatus();

  // add/erase -> items_sel
  if( it->GetStatus() )
    items_sel.push_back(it);
  else{
    for(UInt_t k=0; k<items_sel.size(); k++){
      if( items_sel[k]->GetId() == id )
        items_sel.erase(items_sel.begin()+k);
    }
  }

}

void Plotter::OnItemDoubleClick(TGFrame* f, Int_t btn)
{
  /* Handle double click on item
     (Mouse buttons-> 1: left, 2: middle, 3: right, 4-5: wheel)

     - if IsPlotable -> Draw
  */


  if(btn==1) {

    Int_t id = ((TGLBEntry*)f)->EntryId();

    Int_t nentry = id_to_entry(id);
    Int_t nfile  = id_to_file(id);

    Item *it = boxes[nfile]->GetItem(nentry);

    if(it->IsPlotable()){
      //Draw(Plot::Normal);
      boxes[nfile]->GetContent()->GetEntry(id)->Activate(false);
      OnItemClick(id);
    }
  }
  else if(btn==3){
    //for(UInt_t k=0; k<number_of_files; k++){
      //if(vs[k].size() <= nentry) continue;

      //vs[k][nentry]->SetStatus(true);

    //   v_sel.push_back(vs[k][nentry]);
    // }

    // RefreshGui();
  }
}

void Plotter::OnButtonClearSelection()
{
  ClearSelection();
}

void Plotter::OnButtonDraw()
{
  Draw(Plot::Normal);
}

void Plotter::OnButtonDrawEfficiency()
{
  //  Draw(Plot::Efficiency);
  cout << endl;
  for(int i=0; i<items_sel.size(); i++)
    cout << items_sel[i]->GetName();
  cout<<endl;
}

void Plotter::OnButtonDrawRatio()
{
  Draw(Plot::Ratio);
}

void Plotter::OnButtonExit()
{
  Exit();
}

void Plotter::OnButtonSaveColours()
{
  SaveSettings();
}

void Plotter::ShowHideColours()
{
  if(frame_main->IsVisible(frame_colours)){
    frame_main->HideFrame(frame_colours);
    menu_view->UnCheckEntry(M_VIEW_COLOURS);
    frame_main->Resize(frame_main->GetWidth() - 70, frame_main->GetHeight());
    this->Resize(this->GetWidth() - 70, this->GetHeight());
  }
  else {
    frame_main->ShowFrame(frame_colours);
    menu_view->CheckEntry(M_VIEW_COLOURS);
    frame_main->Resize(frame_main->GetWidth() + 70, frame_main->GetHeight());
    this->Resize(this->GetWidth() + 70, this->GetHeight());
  }
}

void Plotter::ShowHideCuts()
{
  if(this->IsVisible(entry_cuts)){
    this->HideFrame(entry_cuts);
    menu_view->UnCheckEntry(M_VIEW_CUTS);
    this->Resize(this->GetWidth(), this->GetHeight() - 30);
  }
  else {
    this->ShowFrame(entry_cuts);
    menu_view->CheckEntry(M_VIEW_CUTS);
    this->Resize(this->GetWidth(), this->GetHeight() + 30);
  }
}
