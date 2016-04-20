/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                                  wxparaver                                *
 *              Paraver Trace Visualization and Analysis Tool                *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL$
 | @last_commit: $Date: 2016/03/02 14:24:12 $
 | @version:     $Revision: 1.359 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includesMPI
#include "wx/imaglist.h"
////@end includes

#include "wx/tipdlg.h"
#include "wx/aboutdlg.h"
#include "wx/html/htmlwin.h"

#include "wx/fs_mem.h"
#include "wx/filesys.h"
#include "wx/wfstream.h"
#include "logoBSC.xpm"

#include "config.h"
#include "paravermain.h"
#include "paraverkernelexception.h"
#include "cfg.h"
#include "window.h"
#include "histogram.h"
#include "gtimeline.h"
#include "ghistogram.h"
//#include <wx/propgrid/advprops.h>
#include "pg_extraprop.h"
#include "progresscontroller.h"
#include "loadedwindows.h"
#include "filter.h"
#include "pg_util.h"
#include "saveconfigurationdialog.h"
//#include "preferencesdialog.h"
#include "windows_tree.h"
#include "derivedtimelinedialog.h"
#include "histogramdialog.h"
#include "cutfilterdialog.h"
#include "labelconstructor.h"
#include "sessionsaver.h"
#include "helpcontents.h"
#include "filedialogext.h"
#include "runscript.h"

#include <signal.h>
#include <iostream>
#include <algorithm>

#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <shlobj.h>
#include <Shlwapi.h>

#define MAX_LEN_PATH 2048
#endif

#ifdef __WXMAC__
#include <CoreFoundation/CoreFoundation.h>
#endif

////@begin XPM images
#include "new_window.xpm"
#include "new_derived_window.xpm"
#include "new_histogram.xpm"
#include "delete.xpm"
#include "cut_trace.xpm"
#include "run_script.xpm"
#include "file_browser.xpm"
#include "window_properties.xpm"
////@end XPM images

#include "table.xpm"
#include "timeline.xpm"

#include "derived_add.xpm"
#include "derived_product.xpm"
#include "derived_substract.xpm"
#include "derived_divide.xpm"
#include "derived_maximum.xpm"
#include "derived_minimum.xpm"
#include "derived_different.xpm"
#include "derived_controlled_clear_by.xpm"
#include "derived_controlled_maximum.xpm"
#include "derived_controlled_add.xpm"

#include <algorithm>

#ifdef WIN32
inline double rint( double nr )
{
  double f = floor( nr );
  double c = ceil( nr );
  return ( ( ( c - nr ) >= ( nr - f ) ) ? f : c );
}
#endif


/*!
 * paraverMain type definition
 */

IMPLEMENT_CLASS( paraverMain, wxFrame )


/*!
 * paraverMain event table definition
 */

BEGIN_EVENT_TABLE( paraverMain, wxFrame )

////@begin paraverMain event table entries
  EVT_CLOSE( paraverMain::OnCloseWindow )
  EVT_ICONIZE( paraverMain::OnIconize )
  EVT_SIZE( paraverMain::OnSize )
  EVT_IDLE( paraverMain::OnIdle )
  EVT_MENU( wxID_OPEN, paraverMain::OnOpenClick )
  EVT_UPDATE_UI( ID_RECENTTRACES, paraverMain::OnRecenttracesUpdate )
  EVT_MENU( ID_UNLOADTRACE, paraverMain::OnUnloadtraceClick )
  EVT_UPDATE_UI( ID_UNLOADTRACE, paraverMain::OnUnloadtraceUpdate )
  EVT_MENU( ID_MENULOADCFG, paraverMain::OnMenuloadcfgClick )
  EVT_UPDATE_UI( ID_MENULOADCFG, paraverMain::OnMenuloadcfgUpdate )
  EVT_UPDATE_UI( ID_RECENTCFGS, paraverMain::OnMenuloadcfgUpdate )
  EVT_MENU( ID_MENUSAVECFG, paraverMain::OnMenusavecfgClick )
  EVT_UPDATE_UI( ID_MENUSAVECFG, paraverMain::OnMenusavecfgUpdate )
  EVT_MENU( ID_MENULOADSESSION, paraverMain::OnMenuloadsessionClick )
  EVT_MENU( ID_MENUSAVESESSION, paraverMain::OnMenusavesessionClick )
  EVT_MENU( wxID_PREFERENCES, paraverMain::OnPreferencesClick )
  EVT_UPDATE_UI( wxID_PREFERENCES, paraverMain::OnPreferencesUpdate )
  EVT_MENU( wxID_EXIT, paraverMain::OnExitClick )
  EVT_MENU( wxID_HELPCONTENTS, paraverMain::OnHelpcontentsClick )
  EVT_MENU( wxID_TUTORIALS, paraverMain::OnTutorialsClick )
  EVT_MENU( wxID_ABOUT, paraverMain::OnAboutClick )
  EVT_MENU( ID_NEW_WINDOW, paraverMain::OnToolNewWindowClick )
  EVT_UPDATE_UI( ID_NEW_WINDOW, paraverMain::OnToolNewWindowUpdate )
  EVT_MENU( ID_NEW_DERIVED_WINDOW, paraverMain::OnNewDerivedWindowClick )
  EVT_UPDATE_UI( ID_NEW_DERIVED_WINDOW, paraverMain::OnNewDerivedWindowUpdate )
  EVT_MENU( ID_NEW_HISTOGRAM, paraverMain::OnNewHistogramClick )
  EVT_UPDATE_UI( ID_NEW_HISTOGRAM, paraverMain::OnNewHistogramUpdate )
  EVT_MENU( ID_TOOLDELETE, paraverMain::OnTooldeleteClick )
  EVT_UPDATE_UI( ID_TOOLDELETE, paraverMain::OnTooldeleteUpdate )
  EVT_MENU( ID_TOOL_CUT_TRACE, paraverMain::OnToolCutTraceClick )
  EVT_UPDATE_UI( ID_TOOL_CUT_TRACE, paraverMain::OnToolCutTraceUpdate )
  EVT_MENU( ID_TOOL_RUN_APPLICATION, paraverMain::OnToolRunApplicationClick )
  EVT_CHOICEBOOK_PAGE_CHANGED( ID_CHOICEWINBROWSER, paraverMain::OnChoicewinbrowserPageChanged )
  EVT_UPDATE_UI( ID_CHOICEWINBROWSER, paraverMain::OnChoicewinbrowserUpdate )
  EVT_UPDATE_UI( ID_FOREIGN, paraverMain::OnForeignUpdate )
  EVT_BUTTON( ID_BUTTON_ACTIVE_WORKSPACES, paraverMain::OnButtonActiveWorkspacesClick )
////@end paraverMain event table entries

  EVT_TREE_SEL_CHANGED( wxID_ANY, paraverMain::OnTreeSelChanged )
  EVT_TREE_ITEM_ACTIVATED( wxID_ANY, paraverMain::OnTreeItemActivated )
  EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, paraverMain::OnTreeRightClick)
  
  EVT_TREE_BEGIN_DRAG(wxID_ANY, paraverMain::OnTreeBeginDrag)
  EVT_TREE_END_DRAG( wxID_ANY, paraverMain::OnTreeEndDrag)

  EVT_PG_CHANGED( ID_FOREIGN, paraverMain::OnPropertyGridChange )

  EVT_ACTIVATE(paraverMain::OnActivate)
  
  EVT_TIMER( wxID_ANY, paraverMain::OnSessionTimer )

END_EVENT_TABLE()


paraverMain* paraverMain::myParaverMain = NULL;

wxProgressDialog *paraverMain::dialogProgress = NULL;

wxSize paraverMain::defaultTitleBarSize = wxSize(0,0);

Window *paraverMain::beginDragWindow = NULL;
Window *paraverMain::endDragWindow = NULL;

extern volatile bool sig1;
extern volatile bool sig2;
extern struct sigaction act;

static const std::string wxparaverMessages[ UserMessageSize ] =
{
  "None of the events specified in the filter appear in the trace. Continue loading CFG file?",
  "Some of the events specified in the filter doesn't appear in the trace. Continue loading CFG file?",
  "Some timeline has 0 objects selected at some level. Continue loading CFG file?"
};

static bool userMessage( UserMessageID message )
{
  wxMessageDialog tmpDialog( NULL, wxString::FromAscii( wxparaverMessages[ message ].c_str() ), _( "Paraver question" ), wxYES_NO | wxICON_QUESTION );
  paraverMain::myParaverMain->SetRaiseCurrentWindow( false );
  int tmpResult = tmpDialog.ShowModal();
  paraverMain::myParaverMain->SetRaiseCurrentWindow( true );
  return tmpResult == wxID_YES;
}

wxImageList *paraverMain::getImageList()
{
  wxImageList *images;

  images = new wxImageList( 16, 16 );
  images->Add( wxIcon( table_xpm ) );
  images->Add( wxIcon( timeline_xpm ) );

  // Derived icons added in same order than kernel returns
  images->Add( wxIcon( derived_add_xpm ) );
  images->Add( wxIcon( derived_product_xpm ) );
  images->Add( wxIcon( derived_substract_xpm ) );
  images->Add( wxIcon( derived_divide_xpm ) );
  images->Add( wxIcon( derived_maximum_xpm ) );
  images->Add( wxIcon( derived_minimum_xpm ) );
  images->Add( wxIcon( derived_different_xpm ) );
  images->Add( wxIcon( derived_controlled_clear_by_xpm ) );
  images->Add( wxIcon( derived_controlled_maximum_xpm ) );
  images->Add( wxIcon( derived_controlled_add_xpm ) );

  return images;
}


/*!
 * paraverMain constructors
 */

paraverMain::paraverMain()
{
  imageList = getImageList();

  LocalKernel::init();
  localKernel = new LocalKernel( userMessage );
  myParaverMain = this;

  Init();
  ShowToolTips();
}

paraverMain::paraverMain( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  imageList = getImageList();

  LocalKernel::init();
  localKernel = new LocalKernel( userMessage );
  myParaverMain = this;

  Init();
  Create( parent, id, caption, pos, size, style );
  
  defaultTitleBarSize = GetSize() - GetClientSize();
  ShowToolTips();
}


void paraverMain::commandLineLoadings( wxCmdLineParser &parser )
{
  string fileName;
  for ( unsigned int i = 0; i < parser.GetParamCount(); ++i )
  {
    fileName = parser.GetParam( i ).mb_str();
    
    if ( Trace::isTraceFile( fileName ) )
    {
      DoLoadTrace( fileName );
    }
    else if ( CFGLoader::isCFGFile( fileName ) && !loadedTraces.empty() )
    {
      DoLoadCFG( fileName );
    }

    fileName.erase();
  }
}

/*!
 * paraverMain creator
 */

bool paraverMain::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin paraverMain creation
  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls();
////@end paraverMain creation

  return true;
}


/*!
 * paraverMain destructor
 */

paraverMain::~paraverMain()
{
////@begin paraverMain destruction
  GetAuiManager().UnInit();
////@end paraverMain destruction

// DISCONNECTS
  wxMenuItem *tmpItem2 = menuFile->FindItem( ID_RECENTTRACES );
  wxMenu *menuTraces2 = tmpItem2->GetSubMenu();
  wxMenuItemList& menuItems2 = menuTraces2->GetMenuItems();
  for (wxMenuItemList::iterator menuIt = menuItems2.begin(); menuIt != menuItems2.end() ; ++menuIt )
  {
    wxMenuItem *tmp = *menuIt;
    int id = tmp->GetId();
    Disconnect( id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&paraverMain::OnPreviousTracesClick );
  }

  wxMenuItem *tmpItem = menuFile->FindItem( ID_RECENTCFGS );
  wxMenu *menuCFGs = tmpItem->GetSubMenu();
  wxMenuItemList& menuItems = menuCFGs->GetMenuItems();
  for (wxMenuItemList::iterator menuIt = menuItems.begin(); menuIt != menuItems.end() ; ++menuIt )
  {
    wxMenuItem *tmp = *menuIt;
    int id = tmp->GetId();
    Disconnect( id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&paraverMain::OnPreviousCFGsClick );
  }

  for( vector<Trace *>::iterator it = loadedTraces.begin(); it != loadedTraces.end(); ++it )
    delete *it;
  delete localKernel;
  
  delete imageList;
  if ( tutorialsWindow != NULL )
  {
    delete tutorialsWindow;
  }
  if ( helpContents != NULL )
  {
    delete helpContents;
  }
  
  wxMemoryFSHandler::RemoveFile( wxT("logoBSC.xpm") );
}


/*!
 * Member initialisation
 */

void paraverMain::Init()
{
////@begin paraverMain member initialisation
  CFGLoadedBefore = false;
  XMLLoadedBefore = false;
  canServeSignal = true;
  clusteringWindow = NULL;
  currentHisto = NULL;
  currentTimeline = NULL;
  currentTrace = -1;
  currentWindow = NULL;
  helpContents = NULL;
  lastHisto = NULL;
  lastTimeline = NULL;
  numNewDerived = 0;
  numNewHistograms = 0;
  numNewWindows = 0;
  paraverConfig = ParaverConfig::getInstance();
  previousCFGs = PreviousFiles::createPreviousCFGs();
  previousCutFilteredTraces = PreviousFiles::createPreviousTreatedTraces();
  previousTraces = PreviousFiles::createPreviousTraces();
  raiseCurrentWindow = true;
  sessionTimer = new wxTimer( this );
  traceLoadedBefore = false;
  tutorialsWindow = NULL;
  workspacesManager = WorkspaceManager::getInstance();
  menuFile = NULL;
  menuHints = NULL;
  menuHelp = NULL;
  tbarMain = NULL;
  choiceWindowBrowser = NULL;
  toolBookFilesProperties = NULL;
  dirctrlFiles = NULL;
  windowProperties = NULL;
  txtActiveWorkspaces = NULL;
  btnActiveWorkspaces = NULL;
////@end paraverMain member initialisation

  traceLoadedBefore = false;
  CFGLoadedBefore = false;

  wxFileSystem::AddHandler( new wxMemoryFSHandler() );
#ifdef WIN32
  wxBitmap bmp( logoBSC_xpm );
#endif

  wxMemoryFSHandler::AddFile( wxT( "logoBSC.xpm" ),
#ifdef WIN32
                              bmp,
#else
                              wxBITMAP( logoBSC ),
#endif
                              wxBITMAP_TYPE_XPM );
                              
  workspacesManager->loadXML();
}

/*!
 * Control creation for paraverMain
 */

void paraverMain::CreateControls()
{    
////@begin paraverMain content construction
  paraverMain* itemFrame1 = this;

  GetAuiManager().SetManagedWindow(this);

  wxMenuBar* menuBar = new wxMenuBar;
  menuFile = new wxMenu;
  menuFile->Append(wxID_OPEN, _("Load &Trace..."), wxEmptyString, wxITEM_NORMAL);
  wxMenu* itemMenu5 = new wxMenu;
  menuFile->Append(ID_RECENTTRACES, _("Previous Traces"), itemMenu5);
  menuFile->Append(ID_UNLOADTRACE, _("Unload Traces..."), wxEmptyString, wxITEM_NORMAL);
  menuFile->AppendSeparator();
  menuFile->Append(ID_MENULOADCFG, _("Load &Configuration..."), wxEmptyString, wxITEM_NORMAL);
  wxMenu* itemMenu9 = new wxMenu;
  menuFile->Append(ID_RECENTCFGS, _("Previous Configurations"), itemMenu9);
  menuFile->Append(ID_MENUSAVECFG, _("&Save Configuration..."), wxEmptyString, wxITEM_NORMAL);
  menuFile->AppendSeparator();
  menuFile->Append(ID_MENULOADSESSION, _("Load Session...\tCTRL+l"), wxEmptyString, wxITEM_NORMAL);
  menuFile->Append(ID_MENUSAVESESSION, _("Save Session...\tCTRL+S"), wxEmptyString, wxITEM_NORMAL);
  menuFile->AppendSeparator();
  menuFile->Append(wxID_PREFERENCES, _("&Preferences..."), wxEmptyString, wxITEM_NORMAL);
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT, _("&Quit"), wxEmptyString, wxITEM_NORMAL);
  menuBar->Append(menuFile, _("&File"));
  menuHints = new wxMenu;
  menuBar->Append(menuHints, _("Hints"));
  menuHelp = new wxMenu;
  menuHelp->Append(wxID_HELPCONTENTS, _("&Help Contents..."), wxEmptyString, wxITEM_NORMAL);
  menuHelp->Append(wxID_TUTORIALS, _("&Tutorials..."), wxEmptyString, wxITEM_NORMAL);
  menuHelp->Append(wxID_ABOUT, _("&About..."), wxEmptyString, wxITEM_NORMAL);
  menuBar->Append(menuHelp, _("&Help"));
  itemFrame1->SetMenuBar(menuBar);

  tbarMain = new wxToolBar( itemFrame1, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER|wxWANTS_CHARS );
  wxBitmap itemtool24Bitmap(itemFrame1->GetBitmapResource(wxT("new_window.xpm")));
  wxBitmap itemtool24BitmapDisabled;
  tbarMain->AddTool(ID_NEW_WINDOW, _("Create new window"), itemtool24Bitmap, itemtool24BitmapDisabled, wxITEM_NORMAL, _("New single timeline window"), wxEmptyString);
  tbarMain->EnableTool(ID_NEW_WINDOW, false);
  wxBitmap itemtool25Bitmap(itemFrame1->GetBitmapResource(wxT("new_derived_window.xpm")));
  wxBitmap itemtool25BitmapDisabled;
  tbarMain->AddTool(ID_NEW_DERIVED_WINDOW, _("Create new derived window"), itemtool25Bitmap, itemtool25BitmapDisabled, wxITEM_NORMAL, _("New derived timeline window"), wxEmptyString);
  tbarMain->EnableTool(ID_NEW_DERIVED_WINDOW, false);
  wxBitmap itemtool26Bitmap(itemFrame1->GetBitmapResource(wxT("new_histogram.xpm")));
  wxBitmap itemtool26BitmapDisabled;
  tbarMain->AddTool(ID_NEW_HISTOGRAM, _("Create new histogram"), itemtool26Bitmap, itemtool26BitmapDisabled, wxITEM_NORMAL, _("New histogram"), wxEmptyString);
  tbarMain->EnableTool(ID_NEW_HISTOGRAM, false);
  tbarMain->AddSeparator();
  wxBitmap itemtool28Bitmap(itemFrame1->GetBitmapResource(wxT("delete.xpm")));
  wxBitmap itemtool28BitmapDisabled;
  tbarMain->AddTool(ID_TOOLDELETE, _("Delete window"), itemtool28Bitmap, itemtool28BitmapDisabled, wxITEM_NORMAL, _("Delete selected window"), wxEmptyString);
  tbarMain->AddSeparator();
  wxBitmap itemtool30Bitmap(itemFrame1->GetBitmapResource(wxT("cut_trace.xpm")));
  wxBitmap itemtool30BitmapDisabled;
  tbarMain->AddTool(ID_TOOL_CUT_TRACE, _("Filter Trace"), itemtool30Bitmap, itemtool30BitmapDisabled, wxITEM_NORMAL, _("Filter Trace"), wxEmptyString);
  wxBitmap itemtool31Bitmap(itemFrame1->GetBitmapResource(wxT("run_script.xpm")));
  wxBitmap itemtool31BitmapDisabled;
  tbarMain->AddTool(ID_TOOL_RUN_APPLICATION, _("Run Application"), itemtool31Bitmap, itemtool31BitmapDisabled, wxITEM_NORMAL, _("Run Application"), wxEmptyString);
  tbarMain->Realize();
  itemFrame1->GetAuiManager().AddPane(tbarMain, wxAuiPaneInfo()
    .ToolbarPane().Name(wxT("auiTBarMain")).Top().Layer(10).CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(false).Floatable(false).Gripper(true));

  choiceWindowBrowser = new wxChoicebook( itemFrame1, ID_CHOICEWINBROWSER, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT|wxWANTS_CHARS );

  itemFrame1->GetAuiManager().AddPane(choiceWindowBrowser, wxAuiPaneInfo()
    .Name(wxT("auiWindowBrowser")).Caption(_("Window browser")).Centre().Position(1).CloseButton(false).DestroyOnClose(false).Resizable(true));

  toolBookFilesProperties = new wxToolbook( itemFrame1, ID_TOOLBOOKFILESANDPROPERTIES, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );
  wxImageList* toolBookFilesPropertiesImageList = new wxImageList(16, 16, true, 2);
  {
    wxIcon toolBookFilesPropertiesIcon0(itemFrame1->GetIconResource(wxT("file_browser.xpm")));
    toolBookFilesPropertiesImageList->Add(toolBookFilesPropertiesIcon0);
    wxIcon toolBookFilesPropertiesIcon1(itemFrame1->GetIconResource(wxT("window_properties.xpm")));
    toolBookFilesPropertiesImageList->Add(toolBookFilesPropertiesIcon1);
  }
  toolBookFilesProperties->AssignImageList(toolBookFilesPropertiesImageList);

  dirctrlFiles = new wxGenericDirCtrl( toolBookFilesProperties, ID_DIRCTRLFILES, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_SELECT_FIRST|wxDIRCTRL_SHOW_FILTERS, wxT("Paraver files|*.prv;*.prv.gz;*.cfg|CFG files (*.cfg)|*.cfg|PRV Files (*.prv, *.prv.gz)|*.prv;*.prv.gz"), 0 );

  toolBookFilesProperties->AddPage(dirctrlFiles, wxEmptyString, false, 0);

  windowProperties = new wxPropertyGrid( toolBookFilesProperties, ID_FOREIGN, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxWANTS_CHARS );

  toolBookFilesProperties->AddPage(windowProperties, wxEmptyString, false, 1);

  itemFrame1->GetAuiManager().AddPane(toolBookFilesProperties, wxAuiPaneInfo()
    .Name(wxT("auiCfgAndProperties")).Caption(_("Files & Window Properties")).Centre().Position(2).CloseButton(false).DestroyOnClose(false).Resizable(true).PaneBorder(false));

  wxPanel* itemPanel36 = new wxPanel( itemFrame1, ID_PANEL_WORKSPACES, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  itemFrame1->GetAuiManager().AddPane(itemPanel36, wxAuiPaneInfo()
    .Name(wxT("Pane1")).Caption(_("Workspaces")).Top().Dockable(false).CloseButton(false).DestroyOnClose(false).Resizable(false).Floatable(false).Movable(false).PaneBorder(false));

  wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxHORIZONTAL);
  itemPanel36->SetSizer(itemBoxSizer37);

  txtActiveWorkspaces = new wxTextCtrl( itemPanel36, ID_TEXT_ACTIVE_WORKSPACE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  itemBoxSizer37->Add(txtActiveWorkspaces, 1, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  btnActiveWorkspaces = new wxButton( itemPanel36, ID_BUTTON_ACTIVE_WORKSPACES, _(" ... "), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  btnActiveWorkspaces->Show(false);
  itemBoxSizer37->Add(btnActiveWorkspaces, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  // Fit to content
  itemFrame1->GetAuiManager().GetPane(wxT("Pane1")).BestSize(itemPanel36->GetSizer()->Fit(itemPanel36)).MinSize(itemPanel36->GetSizer()->GetMinSize());

  GetAuiManager().Update();

////@end paraverMain content construction
  wxTreeCtrl* tmpTree = createTree( imageList );
  choiceWindowBrowser->AddPage( tmpTree, _( "All Traces" ) );
#if wxMAJOR_VERSION>=3
  choiceWindowBrowser->AddPage( createTree( imageList ), _( "Dummy Tree" ) );
  choiceWindowBrowser->DeletePage( 1 );
#endif
  
  toolBookFilesProperties->GetToolBar()->SetToolShortHelp( 1, wxT("Paraver Files") );
  toolBookFilesProperties->GetToolBar()->SetToolShortHelp( 2, wxT("Window Properties") );
  
  dirctrlFiles->SetPath( wxString( ParaverConfig::getInstance()->getGlobalCFGsPath().c_str(), wxConvUTF8 ));
  
  setActiveWorkspacesText();
//  refreshMenuHints();

  // These are here because no UpdateUI for MenuBar
  wxUpdateUIEvent tmpEvent;
  OnMenuHintUpdate( tmpEvent );
}


void paraverMain::refreshMenuHints()
{
  bool separator = false;

  // Destroy previous if any
  size_t maxItems = menuHints->GetMenuItemCount();
  for ( size_t i = 0; i < maxItems; ++i )
  {
    wxMenuItem *currentItem = menuHints->FindItemByPosition( 0 );
    //int id = currentItem->GetId();
    //Disconnect( id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&paraverMain::OnHintClick );
    menuHints->Destroy( currentItem );
  }

  if( currentTrace == -1 )
    return;

  // Create updated one
  size_t currentWorkspace = 0;
  for ( vector< string >::iterator it = traceWorkspaces[ loadedTraces[ currentTrace ] ].begin(); it != traceWorkspaces[  loadedTraces[ currentTrace ]  ].end(); ++it )
  {
    wxString currentWorkspaceName = wxString::FromAscii( it->c_str() );
    wxMenu *currentWorkspaceMenu = new wxMenu();

    std::vector< std::pair< std::string, std::string > > currentHints;
    if( currentWorkspace < firstUserWorkspace[ loadedTraces[ currentTrace ] ] ) // Distributed workspaces
    {
      currentHints = workspacesManager->getWorkspace( *it, WorkspaceManager::DISTRIBUTED ).getHintCFGs();
      if( workspacesManager->existWorkspace( *it, WorkspaceManager::USER_DEFINED ) )
      {
        vector< TEventType > tmpDistAutoTypes = workspacesManager->getWorkspace( *it, WorkspaceManager::DISTRIBUTED ).getAutoTypes();
        sort( tmpDistAutoTypes.begin(), tmpDistAutoTypes.end() );
        vector< TEventType > tmpUserAutoTypes = workspacesManager->getWorkspace( *it, WorkspaceManager::USER_DEFINED ).getAutoTypes();
        sort( tmpUserAutoTypes.begin(), tmpUserAutoTypes.end() );
        if( includes( tmpDistAutoTypes.begin(), tmpDistAutoTypes.end(),
                      tmpUserAutoTypes.begin(), tmpUserAutoTypes.end() ) )
        {
          currentHints.push_back( std::pair< std::string, std::string >( "WXSEPARATOR", "WXSEPARATOR" ) );
          std::vector< std::pair< std::string, std::string > > tmpHints = workspacesManager->getWorkspace( *it, WorkspaceManager::USER_DEFINED ).getHintCFGs();
          currentHints.insert( currentHints.end(), tmpHints.begin(), tmpHints.end() );
        }
      }
    }
    else // User defined workspaces
    {
      if( !separator )
      {
        menuHints->AppendSeparator();
        separator = true;
      }
      if( workspacesManager->existWorkspace( *it, WorkspaceManager::DISTRIBUTED ) )
        currentWorkspaceName += wxT( "#2" );
      currentHints = workspacesManager->getWorkspace( *it, WorkspaceManager::USER_DEFINED ).getHintCFGs();
    }

    for ( std::vector<std::pair<std::string,std::string> >::iterator it2 = currentHints.begin(); it2 != currentHints.end(); ++it2 )
    {
      if( (*it2).first == "WXSEPARATOR" && (*it2).second == "WXSEPARATOR" )
      {
        currentWorkspaceMenu->AppendSeparator();
        continue;
      }
      wxString tmpName = getHintComposed( *it2 );
      wxMenuItem *currentHint = new wxMenuItem( currentWorkspaceMenu, wxID_ANY, tmpName );
      currentWorkspaceMenu->Append( currentHint );
      Connect( currentHint->GetId(),
               wxEVT_COMMAND_MENU_SELECTED,
               (wxObjectEventFunction)&paraverMain::OnHintClick );
    }
      
    menuHints->AppendSubMenu( currentWorkspaceMenu, currentWorkspaceName );

    ++currentWorkspace;
  }
}


// Initial set, to be called after inserting new trace
void paraverMain::setTraceWorkspaces( Trace *whichTrace )
{
  set< TEventType > tmpLoadedTypes = whichTrace->getLoadedEvents();
  firstUserWorkspace[ whichTrace ] = 0;
  traceWorkspaces[ whichTrace ].clear();
  workspacesManager->getMergedWorkspaces( tmpLoadedTypes, traceWorkspaces[ whichTrace ], firstUserWorkspace[ whichTrace ] );
}


bool paraverMain::DoLoadTrace( const string &path )
{
  Trace *tr = NULL;
  bool loaded = true;

  canServeSignal = false;

  // Append whole path.
  wxFileName tmpFileName( wxString( path.c_str(), wxConvUTF8 ) );
    
#ifdef WIN32
  tmpFileName.Normalize( wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE |
                         wxPATH_NORM_LONG );
#else
  tmpFileName.Normalize( wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE |
                         wxPATH_NORM_LONG | wxPATH_NORM_TILDE );
#endif

  string tmpPath = std::string( tmpFileName.GetFullPath().mb_str() );

  if( !localKernel->checkTraceSize( tmpPath, ParaverConfig::getInstance()->getFiltersFilterTraceUpToMB() * 1E6 ) )
  {
    TTraceSize traceSize = localKernel->getTraceSize( tmpPath );
    wxString tmpSize;
    tmpSize << rint( traceSize / 1E6 );
    wxMessageDialog maxSizeDialog( this, 
            wxString( wxT( "The size (" ) ) +
                      tmpSize +
                      wxString( wxT( " MB) of the trace\n\n  " ) ) +
                      wxString::FromAscii( tmpPath.c_str() ) +
                      wxString( wxT( "\n\nexceeds the maximum loadable defined"
                                     " in Preferences.\n\nWould you like to cut/filter the trace?" ) ),
            wxT( "Reduce trace size" ),
            wxYES_NO|wxCANCEL|wxICON_QUESTION );

    switch( maxSizeDialog.ShowModal() )
    {
      case wxID_YES:
        ShowCutTraceWindow( tmpPath, true );
        canServeSignal = true;
        return true;
        break;
        
      case wxID_NO:
        break;
      
      case wxID_CANCEL:
        canServeSignal = true;
        return true;
        break;
        
    }
  }
  
  map< string, PRV_UINT32 >::iterator it = traceInstance.find( std::string( tmpFileName.GetFullName().mb_str() ) );
  if ( it == traceInstance.end() )
    traceInstance[ std::string( tmpFileName.GetFullName().mb_str() ) ] = 0;

  ProgressController *progress = ProgressController::create( localKernel );
  progress->setHandler( progressFunction, this );

  try
  {
    if( paraverMain::dialogProgress == NULL )
      paraverMain::dialogProgress = new wxProgressDialog( wxT("Loading trace..."),
                                                          wxT(""),
                                                          numeric_limits<int>::max(),
                                                          this,
                                                          wxPD_CAN_ABORT|wxPD_AUTO_HIDE|\
                                                          wxPD_APP_MODAL|wxPD_ELAPSED_TIME|\
                                                          wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME );
    string reducePath;

    if( tmpPath.length() > 40 && tmpPath.find_last_of( PATH_SEP ) != string::npos )
    {
      string file = tmpPath.substr( tmpPath.find_last_of( PATH_SEP ) );
      string tmp = tmpPath.substr( 0, tmpPath.find_last_of( PATH_SEP ) );
      if ( tmp.find_last_of( PATH_SEP ) != string::npos )
      {
        reducePath = "/..." + tmpPath.substr( tmp.find_last_of( PATH_SEP ),
                                              tmp.length() - tmp.find_last_of( PATH_SEP ) )
                     + file;
      }
      else
      {
        reducePath = "/..." + file;
      }
    }
    else
      reducePath = tmpPath;
    reducePath += "\t";

    paraverMain::dialogProgress->Pulse( wxString::FromAscii( reducePath.c_str() ) );
    paraverMain::dialogProgress->Fit();
    paraverMain::dialogProgress->Show();

    tr = Trace::create( localKernel, tmpPath, false, progress );
    tr->setInstanceNumber( traceInstance[ std::string( tmpFileName.GetFullName().mb_str() ) ]++ );

    loadedTraces.push_back( tr );
    currentTrace = loadedTraces.size() - 1;
    setTraceWorkspaces( tr );
 
    wxTreeCtrl *newTree = createTree( imageList );

    if( paraverConfig->getGlobalFullTracePath() )
      choiceWindowBrowser->AddPage( newTree, wxString::FromAscii( tr->getFileNameNumbered().c_str() ) );
    else
      choiceWindowBrowser->AddPage( newTree, wxFileName( wxString::FromAscii( tr->getFileNameNumbered().c_str() ) ).GetFullName() );
    choiceWindowBrowser->ChangeSelection( choiceWindowBrowser->GetPageCount() - 1 );

    previousTraces->add( tmpPath );

    currentTimeline = NULL;
    currentHisto = NULL;

    traceLoadedBefore = true;
    tracePath = tmpFileName.GetPath();
  }
  catch( ParaverKernelException& ex )
  {
    loaded = false;
    wxMessageDialog message( this, wxString::FromAscii( ex.what() ), _( "Error loading trace" ), wxOK );
    raiseCurrentWindow = false;
    message.ShowModal();
    raiseCurrentWindow = true;
  }
  paraverMain::dialogProgress->Show( false );
  delete paraverMain::dialogProgress;
  paraverMain::dialogProgress = NULL;
  delete progress;

  setActiveWorkspacesText();
  refreshMenuHints();

  canServeSignal = true;
  
#ifndef WIN32
  if ( sig1 || sig2 )
    OnSignal();
#endif
    
  // These are here because no UpdateUI for MenuBar
  wxUpdateUIEvent tmpEvent;
  OnMenuHintUpdate( tmpEvent );

  return loaded;
}

bool paraverMain::DoLoadCFG( const string &path )
{
  if( !CFGLoader::isCFGFile( path ))
  {
    wxString errMessage = wxString::FromAscii( path.c_str() ) + _( " isn't a valid cfg." );
    wxMessageDialog message( this, errMessage, _( "Invalid file" ), wxOK );
    raiseCurrentWindow = false;
    message.ShowModal();
    raiseCurrentWindow = true;
  }
  else
  {
    vector<Window *> newWindows;
    vector<Histogram *> newHistograms;
    SaveOptions options;

    if( !CFGLoader::loadCFG( localKernel, path, loadedTraces[ currentTrace ],
                             newWindows, newHistograms, options ) )
    {
      wxString errMessage = wxString::FromAscii( path.c_str() ) + _( " failed to load in:\n'" ) + wxString::FromAscii( CFGLoader::errorLine.c_str() ) + _( "'" );
      wxMessageDialog message( this, errMessage, _( "Loading error" ), wxOK );
      raiseCurrentWindow = false;
      message.ShowModal();
      raiseCurrentWindow = true;
    }
    else
    {
      wxTreeItemId child;
      for( vector<Window *>::iterator it = newWindows.begin(); it != newWindows.end(); ++it )
      {
        wxTreeCtrl *allTracesPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( 0 );
        wxTreeCtrl *currentPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( currentTrace + 1 );

        if ( (*it)->getChild() == NULL )
          BuildTree( this, allTracesPage, allTracesPage->GetRootItem(), currentPage, currentPage->GetRootItem(), *it );

        if ( it + 1 == newWindows.end() )
          currentTimeline = *it;
      }

      for( vector<Histogram *>::iterator it = newHistograms.begin(); it != newHistograms.end(); ++it )
      {
        wxPoint tmpPos( (*it)->getPosX(), (*it)->getPosY() );
        gHistogram* tmpHisto = new gHistogram( this, wxID_ANY, wxString::FromAscii( (*it)->getName().c_str() ), tmpPos );
        tmpHisto->SetHistogram( *it );

        appendHistogram2Tree( tmpHisto );
        LoadedWindows::getInstance()->add( (*it) );

        tmpHisto->SetClientSize( wxSize( (*it)->getWidth(), (*it)->getHeight() ) );
        if( (*it)->getShowWindow() )
        {
          tmpHisto->Show();
        }
        tmpHisto->execute();
      }

      previousCFGs->add( path );
    }
  }
  return true;
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_OPEN
 */
void paraverMain::OnOpenClick( wxCommandEvent& event )
{
  if ( !traceLoadedBefore )
    tracePath = wxString::FromAscii( paraverConfig->getGlobalTracesPath().c_str() );

  wxFileDialog dialog( this, _( "Load Trace" ), tracePath, _( "" ), 
    _( "Paraver trace (*.prv;*.prv.gz)|*.prv;*.prv.gz|All files (*.*)|*.*" ),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );

  raiseCurrentWindow = false;
  if( dialog.ShowModal() == wxID_OK )
  {
    wxString path = dialog.GetPath();
    DoLoadTrace( std::string( path.mb_str() ) );
  }
  raiseCurrentWindow = true;
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENULOADCFG
 */

void paraverMain::OnMenuloadcfgClick( wxCommandEvent& event )
{
  if ( !CFGLoadedBefore )
   CFGPath =  wxString::FromAscii( paraverConfig->getGlobalCFGsPath().c_str() );

  wxFileDialog dialog( this, _( "Load Configuration" ), CFGPath, _( "" ),
                        _( "Paraver configuration file (*.cfg)|*.cfg|All files (*.*)|*.*" ),
                        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  raiseCurrentWindow = false;
  if( dialog.ShowModal() == wxID_OK )
  {
    wxString path = dialog.GetPath();
    CFGLoadedBefore = true;
    CFGPath = wxFileName( path ).GetPath();
    DoLoadCFG( std::string( path.mb_str() ) );
  }
  raiseCurrentWindow = true;
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
 */

void paraverMain::OnExitClick( wxCommandEvent& event )
{
  if ( !LoadedWindows::getInstance()->emptyWindows() )
  {
    if ( wxMessageBox( wxT( "Some windows already opened... continue closing?" ),
                       wxT( "Please confirm" ),
                       wxICON_QUESTION | wxYES_NO) != wxYES )
    {
      event.Skip();
      return;
    }
  }
  PrepareToExit();
  Destroy();
}


/*!
 * Should we show tooltips?
 */

bool paraverMain::ShowToolTips()
{
  static size_t s_index = (size_t) -1;
  if ( s_index == (size_t)-1 )
  {
    srand( time( NULL ));
    s_index = rand() % 2; // number of tips
  }
  /*
  wxTipProvider *tipProvider = wxCreateFileTipProvider( wxT( "./tips.txt" ), s_index );
  
  // this returns a bool 
  wxShowTip( this, tipProvider, true);
  delete tipProvider;
  */
  
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap paraverMain::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin paraverMain bitmap retrieval
  wxUnusedVar(name);
  if (name == wxT("new_window.xpm"))
  {
    wxBitmap bitmap(application_star_xpm);
    return bitmap;
  }
  else if (name == wxT("new_derived_window.xpm"))
  {
    wxBitmap bitmap(application_add_xpm);
    return bitmap;
  }
  else if (name == wxT("new_histogram.xpm"))
  {
    wxBitmap bitmap(new_histogram_xpm);
    return bitmap;
  }
  else if (name == wxT("delete.xpm"))
  {
    wxBitmap bitmap(delete_xpm);
    return bitmap;
  }
  else if (name == wxT("cut_trace.xpm"))
  {
    wxBitmap bitmap(cut_trace_xpm);
    return bitmap;
  }
  else if (name == wxT("run_script.xpm"))
  {
    wxBitmap bitmap(run_script_xpm);
    return bitmap;
  }
  return wxNullBitmap;
////@end paraverMain bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon paraverMain::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin paraverMain icon retrieval
  wxUnusedVar(name);
  if (name == wxT("file_browser.xpm"))
  {
    wxIcon icon(file_browser_xpm);
    return icon;
  }
  else if (name == wxT("window_properties.xpm"))
  {
    wxIcon icon(window_properties_xpm);
    return icon;
  }
  return wxNullIcon;
////@end paraverMain icon retrieval
}

/*!
 * wxEVT_PG_CHANGED event handler for ID_FOREIGN
 */
void paraverMain::OnPropertyGridChange( wxPropertyGridEvent& event )
{
  wxPGProperty *property = event.GetProperty();

  if( !property )
    return;

  const wxString& propName = property->GetName();

  if( propName == _( "Mode" ) )
  {
    if( currentTimeline != NULL )
    {
      currentTimeline->setCFG4DMode( property->GetValue().GetLong() == 1 );
      currentTimeline->setChanged( true );
    }
    else if ( currentHisto != NULL )
    {
      currentHisto->setCFG4DMode( property->GetValue().GetLong() == 1 );
      currentHisto->setChanged( true );
    }
  }
  else if( propName == wxString( "Name", wxConvUTF8 ) )
  {
    wxString tmpName = property->GetValue().GetString();
    if( currentTimeline != NULL )
    {
      currentTimeline->setName( std::string( tmpName.mb_str() ) );
      currentTimeline->setChanged( true );
    }
    else if( currentHisto != NULL )
    {
      currentHisto->setName( std::string( tmpName.mb_str() ) );
      currentHisto->setChanged( true );
    }
  }
  else if( propName == _( "Begin time" ) )
  {
    TTime tmpValue;
    if( currentTimeline != NULL )
    {
      bool done = LabelConstructor::getTimeValue( std::string( property->GetValue().GetString().mb_str()),
                                                  currentTimeline->getTimeUnit(),
                                                  ParaverConfig::getInstance()->getTimelinePrecision(),
                                                  tmpValue );

      if (!done)
        tmpValue = property->GetValue().GetDouble();

      currentTimeline->setWindowBeginTime( currentTimeline->windowUnitsToTraceUnits( tmpValue ) );

      // modify current zoom directly
      pair< TTime, TTime > zoomInfo = currentTimeline->getZoomFirstDimension();
      zoomInfo.first = property->GetValue().GetDouble(); // begin
      currentTimeline->setZoomFirstDimension( zoomInfo );

      currentTimeline->setChanged( true );
      currentTimeline->setRedraw( true );
    }
    else if( currentHisto != NULL )
    {
      bool done = LabelConstructor::getTimeValue( std::string( property->GetValue().GetString().mb_str()),
                                                  currentHisto->getControlWindow()->getTimeUnit(),
                                                  ParaverConfig::getInstance()->getTimelinePrecision(),
                                                  tmpValue );

      if (!done)
        tmpValue = property->GetValue().GetDouble();

      currentHisto->setWindowBeginTime( currentHisto->getControlWindow()->windowUnitsToTraceUnits( tmpValue ) );
      currentHisto->setChanged( true );
      currentHisto->setRecalc( true );
    }
  }
  else if( propName == _( "End time" ) )
  {
    TTime tmpValue;
    if( currentTimeline != NULL )
    {
      bool done = LabelConstructor::getTimeValue( std::string( property->GetValue().GetString().mb_str()),
                                                  currentTimeline->getTimeUnit(),
                                                  ParaverConfig::getInstance()->getTimelinePrecision(),
                                                  tmpValue );

      if (!done)
        tmpValue = property->GetValue().GetDouble();

      currentTimeline->setWindowEndTime( currentTimeline->windowUnitsToTraceUnits( tmpValue ) );

      // modify current zoom directly
      pair< TTime, TTime > zoomInfo = currentTimeline->getZoomFirstDimension();
      zoomInfo.second = property->GetValue().GetDouble(); // end
      currentTimeline->setZoomFirstDimension( zoomInfo );

      currentTimeline->setChanged( true );
      currentTimeline->setRedraw( true );
    }
    else if( currentHisto != NULL )
    {
      bool done = LabelConstructor::getTimeValue( std::string( property->GetValue().GetString().mb_str()),
                                                  currentHisto->getControlWindow()->getTimeUnit(),
                                                  ParaverConfig::getInstance()->getTimelinePrecision(),
                                                  tmpValue );

      if (!done)
        tmpValue = property->GetValue().GetDouble();

      currentHisto->setWindowEndTime( currentHisto->getControlWindow()->windowUnitsToTraceUnits( tmpValue ) );
      currentHisto->setChanged( true );
      currentHisto->setRecalc( true );
    }
  }
  else if( propName == _( "Semantic Minimum" ) )
  {
    double tmpValue = property->GetValue().GetDouble();
    currentTimeline->setMinimumY( tmpValue );
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Semantic Maximum" ) )
  {
    double tmpValue = property->GetValue().GetDouble();
    currentTimeline->setMaximumY( tmpValue );
    currentTimeline->setRedraw( true );
  }
  // Control Window related properties
  else if( propName == _( "ControlWindow" ) )
  {
    currentHisto->setControlWindow( LoadedWindows::getInstance()->getWindow( property->GetValue().GetLong() ) );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "ControlMinimum" ) )
  {
    currentHisto->setControlMin( property->GetValue().GetDouble() );

    // modify current zoom directly
    pair< HistogramProxy::TZoomInfo, HistogramProxy::TZoomInfo > zoomInfo = currentHisto->getZoomFirstDimension();
    zoomInfo.first.begin = property->GetValue().GetDouble(); // minimum
    currentHisto->setZoomFirstDimension( zoomInfo );

    currentHisto->setCompute2DScale( false );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "ControlMaximum" ) )
  {
    currentHisto->setControlMax( property->GetValue().GetDouble() );

    // modify current zoom directly
    pair< HistogramProxy::TZoomInfo, HistogramProxy::TZoomInfo > zoomInfo = currentHisto->getZoomFirstDimension();
    zoomInfo.first.end = property->GetValue().GetDouble(); // maximum
    currentHisto->setZoomFirstDimension( zoomInfo );

    currentHisto->setCompute2DScale( false );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "ControlDelta" ) )
  {
    if( property->GetValue().GetDouble() == 0 )
    {
      property->SetValue( currentHisto->getControlDelta() );
      return;
    }
    currentHisto->setControlDelta( property->GetValue().GetDouble() );

    // modify current zoom directly
    pair< HistogramProxy::TZoomInfo, HistogramProxy::TZoomInfo > zoomInfo = currentHisto->getZoomFirstDimension();
    zoomInfo.second.begin = property->GetValue().GetDouble(); // delta
    currentHisto->setZoomFirstDimension( zoomInfo );

    currentHisto->setCompute2DScale( false );
    currentHisto->setRecalc( true );
  }
  // Data Window related properties
  else if( propName == _( "DataWindow" ) )
  {
    currentHisto->setDataWindow( LoadedWindows::getInstance()->getWindow( property->GetValue().GetLong() ) );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "DataMinimum" ) )
  {
    currentHisto->setMinGradient( property->GetValue().GetDouble() );
    currentHisto->setComputeGradient( false );
    currentHisto->setRedraw( true );
  }
  else if( propName == _( "DataMaximum" ) )
  {
    currentHisto->setMaxGradient( property->GetValue().GetDouble() );
    currentHisto->setComputeGradient( false );
    currentHisto->setRedraw( true );
  }
  // Histogram related properties
  else if( propName == wxString( "Type", wxConvUTF8 ) )
  {
    if( property->GetValue().GetLong() == 0 )
      currentHisto->setCurrentStat( currentHisto->getFirstCommStatistic() );
    else
      currentHisto->setCurrentStat( currentHisto->getFirstStatistic() );
    currentHisto->setRedraw( true );
    currentHisto->setChanged( true );
  }
  else if( propName == _( "Statistic" ) )
  {
    bool getOriginalList = ( !currentHisto->getCFG4DEnabled() || !currentHisto->getCFG4DMode() );
    if ( getOriginalList )
    {
      currentHisto->setCurrentStat( std::string( property->GetDisplayedString().mb_str() ) );
    }
    else
    {
      map< string, string > statList( currentHisto->getCFG4DStatisticsAliasList() );
      string selected( std::string( property->GetDisplayedString().mb_str() ) );
      for ( map< string, string >::iterator it = statList.begin(); it != statList.end(); ++it )
      {
        if ( it->second == selected )
        {
          selected = it->first;
        }
      }
      currentHisto->setCurrentStat( selected );
    }

    currentHisto->setRedraw( true );
  }
  else if( propName == _( "3D3rdWindow" ) )
  {
    if( property->GetValue().GetLong() == -1 )
      currentHisto->clearExtraControlWindow();
    else
      currentHisto->setExtraControlWindow( LoadedWindows::getInstance()->getWindow( property->GetValue().GetLong() ) );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "3DMinimum" ) )
  {
    currentHisto->setExtraControlMin( property->GetValue().GetDouble() );
    currentHisto->setCompute3DScale( false );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "3DMaximum" ) )
  {
    currentHisto->setExtraControlMax( property->GetValue().GetDouble() );
    currentHisto->setCompute3DScale( false );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "3DDelta" ) )
  {
    if( property->GetValue().GetDouble() == 0 )
    {
      property->SetValue( currentHisto->getExtraControlDelta() );
      return;
    }
    
    currentHisto->setExtraControlDelta( property->GetValue().GetDouble() );
    currentHisto->setCompute3DScale( false );
    currentHisto->setRecalc( true );
  }
  else if( propName == _( "3DPlane" ) )
  {
    currentHisto->setSelectedPlane( property->GetValue().GetLong() );
    currentHisto->setRedraw( true );
  }
  
  // Timeline related properties
  else if( propName == _( "Level" ) )
  {
    currentTimeline->setLevel( (TWindowLevel)property->GetValue().GetLong() );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Time Unit" ) )
  {
    currentTimeline->setTimeUnit( (TWindowLevel)property->GetValue().GetLong() );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Logical" ) )
  {
    currentTimeline->getFilter()->setLogical( property->GetValue().GetBool() );
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Physical" ) )
  {
    currentTimeline->getFilter()->setPhysical( property->GetValue().GetBool() );
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Comm from.FromFunction" ) )
  {
    currentTimeline->getFilter()->setCommFromFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Comm from.From" ) )
  {
    prvRowsSelectionProperty *myProperty = (prvRowsSelectionProperty *)event.GetProperty();
  
    Filter *filter = currentTimeline->getFilter();
    filter->clearCommFrom();
    vector< TObjectOrder > selection;
    myProperty->GetSelectionAsVector( currentTimeline->getLevel(), selection );
    for( vector< TObjectOrder >::iterator it = selection.begin(); it != selection.end(); ++it )
    {
      filter->insertCommFrom( long(*it) );
    }
    
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "FromToOp" ) )
  {
    long op = property->GetValue().GetLong();
    Filter *filter = currentTimeline->getFilter();
    if( op == 0 )
      filter->setOpFromToAnd();
    else if( op == 1 )
      filter->setOpFromToOr();
      
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Comm to.ToFunction" ) )
  {
    currentTimeline->getFilter()->setCommToFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Comm to.To" ) )
  {
    prvRowsSelectionProperty *myProperty = (prvRowsSelectionProperty *)event.GetProperty();
  
    Filter *filter = currentTimeline->getFilter();
    filter->clearCommTo();
    vector< TObjectOrder > selection;
    myProperty->GetSelectionAsVector( currentTimeline->getLevel(), selection );
    for( vector< TObjectOrder >::iterator it = selection.begin(); it != selection.end(); ++it )
    {
      filter->insertCommTo( long(*it) );
    }
    
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Comm tag.TagFunction" ) )
  {
    currentTimeline->getFilter()->setCommTagFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Comm tag.Tag" ) )
  {
    Filter *filter = currentTimeline->getFilter();
    filter->clearCommTags();
    wxArrayString value = property->GetValue().GetArrayString();
    for( unsigned int idx = 0; idx < value.GetCount(); idx++ )
    {
      long long tmpLong;
      value[ idx ].ToLongLong( &tmpLong );
      filter->insertCommTag( tmpLong );
    }
    
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "TagSizeOp" ) )
  {
    long op = property->GetValue().GetLong();
    Filter *filter = currentTimeline->getFilter();
    if( op == 0 )
      filter->setOpTagSizeAnd();
    else if( op == 1 )
      filter->setOpTagSizeOr();
      
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Comm size.SizeFunction" ) )
  {
    currentTimeline->getFilter()->setCommSizeFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Comm size.Size" ) )
  {
    Filter *filter = currentTimeline->getFilter();
    filter->clearCommSizes();
    wxArrayString value = property->GetValue().GetArrayString();
    for( unsigned int idx = 0; idx < value.GetCount(); idx++ )
    {
      long long tmpLong;
      value[ idx ].ToLongLong( &tmpLong );
      // cout << value[idx] << endl;
      filter->insertCommSize( tmpLong );
    }
    
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Comm bandwidth.BWFunction" ) )
  {
    currentTimeline->getFilter()->setBandWidthFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Comm bandwidth.Bandwidth" ) )
  {
    Filter *filter = currentTimeline->getFilter();
    filter->clearBandWidth();
    wxArrayString value = property->GetValue().GetArrayString();
    for( unsigned int idx = 0; idx < value.GetCount(); idx++ )
    {
      double tmpDouble;
      value[ idx ].ToDouble( &tmpDouble );
      filter->insertBandWidth( tmpDouble );
    }
    
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Event type.TypeFunction" ) )
  {
    currentTimeline->getFilter()->setEventTypeFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Event type.Types" ) )
  {
    Filter *filter = currentTimeline->getFilter();
    filter->clearEventTypes();
    wxArrayInt value = ( (prvEventTypeProperty *) property )->GetValueAsArrayInt();
    for( unsigned int idx = 0; idx < value.GetCount(); idx++ )
      filter->insertEventType( value[ idx ] );
    
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "TypeValueOp" ) )
  {
    long op = property->GetValue().GetLong();
    Filter *filter = currentTimeline->getFilter();
    if( op == 0 )
      filter->setOpTypeValueAnd();
    else if( op == 1 )
      filter->setOpTypeValueOr();
      
    currentTimeline->setRedraw( true );
  }
  else if( propName == _( "Event value.ValueFunction" ) )
  {
    currentTimeline->getFilter()->setEventValueFunction( std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Event value.Values" ) )
  {
    Filter *filter = currentTimeline->getFilter();
    filter->clearEventValues();

    wxArrayString value = property->GetValue().GetArrayString();
    for( unsigned int idx = 0; idx < value.GetCount(); idx++ )
    {
      unsigned long long tmpLong;
      value[ idx ].ToULongLong( &tmpLong );
      filter->insertEventValue( tmpLong );
    }

    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Top Compose 1" ) )
  {
    currentTimeline->setLevelFunction( TOPCOMPOSE1, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Top Compose 2" ) )
  {
    currentTimeline->setLevelFunction( TOPCOMPOSE2, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Factor #1" ) )
  {
    currentTimeline->setFactor( 0, property->GetValue().GetDouble() );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Derived" ) )
  {
    currentTimeline->setLevelFunction( DERIVED, std::string( property->GetDisplayedString().mb_str() ) );

    // Change Derived operation Icon: find icon in list
    int iconNumber = 1; // number of timeline icon
    string derivedFunctionName;
    if ( currentTimeline->isDerivedWindow() )
    {
      derivedFunctionName = currentTimeline->getLevelFunction( DERIVED );

      // GUI should'nt know these tags -> add operation to kernel
      if ( derivedFunctionName == "add" )
        iconNumber = 2;
      else if  ( derivedFunctionName == "product" )
        iconNumber = 3;
      else if  ( derivedFunctionName == "substract" )
        iconNumber = 4;
      else if  ( derivedFunctionName == "divide" )
        iconNumber = 5;
      else if  ( derivedFunctionName == "maximum" )
        iconNumber = 6;
      else if  ( derivedFunctionName == "minimum" )
        iconNumber = 7;
      else if  ( derivedFunctionName == "different" )
        iconNumber = 8;
      else if  ( derivedFunctionName == "controlled: clear by" )
        iconNumber = 9;
      else if  ( derivedFunctionName == "controlled: maximum" )
        iconNumber = 10;
      else if ( derivedFunctionName == "controlled: add" )
        iconNumber = 11;
    }

    // Change Derived operation Icon: set both trees, global and trace tree
    wxTreeCtrl *globalTreePage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( 0 );
    wxTreeCtrl *currentTraceTreePage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( currentTrace + 1 );
    bool found;
    wxTreeItemId idInGlobalTree = getItemIdFromWindow( globalTreePage->GetRootItem(), currentTimeline, found );
    globalTreePage->SetItemImage( idInGlobalTree, iconNumber );
    wxTreeItemId idInCurrentTraceTree = getItemIdFromWindow( currentTraceTreePage->GetRootItem(), currentTimeline, found );
    currentTraceTreePage->SetItemImage( idInCurrentTraceTree, iconNumber );

    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Factor #2" ) )
  {
    currentTimeline->setFactor( 1, property->GetValue().GetDouble() );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose Workload" ) )
  {
    currentTimeline->setLevelFunction( COMPOSEWORKLOAD, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Workload" ) )
  {
    currentTimeline->setLevelFunction( WORKLOAD, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose Appl" ) )
  {
    currentTimeline->setLevelFunction( COMPOSEAPPLICATION, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Application" ) )
  {
    currentTimeline->setLevelFunction( APPLICATION, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose Task" ) )
  {
    currentTimeline->setLevelFunction( COMPOSETASK, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Task" ) )
  {
    currentTimeline->setLevelFunction( TASK, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose System" ) )
  {
    currentTimeline->setLevelFunction( COMPOSESYSTEM, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "System" ) )
  {
    currentTimeline->setLevelFunction( SYSTEM, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose Node" ) )
  {
    currentTimeline->setLevelFunction( COMPOSENODE, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Node" ) )
  {
    currentTimeline->setLevelFunction( NODE, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose CPU" ) )
  {
    currentTimeline->setLevelFunction( COMPOSECPU, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "CPU" ) )
  {
    currentTimeline->setLevelFunction( CPU, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Compose Thread" ) )
  {
    currentTimeline->setLevelFunction( COMPOSETHREAD, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName == _( "Thread" ) )
  {
    currentTimeline->setLevelFunction( THREAD, std::string( property->GetDisplayedString().mb_str() ) );
    currentTimeline->setRedraw( true );
    currentTimeline->setChanged( true );
  }
  else if( propName.BeforeFirst( ' ' ) == _( "Param" ) )
  {
    wxString paramData = propName.AfterFirst( ' ' );
    TParamIndex paramIdx;
    TWindowLevel functionLevel;
    unsigned long tmpLong;
    
    paramData.BeforeFirst( ' ' ).ToULong( &tmpLong );
    paramIdx = tmpLong;
    paramData.AfterFirst( ' ' ).ToULong( &tmpLong );
    functionLevel = (TWindowLevel)tmpLong;
    
    wxArrayString valuesStr = property->GetValue().GetArrayString();
    TParamValue values;
    for( unsigned int idx = 0; idx < valuesStr.GetCount(); idx++ )
    {
      double tmpDouble;
      valuesStr[ idx ].ToDouble( &tmpDouble );
      values.push_back( tmpDouble );
    }
    currentTimeline->setFunctionParam( functionLevel, paramIdx, values );
    currentTimeline->setRedraw( true );
  }
}


/*!
 * wxEVT_TREE_SEL_CHANGED event handler for wxID_ANY
 */
void paraverMain::OnTreeSelChanged( wxTreeEvent& event )
{
  wxTreeCtrl *tmpTree = static_cast<wxTreeCtrl *>( event.GetEventObject() );
  if( tmpTree->GetParent()->GetId() == ID_DIRCTRLFILES )
  {
    event.Skip();
    return;
  }
  TreeBrowserItemData *itemData = static_cast<TreeBrowserItemData *>( tmpTree->GetItemData( event.GetItem() ) );

  endDragWindow = NULL;
  if( gHistogram *histo = itemData->getHistogram() ) // Is a histogram?
  {
    currentHisto = histo->GetHistogram();
    currentWindow = (wxWindow *)histo;

    currentTimeline = NULL;
    beginDragWindow = NULL;

    if( histo->IsShown() )
      histo->Raise();
  }
  else if( gTimeline *timeline = itemData->getTimeline() ) // Is a timeline.
  {
    currentTimeline = timeline->GetMyWindow();
    beginDragWindow = timeline->GetMyWindow();
    currentWindow = (wxWindow *)timeline;

    currentHisto = NULL;

    if( timeline->IsShown() )
      timeline->Raise();
  }
}

/*!
 * wxEVT_TREE_ITEM_ACTIVATED event handler for wxID_ANY
 */
void paraverMain::OnTreeItemActivated( wxTreeEvent& event )
{
  wxTreeCtrl *tmpTree = static_cast<wxTreeCtrl *>( event.GetEventObject() );
  if( tmpTree->GetParent()->GetId() == ID_DIRCTRLFILES )
  {
    wxFileName fileName( dirctrlFiles->GetPath() );
    if( !fileName.IsDir() )
    {
      string auxFile = std::string( fileName.GetFullPath().mb_str() );
      if( Trace::isTraceFile( auxFile ))
        DoLoadTrace( auxFile );
      else if( CFGLoader::isCFGFile( auxFile ) && loadedTraces.size() > 0 )
        DoLoadCFG( auxFile );
    }
    event.Skip();
    return;
  }
  TreeBrowserItemData *itemData = static_cast<TreeBrowserItemData *>( tmpTree->GetItemData( event.GetItem() ) );

  endDragWindow = NULL;
  if( gHistogram *histo = itemData->getHistogram() )
  {
    Histogram *tmpHisto = histo->GetHistogram();

    beginDragWindow = NULL;

    tmpHisto->setShowWindow( !tmpHisto->getShowWindow() );
    if( tmpHisto->getShowWindow() )
      histo->Raise();
  }
  else if( gTimeline *timeline = itemData->getTimeline() )
  {
    Window *tmpWin = timeline->GetMyWindow();

    beginDragWindow = timeline->GetMyWindow();

    tmpWin->setShowWindow( !tmpWin->getShowWindow() );
    if( tmpWin->getShowWindow() )
      timeline->Raise();
  }
}


void paraverMain::OnTreeRightClick( wxTreeEvent& event )
{
  wxTreeCtrl *tmpTree = static_cast<wxTreeCtrl *>( event.GetEventObject() );
  if( tmpTree->GetParent()->GetId() == ID_DIRCTRLFILES )
  {
    event.Skip();
    return;
  }
  TreeBrowserItemData *itemData = static_cast<TreeBrowserItemData *>( tmpTree->GetItemData( event.GetItem() ) );
  
  endDragWindow = NULL;

  if( gHistogram *histo = itemData->getHistogram() )
  {
    beginDragWindow = NULL;

    histo->rightDownManager();
  }
  else if( gTimeline *timeline = itemData->getTimeline() )
  {
    beginDragWindow = timeline->GetMyWindow();

    timeline->rightDownManager();
  }
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_CHOICEWINBROWSER
 */

void paraverMain::OnChoicewinbrowserUpdate( wxUpdateUIEvent& event )
{
  // Get a copy of all Windows and Histograms from LoadedWindows
  vector< Window * > allWindows;
  LoadedWindows::getInstance()->getAll( allWindows );
  vector< Histogram * > allHistograms;
  bool destroyed = false;

  // Update loop and delete
  for( unsigned int iPage = 0; iPage < choiceWindowBrowser->GetPageCount(); ++iPage )
  {
    if( iPage > 0 && choiceWindowBrowser->GetSelection() > 0 )
      currentWindow = NULL;
    wxTreeCtrl *currentTree = (wxTreeCtrl *) choiceWindowBrowser->GetPage( iPage );
    wxTreeItemId root = currentTree->GetRootItem();
    wxTreeItemIdValue cookie;
    wxTreeItemId currentChild = currentTree->GetFirstChild( root, cookie );
    unsigned int numberChild = currentTree->GetChildrenCount( root, false );
    unsigned int current = 0;
    while( current < numberChild )
    {
      if ( currentChild.IsOk() )
      {
        destroyed = updateTreeItem( currentTree, currentChild, allWindows, allHistograms, &currentWindow, iPage == 0 );
        if( iPage > 0 && choiceWindowBrowser->GetSelection() > 0 && currentWindow != NULL )
          choiceWindowBrowser->SetSelection( iPage );
      }
      else
        destroyed = false;

      if( !destroyed )
      {
        currentChild = currentTree->GetNextChild( root, cookie );
        ++current;
      }
      else
      {
        currentChild = currentTree->GetFirstChild( root, cookie );
        if( current == numberChild - 1 )
          ++current;
        else
          current = 0;
      }
    }
  }

   // add pending window or histogram
  for( vector<Window *>::iterator it = allWindows.begin(); it != allWindows.end(); ++it )
  {
    if( (*it)->getDestroy() )
      continue;

    wxTreeCtrl *allTracesPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( 0 );
    wxTreeCtrl *currentPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( currentTrace + 1 );

    if ( (*it)->getChild() == NULL )
      BuildTree( this, allTracesPage, allTracesPage->GetRootItem(), currentPage, currentPage->GetRootItem(), *it );
  }

  for( vector<Histogram *>::iterator it = allHistograms.begin(); it != allHistograms.end(); ++it )
  {
    if( (*it)->getDestroy() )
      continue;
    gHistogram* tmpHisto = new gHistogram( this, wxID_ANY, wxString::FromAscii( (*it)->getName().c_str() ) );
    tmpHisto->SetHistogram( *it );

    appendHistogram2Tree( tmpHisto );
  }

  // No window or histogram? Disable current selection.
  if ( loadedTraces.size() > 0 )
  {
    allWindows.clear();
    allHistograms.clear();
    if( currentTrace == -1 )
    {
      LoadedWindows::getInstance()->getAll( allWindows );
      LoadedWindows::getInstance()->getAll( allHistograms );
    }
    else
    {
      LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], allWindows );
      LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], allHistograms );
    }

    if ( allWindows.size() == 0 && allHistograms.size() == 0 )
    {
      currentTimeline = NULL;
      currentHisto = NULL;
    }
  }
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_FOREIGN
 */

void paraverMain::OnForeignUpdate( wxUpdateUIEvent& event )
{
  if( currentHisto != NULL )
  {
    lastTimeline = NULL;
    if( lastHisto == currentHisto && !currentHisto->getChanged() )
      return;
    lastHisto = currentHisto;

    updateHistogramProperties( windowProperties, currentHisto );
  }
  else if( currentTimeline != NULL )
  {
    lastHisto = NULL;
    if( lastTimeline == currentTimeline && !currentTimeline->getChanged() )
      return;
    lastTimeline = currentTimeline;

    updateTimelineProperties( windowProperties, currentTimeline );
  }
}


void paraverMain::OnPreviousTracesClick( wxCommandEvent& event )
{
  int eventId = event.GetId();
  int i = 0;

  wxMenuItem *item = menuFile->FindItem( ID_RECENTTRACES );
  wxMenu *menu = item->GetSubMenu();
  wxMenuItemList& menuItems = menu->GetMenuItems();
  for ( wxMenuItemList::iterator menuIt = menuItems.begin(); menuIt != menuItems.end(); ++menuIt )
  {
    wxMenuItem *tmp = *menuIt;
    int currentId = tmp->GetId();
    if ( currentId == eventId )
    {
      DoLoadTrace( previousTraces->getFiles()[i] );
    }
    i++;
  }
}


void paraverMain::OnPreviousCFGsClick( wxCommandEvent& event )
{
  int eventId = event.GetId();
  int i = 0;
  
  wxMenuItem *item = menuFile->FindItem( ID_RECENTCFGS );
  wxMenu *menu = item->GetSubMenu();
  wxMenuItemList& menuItems = menu->GetMenuItems();
  
  for ( wxMenuItemList::iterator menuIt = menuItems.begin(); menuIt != menuItems.end(); ++menuIt )
  {
    wxMenuItem *tmp = *menuIt;
    int currentId = tmp->GetId();
    if ( currentId == eventId )
      DoLoadCFG( previousCFGs->getFiles()[i] );
    i++;
  }
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_RECENTTRACES
 */

void paraverMain::OnRecenttracesUpdate( wxUpdateUIEvent& event )
{
  vector<string> v = previousTraces->getFiles();

  wxMenuItem *tmpItem = menuFile->FindItem(ID_RECENTTRACES );
  wxMenu *menuTraces = tmpItem->GetSubMenu();

  wxMenuItemList& menuItems = menuTraces->GetMenuItems();
  wxMenuItemList::iterator menuIt = menuItems.begin();

  for ( vector<string>::iterator it = v.begin(); it != v.end(); ++it )
  {
    if( menuIt == menuItems.end() )
    {
      wxMenuItem *newItem = new wxMenuItem( menuTraces, wxID_ANY, wxString::FromAscii( (*it).c_str() ) );
      menuTraces->Append( newItem );
      Connect( newItem->GetId(),
               wxEVT_COMMAND_MENU_SELECTED,
               (wxObjectEventFunction)&paraverMain::OnPreviousTracesClick );
    }
    else
    {
      wxMenuItem *tmp = *menuIt;
      tmp->SetItemLabel( wxString::FromAscii( (*it).c_str() ) );
      ++menuIt;
    }
  }
}
/*!
 * wxEVT_UPDATE_UI event handler for ID_MENULOADCFG
 */

void paraverMain::OnMenuloadcfgUpdate( wxUpdateUIEvent& event )
{
  //event.Enable( loadedTraces.size() > 0 ); // why not this one?
  event.Enable( currentTrace != -1 );

  vector<string> v = previousCFGs->getFiles();

  wxMenuItem *tmpItem = menuFile->FindItem( ID_RECENTCFGS );
  wxMenu *menuCFGs = tmpItem->GetSubMenu();

  wxMenuItemList& menuItems = menuCFGs->GetMenuItems();
  wxMenuItemList::iterator menuIt = menuItems.begin();

  for ( vector<string>::iterator it = v.begin(); it != v.end(); ++it )
  {
    if( menuIt == menuItems.end() )
    {
      wxMenuItem *newItem = new wxMenuItem( menuCFGs, wxID_ANY, wxString::FromAscii( (*it).c_str() ) );
      menuCFGs->Append( newItem );
      Connect( newItem->GetId(),
               wxEVT_COMMAND_MENU_SELECTED,
              (wxObjectEventFunction)&paraverMain::OnPreviousCFGsClick );
    }
    else
    {
      wxMenuItem *tmp = *menuIt;
      tmp->SetItemLabel( wxString::FromAscii( (*it).c_str() ) );
      ++menuIt;
    }
  }
}

void progressFunction( ProgressController *progress, void *callerWindow )
{
  int p;
  if ( progress->getCurrentProgress() > progress->getEndLimit() )
    p = numeric_limits<int>::max();
  else 
    p = (int)floor( ( progress->getCurrentProgress() * numeric_limits<int>::max() ) / progress->getEndLimit() );

  wxString newMessage;
  if( progress->getMessageChanged() )
  {
    newMessage = wxString::FromAscii( progress->getMessage().c_str() );
    progress->clearMessageChanged();
  }
  
  if( !paraverMain::dialogProgress->Update( p, newMessage ) )
    progress->setStop( true );
//  app->Yield();
}

/*!
 * wxEVT_IDLE event handler for ID_PARAVERMAIN
 */

void paraverMain::OnIdle( wxIdleEvent& event )
{
#ifdef __WXMAC__
  wxUpdateUIEvent tmpEvent;
  OnRecenttracesUpdate( tmpEvent );
  OnMenuloadcfgUpdate( tmpEvent );
#endif
  
  if( canServeSignal )
  {
    while( !loadFilesQueue.empty() )
    {
      string fileStr = loadFilesQueue.front();
      loadFilesQueue.pop();
      if( fileStr.substr( fileStr.length() - 3 ) == "cfg" )
      {
        if( loadedTraces.size() > 0 )
          DoLoadCFG( fileStr );
      }
      else
        DoLoadTrace( fileStr );
    }
  }
  
  if( wxTheApp->IsActive() )
  {
    int iTrace = 0;
    size_t prevSize = loadedTraces.size();
    vector<Trace *>::iterator it = loadedTraces.begin();
    while( it != loadedTraces.end() )
    {
      if( (*it)->getUnload() )
      {
        vector<Window *> windows;
        vector<Histogram *> histograms;
        
        LoadedWindows::getInstance()->getAll( *it, windows );
        LoadedWindows::getInstance()->getAll( *it, histograms );
        
        if( windows.begin() == windows.end() && histograms.begin() == histograms.end() )
        {
          if( currentTrace == iTrace )
            currentTrace = -1;
          else if ( currentTrace > -1 )
            --currentTrace;

          Trace *tmpTrace = *it;
          if( it == loadedTraces.begin() )
          {
            loadedTraces.erase( it );
            it = loadedTraces.begin();
          }
          else
          {
            vector<Trace *>::iterator tmpIt = it;
            --tmpIt;
            loadedTraces.erase( it );
            ++tmpIt;
            it = tmpIt;
          }
          delete tmpTrace;
          choiceWindowBrowser->DeletePage( iTrace + 1 );

          continue;
        }
        else
          ++iTrace;
      }
      else
        ++iTrace;

      ++it;
    }
    if( currentTrace == -1 && loadedTraces.size() > 0 )
      currentTrace = loadedTraces.size() - 1;

    // These are here because no UpdateUI for MenuBar
    if( loadedTraces.size() != prevSize )
    {
      wxUpdateUIEvent tmpEvent;
      OnMenuHintUpdate( tmpEvent );
    }
  }

#ifndef WIN32
  if ( sig1 || sig2 )
    OnSignal();
#endif


  if( ParaverConfig::getInstance()->getGlobalSessionSaveTime() == 0 )
    sessionTimer->Stop();
  else if( ParaverConfig::getInstance()->getGlobalSessionSaveTime() > 0 &&
           sessionTimer->GetInterval() != ParaverConfig::getInstance()->getGlobalSessionSaveTime() )
  {
    sessionTimer->Stop();
    sessionTimer->Start( ParaverConfig::getInstance()->getGlobalSessionSaveTime() * 1E3 * 60 );
  }

}


/*!
 * wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED event handler for ID_CHOICEWINBROWSER
 */

void paraverMain::OnChoicewinbrowserPageChanged( wxChoicebookEvent& event )
{
  int selPage = event.GetSelection();
  if( selPage == 0 )
    currentTrace = loadedTraces.size() - 1;
  else
    currentTrace = selPage - 1;
    
  wxTreeCtrl *tree = (wxTreeCtrl *) choiceWindowBrowser->GetCurrentPage();
  if( tree->GetSelection().IsOk() )
  {
    TreeBrowserItemData *item = (TreeBrowserItemData *) tree->GetItemData( tree->GetSelection() );
    if( item->getTimeline() != NULL )
    {
      currentWindow = item->getTimeline();
      currentTimeline = item->getTimeline()->GetMyWindow();
      currentHisto = NULL;
    }
    else if( item->getHistogram() != NULL )
    {
      currentWindow = item->getHistogram();
      currentHisto = item->getHistogram()->GetHistogram();
      currentTimeline = NULL;
    }
  }
  
  setActiveWorkspacesText();
  refreshMenuHints();
}


void paraverMain::SaveConfigurationFile( wxWindow *parent,
                                          SaveOptions options,
                                          vector< Window * > timelines,
                                          vector< Histogram * > histograms )
{
  if ( !CFGLoadedBefore )
    CFGPath =  wxString::FromAscii( paraverConfig->getGlobalCFGsPath().c_str() );
 
  vector< wxString > extensions;
  extensions.push_back( wxT( "cfg" ) );
  
  FileDialogExtension dialog( parent,
                              _( "Save Configuration" ),
                              CFGPath,
                              _( "" ),
                              _( "Paraver configuration file (*.cfg)|*.cfg" ),
                              wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR,
                              wxDefaultPosition,
                              wxDefaultSize,
                              _( "filedlg" ),
                              extensions );
  if( dialog.ShowModal() == wxID_OK )
  {
    CFGPath = dialog.GetPath();
    CFGLoadedBefore = true;
    CFGLoader::saveCFG( std::string( CFGPath.mb_str() ), options, timelines, histograms );
    previousCFGs->add( std::string( CFGPath.mb_str() ) );
  }
}   



/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUSAVECFG
 */

void paraverMain::OnMenusavecfgClick( wxCommandEvent& event )
{
  SaveOptions options;
  vector<Window *> timelines;
  vector<Histogram *> histograms;
  SaveConfigurationDialog saveDialog( this );
  
  saveDialog.SetOptions( options );
  LoadedWindows::getInstance()->getAll( timelines );
  LoadedWindows::getInstance()->getAll( histograms );
  saveDialog.SetTimelines( timelines );
  saveDialog.SetHistograms( histograms );

  // Find trace to put in the selector
  Trace *selectedTrace = NULL;
  vector<Window *> auxWindows;
  vector<Histogram *> auxHistograms;

  int currentPage = choiceWindowBrowser->GetSelection();
  LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], auxWindows );
  LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], auxHistograms );

  if (( currentPage != 0 ) && ( auxWindows.size() > 0 || auxHistograms.size() > 0 ))
  {
    selectedTrace = loadedTraces[ currentTrace ];
  }
  saveDialog.SetInitialTrace( selectedTrace );


  raiseCurrentWindow = false;
  if( saveDialog.ShowModal() == wxID_OK )
  {
    timelines = saveDialog.GetSelectedTimelines();
    histograms = saveDialog.GetSelectedHistograms();
    options = saveDialog.GetOptions();

    SaveConfigurationFile( (wxWindow *)this, options, timelines, histograms );

    // Disable CFG4D once it is saved
    for( vector< Window * >::iterator it = timelines.begin(); it != timelines.end(); ++it )
    {
      (*it)->setCFG4DEnabled( false );
    }
    for( vector< Histogram * >::iterator it = histograms.begin(); it != histograms.end(); ++it )
    {
      (*it)->setCFG4DEnabled( false );
    }
  }
  raiseCurrentWindow = true;
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_MENUSAVECFG
 */

void paraverMain::OnMenusavecfgUpdate( wxUpdateUIEvent& event )
{
  if( LoadedWindows::getInstance()->emptyWindows() && 
      LoadedWindows::getInstance()->emptyHistograms() )
    event.Enable( false );
  else
    event.Enable( true );
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL
 */
wxSize paraverMain::defaultWindowSize = wxSize( 600, 115 );
int paraverMain::initialPosX = 0;
int paraverMain::initialPosY = 0;


Window *paraverMain::createBaseWindow( wxString whichName )
{
  // Create new window
  Window *newWindow = Window::create( localKernel, loadedTraces[ currentTrace ] );
  ++numNewWindows;
  
  if ( whichName.IsEmpty() )
  {
    whichName = wxString( wxT( "New window #" ) );
    whichName << numNewWindows;
  }
  
  newWindow->setName( std::string( whichName.mb_str() ) );
  newWindow->setTimeUnit( loadedTraces[ currentTrace ]->getTimeUnit());
  newWindow->addZoom( 0, loadedTraces[ currentTrace ]->getEndTime(),
                      0, newWindow->getWindowLevelObjects() - 1 );

  // Position window in screen
  newWindow->setWidth( defaultWindowSize.GetWidth() ); // magic numbers!
  newWindow->setHeight( defaultWindowSize.GetHeight() );

  /*
  if ( initialPosX != 0 )
    initialPosX += defaultTitleBarSize.GetHeight();
  else
    initialPosX += GetSize().GetWidth();
  */

#ifdef __WXMAC__
  if( initialPosY < GetPosition().y )
    initialPosY = GetPosition().y;
#endif

  //newWindow->setPosX( initialPosX );
  newWindow->setPosX( GetNextPosX() );
  newWindow->setPosY( initialPosY );
  initialPosY += defaultTitleBarSize.GetHeight();

  // Its default semantic
  for ( PRV_UINT16 windowLevel = TOPCOMPOSE1; windowLevel <= COMPOSECPU; windowLevel++ )
    newWindow->setLevelFunction( (TWindowLevel)windowLevel, "As Is" );

  string semanticFunction = loadedTraces[ currentTrace ]->getDefaultSemanticFunc( THREAD );
  if ( semanticFunction != "" )
    newWindow->setLevelFunction( THREAD, semanticFunction );
  else
    newWindow->setLevelFunction( THREAD, "State As Is" );
    
  return newWindow;
}


void paraverMain::insertInTree( Window *whichWindow )
{
  // Build gtimeline and append new window to windows tree
  wxTreeCtrl *allTracesPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( 0 );
  wxTreeCtrl *currentPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( currentTrace + 1 );

  BuildTree( this,
             allTracesPage, allTracesPage->GetRootItem(), 
             currentPage,   currentPage->GetRootItem(),
             whichWindow );
}


void paraverMain::OnToolNewWindowClick( wxCommandEvent& event )
{
  insertInTree( createBaseWindow() );
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL
 */

void paraverMain::OnToolNewWindowUpdate( wxUpdateUIEvent& event )
{
  if ( loadedTraces.size() > 0 )
    tbarMain->EnableTool( ID_NEW_WINDOW, true );
  else
    tbarMain->EnableTool( ID_NEW_WINDOW, false );

  if ( currentTimeline != NULL )
    tbarMain->EnableTool( ID_NEW_WINDOW, true );
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_NEW_DERIVED_WINDOW
 */

void paraverMain::ShowDerivedDialog()
{
  DerivedTimelineDialog derivedDialog( this );
  vector<Window *> timelines;
  LoadedWindows::getInstance()->getDerivedCompatible( loadedTraces[ currentTrace ], timelines );

  ++numNewDerived;
  wxString tmpName( _( "New Derived Window #" ) );
  tmpName << numNewDerived;
  derivedDialog.SetTimelineName( std::string( tmpName.mb_str() ) );

  // Set timelines list
  derivedDialog.SetTimelines1( timelines );
  derivedDialog.SetTimelines2( timelines );

  // Set current window
  if ( beginDragWindow == NULL ||
      beginDragWindow->getTrace() == loadedTraces[ currentTrace ] )
    derivedDialog.SetCurrentWindow1( beginDragWindow );
  else 
    derivedDialog.SetCurrentWindow1( NULL );

  if ( endDragWindow == NULL ||
      endDragWindow->getTrace() == loadedTraces[ currentTrace ] )
    derivedDialog.SetCurrentWindow2( endDragWindow );
  else 
    derivedDialog.SetCurrentWindow2( NULL );

  raiseCurrentWindow = false;
  if( derivedDialog.ShowModal() == wxID_OK )
  {
    vector< Window * > selectedTimeline = derivedDialog.GetTimelines1();
    beginDragWindow = selectedTimeline[0]->clone();
    beginDragWindow->setPosX( GetNextPosX() );
    beginDragWindow->setPosY( GetNextPosY() );

    selectedTimeline.clear();
    selectedTimeline = derivedDialog.GetTimelines2();

//  if ( endDragWindow == beginDragWindow )
    endDragWindow = selectedTimeline[0]->clone();

    endDragWindow->setPosX( GetNextPosX() );
    endDragWindow->setPosY( GetNextPosY() );

    // Create new derived window
    Window *newWindow = Window::create( localKernel, beginDragWindow, endDragWindow );
    beginDragWindow->setChild( newWindow );
    endDragWindow->setChild( newWindow );
    newWindow->setPosX( GetNextPosX() );
    newWindow->setPosY( GetNextPosY() );

    newWindow->setName( derivedDialog.GetTimelineName() );
    newWindow->setTimeUnit( beginDragWindow->getTimeUnit() );
    newWindow->setWindowBeginTime( beginDragWindow->getWindowBeginTime() );
    newWindow->setWindowEndTime( beginDragWindow->getWindowEndTime() );
    newWindow->addZoom( beginDragWindow->getWindowBeginTime(),
                        beginDragWindow->getWindowEndTime(),
                        0, newWindow->getWindowLevelObjects() - 1 );
    newWindow->setShowChildrenWindow( false );

    // Size
    newWindow->setWidth( beginDragWindow->getWidth() ); // magic numbers!
    newWindow->setHeight( beginDragWindow->getHeight() );

    newWindow->setMaximumY( beginDragWindow->getMaximumY() );
    newWindow->setMinimumY( beginDragWindow->getMinimumY() );
    newWindow->setDrawCommLines( beginDragWindow->getDrawCommLines() );
    newWindow->setDrawFlags( beginDragWindow->getDrawFlags() );
    if( beginDragWindow->isCodeColorSet() )
      newWindow->setCodeColorMode();
    else if( beginDragWindow->isGradientColorSet() )
      newWindow->setGradientColorMode();
    else if( beginDragWindow->isNotNullGradientColorSet() )
      newWindow->setNotNullGradientColorMode();
    else if( beginDragWindow->isFunctionLineColorSet() )
      newWindow->setFunctionLineColorMode();
    
    newWindow->setDrawModeObject( beginDragWindow->getDrawModeObject() );
    newWindow->setDrawModeTime( beginDragWindow->getDrawModeTime() );
    newWindow->getGradientColor().setGradientFunction(
      beginDragWindow->getGradientColor().getGradientFunction() );
    newWindow->setLevel( beginDragWindow->getLevel() );
    vector<bool> tmpSel;
    for( int level = APPLICATION; level <= THREAD; ++level )
    {
      tmpSel.clear();
      beginDragWindow->getSelectedRows( (TWindowLevel)level, tmpSel );
      newWindow->setSelectedRows( (TWindowLevel)level, tmpSel );
    }
    if( beginDragWindow->getTrace()->existResourceInfo() )
    {
      for( int level = NODE; level <= CPU; ++level )
      {
        tmpSel.clear();
        beginDragWindow->getSelectedRows( (TWindowLevel)level, tmpSel );
        newWindow->setSelectedRows( (TWindowLevel)level, tmpSel );
      }
    }
    
    // Semantic
    vector< string > auxCompose = derivedDialog.GetTopCompose1();
    newWindow->setLevelFunction( TOPCOMPOSE1, auxCompose[0] );
    auxCompose = derivedDialog.GetTopCompose2();
    newWindow->setLevelFunction( TOPCOMPOSE2, auxCompose[0] );

    newWindow->setFactor( 0, derivedDialog.GetFactorTimeline1() );
    newWindow->setFactor( 1, derivedDialog.GetFactorTimeline2() );

    TParamValue auxParam = derivedDialog.GetMinCompose1();
    if ( auxParam.size() > 0 )
      newWindow->setFunctionParam( TOPCOMPOSE1, 0, auxParam );
    auxParam.clear();

    auxParam =  derivedDialog.GetMaxCompose1();
    if ( auxParam.size() > 0 )
      newWindow->setFunctionParam( TOPCOMPOSE1, 1, auxParam );
    auxParam.clear();

    auxParam =  derivedDialog.GetMinCompose2();
    if ( auxParam.size() > 0 )
      newWindow->setFunctionParam( TOPCOMPOSE2, 0, auxParam );
    auxParam.clear();

    auxParam =  derivedDialog.GetMaxCompose2();
    if ( auxParam.size() > 0 )
      newWindow->setFunctionParam( TOPCOMPOSE2, 1, auxParam );
    auxParam.clear();

    vector< string > semanticDerivedFunction = derivedDialog.GetOperations();
    newWindow->setLevelFunction( DERIVED, semanticDerivedFunction[0] );

    // Build gtimeline and append new window to windows tree
    wxTreeCtrl *allTracesPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( 0 );
    wxTreeCtrl *currentPage = (wxTreeCtrl *) choiceWindowBrowser->GetPage( currentTrace + 1 );

    BuildTree( this,
               allTracesPage, allTracesPage->GetRootItem(), 
               currentPage,   currentPage->GetRootItem(),
               newWindow );

    bool found;
    gTimeline *last = getGTimelineFromWindow( currentPage->GetRootItem(), newWindow, found );
    if ( found )
      currentWindow = last;
  }
  raiseCurrentWindow = true;
}


void paraverMain::ShowHistogramDialog()
{
//  HistogramDialog histogramDialog( this );
  HistogramDialog histogramDialog( NULL );

  vector<TWindowID> timelines;
  LoadedWindows::getInstance()->getAll( timelines );

  histogramDialog.SetControlTimelines( timelines );

  vector< pair< TRecordTime, TRecordTime > > ranges;
  // Window Times
  ranges.push_back( make_pair( currentTimeline->getWindowBeginTime(), currentTimeline->getWindowEndTime() ) );
  // Trace Times
  ranges.push_back( make_pair( 0.0, currentTimeline->getTrace()->getEndTime() ) );

  histogramDialog.SetTimeRange( ranges );

  histogramDialog.TransferDataToWindow( currentTimeline );

  raiseCurrentWindow = false;
  if( histogramDialog.ShowModal() == wxID_OK )
  {
    vector< TWindowID > controlTimeline = histogramDialog.GetControlTimelines();
    vector< TWindowID > dataTimeline = histogramDialog.GetDataTimelines();
    vector< TWindowID > extraControlTimeline = histogramDialog.GetExtraControlTimelines();
    ranges.clear();
    ranges = histogramDialog.GetTimeRange();

    Histogram *newHistogram = Histogram::create( localKernel );

    ++numNewHistograms;
    wxString tmpName( _( "New Histogram #" ) );
    tmpName << numNewHistograms;
    newHistogram->setName( std::string( tmpName.mb_str() ) );
/*
   newHistogram->setPosX();
    newHistogram->setPosY();
    newHistogram->setWidth();
    newHistogram->setHeigth();
*/
    newHistogram->setShowWindow( true );
    newHistogram->setCalculateAll( true );
    newHistogram->setCurrentStat( "Time" );

    newHistogram->setWindowBeginTime( ranges[0].first );
    newHistogram->setWindowEndTime( ranges[0].second );

    newHistogram->setControlWindow( LoadedWindows::getInstance()->getWindow( controlTimeline[0] ) );

    if ( histogramDialog.GetControlTimelineAutofit() )
      newHistogram->setCompute2DScale( true );
    else
    {
      newHistogram->setCompute2DScale( false );

      newHistogram->setControlMin( histogramDialog.GetControlTimelineMin() );
      newHistogram->setControlMax( histogramDialog.GetControlTimelineMax() );
      newHistogram->setControlDelta( histogramDialog.GetControlTimelineDelta() );
    }

    newHistogram->setDataWindow( LoadedWindows::getInstance()->getWindow( dataTimeline[0] ) );

    if ( !extraControlTimeline.empty() )
    {
      newHistogram->setExtraControlWindow( LoadedWindows::getInstance()->getWindow( extraControlTimeline[0] ) );

      if ( histogramDialog.GetExtraControlTimelineAutofit() )
        newHistogram->setCompute3DScale( true );
      else
      {
        newHistogram->setCompute3DScale( false );

        newHistogram->setExtraControlMin( histogramDialog.GetExtraControlTimelineMin() );
        newHistogram->setExtraControlMax( histogramDialog.GetExtraControlTimelineMax() );
        newHistogram->setExtraControlDelta( histogramDialog.GetExtraControlTimelineDelta() );
      }
    }

    string composedName = newHistogram->getName() + " @ " +
                          newHistogram->getControlWindow()->getTrace()->getTraceName();
    gHistogram* tmpHisto = new gHistogram( this, wxID_ANY, wxString::FromAscii( composedName.c_str() ) );
    tmpHisto->SetHistogram( newHistogram );

    appendHistogram2Tree( tmpHisto );
    LoadedWindows::getInstance()->add( newHistogram );

    tmpHisto->SetClientSize( wxRect( newHistogram->getPosX(), newHistogram->getPosY(),
                                     newHistogram->getWidth(), newHistogram->getHeight() ) );
    if( newHistogram->getShowWindow() )
    {
      tmpHisto->Show();
    }
    tmpHisto->execute();
    
    currentWindow = (wxWindow *)tmpHisto;
  }
  raiseCurrentWindow = true;
}


void paraverMain::OnNewDerivedWindowClick( wxCommandEvent& event )
{
  ShowDerivedDialog();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_NEW_DERIVED_WINDOW
 */

void paraverMain::OnNewDerivedWindowUpdate( wxUpdateUIEvent& event )
{

  if ( loadedTraces.size() > 0 && currentTrace > -1 )
  {
    vector<Window *> timelines;
    LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], timelines );

    event.Enable( ( timelines.size() > 0 ) && ( currentTimeline != NULL ) );
  }
  else
    event.Enable( false );

  if ( currentHisto != NULL )
    event.Enable( false );
}


void paraverMain::OnTreeBeginDrag( wxTreeEvent& event )
{
  wxTreeCtrl *tmpTree = static_cast<wxTreeCtrl *>( event.GetEventObject() );
  TreeBrowserItemData *itemData = static_cast<TreeBrowserItemData *>( tmpTree->GetItemData( event.GetItem() ) );

  beginDragWindow = NULL;

  if( gTimeline *timeline = itemData->getTimeline() )
  {
    beginDragWindow = timeline->GetMyWindow();
    event.Allow();
  }
}


int paraverMain::GetDefaultTitleBarHeight()
{
  return defaultTitleBarSize.GetHeight();
}


int paraverMain::GetNextPosX()
{
  if ( initialPosX != 0 )
    initialPosX += GetDefaultTitleBarHeight();
  else
    initialPosX += GetSize().GetWidth();
    
  return initialPosX;
}


int paraverMain::GetNextPosY()
{
  initialPosY += GetDefaultTitleBarHeight();
  
  return initialPosY;
}


void paraverMain::OnTreeEndDrag( wxTreeEvent& event )
{
  if( static_cast<wxTreeCtrl *>( event.GetEventObject() ) == dirctrlFiles->GetTreeCtrl() )
    return;

  if ( event.GetItem().IsOk() )
  {
    wxTreeCtrl *tmpTree = static_cast<wxTreeCtrl *>( event.GetEventObject() );
    TreeBrowserItemData *itemData = static_cast<TreeBrowserItemData *>( tmpTree->GetItemData( event.GetItem() ) );

    endDragWindow = NULL;

    if ( gTimeline *timeline = itemData->getTimeline())
    {
      endDragWindow = timeline->GetMyWindow();
      ShowDerivedDialog();
    }
  }
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_NEW_HISTOGRAM
 */

void paraverMain::OnNewHistogramClick( wxCommandEvent& event )
{
  ShowHistogramDialog();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_NEW_HISTOGRAM
 */

void paraverMain::OnNewHistogramUpdate( wxUpdateUIEvent& event )
{
  if ( loadedTraces.size() > 0 )
  {
    vector<Window *> timelines;
    if( currentTrace == -1 )
      LoadedWindows::getInstance()->getAll( timelines );
    else
      LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], timelines );
    tbarMain->EnableTool( ID_NEW_HISTOGRAM, ( timelines.size() > 0 ) && ( currentTimeline != NULL ) );
  }
  else
    tbarMain->EnableTool( ID_NEW_HISTOGRAM, false );
}


void paraverMain::ShowPreferences( wxWindowID whichPanelID )
{
  PreferencesDialog preferences( this );

  // GLOBAL
  preferences.SetGlobalFillStateGaps( paraverConfig->getGlobalFillStateGaps() );
  preferences.SetGlobalFullTracePath( paraverConfig->getGlobalFullTracePath() );
  preferences.SetTracesPath( paraverConfig->getGlobalTracesPath() );
  preferences.SetCfgsPath( paraverConfig->getGlobalCFGsPath() );
  preferences.SetTutorialsPath( paraverConfig->getGlobalTutorialsPath() );
  preferences.SetTmpPath( paraverConfig->getGlobalTmpPath() );
  preferences.SetMaximumTraceSize( paraverConfig->getFiltersFilterTraceUpToMB() );
  preferences.SetSingleInstance( paraverConfig->getGlobalSingleInstance() );
  preferences.SetSessionSaveTime( paraverConfig->getGlobalSessionSaveTime() );

  // TIMELINE

  preferences.SetTimelineNameFormatPrefix( paraverConfig->getTimelineDefaultName() );
  preferences.SetTimelineNameFormatFull( paraverConfig->getTimelineNameFormat() );
  //preferences.GetTimelineDefaultCFG( paraverConfig->setTimelineDefaultCFG() );
  preferences.SetTimelineEventLines( paraverConfig->getTimelineViewEventsLines() );
  preferences.SetTimelineCommunicationLines( paraverConfig->getTimelineViewCommunicationsLines() );
  preferences.SetTimelineColor( ( PRV_UINT32 )paraverConfig->getTimelineColor() );
  preferences.SetTimelineGradientFunction( ( PRV_UINT32 )paraverConfig->getTimelineGradientFunction() );
  preferences.SetTimelineDrawmodeTime( ( PRV_UINT32 )paraverConfig->getTimelineDrawmodeTime() );
  preferences.SetTimelineDrawmodeObjects( ( PRV_UINT32 )paraverConfig->getTimelineDrawmodeObjects() );
  preferences.SetTimelinePixelSize( ( PRV_UINT32 )paraverConfig->getTimelinePixelSize() );
  preferences.SetTimelineObjectLabels( ( PRV_UINT32 )paraverConfig->getTimelineLabels() );
  preferences.SetTimelineObjectAxis( ( PRV_UINT32 )paraverConfig->getTimelineObjectAxisSize() );
  preferences.SetTimelineWWPrecision( paraverConfig->getTimelinePrecision() );
  preferences.SetTimelineWWSemantic( paraverConfig->getTimelineWhatWhereSemantic() );
  preferences.SetTimelineWWEvents( paraverConfig->getTimelineWhatWhereEvents() );
  preferences.SetTimelineWWCommunications( paraverConfig->getTimelineWhatWhereCommunications() );
  preferences.SetTimelineWWPreviousNext( paraverConfig->getTimelineWhatWherePreviousNext() );
  preferences.SetTimelineWWText( paraverConfig->getTimelineWhatWhereText() );
  preferences.SetTimelineWWEventPixels( paraverConfig->getTimelineWhatWhereEventPixels() );
  preferences.SetTimelineSaveImageFormat( ( PRV_UINT32 )paraverConfig->getTimelineSaveImageFormat() );
  preferences.SetTimelineSaveTextFormat( ( PRV_UINT32 )paraverConfig->getTimelineSaveTextFormat() );

  // HISTOGRAM
  //preferences.SetHistogramNameFormatPrefix( paraverConfig->getHistogramDefaultName() );
  preferences.SetHistogramNameFormatPrefix( std::string( "New Histogram # %N" ) );
  //preferences.SetHistogramNameFormatFull( paraverConfig->getHistogramNameFormat() );
  preferences.SetHistogramNameFormatFull( std::string( "%P @ %T" ) );
  preferences.SetHistogramZoom( paraverConfig->getHistogramViewZoom() );
  preferences.SetHistogramHorizontal( paraverConfig->getHistogramViewHorizontal() );
  preferences.SetHistogramHideEmpty( !paraverConfig->getHistogramViewEmptyColumns() );
  preferences.SetHistogramShowGradient( paraverConfig->getHistogramViewGradientColors() );
  preferences.SetHistogramLabelsColor( paraverConfig->getHistogramViewFirstRowColored() );
  preferences.SetHistogramGradientFunction( paraverConfig->getHistogramGradientFunction() );
  preferences.SetHistogramDrawmodeSemantic( ( PRV_UINT32 ) paraverConfig->getHistogramDrawmodeSemantic() );
  preferences.SetHistogramDrawmodeObjects( ( PRV_UINT32 ) paraverConfig->getHistogramDrawmodeObjects() );
  preferences.SetHistogramScientificNotation( paraverConfig->getHistogramScientificNotation() );
  preferences.SetHistogramThousandSeparator( paraverConfig->getHistogramThousandSep() );
  preferences.SetHistogramShowUnits( paraverConfig->getHistogramShowUnits() );
  preferences.SetHistogramPrecision( paraverConfig->getHistogramPrecision() );
  preferences.SetHistogramAutofitControlScale( paraverConfig->getHistogramAutofitControlScale() );
  preferences.SetHistogramAutofit3DScale( paraverConfig->getHistogramAutofitThirdDimensionScale() );
  preferences.SetHistogramAutofitDataGradient( paraverConfig->getHistogramAutofitDataGradient() );
  preferences.SetHistogramNumColumns( paraverConfig->getHistogramNumColumns() );
//  preferences.SetHistogramMaxNumColumns( 400 ); // TO IMPLEMENT
//  preferences.SetHistogramMaxPrecision( 20 ); // TO IMPLEMENT
  preferences.SetHistogramSaveImageFormat( ( PRV_UINT32 )paraverConfig->getHistogramSaveImageFormat() );
  preferences.SetHistogramSaveTextFormat( ( PRV_UINT32 )paraverConfig->getHistogramSaveTextFormat() );

  // COLORS
  preferences.SetTimelineColourBackground( paraverConfig->getColorsTimelineBackground() );
  preferences.SetTimelineColourAxis( paraverConfig->getColorsTimelineAxis() );
  preferences.SetColorUseZero( paraverConfig->getColorsTimelineUseZero() );
  preferences.SetTimelineColourZero( paraverConfig->getColorsTimelineColorZero() );
  preferences.SetTimelineColourLogical( paraverConfig->getColorsTimelineLogicalCommunications() );
  preferences.SetTimelineColourPhysical( paraverConfig->getColorsTimelinePhysicalCommunications() );

  preferences.SetGradientColourBegin( paraverConfig->getColorsBeginGradient() );
  preferences.SetGradientColourEnd( paraverConfig->getColorsEndGradient() );
  preferences.SetGradientColourLow( paraverConfig->getColorsLowGradient() );
  preferences.SetGradientColourTop( paraverConfig->getColorsTopGradient() );

  // FILTER
  preferences.SetFiltersXMLPath( paraverConfig->getFiltersXMLPath() );

  preferences.SetPanel( whichPanelID );

  // ShowModal calls it by default
  //preferences.TransferDataToWindow();
  
  raiseCurrentWindow = true;
  if ( preferences.ShowModal() == wxID_OK )
  {
    //It is called by default
    //preferences.TransferDataFromWindow();

    // Apply Preferences

    // GLOBAL
    paraverConfig->setGlobalFillStateGaps( preferences.GetGlobalFillStateGaps() );
    paraverConfig->setGlobalFullTracePath( preferences.GetGlobalFullTracePath() );
    paraverConfig->setGlobalTracesPath( preferences.GetTracesPath() );
    paraverConfig->setGlobalCFGsPath( preferences.GetCfgsPath() );
    paraverConfig->setGlobalTutorialsPath( preferences.GetTutorialsPath() );
    paraverConfig->setGlobalTmpPath( preferences.GetTmpPath() );
    paraverConfig->setFiltersFilterTraceUpToMB( (float)preferences.GetMaximumTraceSize() );
    paraverConfig->setGlobalSingleInstance( preferences.GetSingleInstance() );
    paraverConfig->setGlobalSessionSaveTime( preferences.GetSessionSaveTime() );

    // TIMELINE
    paraverConfig->setTimelineDefaultName( preferences.GetTimelineNameFormatPrefix() );
    paraverConfig->setTimelineNameFormat( preferences.GetTimelineNameFormatFull() );
    //paraverConfig->setTimelineDefaultCFG( preferences.GetTimelineDefaultCFG() );
    paraverConfig->setTimelinePrecision( preferences.GetTimelineWWPrecision() );
    paraverConfig->setTimelineViewEventsLines( preferences.GetTimelineEventLines() );
    paraverConfig->setTimelineViewCommunicationsLines( preferences.GetTimelineCommunicationLines() );
    paraverConfig->setTimelineColor( (SemanticColor::TColorFunction)preferences.GetTimelineColor() );
    paraverConfig->setTimelineGradientFunction( (GradientColor::TGradientFunction)preferences.GetTimelineGradientFunction() );
    paraverConfig->setTimelineDrawmodeTime( (DrawModeMethod)preferences.GetTimelineDrawmodeTime() );
    paraverConfig->setTimelineDrawmodeObjects( (DrawModeMethod)preferences.GetTimelineDrawmodeObjects() );
    paraverConfig->setTimelinePixelSize( preferences.GetTimelinePixelSize() );
    paraverConfig->setTimelineLabels( (Window::TObjectLabels)preferences.GetTimelineObjectLabels() );
    paraverConfig->setTimelineObjectAxisSize( (Window::TObjectAxisSize)preferences.GetTimelineObjectAxis() );
    paraverConfig->setTimelineWhatWhereSemantic( preferences.GetTimelineWWSemantic() );
    paraverConfig->setTimelineWhatWhereEvents( preferences.GetTimelineWWEvents() );
    paraverConfig->setTimelineWhatWhereCommunications( preferences.GetTimelineWWCommunications() );
    paraverConfig->setTimelineWhatWherePreviousNext( preferences.GetTimelineWWPreviousNext() );
    paraverConfig->setTimelineWhatWhereText( preferences.GetTimelineWWText() );
    paraverConfig->setTimelineWhatWhereEventPixels( preferences.GetTimelineWWEventPixels() );
    paraverConfig->setTimelineSaveImageFormat( (ParaverConfig::TImageFormat)preferences.GetTimelineSaveImageFormat() );
    paraverConfig->setTimelineSaveTextFormat( (ParaverConfig::TTextFormat)preferences.GetTimelineSaveTextFormat() );

    // HISTOGRAM
    //paraverConfig->setHistogramDefaultName( preferences.GetHistogramNameFormatPrefix() );
    //paraverConfig->setHistogramNameFormat( preferences.GetHistogramNameFormatFull() );
    paraverConfig->setHistogramViewZoom( preferences.GetHistogramZoom() );
    paraverConfig->setHistogramViewHorizontal( preferences.GetHistogramHorizontal() );
    paraverConfig->setHistogramViewEmptyColumns( !preferences.GetHistogramHideEmpty() );
    paraverConfig->setHistogramViewGradientColors( preferences.GetHistogramShowGradient() );
    paraverConfig->setHistogramViewFirstRowColored( preferences.GetHistogramLabelsColor() );
    paraverConfig->setHistogramGradientFunction( (GradientColor::TGradientFunction)preferences.GetHistogramGradientFunction() );
    paraverConfig->setHistogramDrawmodeSemantic( ( DrawModeMethod ) preferences.GetHistogramDrawmodeSemantic() );
    paraverConfig->setHistogramDrawmodeObjects( ( DrawModeMethod ) preferences.GetHistogramDrawmodeObjects() );
    paraverConfig->setHistogramScientificNotation( preferences.GetHistogramScientificNotation() );
    paraverConfig->setHistogramThousandSep( preferences.GetHistogramThousandSeparator() );
    paraverConfig->setHistogramShowUnits( preferences.GetHistogramShowUnits() );
    paraverConfig->setHistogramPrecision( preferences.GetHistogramPrecision() );
    paraverConfig->setHistogramAutofitControlScale( preferences.GetHistogramAutofitControlScale() );
    paraverConfig->setHistogramAutofitThirdDimensionScale( preferences.GetHistogramAutofit3DScale() );
    paraverConfig->setHistogramAutofitDataGradient( preferences.GetHistogramAutofitDataGradient() );
    paraverConfig->setHistogramNumColumns( preferences.GetHistogramNumColumns() );
    paraverConfig->setHistogramSaveImageFormat( ( ParaverConfig::TImageFormat ) preferences.GetHistogramSaveImageFormat() );
    paraverConfig->setHistogramSaveTextFormat( ( ParaverConfig::TTextFormat ) preferences.GetHistogramSaveTextFormat() );

    // COLORS 
    paraverConfig->setColorsTimelineBackground( preferences.GetTimelineColourBackground() );
    paraverConfig->setColorsTimelineAxis( preferences.GetTimelineColourAxis() );
    paraverConfig->setColorsTimelineUseZero( preferences.GetColorUseZero() );
    paraverConfig->setColorsTimelineColorZero( preferences.GetTimelineColourZero() );
    paraverConfig->setColorsTimelineLogicalCommunications( preferences.GetTimelineColourLogical() );
    paraverConfig->setColorsTimelinePhysicalCommunications( preferences.GetTimelineColourPhysical() );

    paraverConfig->setColorsBeginGradient( preferences.GetGradientColourBegin() );
    paraverConfig->setColorsEndGradient( preferences.GetGradientColourEnd() );
    paraverConfig->setColorsLowGradient( preferences.GetGradientColourLow() );
    paraverConfig->setColorsTopGradient( preferences.GetGradientColourTop() );

    // FILTER
    paraverConfig->setFiltersXMLPath( preferences.GetFiltersXMLPath() );

    // Save Preferences to File
    paraverConfig->writeParaverConfigFile();

    // WORKSPACES
    workspacesManager->saveXML();

    for ( vector< Trace * >::iterator it = loadedTraces.begin(); it != loadedTraces.end(); ++it )
    {
      setTraceWorkspaces( *it );
    }
    
    setActiveWorkspacesText();
    refreshMenuHints();
  }
  raiseCurrentWindow = false;
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_PREFERENCES
 */

void paraverMain::OnPreferencesClick( wxCommandEvent& event )
{
  ShowPreferences();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_PREFERENCES
 */

void paraverMain::OnPreferencesUpdate( wxUpdateUIEvent& event )
{
////@begin wxEVT_UPDATE_UI event handler for ID_PREFERENCES in paraverMain.
  // Before editing this code, remove the block markers.
  event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_PREFERENCES in paraverMain. 
}

void paraverMain::selectTrace( Trace *trace )
{
  size_t currentTrace;
  for ( currentTrace = 0; currentTrace < loadedTraces.size(); ++currentTrace )
    if ( loadedTraces[ currentTrace ] == trace )
      break;

  int currentPage  = choiceWindowBrowser->GetSelection();

  if ( currentPage != 0 )
    choiceWindowBrowser->SetSelection( currentTrace + 1 );

  SetCurrentTrace( currentTrace );
}


PRV_UINT16 paraverMain::getTracePosition( Trace *trace )
{
  PRV_UINT16 currentTrace;

  for ( currentTrace = 0; currentTrace < loadedTraces.size(); ++currentTrace )
    if ( loadedTraces[ currentTrace ] == trace )
      break;

  return currentTrace;
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOLDELETE
 */

void paraverMain::OnTooldeleteClick( wxCommandEvent& event )
{
  if( currentHisto != NULL )
    currentHisto->setDestroy( true );
  if( currentTimeline != NULL )
  {
    if( currentTimeline->getChild() == NULL )
    {
      if( !currentTimeline->getUsedByHistogram() )
        currentTimeline->setDestroy( true );
      else
        wxMessageBox( _( "Cannot delete windows used by histograms." ),
                      _( "Paraver information" ),
                      wxOK | wxICON_INFORMATION );
    }
    else
      wxMessageBox( _( "Cannot delete parent windows. Delete first derived window" ),
                    _( "Paraver information" ),
                    wxOK | wxICON_INFORMATION );
  }
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOLDELETE
 */

void paraverMain::OnTooldeleteUpdate( wxUpdateUIEvent& event )
{
  if( currentTimeline != NULL || currentHisto != NULL )
    event.Enable( true );
  else
    event.Enable( false );
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_UNLOADTRACE
 */

void paraverMain::OnUnloadtraceClick( wxCommandEvent& event )
{
  wxArrayString choices;

  for( vector<Trace *>::iterator it = loadedTraces.begin(); it != loadedTraces.end(); ++it )
    choices.Add( wxString::FromAscii( (*it)->getTraceNameNumbered().c_str() ) );
  wxMultiChoiceDialog dialog( this, _("Select the traces to unload:"), _("Unload Traces"), choices );

  raiseCurrentWindow = false;

  dialog.ShowModal();
  if ( dialog.GetReturnCode() == wxID_OK )
  {
    wxArrayInt sel = dialog.GetSelections();
    for( size_t i = 0; i < sel.GetCount(); ++i )
      UnloadTrace( sel.Item( i ) );
  }

  raiseCurrentWindow = true;
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_UNLOADTRACE
 */

void paraverMain::OnUnloadtraceUpdate( wxUpdateUIEvent& event )
{
  event.Enable( loadedTraces.begin() != loadedTraces.end() );
}


void paraverMain::UnloadTrace( int whichTrace )
{
  vector<Window *> windows;
  vector<Histogram *> histograms;
  
  LoadedWindows::getInstance()->getAll( loadedTraces[ whichTrace ], windows );
  LoadedWindows::getInstance()->getAll( loadedTraces[ whichTrace ], histograms );
  
  for( vector<Window *>::iterator it = windows.begin(); it != windows.end(); ++it )
  {
    (*it)->setShowWindow( false );
    if( (*it)->getChild() == NULL )
      (*it)->setDestroy( true );
  }

  for( vector<Histogram *>::iterator it = histograms.begin(); it != histograms.end(); ++it )
  {
    (*it)->clearControlWindow();
    (*it)->clearDataWindow();
    (*it)->clearExtraControlWindow();
    (*it)->setShowWindow( false );
    (*it)->setDestroy( true );
  }
  
  traceWorkspaces.erase( loadedTraces[ whichTrace ] );
  firstUserWorkspace.erase( loadedTraces[ whichTrace ] );
  loadedTraces[ whichTrace ]->setUnload( true );
}

void paraverMain::clearProperties()
{
  windowProperties->Clear();
}

void paraverMain::OnActivate( wxActivateEvent& event )
{
#if 0
  if( event.GetActive() )
  {
    for( set<wxWindow *>::iterator it = activeWindows.begin();
        it != activeWindows.end(); ++it )
      (*it)->Raise();
    Raise();
  }
#endif
  event.Skip();
}

void paraverMain::PrepareToExit()
{
  vector<Histogram *> histograms;
  LoadedWindows::getInstance()->getAll( histograms );
  
  for( vector<Histogram *>::iterator it = histograms.begin(); it != histograms.end(); ++it )
  {
    (*it)->clearControlWindow();
    (*it)->clearDataWindow();
    (*it)->clearExtraControlWindow();
  }
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_PARAVERMAIN
 */

void paraverMain::OnCloseWindow( wxCloseEvent& event )
{
  if ( event.CanVeto() && !LoadedWindows::getInstance()->emptyWindows() )
  {
    if ( wxMessageBox( wxT( "Some windows already opened... continue closing?" ),
                       wxT( "Please confirm" ),
                       wxICON_QUESTION | wxYES_NO) != wxYES )
    {
      event.Veto();
      return;
    }
  }
  PrepareToExit();
  Destroy();
}


#ifndef WIN32

bool paraverMain::matchTraceNames( const string &fileName1,  // path1/name1 or name1
                                   const string &traceName1, // name1
                                   const string &fileName2 ) // path2/name2 or name2
{
// not working for Windows !!
//  string::size_type position = auxLine.find_last_of( '\\' );
  string::size_type position = fileName2.find_last_of( '/' );

  string traceName2;
  if ( position != string::npos )
    traceName2 = fileName2.substr( position + 1 );
  else
    traceName2 = fileName2;

  return ((( fileName1 != traceName1 ) && ( fileName1 == fileName2 )) ||
          (( fileName1 == traceName1 ) && ( traceName1 == traceName2 )));
}


void paraverMain::OnSignal( )
{
  // PRECOND: sig1 XOR sig2 == true;
  if( !canServeSignal )
    return;

  bool mySig1 = sig1;

  // Clear global variables
  sig1 = false;
  sig2 = false;

  // Is some trace loading now?
  if ( paraverMain::dialogProgress != NULL )
  {
    // Restore global variables
    sig1 = mySig1;
    sig2 = !mySig1;
    return;
  }

  // Does paraload.sig exists?
  string path = getenv("HOME");
  string filename = "paraload.sig";
  string fullName = path + "/" + filename;
  ifstream paraloadFile;
  paraloadFile.open( fullName.c_str() );
  if ( !paraloadFile  )
  {
    wxMessageDialog message( this,
                             _( "File ./paraload.sig not found" ),
                             _( "Signal Handler Manager" ),
                             wxOK | wxICON_EXCLAMATION );
    raiseCurrentWindow = false;
    message.ShowModal();
    raiseCurrentWindow = true;
    return;
  }

  string::size_type position;

  // Read all lines in paraload.sig
  vector<string> lines;
  string auxLine;
  while ( !paraloadFile.eof() )
  {
    // get line and trim its leading or trailing spaces or tabs
    std::getline( paraloadFile, auxLine );
    position = auxLine.find_last_not_of(" \t");
    if ( position != string::npos   )
    {
      auxLine.erase( position + 1 );
      position = auxLine.find_first_not_of(" \t");
      if ( position != string::npos )
      {
        auxLine.erase( 0, position );
      }
    }
    else
    {
      auxLine.clear();
    }
    
    // Is it useful? Save it!
    if ( auxLine.length() > 0 && auxLine[0] != '#' )
    {
      lines.push_back( auxLine );
      auxLine.clear();
    }
  }
  
  // POSTCOND:
  // lines[0] must contain paraver cfg
  // lines[1] must contain time range to set the window
  // if exists, lines[2] must contain a trace to load

  if ( lines.size() > 2 ) // any trace?
  {
    bool found = false;
    PRV_INT16 current = currentTrace;
  
    // Is that trace loaded? First, try with current!
    if ( loadedTraces.size() > 0 )
    {
      found = matchTraceNames( loadedTraces[ current ]->getFileName(),
                               loadedTraces[ current ]->getTraceName(),
                               lines[ 2 ] );
    }
    
    if ( !found )
    {
      // then continue with all the list
      for ( current = 0; current < (PRV_INT16)loadedTraces.size(); ++current )
      {
        found = matchTraceNames( loadedTraces[ current ]->getFileName(),
                                 loadedTraces[ current ]->getTraceName(),
                                 lines[ 2 ] );
        if ( found )
        {
          currentTrace = current; // select it !!
          break;
        }
      }
    }

    if ( current <= 0 || !found ) // No trace loaded or not found!!
      DoLoadTrace( lines[ 2 ] );
  }

  // Anyway, Am I able to load any cfg?
  if ( loadedTraces.size() == 0 )
  {
    wxMessageDialog message( this,
                             _( "No trace loaded" ),
                             _( "Signal Handler Manager" ),
                             wxOK | wxICON_EXCLAMATION );
    raiseCurrentWindow = false;
    message.ShowModal();
    raiseCurrentWindow = true;
    return;
  }
  
  // Read cfg location
  string cfgFullName = lines[ 0 ];

  // Code only for sigusr1: load cfg
  if( mySig1 )
    DoLoadCFG( cfgFullName );

  // Code for both sigusr1 and sigusr2: zoom
  Window* myCurrentTimeline;
  vector<Window *> timelines;
  
  LoadedWindows::getInstance()->getAll( loadedTraces[ currentTrace ], timelines );
  if ( currentTimeline == NULL )
  {
    if ( timelines.size() > 0 )
    {
      myCurrentTimeline = timelines[ timelines.size() - 1  ];
    }
    else
    {
      wxMessageDialog message( this, _( "No timeline created" ), _( "Signal Handler Manager" ), wxOK | wxICON_EXCLAMATION );
      raiseCurrentWindow = false;
      message.ShowModal();
      raiseCurrentWindow = true;
      return;
    }
  }
  else
    myCurrentTimeline = currentTimeline;

  // Read paraload.sig second line: times
  string times = lines[1];

  size_t pos = times.find(":");
  if ( pos == string::npos )
  {
    wxMessageDialog message( this, _( "Missing times separator ':' in file paraload.sig" ), _( "Signal Handler Manager" ), wxOK | wxICON_EXCLAMATION );
    raiseCurrentWindow = false;
    message.ShowModal();
    raiseCurrentWindow = true;
    return;
  }
  else
  {
    // Get begin time
    string time = times.substr( 0, pos );
    stringstream aux( time );
    double auxt1;
    aux >> auxt1;

    if ( auxt1 == -1.0 )
      auxt1 = 0.0;

    // Get end time
    string time2 = times.substr( ++pos,times.size() - 1  );
    stringstream aux2( time2 );
    double auxt2;
    aux2 >> auxt2;

    if ( auxt2 == -1.0 )
      auxt2 = myCurrentTimeline->getTrace()->getEndTime();

    // Zoom
    myCurrentTimeline->setWindowBeginTime( auxt1 );
    myCurrentTimeline->setWindowEndTime( auxt2 );
    myCurrentTimeline->addZoom( auxt1, auxt2 );

    myCurrentTimeline->setRedraw( true );
    myCurrentTimeline->setChanged( true );
  }

  // Refresh
  choiceWindowBrowser->UpdateWindowUI();
}
#endif


/*!
 * wxEVT_UPDATE_UI event handler for ID_TOOL_CUT_TRACE
 */

void paraverMain::OnToolCutTraceUpdate( wxUpdateUIEvent& event )
{
  event.Enable( true );
}


void paraverMain::enqueueFile( string whichFile )
{
  loadFilesQueue.push( whichFile );
}


#ifdef WIN32
void paraverMain::OnKeyCopy()
{
  wxTreeCtrl *tree = (wxTreeCtrl *) choiceWindowBrowser->GetCurrentPage();
  if( !tree->GetSelection().IsOk() )
    return;
  TreeBrowserItemData *item = (TreeBrowserItemData *) tree->GetItemData( tree->GetSelection() );
  if( item->getTimeline() != NULL )
    item->getTimeline()->OnPopUpCopy();
  else if( item->getHistogram() != NULL )
    item->getHistogram()->OnPopUpCopy();
}


void paraverMain::OnKeyPaste()
{
  wxTreeCtrl *tree = (wxTreeCtrl *) choiceWindowBrowser->GetCurrentPage();
  if( !tree->GetSelection().IsOk() )
    return;
  TreeBrowserItemData *item = (TreeBrowserItemData *) tree->GetItemData( tree->GetSelection() );
  if( item->getTimeline() != NULL )
    item->getTimeline()->OnPopUpPasteSpecial();
  else if( item->getHistogram() != NULL )
    item->getHistogram()->OnPopUpPasteSpecial();
}
#endif

void paraverMain::OnFindDialog()
{
  wxTreeCtrl *tree = (wxTreeCtrl *) choiceWindowBrowser->GetCurrentPage();
  if( !tree->GetSelection().IsOk() )
    return;
  TreeBrowserItemData *item = (TreeBrowserItemData *) tree->GetItemData( tree->GetSelection() );
  if( item->getTimeline() != NULL )
    item->getTimeline()->OnFindDialog();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_CUT_TRACE
 */
void paraverMain::OnToolCutTraceClick( wxCommandEvent& event )
{
  if ( currentTrace == -1)
    ShowCutTraceWindow();
  else
    ShowCutTraceWindow( loadedTraces[ currentTrace ]->getFileName() );
}


string paraverMain::DoLoadFilteredTrace( string traceSrcFileName,
                                         string traceDstFileName,
                                         TraceOptions *traceOptions,
                                         vector< string > &filterToolIDs )
{
  string tmpTraceIn, tmpTraceOut;
  string tmpNameIn, tmpNameOut, tmpPathOut, tmpPathOutBackup;
  string strOutputFile;
  vector< string > tmpFiles;

  // Only for cutter
  vector< TEventType > allTypes;
  vector< TEventType > typesWithValueZero;
  EventLabels labels;
  map< TEventValue, string > currentEventValues;

#ifdef OLD_PCFPARSER
  ParaverTraceConfig *myConfig;
#else
  UIParaverTraceConfig *myConfig;
#endif

  string pcf_name;

  ProgressController *progress = ProgressController::create( localKernel );
  progress->setHandler( progressFunction, this );

  if( paraverMain::dialogProgress == NULL )
    paraverMain::dialogProgress = new wxProgressDialog( wxT("Processing trace..."),
                                                        wxT(""),
                                                        numeric_limits<int>::max(),
                                                        this,
                                                        wxPD_CAN_ABORT|wxPD_AUTO_HIDE|\
                                                        wxPD_APP_MODAL|wxPD_ELAPSED_TIME|\
                                                        wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME );


  tmpNameOut = traceSrcFileName;

  for( PRV_UINT16 i = 0; i < filterToolIDs.size(); ++i )
  {
    tmpNameIn = tmpNameOut;

    if ( i == filterToolIDs.size() - 1 )
      tmpNameOut = traceDstFileName;
    else
      tmpNameOut = localKernel->getNewTraceName( tmpNameIn, filterToolIDs[ i ], false );

    paraverMain::dialogProgress->Pulse( wxString::FromAscii( tmpNameOut.c_str() ) );
    paraverMain::dialogProgress->Fit();
    paraverMain::dialogProgress->Show();

    if ( filterToolIDs[ i ] == TraceCutter::getID() )
    {
#ifdef WIN32
      struct _stat tmpStatBuffer;
#else
      struct stat tmpStatBuffer;
#endif

      pcf_name = LocalKernel::composeName( tmpNameIn, string( "pcf" ) );
      int statReturn;
#ifdef WIN32
      statReturn = _stat( pcf_name.c_str(), &tmpStatBuffer );
#else
      statReturn = stat( pcf_name.c_str(), &tmpStatBuffer );
#endif

      if( statReturn == 0 && tmpStatBuffer.st_size > 0 )
      {
#ifdef OLD_PCFPARSER
        myConfig = new ParaverTraceConfig( pcf_name );
        myConfig->parse();
#else
        myConfig = new UIParaverTraceConfig();
        myConfig->parse( pcf_name );
#endif
        labels = EventLabels( *myConfig, set<TEventType>() );
        labels.getTypes( allTypes );
        for( vector< TEventType >::iterator it = allTypes.begin(); it != allTypes.end(); ++it )
        {
          if ( labels.getValues( *it, currentEventValues ) )
          {
            if ( currentEventValues.find( TEventValue( 0 )) != currentEventValues.end() )
            {
              typesWithValueZero.push_back( *it );
            }
            currentEventValues.clear();
          }
        }

        delete myConfig;
      }

      TraceCutter *traceCutter = localKernel->newTraceCutter( traceOptions, typesWithValueZero );
      traceCutter->execute( (char *)tmpNameIn.c_str(), (char *)tmpNameOut.c_str(), progress );
      localKernel->copyPCF( tmpNameIn, tmpNameOut );
      delete traceCutter;
    }
    else  if ( filterToolIDs[ i ] == TraceFilter::getID() )
    {
      map< TTypeValuePair, TTypeValuePair > dummyTranslation; // it it's empty, it's ignored

      TraceFilter *traceFilter = localKernel->newTraceFilter( (char *)tmpNameIn.c_str(),
                                                              (char *)tmpNameOut.c_str(),
                                                              traceOptions,
                                                              dummyTranslation,
                                                              progress );
      localKernel->copyPCF( tmpNameIn, tmpNameOut );
      delete traceFilter;
    }
    else if ( filterToolIDs[ i ] == TraceSoftwareCounters::getID() )
    {
      TraceSoftwareCounters *traceSoftwareCounters =
              localKernel->newTraceSoftwareCounters( (char *)tmpNameIn.c_str(),
                                                     (char *)tmpNameOut.c_str(),
                                                     traceOptions,
                                                     progress );
      // traceSoftwareCounters modifies the pcf, don't copy here!
      delete traceSoftwareCounters;
    }
    else
    {
    }

    localKernel->copyROW( tmpNameIn, tmpNameOut );
    tmpFiles.push_back( tmpNameOut );
  }

  // Delete intermediate files
  string pcfName, rowName;
  for( PRV_UINT16 i = 0; i < tmpFiles.size() - 1; ++i )
  {
    pcfName = LocalKernel::composeName( tmpFiles[ i ], string( "pcf" ) );
    rowName = LocalKernel::composeName( tmpFiles[ i ], string( "row" ) );
    remove( tmpFiles[ i ].c_str() );
    remove( pcfName.c_str() );
    remove( rowName.c_str() );
  }

  localKernel->commitNewTraceName( traceDstFileName );

  paraverMain::dialogProgress->Show( false );
  delete paraverMain::dialogProgress;
  paraverMain::dialogProgress = NULL;
  delete progress;

  return tmpNameOut;
}


void paraverMain::MainSettingsCutFilterDialog( CutFilterDialog *cutFilterDialog,
                                                const string& filename,
                                                bool loadTrace )
{
  cutFilterDialog->SetLocalKernel( localKernel );
  cutFilterDialog->SetLoadResultingTrace( loadTrace );

  // Set trace
  if( !filename.empty() )
  {
    // 1) given by parameter
    cutFilterDialog->SetNameSourceTrace( filename );
  }
  else if ( loadedTraces.size() > 0 && currentTrace == -1 )
  {
    // 2) tracePath if "All Traces" selected
    cutFilterDialog->SetNameSourceTrace( std::string( tracePath.mb_str() ) + PATH_SEP );
  }
  else
  {
    // 3) Default
    cutFilterDialog->SetNameSourceTrace( paraverConfig->getGlobalTracesPath() + PATH_SEP );
  }
}                                                    


void paraverMain::OptionsSettingCutFilterDialog( CutFilterDialog *cutFilterDialog,
                                                  TraceOptions *traceOptions, 
                                                  const string& xmlFile,
                                                  vector< string > &filterToolOrder )
{
  if ( !xmlFile.empty() )
  {
    filterToolOrder = traceOptions->parseDoc( (char *)xmlFile.c_str() );

    // Keep my XMLPath
    wxString auxName = wxString::FromAscii( xmlFile.c_str() );
    wxString auxPath = wxFileName( auxName ).GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
    XMLPath = std::string( auxPath.mb_str() );

    bool refresh = false;
    cutFilterDialog->SetXMLFile( auxName, refresh );
    //cutFilterDialog->TransferTraceOptionsToWindow( traceOptions, filterToolOrder );
  }
  else if ( !XMLLoadedBefore )
  {
    // Default Path
    XMLPath =  paraverConfig->getFiltersXMLPath() + PATH_SEP;
  }
  
  // If xmlFile empty we consider that information is given by parameter
  cutFilterDialog->TransferTraceOptionsToWindow( traceOptions, filterToolOrder );
}


void paraverMain::OnOKCutFilterDialog( CutFilterDialog *cutFilterDialog,
                                        vector< string > filterToolOrder )
{
  filterToolOrder   = cutFilterDialog->GetFilterToolOrder();
  string srcTrace   = cutFilterDialog->GetNameSourceTrace();
  string dstTrace   = cutFilterDialog->GetNameDestinyTrace();
  
  DoLoadFilteredTrace( srcTrace, dstTrace, cutFilterDialog->GetTraceOptions(), filterToolOrder );

  if ( cutFilterDialog->GetLoadResultingTrace() )
  {
    DoLoadTrace( dstTrace );
  }

  XMLLoadedBefore = cutFilterDialog->GetLoadedXMLPath( XMLPath );
  
  if ( cutFilterDialog->GetRunAppWithResultingTrace() )
  {
    wxString auxTrace =  wxString::FromAscii( dstTrace.c_str() );
    ShowRunCommand( auxTrace );
  }
}


void paraverMain::ShowCutTraceWindow( const string& filename,
                                       bool loadTrace,
                                       const string& xmlFile )
{
  CutFilterDialog *cutFilterDialog = new CutFilterDialog( this );  

  MainSettingsCutFilterDialog( cutFilterDialog, filename, loadTrace );
  
  TraceOptions *traceOptions = TraceOptions::create( localKernel );
  vector< string > filterToolOrder;
  OptionsSettingCutFilterDialog( cutFilterDialog, traceOptions, xmlFile, filterToolOrder );
  
  if( cutFilterDialog->ShowModal() == wxID_OK )
  {
    OnOKCutFilterDialog( cutFilterDialog, filterToolOrder );
  }

#if wxMAJOR_VERSION<3
  cutFilterDialog->MakeModal( false );
#endif
  
  delete traceOptions;
  delete cutFilterDialog;
}


/*!
 * wxEVT_ICONIZE event handler for ID_PARAVERMAIN
 */

void paraverMain::OnIconize( wxIconizeEvent& event )
{
  for( unsigned int iPage = 0; iPage < choiceWindowBrowser->GetPageCount(); iPage++ )
  {
    wxTreeCtrl *currentTree = (wxTreeCtrl *) choiceWindowBrowser->GetPage( iPage );
    wxTreeItemId root = currentTree->GetRootItem();
#if wxMAJOR_VERSION<3
    iconizeWindows( currentTree, root, event.Iconized() );
#else
    iconizeWindows( currentTree, root, event.IsIconized() );
#endif
  }
}


/*!
 * wxEVT_SIZE event handler for ID_PARAVERMAIN
 */

void paraverMain::OnSize( wxSizeEvent& event )
{
  paraverConfig->setMainWindowWidth( event.GetSize().GetWidth() );
  paraverConfig->setMainWindowHeight( event.GetSize().GetHeight() );
  
  event.Skip();
}


void paraverMain::OnSessionTimer( wxTimerEvent& event )
{
#ifdef WIN32
  string file( ParaverConfig::getInstance()->getGlobalSessionPath() + "\\paraver.session" );
#else
  string file( ParaverConfig::getInstance()->getGlobalSessionPath() + "/paraver.session" );
#endif
  SessionSaver::SaveSession( wxString::FromAscii( file.c_str() ), GetLoadedTraces() );
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENULOADSESSION
 */

void paraverMain::OnMenuloadsessionClick( wxCommandEvent& event )
{
  wxFileDialog dialog( this, wxT( "Load session" ), _(""), _(""), _("Paraver session (*.session)|*.session|All files|*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST );
  if( dialog.ShowModal() == wxID_OK )
  {
    SessionSaver::LoadSession( dialog.GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUSAVESESSION
 */

void paraverMain::OnMenusavesessionClick( wxCommandEvent& event )
{
  vector< wxString > extensions;
  extensions.push_back( wxT( "session" ) );
  
  FileDialogExtension dialog( this,
                              wxT( "Save session" ),
                              _(""),
                              _(""),
                              _("Paraver session (*.session)|*.session"),
                              wxFD_SAVE|wxFD_OVERWRITE_PROMPT,
                              wxDefaultPosition,
                              wxDefaultSize,
                              _( "filedlg" ),
                              extensions );
  if( dialog.ShowModal() == wxID_OK )
  {
    wxFileName tmpFile( dialog.GetPath() );
    SessionSaver::SaveSession( tmpFile.GetFullPath(), GetLoadedTraces() );
  }
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
 */

void paraverMain::OnAboutClick( wxCommandEvent& event )
{
  wxAboutDialogInfo info;
  wxArrayString developers;
  wxString description;
  
  info.SetName( _("wxParaver") );
  info.SetVersion( wxString() << _( VERSION ) << _( " (Build " ) + \
                   wxString() << _( __DATE__ ) << _( ")" ) );

  description << _( "wxParaver is a trace-based visualization and analysis tool " );
  description << _( "designed to study quantitative detailed metrics and obtain " );
  description << _( "qualitative knowledge of the performance of applications, " );
  description << _( "libraries, processors and whole architectures.\n" );

  description << _( "\nwxParaver, Extrae, and Dimemas belong to the Performance ");
  description << _( "Tools Suite developed at BSC-CNS :\n" );
  description << _( "Barcelona Supercomputing Center - Centro Nacional de Supercomputacion\n\n" );

  description << _( "\n(paraver@bsc.es)\n" );
  info.SetDescription( description );
  developers.Add( _("Eloy Martinez Hortelano (eloy.martinez@bsc.es)") );
  developers.Add( _("Pedro Antonio Gonzalez Navarro (pedro.gonzalez@bsc.es)") );
  info.SetDevelopers( developers );
  info.SetWebSite( _("http://www.bsc.es/paraver") );
  //info.SetCopyright(_T(""));

  wxAboutBox(info);
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
 */

void paraverMain::OnTutorialsClick( wxCommandEvent& event )
{
  wxString tutorialsRoot;

  if ( tutorialsWindow == NULL )
  {
    tutorialsRoot = wxString( paraverMain::myParaverMain->GetParaverConfig()->getGlobalTutorialsPath().c_str(), wxConvUTF8 );
    tutorialsWindow = new TutorialsBrowser( this, tutorialsRoot, wxID_ANY, _("Tutorials") );
  }

  tutorialsWindow->Show( true );
}


void paraverMain::ShowRunCommand( wxString traceFile )
{
  RunScript runApplication( (wxWindow *)this, traceFile );
  
  if ( runApplication.ShowModal() == wxID_OK )
  {
  }
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_RUN_SCRIPT
 */

void paraverMain::OnToolRunApplicationClick( wxCommandEvent& event )
{
  if( !loadedTraces.empty() )
  {
    wxString tmpStr( loadedTraces[ currentTrace ]->getFileName().c_str(), wxConvUTF8 );
    ShowRunCommand( tmpStr );
  }
  else
    ShowRunCommand( wxT("") );
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUITEM
 */

void paraverMain::OnHelpcontentsClick( wxCommandEvent& event )
{
  if ( helpContents != NULL )
  {
    helpContents->Show( true );
  }
  else
  {
/*#ifdef __WXMAC__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef fileURL = CFBundleCopyResourceURL( mainBundle, CFSTR("aaa"), CFSTR("txt"), NULL );
    CFStringRef filePath = CFURLCopyFileSystemPath( fileURL, kCFURLPOSIXPathStyle );
    CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
    const char *path = CFStringGetCStringPtr( filePath, encodingMethod );
    cout<<"File location: "<<path<<endl;
#endif*/
    wxString paraverHome;
#ifdef WIN32
    std::string baseDir;

    char myPath[ MAX_LEN_PATH ];
    HMODULE hModule = GetModuleHandle( NULL );
    if ( hModule != NULL )
    {
      GetModuleFileName( NULL, myPath, ( sizeof( myPath ) ));
      PathRemoveFileSpec( myPath );
      /*char tmpMyPath[ MAX_LEN_PATH ];
      size_t tmpSize;
      wcstombs_s( &tmpSize, tmpMyPath, MAX_LEN_PATH, myPath, MAX_LEN_PATH );
      baseDir = tmpMyPath;*/
      baseDir = myPath;
    }
    paraverHome = wxT( baseDir.c_str() );
    
    if( paraverHome != wxT( "" ) )
#elif defined( __APPLE__ )
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char tmpPath[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)tmpPath, PATH_MAX))
    {
        throw ParaverKernelException();
    }
    CFRelease(resourcesURL);

    paraverHome = tmpPath;

    if( paraverHome != wxT( "" ) )
#else
    if ( wxGetEnv( wxString( wxT( "PARAVER_HOME" ) ), &paraverHome ) )
#endif
    {
      wxString helpContentsDir =
              wxFileName::GetPathSeparator() +
              wxString( wxT( "share" ) ) +
              wxFileName::GetPathSeparator() +
              wxString( wxT( "doc" ) ) +
              wxFileName::GetPathSeparator() +
              wxString( wxT( "wxparaver_help" ) ) +
              wxFileName::GetPathSeparator();

      wxString helpContentsRoot = paraverHome + helpContentsDir;

      if ( wxFileName( helpContentsRoot ).DirExists() )
      {
        helpContents = new HelpContents( this, helpContentsRoot, wxID_ANY, _("Help Contents") );
        helpContents->Show( true );
      }
      else
      {
        wxString msg =
                wxString( wxT( "Path to Help Contents doesn't exist:\n\n  " ) ) +
                helpContentsRoot +
                wxString( wxT( "\n\nPlease check $PARAVER_HOME:\n\n" ) ) +
                paraverHome;
        
        wxMessageDialog message( this, msg, _( "Warning" ), wxOK | wxICON_WARNING );
        message.ShowModal();
      }
    }
    else
    {
      wxString msg =
                wxString( wxT( "Unable to find Help Contents.\n\n$PARAVER_HOME is undefined" ) );
        
      wxMessageDialog message( this, msg, _( "Warning" ), wxOK | wxICON_WARNING );
      message.ShowModal();
    }
  }
}


wxString paraverMain::getHintComposed( const std::pair< std::string, std::string >& hint )
{
//  wxFileName filename( wxString::FromAscii(  hint.first.c_str() ) );
//  return filename.GetName() + _( " - " ) + wxString::FromAscii(  hint.second.c_str() );
  return wxString::FromAscii(  hint.second.c_str() );
}


void paraverMain::OnHintClick( wxCommandEvent& event )
{
  int hintId = event.GetId();
  wxMenu *tmpMenu;
  wxString selectedHint = menuHints->FindItem( hintId, &tmpMenu )->GetItemLabelText();
  
  bool found = false;
  wxString workspaceName;

  wxMenuItemList& menuItems1 = menuHints->GetMenuItems();
  for (wxMenuItemList::iterator menuIt = menuItems1.begin(); menuIt != menuItems1.end() ; ++menuIt )
  {
    if( (*menuIt)->GetKind() == wxITEM_SEPARATOR )
      continue;

    workspaceName = (*menuIt)->GetItemLabelText();

    wxMenuItemList& menuItems2 = (*menuIt)->GetSubMenu()->GetMenuItems();
    for (wxMenuItemList::iterator menuIt2 = menuItems2.begin(); menuIt2 != menuItems2.end() ; ++menuIt2 )
    {
      if ( hintId == (*menuIt2)->GetId() )
      {
        found = true;
        break;
      }
    }
    
    if (found)
      break;
  }

  wxString newWorkspaceName;
  if( workspaceName.EndsWith( wxT( "#2" ), &newWorkspaceName ) )
    workspaceName = newWorkspaceName;

  for ( vector< string >::iterator it = traceWorkspaces[ loadedTraces[ currentTrace ] ].begin(); it != traceWorkspaces[ loadedTraces[ currentTrace ]  ].end(); ++it )
  {
    if ( workspaceName == wxString::FromAscii( it->c_str() ) )
    {
      std::vector< std::pair< std::string, std::string > > currentHints = workspacesManager->getWorkspace( *it, WorkspaceManager::DISTRIBUTED ).getHintCFGs();
      for ( std::vector<std::pair<std::string,std::string> >::iterator it2 = currentHints.begin(); it2 != currentHints.end(); ++it2 )
      {
        wxString hintName = getHintComposed( *it2 );
        if ( selectedHint == hintName )
        {     
          wxFileName tmpCFG( wxString::FromAscii( it2->first.c_str() ) );
          if ( tmpCFG.IsRelative() )
          {
            wxString tmpGlobalCFGs( localKernel->getDistributedCFGsPath().c_str(), wxConvUTF8 );
            tmpCFG.MakeAbsolute( tmpGlobalCFGs );
          }

          DoLoadCFG( std::string( tmpCFG.GetFullPath().mb_str() ) );
          return;
        }
      }
      
      currentHints = workspacesManager->getWorkspace( *it, WorkspaceManager::USER_DEFINED ).getHintCFGs();
      for ( std::vector<std::pair<std::string,std::string> >::iterator it2 = currentHints.begin(); it2 != currentHints.end(); ++it2 )
      {
        wxString hintName = getHintComposed( *it2 );
        if ( selectedHint == hintName )
        {     
          wxFileName tmpCFG( wxString::FromAscii( it2->first.c_str() ) );
          if ( tmpCFG.IsRelative() )
          {
            wxString tmpGlobalCFGs( ParaverConfig::getInstance()->getGlobalCFGsPath().c_str(), wxConvUTF8 );
            tmpCFG.MakeAbsolute( tmpGlobalCFGs );
          }

          DoLoadCFG( std::string( tmpCFG.GetFullPath().mb_str() ) );
          return;
        }
      }
      
    }
  }
}


void paraverMain::OnMenuHintUpdate( wxUpdateUIEvent& event )
{
  if( loadedTraces.empty() || traceWorkspaces[ loadedTraces[ currentTrace ] ].empty() )
    GetMenuBar()->EnableTop( 1, false );
  else
    GetMenuBar()->EnableTop( 1, true );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ACTIVE_WORKSPACES
 */

void paraverMain::OnButtonActiveWorkspacesClick( wxCommandEvent& event )
{
  vector< string > tmpWorkspaces = workspacesManager->getWorkspaces( WorkspaceManager::ALL );
  
  if( tmpWorkspaces.empty() )
  {
    wxMessageDialog emptyWorkspacesDialog( this, 
                                           _( "No Workspaces available.\nDo you want to open Preferences dialog to create any?" ),
                                           _( "No Workspaces" ),
                                           wxYES_NO| wxICON_QUESTION );
    if( emptyWorkspacesDialog.ShowModal() == wxID_YES )
    {
      ShowPreferences( ID_PREFERENCES_WORKSPACES );
    }

    return;
  }
  
  wxArrayString tmpNames;
  wxArrayInt tmpActive;
  int position = 0;
  for ( vector< string >::iterator it = tmpWorkspaces.begin(); it != tmpWorkspaces.end(); ++it )
  {
    tmpNames.Add( wxString::FromAscii( it->c_str() ) );
    if ( std::find( traceWorkspaces[ loadedTraces[ currentTrace ] ].begin(), traceWorkspaces[ loadedTraces[ currentTrace ] ].end(),*it ) != traceWorkspaces[ loadedTraces[ currentTrace ] ].end() )
      tmpActive.Add( position );
    ++position;
  }
  
  wxMultiChoiceDialog tmpChoiceDialog( this, _("Select active workspaces"),  _( "Workspaces" ), tmpNames );  
  tmpChoiceDialog.SetSelections( tmpActive );
  
  if ( tmpChoiceDialog.ShowModal() == wxID_OK )
  {
    tmpActive = tmpChoiceDialog.GetSelections();
    traceWorkspaces[ loadedTraces[ currentTrace ] ].clear();
    for( size_t i = 0; i < tmpActive.GetCount(); ++i )
    {
      traceWorkspaces[ loadedTraces[ currentTrace ] ].push_back( tmpWorkspaces[ tmpActive[ i ] ] );
    }
    
    setActiveWorkspacesText();
    refreshMenuHints();

    // These are here because no UpdateUI for MenuBar
    wxUpdateUIEvent tmpEvent;
    OnMenuHintUpdate( tmpEvent );
  }
}


void paraverMain::setActiveWorkspacesText()
{
  if ( currentTrace == -1 || traceWorkspaces[ loadedTraces[ currentTrace ] ].empty() )
  {
    txtActiveWorkspaces->SetValue( _("None") );
  }
  else
  {
    wxString tmpActive;
    size_t tmpCurrentWorkspace = 0;
    for ( vector< string >::iterator it = traceWorkspaces[ loadedTraces[ currentTrace ] ].begin(); it != traceWorkspaces[ loadedTraces[ currentTrace ] ].end(); ++it )
    {
      if ( !tmpActive.IsEmpty() )
        tmpActive += _( "+" );
        
      if( tmpCurrentWorkspace < firstUserWorkspace[ loadedTraces[ currentTrace ] ] )
        tmpActive += wxString::FromAscii( it->c_str() );
      else
      {
        if( workspacesManager->existWorkspace( it->c_str(), WorkspaceManager::DISTRIBUTED ) )
          tmpActive += wxString::FromAscii( it->c_str() ) + wxT( "#2" );
        else
          tmpActive += wxString::FromAscii( it->c_str() );
      }
      ++tmpCurrentWorkspace;
    }
    
    txtActiveWorkspaces->SetValue( tmpActive );
  }
}



