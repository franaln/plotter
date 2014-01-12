/** @file plotter.cxx
*/

#include "plotter.h"

#include <TEnv.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#include <TBranch.h>
#include <TLeaf.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>

#include "item.h"
#include "itemsbox.h"
#include "obj.h"
#include "plot.h"
//#include "macro.h"

const char* settings_file = ".plotterrc";

const char* default_colours[] = {
  "kBlack",
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
  "kMagenta-9"
};

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
  if(i->GetFile() < j->GetFile()) return true;
  if(i->GetFile() == j->GetFile() && i->GetEntry() < j->GetEntry()) return true;
  return false;
}


ClassImp(Plotter);

Plotter::Plotter(std::vector<TString> filenames, bool merge) :
  TGMainFrame(gClient->GetRoot(), 800, 500),
  m_file_names(filenames),
  m_merge_mode(merge),
  m_macro_recording(false)
{
  SetCleanup(kLocalCleanup);

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
  for(int k=0; k<m_plots.size(); k++) delete m_plots[k];
}

/** Create main window:
    - menu bar
    - main frame (with fileboxes)
    - options frame
    - colours
    - cuts
*/
void Plotter::CreateMainWindow()
{
  CreateMenuBar();
  frame_main = new TGCompositeFrame(this, 1, 1, kHorizontalFrame);
  CreateMainFrame();
  CreateOptionsFrame();
  CreateColoursFrame();
  AddFrame(frame_main, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 2, 0, 2));
  CreateCutsEntry();
}

/** Create menu bar:
    - File: Save all canvases, Exit
    - View : Colours, Cuts
    - Create:
    - Macro: Begin, Reset, Save ROOT macro, Save python macro
*/
void Plotter::CreateMenuBar()
{
  layout_menu_bar = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
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

/* Create frame with fileboxes.
   Each filebox represents a file
*/
void Plotter::CreateMainFrame()
{
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
  gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(), x, y, width, height);

  height = height * 0.75;

  UInt_t h = 600;
  UInt_t w = 200;
  if(n_rows == 1)      h = height - 200;
  else if(n_rows == 2) h = int(height/2);
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

/** @fn CreateOptionsFrame
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
  button_draw_efficiency->SetToolTipText("Plot the efficiency between the two selected histos (hlast/hfirst).");
  button_draw_ratio->SetToolTipText("Plot the ratio between the selected histos wrt the first one selected (hn/hfirst).");

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
  button_draw_efficiency->Connect("Clicked()", "Plotter", this, "OnButtonDrawEfficiency()");
  button_draw_ratio->Connect("Clicked()", "Plotter", this, "OnButtonDrawRatio()");
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
  AddFrame(entry_cuts, new TGLayoutHints(kLHintsExpandX, 5, 2, 2, 2));
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
          SavePlots();
        }
        break;

      case M_FILE_SETTINGS:
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

      case M_MACRO_BEGIN:
        {
          // macro = new Macro("macro");
          // for(unsigned int i=0; i<m_number_of_files; i++){
          //   macro->AddFile(m_file_names[i]);
          // }
          // menu_macro->DisableEntry(M_MACRO_BEGIN);
          // menu_macro->EnableEntry(M_MACRO_RESET);
          // menu_macro->EnableEntry(M_MACRO_CREATE_ROOT);
          // menu_macro->EnableEntry(M_MACRO_CREATE_PYTHON);
          // m_macro_recording = true;
        }
        break;

      case M_MACRO_RESET:
        {
          //macro->Reset();
          //msg("The macro has been reseted.");
        }
        break;

      case M_MACRO_CREATE_ROOT:
        {
          //CreateMacro(MRoot);
        }
        break;

      case M_MACRO_CREATE_PYTHON:
        {
          //CreateMacro(MPython);
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
  for(int k=0; k<m_items.size(); k++){
    colours[k] = TColor::GetColor(pcolors[k]);
    if(frame_main->IsVisible(frame_colours))
      colours[k] = TColor::GetColor(colorselect[k]->GetColor());
  }
}

/**
   - clear items_sel
   - clear plot_list
   - set items status to false
*/
void Plotter::ClearSelection()
{
  m_items.clear();
  for(UInt_t i=0; i<m_number_of_files; i++){
    boxes[i]->Clear();
  }

  return;
}

Obj* Plotter::GetObject(Item* it)
{
  int file = it->GetFile();
  TString name = it->GetName();

  TObject *obj;
  Obj *pobj;

  if(it->IsBranch()){
    TString cut = "";
    cut = TString(entry_cuts->GetText()).EqualTo("Cuts") ? "" : entry_cuts->GetText();

    TTree* tree = boxes[file]->GetCurrentTree();
    tree->Draw(name+">>h", cut, "goff");

    obj = (TH1*)gDirectory->Get("h");
  }
  else {
    TDirectory *dir = boxes[file]->GetCurrentDir();
    obj = dir->Get(name);
  }

  pobj = new Obj((TH1*)obj);

  return pobj;
}

/** Draw function. Creates a plot with the selected items and options
*/
void Plotter::Draw()
{
  if(m_items.size()==0) return;

  // Plot order: 1) Selected order (default). 2) Order by file and entry.
  if(check_order->GetState())  sort(m_items.begin(), m_items.end(), SortVs);

  Plot *p = new Plot();

  GetColours();

  for(UInt_t k=0; k<m_items.size(); k++){
    if(!m_items[k]->IsPlotable()) continue;
    p->Add(GetObject(m_items[k]), colours[k], check_fill[k]->GetState());
  }

  if(check_include_ratio->GetState()) p->SetIncludeRatio(true);
  else if(check_include_diff->GetState()) p->SetIncludeDiff(true);


  TString draw_opts = "";
  if(check_text->GetState()) draw_opts += "text,";
  if(check_hist->GetState()) draw_opts += "hist,";
  if(check_p->GetState())    draw_opts += "P,";
  if(check_pie->GetState())  draw_opts += "PIE,";

  if(radio_scatter->GetState())  draw_opts += "scat,";
  else if(radio_box->GetState()) draw_opts += "box,";
  else  draw_opts += "colz,";

  p->SetDrawOptions(draw_opts);


  p->Create();

  m_plots.push_back(p);

  return;
}

void Plotter::DrawEfficiency()
{
  if(m_items.size() != 2) {
    error("You can only draw the efficiency from two histograms.");
    return;
  }

}

void Plotter::DrawRatio()
{
  if(m_items.size() != 2) {
    error("You can only draw the ratio from two histograms.");
    return;
  }

}

std::vector<int> Plotter::GetNumberOfObjectsInEachFile()
{
  std::vector<int> hsv;
  for(unsigned int f=0; f<m_number_of_files; f++) hsv.push_back(0);

  for(unsigned int k=0; k<m_items.size(); k++){
    hsv[m_items[k]->GetFile()]++;
  }

  sort(hsv.begin(),hsv.end());
  reverse(hsv.begin(),hsv.end());

  return hsv;
}

/** Save all open plots to .eps files
 */
void Plotter::SavePlots()
{
  for(UInt_t k=0; k < m_plots.size(); k++){
    m_plots[k]->Save();
  }

  return;
}

/* Macro
   ----- */

/** End, create and save macro.
    Open a dialog to select the macro name.
*/
void Plotter::CreateMacro(OutputFormat type)
{
  // static TString dir(".");
  // TGFileInfo fi;
  // fi.fIniDir    = StrDup(dir);
  // new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);
  // if(fi.fFilename) {

  //   macro->SaveMacro(fi.fFilename, type);

  //   msg("The macro " << fi.fFilename << " has been created. ");
  // }

}


/** Load configuration values
    Use .plotterrc if exists, or default values otherwise
*/
void Plotter::LoadSettings()
{
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

// void Plotter::SaveSettings()
// {
//   // Save colours in .plotterrc. Por ahora no lo uso

//   // TEnv env(settingsFile);
//   // for(Int_t k=0; k<20; k++){
//   //   pcolors[k] = colorselect[k]->GetColor();
//   //   char temp[50]; sprintf(temp, "Colour%d", k);
//   //   env.SetValue(temp, TColor::PixelAsHexString(pcolors[k]) );
//   // }
//   // env.SaveLevel(kEnvUser);

//   return;
// }

/** Convert string colour to color_t colour.
    Buscar si hay una forma mejor de hacer esto :/
*/
Color_t Plotter::ConvertStringToColour(const char *c)
{
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

/** Handle click on item
    - Toggle item status
    - If status:
    true  -> Add item to items_selected depending its status
    false -> Erase item from items_selected depending its status
*/
void Plotter::OnItemClick(Int_t id)
{
  Int_t entry = id_to_entry(id);
  Int_t file  = id_to_file(id);

  Item *it = boxes[file]->GetItem(entry);

  // if folder/tree do nothing and return
  if(!it->IsPlotable()) {
    ClearSelection();
    return;
  }

  // toggle status
  it->ToggleStatus();

  // add/erase -> items_sel
  if(it->GetStatus())
    m_items.push_back(it);
  else{
    for(UInt_t k=0; k<m_items.size(); k++){
      if( m_items[k]->GetId() == id )
        m_items.erase(m_items.begin()+k);
    }
  }
}

/** Handle double click on item
    (Mouse buttons-> 1: left, 2: middle, 3: right, 4-5: wheel)
    - if IsPlotable -> Draw
*/
void Plotter::OnItemDoubleClick(TGFrame* f, Int_t btn)
{
  if(btn==1) {

    Int_t id = ((TGLBEntry*)f)->EntryId();

    Int_t entry = id_to_entry(id);
    Int_t file  = id_to_file(id);

    Item *it = boxes[file]->GetItem(entry);

    if(it->IsPlotable()){
      Draw();
      boxes[file]->GetContent()->GetEntry(id)->Activate(false);
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
