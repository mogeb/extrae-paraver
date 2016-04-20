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
 | @last_commit: $Date: 2016/01/28 15:53:35 $
 | @version:     $Revision: 1.66 $
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

#include "labelconstructor.h"

// Signal handling
#include <signal.h>
#include <stdio.h>

#include <wx/filefn.h>

#ifdef __WXMAC__
#include <wx/sysopt.h>
#endif

#include "config.h"
#include "wxparaverapp.h"
#include "connection.h"
#include <wx/filename.h>
#include "sessionsaver.h"
#include "helpcontents.h"
#include "gtimeline.h"
#include "ghistogram.h"

////@begin XPM images
////@end XPM images

#ifndef WIN32
struct sigaction act;
#endif

/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( wxparaverApp )
////@end implement app


/*!
 * wxparaverApp type definition
 */

IMPLEMENT_CLASS( wxparaverApp, wxApp )


/*!
 * wxparaverApp event table definition
 */

BEGIN_EVENT_TABLE( wxparaverApp, wxApp )

////@begin wxparaverApp event table entries
////@end wxparaverApp event table entries

END_EVENT_TABLE()

#if wxMAJOR_VERSION<3
wxCmdLineEntryDesc wxparaverApp::argumentsParseSyntax[] =
{
  { wxCMD_LINE_SWITCH, 
    wxT("v"),
    wxT("version"),
    wxT("Show wxparaver version.") },

  { wxCMD_LINE_SWITCH, 
    wxT("h"),
    wxT("help"),
    wxT("Show this help."),
    wxCMD_LINE_VAL_NONE,
    wxCMD_LINE_OPTION_HELP },

  { wxCMD_LINE_SWITCH, 
    wxT("i"),
    wxT("image"),
    wxT("Save cfg last window as an image. Once done wxparaver will exit."),
    wxCMD_LINE_VAL_NONE,
    wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, 
    wxT("e"),
    wxT("event"),
    wxT("Event type to code linking."),
    wxCMD_LINE_VAL_NUMBER,
    wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, 
    wxT("t"),
    wxT("tutorial"),
    wxT("Load tutorial. <str> can be the path to the tutorial "
        "containing the index.html file, or the whole url, like "
        "path/file.html (then, other names than 'index' are allowed)."),
    wxCMD_LINE_VAL_STRING,
    wxCMD_LINE_PARAM_OPTIONAL },

   { wxCMD_LINE_PARAM, 
    NULL,
    NULL,
    wxT( "(trace.prv | trace.prv.gz) (configuration.cfg)" ),
    wxCMD_LINE_VAL_STRING,
    wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },

  { wxCMD_LINE_NONE }
};
#else
wxCmdLineEntryDesc wxparaverApp::argumentsParseSyntax[] =
{
  { wxCMD_LINE_SWITCH, 
    "v",
    "version",
    "Show wxparaver version." },

  { wxCMD_LINE_SWITCH, 
    "h",
    "help",
    "Show this help.",
    wxCMD_LINE_VAL_NONE,
    wxCMD_LINE_OPTION_HELP },

  { wxCMD_LINE_SWITCH, 
    "i",
    "image",
    "Save cfg last window as an image; if it's a timeline, also saves its legend",
    wxCMD_LINE_VAL_NONE,
    wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, 
    "e",
    "event",
    "Event type to code linking.",
    wxCMD_LINE_VAL_NUMBER,
    wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, 
    "t",
    "tutorial",
    "Load tutorial. <str> can be the path to the tutorial "
      "containing the index.html file, or the whole url, like "
      "path/file.html (then, other names than 'index' are allowed).",
    wxCMD_LINE_VAL_STRING,
    wxCMD_LINE_PARAM_OPTIONAL },

   { wxCMD_LINE_PARAM, 
    NULL,
    NULL,
    "(trace.prv | trace.prv.gz) (configuration.cfg)",
    wxCMD_LINE_VAL_STRING,
    wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },

  { wxCMD_LINE_NONE }
};
#endif

/*!
 * Constructor for wxparaverApp
 */

wxparaverApp::wxparaverApp()
{
  wxSetEnv(wxT("UBUNTU_MENUPROXY"), wxT("0"));
  Init();
}


/*!
 * Member initialisation
 */

void wxparaverApp::Init()
{
////@begin wxparaverApp member initialisation
	eventTypeForCode = 60000119;
	globalTiming = false;
	globalTimingBegin = 0;
	globalTimingBeginIsSet = false;
	globalTimingCallDialog = NULL;
	globalTimingEnd = 0;
////@end wxparaverApp member initialisation
  m_locale.Init();
#ifdef __WXMAC__
  wxSystemOptions::SetOption( "mac.toolbar.no-native", 1 );
#endif
}

/*!
 * Initialisation for wxparaverApp
 */

paraverMain* wxparaverApp::mainWindow = NULL;


#ifndef WIN32
volatile bool sig1 = false;
volatile bool sig2 = false;

void wxparaverApp::handler( int signalNumber )
{
  sigdelset( &act.sa_mask, SIGUSR1 );
  sigdelset( &act.sa_mask, SIGUSR2 );

//  wxMutexGuiEnter();
//  wxparaverApp::mainWindow->OnSignal( signalNumber );
//  wxMutexGuiLeave();
  if ( signalNumber == SIGUSR1 )
  {
    sig1 = true;
  }
  else
  {
    sig1 = false;
  }

  sig2 = !sig1;

//  wxIdleEvent event;
//  event.RequestMore();
//  wxTheApp->SendIdleEvents( mainWindow, event );
  mainWindow->Raise();

  sigaddset( &act.sa_mask, SIGUSR1 );
  sigaddset( &act.sa_mask, SIGUSR2 );
  sigaction( SIGUSR1, &act, NULL );
  sigaction( SIGUSR2, &act, NULL );
}


void wxparaverApp::presetUserSignals()
{

  act.sa_handler = &handler;
  act.sa_flags = 0;

  if ( sigemptyset( &act.sa_mask ) != 0 )
  {
    /* Handle error */
  }

  if ( sigaddset( &act.sa_mask, SIGUSR1 ))
  {
    /* Handle error */
  }

  if ( sigaddset( &act.sa_mask, SIGUSR2 ))
  {
    /* Handle error */
  }

  if ( sigaction( SIGUSR1, &act, NULL ) != 0 )
  {
    /* Handle error */
  }

  if ( sigaction( SIGUSR2, &act, NULL ) != 0 )
  {
    /* Handle error */
  }
}
#endif


bool wxparaverApp::OnInit()
{
#ifdef TRACING_ENABLED
  Extrae_init();
#endif
  wxCmdLineParser paraverCommandLineParser( argumentsParseSyntax, argc, argv );
  if ( paraverCommandLineParser.Parse( false ) != 0 )
  {
    std::cout << std::endl;
    paraverCommandLineParser.Usage();
    
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "  Environment variables defined by wxParaver:" << std::endl;
    std::cout << "  -------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "    $PARAVER_LOADED                Once started, it's filled with wxParaver PID" << std::endl;
    std::cout << "    $PARAVER_ALIEN_TRACE_FULL_NAME Contains the absolute path to the current loaded trace." << std::endl;
    std::cout << "                                   Published only:" << std::endl;
    std::cout << "                                     * when middle click is done in a timeline and" << std::endl;
    std::cout << "                                     * if PARAVER_ALIEN* vars are defined (see below)." << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "  Environment variables defined by user (optional):" << std::endl;
    std::cout << "  -------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "    To set alien application invoked from timeline with mouse middle button (both vars needed):"  << std::endl;
    std::cout << "      $PARAVER_ALIEN_APP  Application that will be invoked." << std::endl;
    std::cout << "      $PARAVER_ALIEN_TYPE Event type passed as parameter" << std::endl;
    std::cout << "                          Starting from the click, it's sought backwards" << std::endl;
    std::cout << "                          The value passed is 1) its symbolic translation or 2) its value" << std::endl;
    std::cout << std::endl;
    std::cout << "        Example: Create empty file named with MPI callers at level 1" << std::endl;
    std::cout << "          > PARAVER_ALIEN_APP=touch PARAVER_ALIEN_TYPE=70000001 wxparaver &" << std::endl;
    std::cout << std::endl;
    std::cout << "        Example: Writes in a file the translation of the decimal PAPI_TOT_INST counter value into morse" << std::endl;
    std::cout << "          > PARAVER_ALIEN_APP='> in_morse.txt morse -s' PARAVER_ALIEN_TYPE=42000050 wxparaver &" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    
    /*
    std::cout << "        Example: Search line of a user function label in .pcf file and write it to a file" << std::endl;
    std::cout << "          > export PARAVER_ALIEN_APP='> label.txt xargs -I {} grep -n {} trace.pcf <<<'" << std::endl;
    std::cout << "          > export PARAVER_ALIEN_TYPE=70000001" << std::endl;
    std::cout << "          > wxparaver trace.prv &" << std::endl;
    std::cout << std::endl;
    */
    return false; 
  }

  if( paraverCommandLineParser.Found( wxT("v") ))
  {
    PrintVersion();
    return false;
  }

  try
  {
    ParaverConfig::getInstance()->readParaverConfigFile();
  }
  catch( ... )
  {
    wxMessageBox( wxT( "Error while loading preferences. Using default options." ),
                  wxT( "Preferences error" ),
                  wxOK|wxICON_ERROR );
  }
  
  if( ParaverConfig::getInstance()->getGlobalSingleInstance() )
  {
    const wxString name = wxString::Format( _( "wxparaver-%s" ), wxGetUserId().c_str());
    wxLogNull *tmpLogNull = new wxLogNull();
    m_checker = new wxSingleInstanceChecker(name);
    delete tmpLogNull;
    if ( !m_checker->IsAnotherRunning() )
    {
      m_server = new stServer;
    
#ifdef WIN32
      if( !m_server->Create( wxT( "wxparaver_service" ) ) )
#else
      const wxString service_name = wxString::Format( _( "/tmp/wxparaver_service-%s" ), wxGetUserId().c_str());
      if( !m_server->Create( service_name ) )
#endif
        wxLogDebug( wxT( "Failed to create an IPC service." ) );
    }
    else
    {
      wxLogNull logNull;
    
      stClient *client = new stClient;
      wxString hostName = wxT( "localhost" );
#ifdef WIN32
      wxConnectionBase *connection = client->MakeConnection( hostName, 
                                                             wxT( "wxparaver_service" ),
                                                             wxT( "wxparaver" ) );
#else
      const wxString service_name = wxString::Format( _( "/tmp/wxparaver_service-%s" ), wxGetUserId().c_str());
      wxConnectionBase *connection = client->MakeConnection( hostName, 
                                                             service_name,
                                                             wxT( "wxparaver" ) );
#endif
      if( connection )
      {
        connection->Execute( wxT( "BEGIN" ) );
        connection->Execute( argv[ 0 ] );
        for( int i = 1; i < argc; ++i )
        {
          if ( argv[ i ][ 0 ] == '-' )
          {
            connection->Execute( argv[ i ] );
          }
          else
          {
            if ( wxFileName::FileExists( argv[ i ] ) ) 
            {
              wxFileName tmpFile( argv[ i ] );
              tmpFile.Normalize();
              connection->Execute( tmpFile.GetFullPath().c_str() );
            }
            else
            {
              connection->Execute( argv[ i ] );
            }
          }          
        }
        connection->Execute( wxT( "END" ) );
        connection->Disconnect();
        delete connection;
      }
      else
      {
        wxMessageBox( wxT( "Sorry, the existing instance may be too busy to respond." ),
                      wxT( "wxparaver" ), wxICON_INFORMATION|wxOK );
      }
    
      delete client;
      return false;
    }
  }

#if wxUSE_XPM
  wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
  wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
  wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
  wxImage::AddHandler(new wxGIFHandler);
#endif

  wxSize mainWindowSize( ParaverConfig::getInstance()->getMainWindowWidth(),
                         ParaverConfig::getInstance()->getMainWindowHeight() );
                         
  mainWindow = new paraverMain( NULL, SYMBOL_PARAVERMAIN_IDNAME, SYMBOL_PARAVERMAIN_TITLE, SYMBOL_PARAVERMAIN_POSITION, mainWindowSize );

  mainWindow->Show(true);

  ParseCommandLine( paraverCommandLineParser );

#ifndef WIN32
  presetUserSignals();
#endif

  return true;
}


void wxparaverApp::ParseCommandLine( wxCmdLineParser& paraverCommandLineParser )  
{
  if ( paraverCommandLineParser.Found( wxT( "i" ) ) )
  {
    string fileName;
    Trace *currentTrace = NULL;
    for ( unsigned int i = 0; i < paraverCommandLineParser.GetParamCount(); ++i )
    {
      fileName = paraverCommandLineParser.GetParam( i ).mb_str();
      
      if ( Trace::isTraceFile( fileName ) )
      {
        if( currentTrace != NULL )
          delete currentTrace;

        wxFileName tmpFileName( wxString( fileName.c_str(), wxConvUTF8 ) );
          
#ifdef WIN32
        tmpFileName.Normalize( wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE |
                               wxPATH_NORM_LONG );
#else
        tmpFileName.Normalize( wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE |
                               wxPATH_NORM_LONG | wxPATH_NORM_TILDE );
#endif

        string tmpPath = std::string( tmpFileName.GetFullPath().mb_str() );
      
        currentTrace = Trace::create( mainWindow->GetLocalKernel(), tmpPath, false, NULL );
      }
      else if ( CFGLoader::isCFGFile( fileName ) && currentTrace != NULL )
      {
        vector<Window *> newWindows;
        vector<Histogram *> newHistograms;
        SaveOptions options;

        if( CFGLoader::loadCFG( mainWindow->GetLocalKernel(), fileName, currentTrace,
                                newWindows, newHistograms, options ) )
        {
          if( !newHistograms.empty() )
          {
            // save histogram image
            Histogram *histo = newHistograms.back();
            histo->setRedraw( false );
            string composedName = histo->getName() + " @ " +
                                  histo->getControlWindow()->getTrace()->getTraceName();
                                  
            gHistogram* tmpGHisto = new gHistogram( mainWindow, wxID_ANY, wxString::FromAscii( composedName.c_str() ) );
            tmpGHisto->SetHistogram( histo );

            histo->setZoom( true );
            histo->setRecalc( false );
            tmpGHisto->execute();

            tmpGHisto->saveImage( false );
            
            delete tmpGHisto;
            newHistograms.pop_back();
          }
          else
          {
            // save timeline image
            Window *window = newWindows.back();
            window->setRedraw( false );
            string composedName = window->getName() + " @ " +
                                  window->getTrace()->getTraceName();

            wxPoint tmpPos( window->getPosX(), window->getPosY() );
            gTimeline* tmpTimeline = new gTimeline( mainWindow, wxID_ANY, wxString::FromAscii( composedName.c_str() ), tmpPos );
            tmpTimeline->SetMyWindow( window );
            tmpTimeline->SetClientSize( wxSize( window->getWidth(), window->getHeight() ) );
            
            tmpTimeline->redraw();
            tmpTimeline->saveImage( false );
            tmpTimeline->saveImageLegend( false );
            
            delete tmpTimeline;
            newWindows.pop_back();
          }

          for( vector<Histogram *>::iterator it = newHistograms.begin();
               it != newHistograms.end(); ++it )
            delete (*it);
          newHistograms.clear();

          for( vector<Window *>::iterator it = newWindows.begin();
               it != newWindows.end(); ++it )
            delete (*it);
          newWindows.clear();
        }
      }

      fileName.erase();
    }
    mainWindow->Close();
    return;
  }

  long int tmpType;
  if( paraverCommandLineParser.Found( wxT( "e" ), &tmpType ) )
    eventTypeForCode = tmpType;

  wxString tmpTutorial;    
  if ( paraverCommandLineParser.Found( wxT( "t" ), &tmpTutorial ) )
  {
    if ( mainWindow->GetTutorialsWindow() == NULL )
      mainWindow->SetTutorialsWindow( 
              new TutorialsBrowser( mainWindow,
                                    wxString( paraverMain::myParaverMain->GetParaverConfig()->getGlobalTutorialsPath().c_str(), wxConvUTF8 ),
                                    wxID_ANY,
                                    _( "Tutorials" ) ) );
 
    if ( !mainWindow->GetTutorialsWindow()->SetHelpContents( tmpTutorial ) )
      wxMessageBox( wxT( "Unable to find index.html in tutorial dir.\n\n"
                          "Please check whole path or create/place index.html file." ),
                    wxT( "Tutorial" ), wxICON_INFORMATION|wxOK );
    else
      mainWindow->GetTutorialsWindow()->Show( true );
  }
  
  mainWindow->commandLineLoadings( paraverCommandLineParser );
}


/*!
 * Cleanup for wxparaverApp
 */

int wxparaverApp::OnExit()
{
//  double w, h;
//  wxparaverApp::mainWindow->GetAuiManager().GetDockSizeConstraint( &w, &h );
//  cout<<w<<" "<<h<<endl;
//  cout << wxparaverApp::mainWindow->GetAuiManager().SavePaneInfo(
//            wxparaverApp::mainWindow->GetAuiManager().GetPane( wxparaverApp::mainWindow->choiceWindowBrowser ) ).mb_str()<<endl;
  ParaverConfig::getInstance()->writeParaverConfigFile();
  
  if( m_checker != NULL )
    delete m_checker;
    
  if( m_server != NULL )
    delete m_server;

#ifdef TRACING_ENABLED
  Extrae_fini();
#endif

////@begin wxparaverApp cleanup
	return wxApp::OnExit();
////@end wxparaverApp cleanup
}

int wxparaverApp::FilterEvent(wxEvent& event)
{
#ifdef WIN32
  if ( event.GetEventType()==wxEVT_KEY_DOWN && 
       ((wxKeyEvent&)event).ControlDown() &&
       ((wxKeyEvent&)event).GetKeyCode() == (long) 'C'
     )
  {
    mainWindow->OnKeyCopy();
    return true;
  }
  if ( event.GetEventType()==wxEVT_KEY_DOWN && 
       ((wxKeyEvent&)event).ControlDown() &&
       ((wxKeyEvent&)event).GetKeyCode() == (long) 'V'
     )
  {
    mainWindow->OnKeyPaste();
    return true;
  }
#endif
  if ( event.GetEventType()==wxEVT_KEY_DOWN && 
       ((wxKeyEvent&)event).ControlDown() &&
       ((wxKeyEvent&)event).GetKeyCode() == (long) 'S'
     )
  {
    wxFileDialog dialog( mainWindow, wxT( "Save session" ), _(""), _(""), _("*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    if( dialog.ShowModal() == wxID_OK )
    {
      SessionSaver::SaveSession( dialog.GetPath(), wxparaverApp::mainWindow->GetLoadedTraces() );
    }
    return true;
  }

  if ( event.GetEventType()==wxEVT_KEY_DOWN && 
       ((wxKeyEvent&)event).ControlDown() &&
       ((wxKeyEvent&)event).GetKeyCode() == (long) 'L'
     )
  {
    wxFileDialog dialog( mainWindow, wxT( "Load session" ), _(""), _(""), _("*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST );
    if( dialog.ShowModal() == wxID_OK )
    {
      SessionSaver::LoadSession( dialog.GetPath() );
    }
    return true;
  }
  if ( event.GetEventType()==wxEVT_KEY_DOWN && 
       ((wxKeyEvent&)event).ControlDown() &&
       ((wxKeyEvent&)event).GetKeyCode() == (long) 'F'
     )
  {
    mainWindow->OnFindDialog();
    return true;
  }
  return -1;
}

void wxparaverApp::ActivateGlobalTiming( wxDialog* whichDialog )
{
  globalTimingCallDialog = whichDialog;
  wxSetCursor( *wxCROSS_CURSOR );
  globalTiming = true;
  globalTimingBeginIsSet = false;
#ifndef __WXMAC__
  globalTimingCallDialog->Enable( false );
#endif

#if wxMAJOR_VERSION<3
  globalTimingCallDialog->MakeModal( false );
#endif

#ifdef WIN32
  globalTimingCallDialog->Iconize( true );
#endif
#ifdef __WXMAC__
  globalTimingCallDialog->Lower();
#endif
  mainWindow->Raise();
}

void wxparaverApp::DeactivateGlobalTiming()
{
  wxSetCursor( wxNullCursor );
  globalTiming = false;
  globalTimingBeginIsSet = false;
  globalTimingCallDialog->Enable( true );

#if wxMAJOR_VERSION<3
  globalTimingCallDialog->MakeModal( true );
#endif

#ifdef WIN32
  globalTimingCallDialog->Iconize( false );
#endif
  globalTimingCallDialog->Raise();
}


void wxparaverApp::PrintVersion()
{
  cout << PACKAGE_STRING;

  bool reverseOrder = true;
  string auxDate = LabelConstructor::getDate( reverseOrder );

  if ( auxDate.compare("") != 0 )
    cout << " Build ";

  cout << auxDate << endl;
}

