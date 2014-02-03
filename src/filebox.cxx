/** @file itemsbox.cxx */

#include <TGPicture.h>
#include <TGResourcePool.h>

#include "filebox.h"

ClassImp(FileBox);

FileBox::FileBox(TGWindow *main, UInt_t w, UInt_t h, TString filename) :
  TGVerticalFrame(main, w, h, kVerticalFrame),
  entry(0)
{
  SetCleanup(kDeepCleanup);

  m_file = new TFile(filename, "open");

  filename.ReplaceAll(".root","");
  while(filename.Contains("/")){
    filename = filename(filename.Index("/")+1, filename.Length());
  }

  CreateGui(filename);

  parent = new ParentItem(0, "", "", Dir);

  BrowseDir(parent);

  ShowItems();

  MapSubwindows();
  Resize(w, h);
  MapWindow();
}

FileBox::~FileBox()
{
  m_file->Close();
  delete m_file;
  delete m_header;
  delete m_content;
}

void FileBox::CreateGui(TString filename)
{
  TString header_text;

  header_text = (filename.Length() <= 50) ? filename : filename(0, 50);

  // header
  m_header = new TGTextEntry(this, header_text, 0);
  AddFrame(m_header, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                    kLHintsExpandX, 2, 2, 2, 2));

  m_header->SetAlignment(kTextCenterX);
  m_header->Resize(155,20);
  m_header->SetMaxLength(200);
  m_header->MoveResize(0,8,150,20);

  // content box
  m_content = new TGListBox(this, 0, kSunkenFrame | kDoubleBorder, 0xffff00);
  m_content->SetMultipleSelections(true);
  m_content->Resize(200, 600);
  m_content->Layout();
  m_content->Associate(this);

  m_content->Connect("Selected(Int_t)", "FileBox", this, "OnItemClick(Int_t)");
  m_content->GetContainer()->Connect("DoubleClicked(TGFrame*, Int_t)", "FileBox",
                                     this, "OnItemDoubleClick(TGFrame*, Int_t)");

  AddFrame(m_content,
           new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
}

void FileBox::BrowseDir(ParentItem* pt)
{
  last_path = current_path;
  current_path = pt->GetName();;

  TDirectory *dir = m_file->GetDirectory(pt->GetName());

  TIter next(dir->GetListOfKeys());
  TKey *key;
  TObject *obj;
  TString name, title;

  while ((key=(TKey*)next())) {

    obj  = key->ReadObj() ;
    name = obj->GetName();
    title = obj->GetTitle();

    if(name.IsNull()) name = obj->GetTitle();

    if(key->IsFolder()){
      if(obj->InheritsFrom("TTree")){
        ParentItem *it = new ParentItem(entry, name, title, Tree);
        pt->AddItem(it);
        entry++;
        BrowseTree(it);

      }
      else{
        ParentItem *it = new ParentItem(entry, name, title, Dir);
        pt->AddItem(it);
        entry++;
        BrowseDir(it);
      }
    }
    else if(obj->InheritsFrom("TH1")) {
      Item *it;
      if(obj->InheritsFrom("TH3"))
        it = new Item(entry, name, title, Hist3D);
      else if(obj->InheritsFrom("TH2"))
        it = new Item(entry, name, title, Hist2D);
      else
        it = new Item(entry, name, title, Hist1D);
      pt->AddItem(it);
      entry++;
    }
    else if (obj->InheritsFrom("TGraph")) {
      Item *it = new Item(entry, name, title, Graph);
      pt->AddItem(it);
      entry++;
    }

  }

}

void FileBox::BrowseTree(ParentItem *pt)
{
  TTree *t = new TTree(pt->GetName(), "");
  m_file->GetObject(pt->GetName(), t);

  TObjArray *l = t->GetListOfBranches();
  int nbranches = l->GetSize();
  for(Int_t k=0; k<nbranches; k++){
    TObject *branch = l->At(k);
    if(!branch) continue;
    Item *it = new Item(entry, branch->GetName(), branch->GetTitle(), Branch);
    pt->AddItem(it);
    entry++;
  }

}

/** Clear and then display the list of items in the ListBox  */
void FileBox::ShowItems()
{
  m_content->RemoveAll();

  for(unsigned int k=0; k<parent->GetN(); k++){
    TGIconLBEntry *it = new TGIconLBEntry(m_content->GetContainer(),
                                          parent->GetItem(k)->GetId(),
                                          parent->GetItem(k)->GetText(),
                                          gClient->GetPicture(parent->GetItem(k)->GetIcon()),
                                          0, kVerticalFrame,
                                          GetWhitePixel());
    m_content->AddEntry(it, new TGLayoutHints(kLHintsExpandX));
  }

  RefreshGui();
}


void FileBox::OpenItem(int id)
{
  ParentItem *pt = (ParentItem*)parent->GetItemFromId(id);

  for(unsigned int k=0; k<pt->GetN(); k++){
    TGIconLBEntry *it = new TGIconLBEntry(m_content->GetContainer(),
                                          pt->GetItem(k)->GetId(),
                                          pt->GetItem(k)->GetText(),
                                          gClient->GetPicture(pt->GetItem(k)->GetIcon()),
                                          0, kVerticalFrame,
                                          GetWhitePixel());

    m_content->InsertEntry(it, new TGLayoutHints(kLHintsExpandX), id+k);
  }

  pt->ToggleStatus();

  RefreshGui();
}

void FileBox::CloseItem(int id)
{
  ParentItem *pt = (ParentItem*) parent->GetItemFromId(id);

  for(unsigned int k=0; k<pt->GetN(); k++){
    m_content->RemoveEntry(id+k+1);
  }

  pt->ToggleStatus();
  RefreshGui();
}

void FileBox::Clear()
{
  for(unsigned int i=0; i<parent->GetN();i++){
    parent->GetItem(i)->SetStatus(false);
    m_content->GetEntry(parent->GetItem(i)->GetId())->Activate(false);
  }

  RefreshGui();
}

/** Trick to refresh the gui  */
void FileBox::RefreshGui()
{
  m_content->Resize(m_content->GetWidth()-1, m_content->GetHeight());
  m_content->Resize(m_content->GetWidth()+1, m_content->GetHeight());
  return;
}


/* Slots
   ---- */
void FileBox::OnItemClick(Int_t id)
{
  //  if(parent->GetItem(k)->IsPlotable()) return;

  if(parent->GetItemFromId(id)->IsTree() || parent->GetItemFromId(id)->IsDir()){
    if(((ParentItem*)parent->GetItemFromId(id))->IsOpen()) CloseItem(id);
    else OpenItem(id);
  }

}

void FileBox::OnItemDoubleClick(TGFrame* f, Int_t btn)
{
  // if (btn!=kButton1) return;

  // TGLBEntry *entry = (TGLBEntry *)f;

  // Int_t k = IdToEntry(entry->EntryId());

  // if(items[k]->IsPlotable()) return;

  // if( items[k]->IsTree() )      BrowseTree( items[k]->GetName() );
  // else if( items[k]->IsDir() )  BrowseItems( items[k]->GetName() );
  // else if( items[k]->IsBack() ) GoBack();
}
