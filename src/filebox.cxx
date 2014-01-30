/** @file itemsbox.cxx
*/

#include <TGPicture.h>
#include <TGResourcePool.h>

#include "filebox.h"

ClassImp(FileBox);

FileBox::FileBox(TGWindow *main, UInt_t w, UInt_t h, Int_t column, TString filename) :
  TGVerticalFrame(main, w, h, kVerticalFrame),
  m_column(column)
{
  SetCleanup(kDeepCleanup);

  m_file = new TFile(filename, "open");
  CreateGui(GetFilenameFromPath(filename));
  BrowseItems();

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

void FileBox::CreateGui(TString header_text)
{
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

void FileBox::BrowseItems(TString fname)
{
  m_items.clear();

  int entry = 0;
  last_path = current_path;
  current_path = fname;

  if(!fname.EqualTo("")){
    m_items.push_back(new Item(m_column, 0, "..", "..", Back));
    entry++;
  }

  TDirectory *dir = m_file->GetDirectory(fname);

  TIter next(dir->GetListOfKeys());
  TKey *key;
  TObject *obj;
  TString name, title;

  while ((key=(TKey*)next())) {

    obj  = key->ReadObj() ;
    name = obj->GetName();
    title = obj->GetTitle();

    if(name.EqualTo("")) name = obj->GetTitle();

    if(key->IsFolder()){
      if(obj->InheritsFrom("TTree")){
        Item *it = new Item(m_column, entry, name, title, Tree);
        m_items.push_back(it);
      }
      else{
        Item *it = new Item(m_column, entry, name, title, Dir);
        m_items.push_back(it);
      }
    }
    else if(obj->InheritsFrom("TH1")) {
      Item *it;
      if(obj->InheritsFrom("TH3"))
        it = new Item(m_column, entry, name, title, Hist3D);
      else if(obj->InheritsFrom("TH2"))
        it = new Item(m_column, entry, name, title, Hist2D);
      else
        it = new Item(m_column, entry, name, title, Hist1D);
      m_items.push_back(it);
    }
    else if (obj->InheritsFrom("TGraph")) {
      Item *it = new Item(m_column, entry, name, title, Graph);
      m_items.push_back(it);
    }
    entry++;
  }

  ShowItems();
}

void FileBox::BrowseTree(TString name)
{
  m_items.clear();

  last_path = current_path;
  current_path = name;

  Item *it = new Item(m_column, 0, "..", "..", Back);
  m_items.push_back(it);

  TTree *t = new TTree(name, "");
  m_file->GetObject(name, t);

  TObjArray *l = t->GetListOfBranches();
  int nbranches = l->GetSize();
  for(Int_t k=0; k<nbranches; k++){
    TObject *branch = l->At(k);
    if(!branch) continue;
    Item *it = new Item(m_column, k+1, branch->GetName(), branch->GetTitle(), Branch);
    m_items.push_back(it);
  }

  ShowItems();
}

/** Clear and then display the current list of items in the ListBox
 */
void FileBox::ShowItems()
{
  m_content->RemoveAll();

  //const TGPicture *iconpic;

  for(unsigned int k=0; k<m_items.size(); k++){


    TGIconLBEntry *it = new TGIconLBEntry(m_content->GetContainer(),
                                          m_items[k]->GetId(),
                                          m_items[k]->GetText(),
                                          gClient->GetPicture(m_items[k]->GetIcon()),
                                          0, kVerticalFrame,
                                          GetWhitePixel());

    m_content->AddEntry(it, new TGLayoutHints(kLHintsExpandX));
  }

  RefreshGui();
}

void FileBox::Clear()
{
  for(unsigned int i=0; i<m_items.size();i++){
    m_items[i]->SetStatus(false);
    m_content->GetEntry(m_items[i]->GetId())->Activate(false);
  }

  RefreshGui();
}

TString FileBox::GetFilenameFromPath(TString path)
{
  TString filename(path);
  filename.ReplaceAll(".root","");
  while(filename.Contains("/")){
    filename = filename(filename.Index("/")+1, filename.Length());
  }

  return (filename.Length() <= 50) ? filename : filename = filename(0, 50);
}

/** Trick to refresh the gui
 */
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
  Int_t k = id_to_entry(id);

  if(m_items[k]->IsPlotable()) return;

  if(m_items[k]->IsTree())      BrowseTree( m_items[k]->GetName() );
  else if(m_items[k]->IsDir())  BrowseItems( m_items[k]->GetName() );
  else if(m_items[k]->IsBack()) GoBack();
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
