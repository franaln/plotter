/* plotter : filebox.h */

#ifndef FILEBOX_H
#define FILEBOX_H

#include <iostream>
using namespace std;

#include <TROOT.h>
#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGTextEntry.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TChain.h>

#include "item.h"

#define MSG(x) cout << ":: " << x << endl;
#define ERROR(x,y) cout << "Error in <Plotter::" << x << ">: " << y << endl;

class FileBox  : public TGVerticalFrame {

public:
  FileBox(TGWindow *main, UInt_t w, UInt_t h, Int_t _ncolumn);
  virtual ~FileBox();


  void        AddFile(TString);
  Item*       GetItem(int entry) { return items[entry]; };
  TString     GetHeaderText() { return header->GetText(); };
  TGListBox*  GetContent() { return content; };
  TFile*      GetFile() { return file; };
  TDirectory* GetCurrentDir()  { return file->GetDirectory(current_path); };
  TTree*      GetCurrentTree() {  return (TTree*)file->Get(current_path); };
  
  void Clear();

  //slot
  void OnItemDoubleClick(TGFrame*, Int_t);
  void OnItemClick(Int_t);

 protected:
  void     CreateGui(TString);
  void     RefreshGui();

  void     BrowseItems(TString fname="");
  void     BrowseTree(TString name);
  void     GoBack();
  void     ShowItems();
  TString  GetFilenameFromPath(TString);  

  Int_t            n;
  vector<TString>  filenames;
  TFile            *file;
  vector<Item*>    items;

  TString last_path, current_path;

  //gui
  TGTextEntry      *header;
  TGListBox        *content;

  ClassDef(FileBox, 0);
};
#endif
