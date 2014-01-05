/* plotter : plotter.h */

#ifndef PLOTTER_H
#define PLOTTER_H

//C++
#include <iostream>
using namespace std;

//ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TStyle.h>
#include <TKey.h>
#include <TList.h>
#include <TCanvas.h>
#include <TString.h>
#include <TColor.h>
#include <TMath.h>
#include <TGraphAsymmErrors.h>

//GUI
#include <TApplication.h>
#include <TGFrame.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFontDialog.h>
#include <TGFSContainer.h>
#include <TGString.h>
#include <TGButton.h>
#include <TGColorSelect.h>
#include <TGClient.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGLayout.h>
#include <TGMenu.h>
#include <TGDockableFrame.h>
#include <TGSlider.h>
#include <TGFileDialog.h>
#include <TGStatusBar.h>
#include <TGListTree.h>

//plotter
#include "base.h"
#include "item.h"
#include "itemsbox.h"
#include "macro.h"

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

namespace Plot {
  enum  Type{
    Normal,
    Efficiency,
    Ratio
  };
}



class Plotter : public TGMainFrame {

 public:
  Plotter(vector<TString> files, bool merge = false);
  virtual ~Plotter();

  // Slots (must be public!)
  void OnItemClick(Int_t);
  void OnItemDoubleClick(TGFrame*, Int_t);
  void OnButtonClearSelection();
  void OnButtonDraw();
  void OnButtonDrawEfficiency();
  void OnButtonDrawRatio();
  void OnButtonExit();
  void OnButtonSaveColours();
  void ShowHideColours();
  void ShowHideCuts();

 private:

  // Gui widgets
  TGCompositeFrame   *fFrame, *fVFrame, *fChecks, *fHistosOptions, *fHist2, *fLog, *fRebin;
  TGVerticalFrame    *fOptionsFrame, *fColoursFrame;
  TGHorizontalFrame  *fRowFrame[3];
  TGStatusBar        *fStatusBar;
  TGLayoutHints      *layoutButtons, *layoutMenuBar, *layoutMenuBarItem, *layoutChecks;
  TGTextButton       *buttonClearSelection, *buttonDraw, *buttonDrawEfficiency, *buttonDrawRatio, *buttonDrawAndRatio, *buttonDrawAndDiff, *buttonExit, *buttonSaveColours;
  TGGroupFrame       *gGeneralOptions, *gDrawOptions, *gHistosOptions, *gHistos2DOptions, *gColours;
  TGTextEntry        *entryCuts;
  TGLabel            *labelRebin, *labelHist2, *labelStatus;
  TGNumberEntry      *nentryRebin;
  TGHSlider          *sliderRebin;
  TGCheckButton      *checkNormalise, *checkNormalise2, *checkHist, *checkScatter, *checkText, *checkBox, *checkStats, *checkAtlas, *checkLogX, *checkLogY, *checkOrder, *checkP, *checkPie, *checkIncludeRatio, *checkIncludeDiff;
  TGRadioButton      *radioColz, *radioScatter, *radioBox;
  TGMenuBar          *menuBar;
  TGPopupMenu        *menuFile, *menuMacro, *menuCreate, *menuView;
  TGColorSelect      *colorselect[20];
  TGCheckButton      *checkFill[20];

  ItemsBox *fb[15];

  //-- fns
  void                CreateMainWindow();
  void                CreateMainFrame();
  void                CreateOptionsFrame();
  void                CreateMenuBar();
  void                CreateStatusBar();
  void                CreateColoursFrame();
  void                CreateCutsEntry();
  void                CreateMergedFileBox();
  Bool_t              ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  void                Exit();
  void                ExitError();
  void                Action(int, int);
  void                ClearSelection();
  Int_t               SaveCanvases();
  void                CloseWindow();
  void                LoadSettings();
  void                SaveSettings();
  TGraphAsymmErrors*  CreateEfficiency();
  TH1*                CreateRatio(int index_first = 1, int index_last = 0, bool down = false);
  TH1*                CreateRelativeDiff(int index_first = 1, int index_last = 0, bool down = false);
  void                GetColours();
  Color_t             ConvertStringToColour(const char *c);
  void                SetStyle();
  void                CreatePlotList();
  void                CreateSelList();
  void                ConfigurePlotList();
  void                Draw(Plot::Type);
  void                PlotHistos();
  void                PlotGraphs();
  bool                PlotRatios(bool down = false);
  bool                PlotRelativeDiffs(bool down = false);
  bool                PlotEfficiency();
  void                PlotLegend(Plot::Type);
  vector<int>         GetNumberOfObjectsInEachFile();
  void                BeginMacro();
  void                ResetMacro();
  void                CreateMacro(OutputFormat);
  TObject*            GetObject(Item* it);


  // global vars
  UInt_t                number_of_files;
  vector<TString>       file_names;
  vector<Item*>         items_sel;
  TList                 *plot_list;
  vector<TCanvas*>      canvas;
  Double_t              x_min, x_max, y_min, y_max;
  Pixel_t               pcolors[20];
  Color_t               colours[20];
  TStyle                *plotter_style;
  short                 marker_style;
  float                 marker_size;
  short                 line_width;
  Macro                 *macro;
  TChain                *merge_chain;

 protected:
  Bool_t  _mergeMode;
  Bool_t  _macroRecording;

  ClassDef(Plotter, 0);
};
#endif //PLOTTER_H
