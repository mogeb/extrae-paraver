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
 | @last_commit: $Date: 2016/01/14 14:09:52 $
 | @version:     $Revision: 1.44 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes
#include <string>
#include <wx/file.h>

#include "helpcontents.h"
#include "paravermain.h"

////@begin XPM images
#include "index.xpm"
#include "arrow_left.xpm"
#include "arrow_right.xpm"
////@end XPM images

/*!
 * HelpContents type definition
 */

IMPLEMENT_DYNAMIC_CLASS( HelpContents, wxDialog )
IMPLEMENT_DYNAMIC_CLASS( TutorialsBrowser, HelpContents )


/*!
 * HelpContents event table definition
 */

BEGIN_EVENT_TABLE( HelpContents, wxDialog )

////@begin HelpContents event table entries
  EVT_HTML_LINK_CLICKED( ID_HTMLWINDOW, HelpContents::OnHtmlwindowLinkClicked )
  EVT_BUTTON( ID_BUTTON_INDEX, HelpContents::OnButtonIndexClick )
  EVT_BUTTON( ID_BITMAPBUTTON_BACK, HelpContents::OnBitmapbuttonBackClick )
  EVT_UPDATE_UI( ID_BITMAPBUTTON_BACK, HelpContents::OnBitmapbuttonBackUpdate )
  EVT_BUTTON( ID_BITMAPBUTTON_FORWARD, HelpContents::OnBitmapbuttonForwardClick )
  EVT_UPDATE_UI( ID_BITMAPBUTTON_FORWARD, HelpContents::OnBitmapbuttonForwardUpdate )
  EVT_BUTTON( ID_BUTTON_CLOSE, HelpContents::OnButtonCloseClick )
////@end HelpContents event table entries

END_EVENT_TABLE()


BEGIN_EVENT_TABLE( TutorialsBrowser, wxDialog )

  EVT_HTML_LINK_CLICKED( ID_HTMLWINDOW, TutorialsBrowser::OnHtmlwindowLinkClicked )  
  EVT_BUTTON( ID_BUTTON_INDEX, TutorialsBrowser::OnButtonIndexClick )
  EVT_BUTTON( ID_BITMAPBUTTON_BACK, TutorialsBrowser::OnBitmapbuttonBackClick )
  EVT_UPDATE_UI( ID_BITMAPBUTTON_BACK, TutorialsBrowser::OnBitmapbuttonBackUpdate )
  EVT_BUTTON( ID_BITMAPBUTTON_FORWARD, TutorialsBrowser::OnBitmapbuttonForwardClick )
  EVT_UPDATE_UI( ID_BITMAPBUTTON_FORWARD, TutorialsBrowser::OnBitmapbuttonForwardUpdate )
  EVT_BUTTON( ID_BUTTON_CLOSE, TutorialsBrowser::OnButtonCloseClick )

END_EVENT_TABLE()

/*!
 * HelpContents constructors
 */

HelpContents::HelpContents()
{
  Init();
}

HelpContents::HelpContents( wxWindow* parent,
                            const wxString& whichHelpContentsRoot,
                            wxWindowID id,
                            const wxString& caption,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style ) :
        helpContentsRoot( whichHelpContentsRoot )
{
  Init();
  Create(parent, id, caption, pos, size, style);
}


/*!
 * HelpContents creator
 */

bool HelpContents::Create( wxWindow* parent,
                            wxWindowID id,
                            const wxString& caption,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style )
{
////@begin HelpContents creation
  SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
  wxDialog::Create( parent, id, caption, pos, size, style );

  CreateControls();
  Centre();
////@end HelpContents creation
  return true;
}


/*!
 * HelpContents destructor
 */

HelpContents::~HelpContents()
{
}


/*!
 * Member initialisation
 */
void HelpContents::Init()
{
  currentHelpContentsDir = _("");
  
  // shouldn't happen
  //if ( helpContentsRoot.IsEmpty() )
  //  SetHelpContentsRoot( paraverMain::myParaverMain->GetParaverConfig()->getGlobalTutorialsPath() );
}


const wxString HelpContents::getHtmlIndex( const wxString& path )
{
  wxString htmlIndex;
  wxFileName index( path + wxFileName::GetPathSeparator() + _("index.html") );

  if ( index.FileExists() )
  {
    htmlIndex = index.GetFullPath();
  }

  return htmlIndex;
}


const wxString HelpContents::getTitle( int numTutorial, const wxString& path )
{
  wxString helpContentsTitle;

  wxHtmlWindow auxHtml( this );
  auxHtml.LoadPage( path  + wxFileName::GetPathSeparator() + _("index.html") );
  helpContentsTitle = auxHtml.GetOpenedPageTitle();
  if ( helpContentsTitle.empty() || helpContentsTitle == _("index.html") ) // never is empty !?!
  {
    helpContentsTitle = _("Section");
    helpContentsTitle << numTutorial;
  }

  return helpContentsTitle;
}


void HelpContents::appendHelpContents( const wxString& title,
                                        const wxString& path,
                                        wxString& htmlDoc )
{
  htmlDoc += _("<LI><P><A HREF=\"") + path + _("\">") + title + _("</A></P></LI>");
}


void HelpContents::htmlMessage( wxString& htmlDoc )
{
  htmlDoc += _("<P><H3>No Help Contents found!?</H3></P>");
  htmlDoc += _("<P>Please check that <B>root directory</B> to Help Contents exists.</P>");
  htmlDoc += _("<P>The current one is ");
  htmlDoc += GetHelpContentsRoot();
  htmlDoc += _("</P>");
  htmlDoc += _("<P>If missing, try to download a newer wxparaver version, or please contact us at paraver@bsc.es.</P>");
}


bool HelpContents::helpContentsFound( wxArrayString & contentsList )
{
  // get contents directory
  wxFileName helpContentsGlobalPath( GetHelpContentsRoot() );

  wxString currentDir = wxFindFirstFile(
          helpContentsGlobalPath.GetLongPath() + wxFileName::GetPathSeparator() + _("*"),
          wxDIR );
  while( !currentDir.empty() )
  {
    if ( getHtmlIndex( currentDir ) != _("")  )
    {
      // index.html found! => we consider this is a tutorial
      contentsList.Add( currentDir );
    }

    currentDir = wxFindNextFile();
  }
  
  return ( contentsList.GetCount() > (size_t)0 );
}


void HelpContents::buildIndexTemplate( wxString title, wxString filePrefix )
{
  // write html index
  wxString contentsHtmlIndex;
  wxString contentsList;
  
  contentsHtmlIndex += _("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">");
  contentsHtmlIndex += _("<HTML>");
  contentsHtmlIndex += _("<HEAD>");
  contentsHtmlIndex += _("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=us-ascii\" />");
  contentsHtmlIndex += _("<TITLE>" ) + title + _( "</TITLE>");
  contentsHtmlIndex += _("</HEAD>");
  contentsHtmlIndex += _("<BODY>");

  contentsHtmlIndex += _("<P ALIGN=LEFT><IMG SRC=\"memory:logoBSC.xpm\" NAME=\"logoBSC\" ALIGN=BOTTOM BORDER=0></P>" );

  wxArrayString contents;
  if ( helpContentsFound( contents ) )
  {  
    // Order them: every content dir must have a numbered prefix
    //   tutorialsPath/ #_title_bla_bla /index.html
    contents.Sort();
    int numSections = int( contents.GetCount() );
    
    for( int i = 0; i < numSections; ++i )
    {
      appendHelpContents( getTitle( numSections, contents[ i ] ),
                          getHtmlIndex( contents[ i ] ),
                          contentsList );
    }

    contentsList += _("</UL>");

    contentsHtmlIndex += _("<P><H3><B>Index</B></H3></P>");
    contentsHtmlIndex += contentsList;
  }
  else
  {
    htmlMessage( contentsHtmlIndex );
  }

  // close html index
  contentsHtmlIndex += _("</BODY></HTML>");

  wxString indexFileName =
        wxString::FromAscii( paraverMain::myParaverMain->GetParaverConfig()->getParaverConfigDir().c_str() ) + 
        wxString( wxFileName::GetPathSeparator() ) +
        filePrefix +
        wxT( "_index.html" );
  wxFile indexFile( indexFileName, wxFile::write );
  if ( indexFile.IsOpened() )
  {
    indexFile.Write( contentsHtmlIndex );
    indexFile.Close();
    htmlWindow->LoadPage( indexFileName );
  }
  else
  {
    htmlWindow->SetPage( contentsHtmlIndex );
  }
}


void HelpContents::buildIndex()
{
  buildIndexTemplate( wxString( wxT( "Help Contents" ) ), wxString( wxT( "help_contents" ) ) );
}


/*!
 * Control creation for HelpContents
 */
void HelpContents::CreateControls()
{    
////@begin HelpContents content construction
  HelpContents* itemDialog1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemDialog1->SetSizer(itemBoxSizer2);

  htmlWindow = new wxHtmlWindow( itemDialog1, ID_HTMLWINDOW, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
  itemBoxSizer2->Add(htmlWindow, 1, wxGROW|wxALL, 5);

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer4, 0, wxGROW|wxALL, 5);

  wxBitmapButton* itemBitmapButton5 = new wxBitmapButton( itemDialog1, ID_BUTTON_INDEX, itemDialog1->GetBitmapResource(wxT("index.xpm")), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  if (HelpContents::ShowToolTips())
    itemBitmapButton5->SetToolTip(_("Tutorials index page"));
  itemBoxSizer4->Add(itemBitmapButton5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  buttonHistoryBack = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_BACK, itemDialog1->GetBitmapResource(wxT("arrow_left.xpm")), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  if (HelpContents::ShowToolTips())
    buttonHistoryBack->SetToolTip(_("Previous page"));
  itemBoxSizer4->Add(buttonHistoryBack, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  buttonHistoryForward = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_FORWARD, itemDialog1->GetBitmapResource(wxT("arrow_right.xpm")), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  if (HelpContents::ShowToolTips())
    buttonHistoryForward->SetToolTip(_("Next page"));
  itemBoxSizer4->Add(buttonHistoryForward, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  itemBoxSizer4->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxButton* itemButton9 = new wxButton( itemDialog1, ID_BUTTON_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer4->Add(itemButton9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end HelpContents content construction
  buildIndex();
}


/*!
 * Should we show tooltips?
 */

bool HelpContents::ShowToolTips()
{
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap HelpContents::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin HelpContents bitmap retrieval
  wxUnusedVar(name);
  if (name == wxT("index.xpm"))
  {
    wxBitmap bitmap(text_list_bullets_xpm);
    return bitmap;
  }
  else if (name == wxT("arrow_left.xpm"))
  {
    wxBitmap bitmap(arrow_left_xpm);
    return bitmap;
  }
  else if (name == wxT("arrow_right.xpm"))
  {
    wxBitmap bitmap(arrow_right_xpm);
    return bitmap;
  }
  return wxNullBitmap;
////@end HelpContents bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon HelpContents::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin HelpContents icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end HelpContents icon retrieval
}


std::string HelpContents::getCurrentHelpContentsFullPath()
{
  std::string fullPath = GetHelpContentsRootStr();
  fullPath += wxString( wxFileName::GetPathSeparator() ).mb_str();
  fullPath += currentHelpContentsDir.mb_str();
  fullPath += wxString( wxFileName::GetPathSeparator() ).mb_str();

  return fullPath;
}


std::string HelpContents::getHrefFullPath( wxHtmlLinkEvent &event )
{
  std::string hrefFullPath = getCurrentHelpContentsFullPath();
  hrefFullPath += std::string( event.GetLinkInfo().GetHref().mb_str() );

  return hrefFullPath;
}


bool HelpContents::matchHrefExtension( wxHtmlLinkEvent &event, const wxString extension )
{
  return ( event.GetLinkInfo().GetHref().Right( extension.Len() ).Cmp( extension ) == 0 );
}


// Path or file exist? If so, change TutorialsRoot and load it
bool HelpContents::SetHelpContents( const wxString& whichPath )
{
  bool htmlFound = false;

  wxFileName candidate( whichPath );

  if ( ( candidate.GetExt().Cmp( _("html") ) == 0 ) ||
       ( candidate.GetExt().Cmp( _("htm") ) == 0  ) ||
       ( candidate.GetExt().Cmp( _("HTML") ) == 0 ) ||
       ( candidate.GetExt().Cmp( _("HTM") ) == 0 ) )
  {
    if ( candidate.FileExists() )
    {
      SetHelpContentsRoot( candidate.GetPathWithSep() );
      htmlWindow->LoadPage( whichPath );
      htmlFound = true;
    }
  }
  else if ( candidate.DirExists() && candidate.IsDirReadable() )
  {
    wxString tmpTutorial = getHtmlIndex( candidate.GetPathWithSep() );
    if ( !tmpTutorial.IsEmpty() )
    {
      htmlWindow->LoadPage( tmpTutorial );
      htmlFound = true;
    }
  }
  
  return htmlFound;
}


bool HelpContents::SetHelpContentsRoot( const wxString& whichRoot )
{
  bool changedRoot = false;
  
  if ( wxFileName::IsDirReadable( whichRoot ) )
  {
    helpContentsRoot = whichRoot;
    changedRoot = true;
  }
    
  return changedRoot;
}


bool HelpContents::SetHelpContentsRoot( const std::string& whichRoot )
{
  return SetHelpContentsRoot( wxString::FromAscii( whichRoot.c_str() ) );
}


const wxString HelpContents::GetHelpContentsRoot()
{
  return helpContentsRoot;
}


const std::string HelpContents::GetHelpContentsRootStr()
{
  return std::string( GetHelpContentsRoot().mb_str() );
}


// TODO: CHANGE COMPARISON
bool HelpContents::DetectHelpContentsIndexInPath( const wxString& whichPath )
{
  bool indexFound = false;

  // Idea to detect tutorial:
  //   /home/user/root-tutorials/ => dir depth tutorials = 3
  //   vs.
  //   /home/user/root-tutorials/tutorial1/index.html => dir depth current = 4
  //   /home/user/root-tutorials/tutorial1/html/.../anyotherpage.html => dir depth current > 4
  wxString anyHelpContentsPath = GetHelpContentsRoot();
  if ( anyHelpContentsPath[ anyHelpContentsPath.Len() - 1 ] != wxString( wxFileName::GetPathSeparator() ))
  {
    // last separator needed to count properly
    anyHelpContentsPath += wxString( wxFileName::GetPathSeparator() );
  }

  wxFileName anyHelpContentsDir( anyHelpContentsPath );

  size_t dirsDepthHelpContents = anyHelpContentsDir.GetDirCount();

  wxFileName currentLink( whichPath );
  size_t dirsDepthCurrentLink = currentLink.GetDirCount();

  if (( dirsDepthCurrentLink == dirsDepthHelpContents + 1 ) && // innocent comparison!
                                                             // path may differ?
      ( currentLink.GetFullName().Cmp( _("index.html") ) == 0 ))
  {
    wxArrayString dirs = currentLink.GetDirs();
    // From /home/user/root-tutorials/tutorial1/index.html GetDirs returns:
    //   home
    //   user
    //   root-tutorials
    //   tutorial1 --> currentHelpContentsDir
    currentHelpContentsDir = dirs[ dirsDepthCurrentLink - 1 ];
    indexFound = true;
  }
  
  return indexFound;
}

/*!
 * wxEVT_COMMAND_HTML_LINK_CLICKED event handler for ID_HTMLWINDOW
 */

void HelpContents::OnHtmlwindowLinkClicked( wxHtmlLinkEvent& event )
{
  // If current clicked link points to a index.html file, keep its
  //    directory name to allow relative references.
  DetectHelpContentsIndexInPath( event.GetLinkInfo().GetHref() );

  // and let the html window browse it.
  event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void HelpContents::OnButtonCloseClick( wxCommandEvent& event )
{
  Close();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_INDEX
 */

void HelpContents::OnButtonIndexClick( wxCommandEvent& event )
{
  buildIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_BACK
 */

void HelpContents::OnBitmapbuttonBackClick( wxCommandEvent& event )
{
  htmlWindow->HistoryBack();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_BITMAPBUTTON_BACK
 */

void HelpContents::OnBitmapbuttonBackUpdate( wxUpdateUIEvent& event )
{
  buttonHistoryBack->Enable( htmlWindow->HistoryCanBack() );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_FORWARD
 */

void HelpContents::OnBitmapbuttonForwardClick( wxCommandEvent& event )
{
  htmlWindow->HistoryForward();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_BITMAPBUTTON_FORWARD
 */

void HelpContents::OnBitmapbuttonForwardUpdate( wxUpdateUIEvent& event )
{
  buttonHistoryForward->Enable( htmlWindow->HistoryCanForward() );
}


/*
  ======================
  Class TutorialsBrowser
  ======================
*/
TutorialsBrowser::TutorialsBrowser( wxWindow* parent,
                                    const wxString& whichHelpContentsRoot,
                                    wxWindowID id,
                                    const wxString& caption,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style)
{
  helpContentsRoot = whichHelpContentsRoot;
  Init();
  Create(parent, id, caption, pos, size, style);
}


TutorialsBrowser::~TutorialsBrowser()
{
}


const wxString TutorialsBrowser::getTitle( int numTutorial, const wxString& path )
{
  wxString helpContentsTitle;

  wxHtmlWindow auxHtml( this );
  auxHtml.LoadPage( path  + wxFileName::GetPathSeparator() + _("index.html") );
  helpContentsTitle = auxHtml.GetOpenedPageTitle();
  if ( helpContentsTitle.empty() || helpContentsTitle == _("index.html") ) // never is empty !?!
  {
    // Deprecated feature: if "tutorial_title" exists in directory, takes it
    std::string auxStrTitleFileName(
            ( path + wxFileName::GetPathSeparator() + _("tutorial_title") ).mb_str() );
    std::string auxLine;

    std::ifstream titleFile;
    titleFile.open( auxStrTitleFileName.c_str() );
    if ( titleFile.good() )
    {
      std::getline( titleFile, auxLine );
    
      if ( auxLine.size() > 0 )
      {
        helpContentsTitle = wxString::FromAscii( auxLine.c_str() );
      }
      else
      {
        helpContentsTitle = _("Tutorial ");
        helpContentsTitle << numTutorial;
      }
    }
    else
    {
      helpContentsTitle = _("Tutorial ");
      helpContentsTitle << numTutorial;
    }

    titleFile.close();
  }

  return helpContentsTitle;
}



void TutorialsBrowser::htmlMessage( wxString& htmlDoc )
{
  htmlDoc += _("<P><H3>No tutorial found!?</H3></P>");
/*
  htmlDoc += _("<P>Before going on, please have in mind that:</P>");
  htmlDoc += _("<UL>");
  htmlDoc += _("<LI>A single <B>tutorials root directory</B> must contain all ");
  htmlDoc += _("the <B>tutorials directories</B> that you want <B>visible</B> for wxparaver.</LI>");
  htmlDoc += _("<LI>In their own directories, every tutorial has a <B>first page</B> called <TT>index.html</TT>, and also related content (like traces, cfgs, etc.).</LI>");
  htmlDoc += _("<LI>You <B>don't</B> need to <B>write</B> any global tutorials page; we walk through the included tutorials ");
  htmlDoc += _("in the given root directory and build it for you.</LI>");
  htmlDoc += _("<LI>The tutorial title showed in this automatically built main index is read from:</LI>");
  htmlDoc += _("<OL type=\"1\">");
  htmlDoc += _("<LI>The <TT>TITLE</TT> tag in the<TT>index.html</TT> file.</LI>");
  htmlDoc += _("<LI>If this tag is missing or empty, from a single line file named <TT>tutorial_title</TT>");
  htmlDoc += _(", also local to this tutorial.</LI>");
  htmlDoc += _("<LI>If no <TT>tutorial_title</TT> file is found, we give a numbered 'Tutorial #'.</LI>");
  htmlDoc += _("</OL>");
  htmlDoc += _("</UL>");
*/
  htmlDoc += _("<P>Please check that <B>root directory</B> to tutorials is properly defined:</P>");
  htmlDoc += _("<OL type=\"1\">");
  htmlDoc += _("<LI>Open <A HREF=\"init_preferences\"><I>Preferences Window</I></A>.</LI>");
  htmlDoc += _("<LI>Select <I>Global</I> tab.</LI>");
  htmlDoc += _("<LI>In the <I>Default directories</I> box, change the <I>Tutorials root</I> directory");
  htmlDoc += _("<LI>Save your new settings clicking the <I>Ok</I> button in the <I>Preferences Window</I>.</LI>");
  htmlDoc += _("<LI>After that, we automatically refresh the tutorials list.</LI>");
  htmlDoc += _("<LI>If nothing happens, come back here and press the button <I>Index</I> to rebuild the tutorials list.");
  htmlDoc += _("</OL>");
  htmlDoc += _("<P>If the button <I>Index</I> doesn't seem to work (you're still reading this help!), please verify that:</P>");
  htmlDoc += _("<UL>");
  htmlDoc += _("<LI>Every tutorial is <B>uncompressed</B>.</LI>");
  htmlDoc += _("<LI>Every tutorial is inside its own <B>subdirectory</B>.</LI>");
  htmlDoc += _("<LI>These subdirectories (or tutorials) are copied/linked into the root directory that ");
  htmlDoc += _("you selected before (i.e: /home/myuser/mytutorials/tut1/, /home/myuser/mytutorials/tut2/, etc).</LI>");
  htmlDoc += _("<LI>Every tutorial has a main <B>index.html</B> (i.e: /home/myuser/mytutorials/tut1/index.html ).</LI>");
  htmlDoc += _("</UL>");
  htmlDoc += _("<P>If you still get this help after checking these steps again, please contact us at paraver@bsc.es.</P>");
}

/*!
 * wxEVT_COMMAND_HTML_LINK_CLICKED event handler for ID_HTMLWINDOW
 */

void TutorialsBrowser::OnHtmlwindowLinkClicked( wxHtmlLinkEvent& event )
{
  wxString auxCommand;

  if ( event.GetLinkInfo().GetHref().StartsWith( _("init_command:"), &auxCommand ) )
  {
  /*
    wxString app("");
    wxString trace("");
    wxString command( auxCommand );
    bool runNow = false;
    paraverMain::myParaverMain->ShowRunCommand( app, trace, command, runNow );
    */
  }
  else if ( event.GetLinkInfo().GetHref().Cmp( _("init_preferences") ) == 0 )
  {
    std::string oldTutorialsPath = GetHelpContentsRootStr();

    paraverMain::myParaverMain->ShowPreferences();

    std::string newTutorialsPath =
            paraverMain::myParaverMain->GetParaverConfig()->getGlobalTutorialsPath();
 
    SetHelpContentsRoot( newTutorialsPath );
    
    if ( newTutorialsPath.compare( oldTutorialsPath ) != 0 )
    {
      // We rebuild the index
      buildIndex();
    }
  }
  else if ( matchHrefExtension( event, _(".prv") ) ||
            matchHrefExtension( event, _(".prv.gz")))
  {
    paraverMain::myParaverMain->DoLoadTrace( getHrefFullPath( event ) );
  }
  else if ( matchHrefExtension( event, _(".cfg")))
  {
    if ( paraverMain::myParaverMain->GetLoadedTraces().size() > 0 )
    {
      paraverMain::myParaverMain->DoLoadCFG( getHrefFullPath( event )  );
    }
    else
    {
      wxMessageDialog message( this, _("No trace loaded."), _( "Warning" ), wxOK );
      message.ShowModal();
    }
  }
  else if ( matchHrefExtension( event, _(".xml")))
  {
    std::string traceName;
    if ( paraverMain::myParaverMain->GetLoadedTraces().size() > 0 )
    {
      traceName = paraverMain::myParaverMain->GetLoadedTraces().front()->getFileName();
    }
    else
    {
      traceName = getCurrentHelpContentsFullPath();
    }
    
    bool loadTrace = true;
    std::string strXmlFile = getHrefFullPath( event );
    
    paraverMain::myParaverMain->ShowCutTraceWindow( traceName, loadTrace, strXmlFile );
  }
  else
  {
    // If current clicked link points to a tutorial index.html file, keep its
    //   tutorial directory name to allow relative references.
    DetectHelpContentsIndexInPath( event.GetLinkInfo().GetHref() );

    // and let the html window browse it.
    event.Skip();
  }
}


void TutorialsBrowser::buildIndex()
{
  buildIndexTemplate( wxString( wxT( "Tutorials" ) ), wxString( wxT( "tutorials" ) ) );
}
