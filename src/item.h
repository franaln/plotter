/* plotter : item.h */

#ifndef ITEM_H
#define ITEM_H

#include "TROOT.h"
#include <iomanip>
#include <iostream>
#include <TString.h>
#include <cmath>
#include <TGPicture.h>
#include <TGResourcePool.h>

#include "base.h"

typedef enum{
  Dir, Tree, 
  H1, H2, H3, 
  Branch, Graph, 
  Back, None
} ItemType;


class Item {
  
 private:
  Int_t     nfile;
  Int_t     nentry;
  TString   name;
  TString   title;
  TString   path;
  ItemType  type;
  Bool_t    status;
  Int_t     id;
  
 public:
  Item(Int_t _nfile, Int_t _nentry, TString _name, TString _title, ItemType _type);
  virtual ~Item();
  
  TString       GetName()    { return name; }
  TString       GetTitle()   { return title; }
  TString       GetText()    { return name.EqualTo("") ? "(no name)" : name; }
  TString       GetLegendText() { return title.EqualTo("") ? name : title; }
  ItemType      GetType()    { return type; };
  Bool_t        GetStatus()  { return status; }
  Int_t         GetId()      { return id; }
  Int_t         GetFile()    { return nfile; };
  Int_t         GetEntry()   { return nentry; };
  
  const TGPicture* GetIcon();

  Bool_t IsDir()       { return type==Dir ? true : false; }
  Bool_t IsTree()      { return type==Tree ? true : false; }
  Bool_t IsBack()      { return type==Back ? true : false; }
  Bool_t IsPlotable()  { return (type==H1 || type==H2 || type==H3 || type==Graph || type==Branch) ? true : false; }
  Bool_t IsBranch()    { return type==Branch ? true : false; }      
  Bool_t IsTypeHist()  { return (type==H1 || type==H2 || type==H3 || type==Branch) ? true : false; }
  Bool_t IsTypeGraph() { return type==Graph ? true : false; }

  void     ToggleStatus() { status = status ? false : true; }
  void     SetStatus(bool st)    { status = st; }
};

#endif

