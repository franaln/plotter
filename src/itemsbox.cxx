/** @file filebox.cxx
*/

#include "itemsbox.h"

ClassImp(ItemsBox);

ItemsBox::ItemsBox(TGWindow *main, UInt_t w, UInt_t h, Int_t column) :
  TGVerticalFrame(main, w, h, kVerticalFrame),
  n(column)
{
  SetCleanup(kDeepCleanup);

  MapSubwindows();
  Resize(w, h);
  MapWindow();
}

ItemsBox::~ItemsBox()
{
  file->Close(); delete file;
  delete header;
  delete content;
}

void ItemsBox::CreateGui(TString header_text)
{
  // header
  header = new TGTextEntry(this, header_text, 0);
  AddFrame(header, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                    kLHintsExpandX, 2, 2, 2, 2));

  header->SetAlignment(kTextCenterX);
  header->Resize(155,20);
  header->SetMaxLength(200);
  header->MoveResize(0,8,150,20);
  header->SetToolTipText("File name (edit to get a good legend)");

  // content box
  content = new TGListBox(this, 0, kSunkenFrame | kDoubleBorder, 0xffff00);
  content->SetMultipleSelections(true);
  content->Resize(200, 600);
  content->Layout();
  content->Associate(this);

  content->Connect("Selected(Int_t)", "ItemsBox", this, "OnItemClick(Int_t)");
  content->GetContainer()->Connect("DoubleClicked(TGFrame*, Int_t)", "ItemsBox",
                                   this, "OnItemDoubleClick(TGFrame*, Int_t)");

  AddFrame(content,
           new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
}

void ItemsBox::AddFile(TString filename)
{
  filenames.push_back(filename);

  file = new TFile(filenames[0], "open");

  CreateGui(GetFilenameFromPath(filenames[0]));

  BrowseItems();
}


void ItemsBox::BrowseItems(TString fname)
{
  items.clear();

  int entry = 0;
  last_path = current_path;
  current_path = fname;

  if(!fname.EqualTo("")){
    items.push_back(new Item(n, 0, "..", "..", Back));
    entry++;
  }

  TDirectory *dir = file->GetDirectory(fname);

  TIter next(dir->GetListOfKeys());
  TKey *key;
  TObject *obj;
  TString name, title;

  while ((key=(TKey*)next())) {

    obj  = key->ReadObj() ;
    name = obj->GetName();
    title = obj->GetTitle();

    if(name.EqualTo("")) name = obj->GetTitle();

    if( key->IsFolder() ){
      if( obj->InheritsFrom("TTree") ){
        Item *tmp = new Item( n, entry, name, title, Tree);
        items.push_back(tmp);
      }
      else{
        Item *tmp = new Item( n, entry, name, title, Dir);
        items.push_back(tmp);
      }
    }
    else if( obj->InheritsFrom("TH1")  ) {
      Item *tmp;
      if(obj->InheritsFrom("TH3"))
        tmp = new Item( n, entry, name, title, Hist3D);
      else if(obj->InheritsFrom("TH2"))
        tmp = new Item( n, entry, name, title, Hist2D);
      else
        tmp = new Item( n, entry, name, title, Hist1D);
      items.push_back(tmp);
    }
    else if ( obj->InheritsFrom("TGraph")  ) {
      Item *tmp = new Item( n, entry, name, title, Graph);
      items.push_back(tmp);
    }
    entry++;
  }

  ShowItems();
}

void ItemsBox::BrowseTree(TString name)
{
  items.clear();

  last_path = current_path;
  current_path = name;

  Item *tmp = new Item( n, 0, "..", "..", Back);
  items.push_back(tmp);

  TTree *t = new TTree(name, "");
  file->GetObject(name, t);

  TObjArray *l = t->GetListOfBranches();
  int nbranches = l->GetSize();
  for(Int_t k=0; k<nbranches; k++){
    TObject *branch = l->At(k);
    if(!branch) continue;
    Item *tmp = new Item( n, k+1, branch->GetName(), branch->GetTitle(), Branch);
    items.push_back(tmp);
  }

  ShowItems();
}

void ItemsBox::ShowItems()
{
  content->RemoveAll();


  for(unsigned int k=0; k<items.size(); k++){
    TGIconLBEntry *it = new TGIconLBEntry(content->GetContainer(),
                                          items[k]->GetId(),
                                          items[k]->GetText(),
                                          items[k]->GetIcon(),
                                          0, kVerticalFrame,
                                          GetWhitePixel());

    content->AddEntry(it, new TGLayoutHints(kLHintsExpandX));

  }

  RefreshGui();
}

void ItemsBox::GoBack()
{
  BrowseItems(last_path);
}

void ItemsBox::Clear()
{
  for(unsigned int i=0; i<items.size();i++){
    items[i]->SetStatus(false);
    content->GetEntry(items[i]->GetId())->Activate(false);
  }

  RefreshGui();
}

TString ItemsBox::GetFilenameFromPath(TString path)
{
  TString filename(path);
  filename.ReplaceAll(".root","");
  while(filename.Contains("/")){
    filename = filename(filename.Index("/")+1, filename.Length());
  }

  return (filename.Length() <= 50) ? filename : filename = filename(0, 50);
}

void ItemsBox::RefreshGui()
{
  /* Trick to refresh the gui */
  content->Resize(content->GetWidth()-1, content->GetHeight());
  content->Resize(content->GetWidth()+1, content->GetHeight());
  return;
}


/* Slots
   ---- */

void ItemsBox::OnItemClick(Int_t id)
{
  Int_t k = id_to_entry(id);

  if(items[k]->IsPlotable()) return;

  if( items[k]->IsTree() )      BrowseTree( items[k]->GetName() );
  else if( items[k]->IsDir() )  BrowseItems( items[k]->GetName() );
  else if( items[k]->IsBack() ) GoBack();
}

void ItemsBox::OnItemDoubleClick(TGFrame* f, Int_t btn)
{
  // if (btn!=kButton1) return;

  // TGLBEntry *entry = (TGLBEntry *)f;

  // Int_t k = IdToEntry(entry->EntryId());

  // if(items[k]->IsPlotable()) return;

  // if( items[k]->IsTree() )      BrowseTree( items[k]->GetName() );
  // else if( items[k]->IsDir() )  BrowseItems( items[k]->GetName() );
  // else if( items[k]->IsBack() ) GoBack();
}
