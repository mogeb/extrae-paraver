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
 | @last_commit: $Date: 2016/03/10 10:03:39 $
 | @version:     $Revision: 1.126 $
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
#include "wx/imaglist.h"
////@end includes

#ifdef WIN32
#include <process.h>
#endif

#include <wx/utils.h> // wxGetEnv
#include <wx/txtstrm.h>
#include <wx/filefn.h> // wxPathList

// Validators
#include <wx/arrstr.h>
#include <wx/validate.h>

#include <vector>
#include <algorithm>

#include "wxparaverapp.h" // paraverMain
#include "runscript.h"
#include "filter.h"
#include "cfg.h"


////@begin XPM images
#include "app_edit.xpm"
////@end XPM images

BEGIN_EVENT_TABLE( RunningProcess, wxProcess )
  EVT_TIMER( ID_TIMER_MESSAGE, RunningProcess::OnTimerMessage )
END_EVENT_TABLE()

wxString RunScript::clusteringXML = wxString( wxT("") );

void RunningProcess::OnTerminate( int pid, int status )
{
  while ( HasInput() )
    ;

  //parent->executionStatus = status;
  parent->OnProcessTerminated( pid );
}


bool RunningProcess::HasInput()
{
  bool hasInput = false;

  if ( IsInputAvailable() )
  {
      wxTextInputStream tis( *GetInputStream() );

//      wxString msg;
//      msg << tis.ReadLine();
//      parent->AppendToLog( msg );
      wxChar tmpC;
      tmpC = tis.GetChar();
      if( tmpC == '\n' )
      {
        parent->AppendToLog( outMsg );
        outMsg.Clear();
      }
      else
      {
        outMsg << tmpC;
        msgTimer.Start( 500 );
      }

      hasInput = true;
  }

  if ( IsErrorAvailable() )
  {
      wxTextInputStream tis( *GetErrorStream() );

//      wxString msg;
//      msg << tis.ReadLine();
//      parent->AppendToLog( msg );
      wxChar tmpC;
      tmpC = tis.GetChar();
      errMsg << tmpC;
      if( errMsg[ errMsg.Len() - 1 ] == '\n' )
      {
        parent->AppendToLog( errMsg );
        errMsg.Clear();
      }

      hasInput = true;
  }

  return hasInput;
}

void RunningProcess::OnTimerMessage( wxTimerEvent& event )
{
  if( outMsg.Len() > 0 )
  {
    parent->AppendToLog( outMsg, false );
    parent->Update();
    outMsg.Clear();
  }
  wxWakeUpIdle();
}

/*!
 * RunScript type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RunScript, wxDialog )


/*!
 * RunScript event table definition
 */

BEGIN_EVENT_TABLE( RunScript, wxDialog )

////@begin RunScript event table entries
  EVT_IDLE( RunScript::OnIdle )
  EVT_CHOICE( ID_CHOICE_APPLICATION, RunScript::OnChoiceApplicationSelected )
  EVT_TEXT( ID_TEXTCTRL_TRACE, RunScript::OnTextctrlTraceTextUpdated )
  EVT_BUTTON( ID_BUTTON_DIMEMAS_GUI, RunScript::OnButtonDimemasGuiClick )
  EVT_UPDATE_UI( ID_BUTTON_DIMEMAS_GUI, RunScript::OnButtonDimemasGuiUpdate )
  EVT_BUTTON( ID_BITMAPBUTTON_CLUSTERING_XML, RunScript::OnBitmapbuttonClusteringXmlClick )
  EVT_UPDATE_UI( ID_BITMAPBUTTON_CLUSTERING_XML, RunScript::OnBitmapbuttonClusteringXmlUpdate )
  EVT_UPDATE_UI( ID_CHECKBOX_CLUSTERING_SEMVAL_AS_CLUSTDIMENSION, RunScript::OnCheckboxClusteringSemvalAsClustdimensionUpdate )
  EVT_UPDATE_UI( ID_CHECKBOX_CLUSTERING_NORMALIZE, RunScript::OnCheckboxClusteringNormalizeUpdate )
  EVT_UPDATE_UI( ID_CHECKBOX_CLUSTERING_GENERATE_SEQUENCES, RunScript::OnCheckboxClusteringGenerateSequencesUpdate )
  EVT_RADIOBUTTON( ID_RADIOBUTTON_CLUSTERING_XMLDEFINED, RunScript::OnRadiobuttonClusteringXmldefinedSelected )
  EVT_RADIOBUTTON( ID_RADIOBUTTON_CLUSTERING_DBSCAN, RunScript::OnRadiobuttonClusteringDbscanSelected )
  EVT_RADIOBUTTON( ID_RADIOBUTTON_CLUSTERING_REFINEMENT, RunScript::OnRadiobuttonClusteringRefinementSelected )
  EVT_CHECKBOX( ID_CHECKBOX_CLUSTERING_REFINEMENT_TUNE, RunScript::OnCheckboxClusteringRefinementTuneClick )
  EVT_UPDATE_UI( ID_CHECKBOX_FOLDING_USE_SEMANTIC_VALUE, RunScript::OnCheckboxFoldingUseSemanticValueUpdate )
  EVT_UPDATE_UI( wxID_LABELCOMMANDPREVIEW, RunScript::OnLabelcommandpreviewUpdate )
  EVT_BUTTON( ID_BUTTON_RUN, RunScript::OnButtonRunClick )
  EVT_UPDATE_UI( ID_BUTTON_RUN, RunScript::OnButtonRunUpdate )
  EVT_BUTTON( ID_BUTTON_KILL, RunScript::OnButtonKillClick )
  EVT_UPDATE_UI( ID_BUTTON_KILL, RunScript::OnButtonKillUpdate )
  EVT_BUTTON( ID_BUTTON_CLEAR_LOG, RunScript::OnButtonClearLogClick )
  EVT_HTML_LINK_CLICKED( ID_LISTBOX_RUN_LOG, RunScript::OnListboxRunLogLinkClicked )
  EVT_BUTTON( ID_BUTTON_EXIT, RunScript::OnButtonExitClick )
////@end RunScript event table entries

END_EVENT_TABLE()


/*!
 * RunScript constructors
 */

RunScript::RunScript()
{
  Init();
}


RunScript::RunScript( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  Init();
  Create(parent, id, caption, pos, size, style);
}


RunScript::RunScript( wxWindow* parent,
                      wxString whichTrace,
                      wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  Init();
  Create(parent, id, caption, pos, size, style);
  
  if ( !whichTrace.IsEmpty() )
  {
     fileBrowserButtonTrace->SetPath( whichTrace );
  }
  
/*
  if ( !whichCommand.IsEmpty() )
  {
    wxString auxCommand = whichCommand;

    int nextPos = auxCommand.Find( wxT( "%20" ) ) ;
    if ( nextPos != wxNOT_FOUND )
    {
      auxCommand.Replace( wxT( "%20" ), wxT( " " ) );
    }
    
    textCtrlDefaultParameters->SetValue( auxCommand );
  }
*/
  helpOption = false; 
}


/*!
 * RunScript creator
 */

bool RunScript::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RunScript creation
  SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
  wxDialog::Create( parent, id, caption, pos, size, style );

  CreateControls();
  Centre();
////@end RunScript creation

  SetSize( wxSize( 600, 700 ) );

  return true;
}


/*!
 * RunScript destructor
 */

RunScript::~RunScript()
{
////@begin RunScript destruction
////@end RunScript destruction
}


/*!
 * Member initialisation
 */

void RunScript::Init()
{
////@begin RunScript member initialisation
  myProcess = NULL;
  myProcessPid = 0;
  choiceApplication = NULL;
  buttonEditApplication = NULL;
  textCtrlTrace = NULL;
  fileBrowserButtonTrace = NULL;
  boxSizerParameters = NULL;
  labelTextCtrlDefaultParameters = NULL;
  textCtrlDefaultParameters = NULL;
  dimemasSection = NULL;
  labelFilePickerDimemasCFG = NULL;
  textCtrlDimemasCFG = NULL;
  fileBrowserButtonDimemasCFG = NULL;
  buttonDimemasGUI = NULL;
  labelTextCtrlOutputTrace = NULL;
  textCtrlOutputTrace = NULL;
  checkBoxReuseDimemasTrace = NULL;
  checkBoxDontTranslateIdleStates = NULL;
  checkBoxDimemasVerbose = NULL;
  textCtrlDimemasBandwidth = NULL;
  textCtrlDimemasLatency = NULL;
  radioButtonDimemasDefaultTasksMapping = NULL;
  radioButtonDimemasFillNodes = NULL;
  radioButtonDimemasInterleaved = NULL;
  radioButtonDimemasTasksPerNode = NULL;
  spinCtrlDimemasTasksPerNode = NULL;
  statsSection = NULL;
  statsLabelTextCtrlOutputName = NULL;
  statsTextCtrlOutputName = NULL;
  statsCheckBoxShowBurstsHistogram = NULL;
  statsCheckBoxShowCommsHistogram = NULL;
  statsCheckBoxOnlyDatFile = NULL;
  statsCheckBoxExclusiveTimes = NULL;
  clusteringSection = NULL;
  textCtrlClusteringXML = NULL;
  fileBrowserButtonClusteringXML = NULL;
  buttonClusteringXML = NULL;
  checkBoxClusteringUseSemanticWindow = NULL;
  checkBoxClusteringCSVValueAsDimension = NULL;
  checkBoxClusteringNormalize = NULL;
  checkBoxClusteringGenerateSeq = NULL;
  clusteringRadioGenerateSeqNumbered = NULL;
  clusteringRadioGenerateSeqFASTA = NULL;
  clusteringSizerAlgorithm = NULL;
  clusteringRadioXMLDefined = NULL;
  clusteringRadioDBScan = NULL;
  clusteringRadioRefinement = NULL;
  clusteringAlgorithmLineSeparator = NULL;
  clusteringSizerDBScan = NULL;
  clusteringTextBoxDBScanEpsilon = NULL;
  clusteringTextBoxDBScanMinPoints = NULL;
  clusteringSizerRefinement = NULL;
  clusteringCheckBoxRefinementPrintData = NULL;
  clusteringCheckBoxRefinementTune = NULL;
  clusteringLabelRefinementEpsilon = NULL;
  clusteringLabelRefinementEpsilonMin = NULL;
  clusteringTextBoxRefinementEpsilonMin = NULL;
  clusteringLabelRefinementEpsilonMax = NULL;
  clusteringTextBoxRefinementEpsilonMax = NULL;
  clusteringLabelRefinementSteps = NULL;
  clusteringTextBoxRefinementSteps = NULL;
  clusteringLabelRefinementMinPoints = NULL;
  clusteringTextBoxRefinementMinPoints = NULL;
  foldingSection = NULL;
  checkboxFoldingUseSemanticValues = NULL;
  comboboxFoldingModel = NULL;
  labelCommandPreview = NULL;
  buttonHelpScript = NULL;
  buttonRun = NULL;
  buttonKill = NULL;
  buttonClearLog = NULL;
  listboxRunLog = NULL;
  buttonExit = NULL;
////@end RunScript member initialisation

  //executionStatus = -2;

  wxString extensionsAllowed[] = { _(".prv"), _(".prv.gz"),
                                   _(".cfg"),
                                   _(".xml"),
                                   _(".csv"), _(".dat"),
                                   _(".gnuplot") };
  extensions = wxArrayString( (size_t)7, extensionsAllowed );
  extensionsDimemas = extensions;
  extensionsDimemas.Remove(_(".cfg"));

  // Names of environment variables
  environmentVariable[ PATH ]         = wxString( wxT("PATH") );
  environmentVariable[ PARAVER_HOME ] = wxString( wxT("PARAVER_HOME") );
  environmentVariable[ DIMEMAS_HOME ] = wxString( wxT("DIMEMAS_HOME") );
  
  // Labels to construct selector & warning dialogs
  applicationLabel[ DIMEMAS_WRAPPER ]= wxString( wxT("Dimemas") );                          
  applicationLabel[ STATS_WRAPPER ]  = wxString( wxT("Stats") );                             
  applicationLabel[ CLUSTERING ]     = wxString( wxT("Clustering") );                             
  applicationLabel[ FOLDING ]        = wxString( wxT("Folding") );                             
  applicationLabel[ USER_DEFINED ]   = wxString( wxT("User defined") );
  // Following only for warning dialogs
  applicationLabel[ DIMEMAS_GUI ]    = wxString( wxT("DimemasGUI") );
  applicationLabel[ STATS ]          = wxString( wxT("Stats") );

  // application names
  application[ DIMEMAS_WRAPPER ]     = wxString( wxT("dimemas-wrapper.sh") );                             
  application[ STATS_WRAPPER ]       = wxString( wxT("stats-wrapper.sh") );                             
  application[ CLUSTERING ]          = wxString( wxT("BurstClustering") );                             
  application[ FOLDING ]             = wxString( wxT("folding") );                             
  application[ USER_DEFINED ]        = wxString( wxT("") ); // NOT USED                           
  application[ DIMEMAS_GUI ]         = wxString( wxT("DimemasGUI") );
  application[ STATS ]               = wxString( wxT("stats") );

  tagFoldingOutputDirectory = wxString( wxT("Output directory:") );
  
  tunePrvLinksForClustering = false;
  tunePrvLinksForFolding = false;
  
  pidDimemasGUI = 0;

  std::stringstream tmpPID;
#ifdef WIN32
  tmpPID << _getpid();
  _putenv_s( "PARAVER_LOADED", (const char *)std::string( tmpPID.str() ).c_str() );
#else
  tmpPID << ::getpid();
  setenv( "PARAVER_LOADED", (const char *)std::string( tmpPID.str() ).c_str(), 1 );
#endif
}


wxString RunScript::getEnvironmentPath( TEnvironmentVar envVar, wxString command )
{
  wxString pathToBin;
  wxString tmpEnv;
  wxPathList currentPathEnv;
  wxFileName auxName;
  
  switch( envVar )
  {
    case PATH:
      currentPathEnv.AddEnvList( environmentVariable[ envVar ] );
      pathToBin = currentPathEnv.FindAbsoluteValidPath( command );
      auxName   = wxFileName( pathToBin );
      pathToBin = auxName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
      
      break;
      
    case PARAVER_HOME:
    case DIMEMAS_HOME:
      
      if ( wxGetEnv( environmentVariable[ envVar ], &tmpEnv ) )
      {
        pathToBin = tmpEnv + wxFileName::GetPathSeparator() + wxString( wxT( "bin" ) ) + wxFileName::GetPathSeparator();
      }
      
      break;
      
    default:
      break;
  }
    
  return pathToBin;
}


/*!
 * Control creation for RunScript
 */

void RunScript::CreateControls()
{    
////@begin RunScript content construction
  RunScript* itemDialog1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemDialog1->SetSizer(itemBoxSizer2);

  wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW|wxALL, 2);

  wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Application"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    itemStaticText5->SetToolTip(_("Select registered application"));
  itemBoxSizer4->Add(itemStaticText5, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxArrayString choiceApplicationStrings;
  choiceApplication = new wxChoice( itemDialog1, ID_CHOICE_APPLICATION, wxDefaultPosition, wxDefaultSize, choiceApplicationStrings, 0 );
  choiceApplication->SetHelpText(_("Select external application"));
  if (RunScript::ShowToolTips())
    choiceApplication->SetToolTip(_("Select external application"));
  itemBoxSizer4->Add(choiceApplication, 12, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  buttonEditApplication = new wxButton( itemDialog1, ID_BUTTON_EDIT_APPLICATION, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
  buttonEditApplication->SetHelpText(_("Edit application preferences"));
  if (RunScript::ShowToolTips())
    buttonEditApplication->SetToolTip(_("Edit application preferences"));
  buttonEditApplication->Show(false);
  itemBoxSizer4->Add(buttonEditApplication, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer3->Add(itemBoxSizer8, 0, wxGROW|wxALL, 2);

  wxStaticText* itemStaticText9 = new wxStaticText( itemDialog1, wxID_STATIC, _("Trace"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    itemStaticText9->SetToolTip(_("Select the input trace read by the application"));
  itemBoxSizer8->Add(itemStaticText9, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlTrace = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_TRACE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    textCtrlTrace->SetToolTip(_("Select the input trace read by the application"));
  itemBoxSizer8->Add(textCtrlTrace, 9, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  fileBrowserButtonTrace = new FileBrowserButton( itemDialog1, ID_BUTTON_TRACE_BROWSER, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    fileBrowserButtonTrace->SetToolTip(_("Select the input trace read by the application"));
  itemBoxSizer8->Add(fileBrowserButtonTrace, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  boxSizerParameters = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(boxSizerParameters, 0, wxGROW|wxALL, 2);

  labelTextCtrlDefaultParameters = new wxStaticText( itemDialog1, wxID_STATIC, _("Command"), wxDefaultPosition, wxDefaultSize, 0 );
  boxSizerParameters->Add(labelTextCtrlDefaultParameters, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlDefaultParameters = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_DEFAULT_PARAMETERS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    textCtrlDefaultParameters->SetToolTip(_("Parameters passed to the script"));
  boxSizerParameters->Add(textCtrlDefaultParameters, 12, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  dimemasSection = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(dimemasSection, 3, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
  dimemasSection->Add(itemBoxSizer16, 0, wxGROW|wxTOP|wxBOTTOM, 2);

  labelFilePickerDimemasCFG = new wxStaticText( itemDialog1, wxID_STATIC, _("Dimemas Cfg"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    labelFilePickerDimemasCFG->SetToolTip(_("Select the Dimemas configuration file to apply"));
  itemBoxSizer16->Add(labelFilePickerDimemasCFG, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlDimemasCFG = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_DIMEMAS_CFG, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    textCtrlDimemasCFG->SetToolTip(_("Select the Dimemas configuration file to apply"));
  itemBoxSizer16->Add(textCtrlDimemasCFG, 9, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  fileBrowserButtonDimemasCFG = new FileBrowserButton( itemDialog1, ID_BUTTON_DIMEMAS_CFG_BROWSER, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    fileBrowserButtonDimemasCFG->SetToolTip(_("Select the Dimemas configuration file to apply"));
  itemBoxSizer16->Add(fileBrowserButtonDimemasCFG, 2, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  buttonDimemasGUI = new wxBitmapButton( itemDialog1, ID_BUTTON_DIMEMAS_GUI, itemDialog1->GetBitmapResource(wxT("app_edit.xpm")), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  if (RunScript::ShowToolTips())
    buttonDimemasGUI->SetToolTip(_("Edit Dimemas Configuration File using DimemasGUI."));
  itemBoxSizer16->Add(buttonDimemasGUI, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

  wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
  dimemasSection->Add(itemBoxSizer21, 0, wxGROW|wxTOP|wxBOTTOM, 4);

  labelTextCtrlOutputTrace = new wxStaticText( itemDialog1, wxID_STATIC, _("Output Trace"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    labelTextCtrlOutputTrace->SetToolTip(_("Write the name given to the output trace; if missing, suffix '.prv' will be appended"));
  itemBoxSizer21->Add(labelTextCtrlOutputTrace, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlOutputTrace = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_OUTPUT_TRACE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    textCtrlOutputTrace->SetToolTip(_("Write the name given to the output trace; if missing, suffix '.prv' will be appended"));
  itemBoxSizer21->Add(textCtrlOutputTrace, 12, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

  wxNotebook* itemNotebook24 = new wxNotebook( itemDialog1, ID_NOTEBOOK_DIMEMAS, wxDefaultPosition, wxDefaultSize, wxBK_TOP );

  wxScrolledWindow* itemScrolledWindow25 = new wxScrolledWindow( itemNotebook24, ID_SCROLLEDWINDOW_DIMEMAS_MAIN, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL|wxTAB_TRAVERSAL );
  itemScrolledWindow25->SetScrollbars(1, 1, 0, 0);
  wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxVERTICAL);
  itemScrolledWindow25->SetSizer(itemBoxSizer26);

  checkBoxReuseDimemasTrace = new wxCheckBox( itemScrolledWindow25, ID_CHECKBOX_DIMEMAS_REUSE, _("Reuse Dimemas trace if previously generated"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxReuseDimemasTrace->SetValue(true);
  if (RunScript::ShowToolTips())
    checkBoxReuseDimemasTrace->SetToolTip(_("Check this if you want to run many simulations varying only the Dimemas parametrization but the translated trace (obtained from given .prv trace). If unchecked, prv2dim is previously called in order to get this translation."));
  itemBoxSizer26->Add(checkBoxReuseDimemasTrace, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 4);

  checkBoxDontTranslateIdleStates = new wxCheckBox( itemScrolledWindow25, ID_CHECKBOX_PRV2DIM_N, _("Don't generate initial idle states when translating"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxDontTranslateIdleStates->SetValue(false);
  if (RunScript::ShowToolTips())
    checkBoxDontTranslateIdleStates->SetToolTip(_("If checked, parameter -n is passed to prv2dim translator, forbidding generation of initial idle states. prv2dim translation phase is engaged only if Dimemas trace is missing or you don't want to reuse it (previous \"Reuse Dimemas\" unchecked)"));
  itemBoxSizer26->Add(checkBoxDontTranslateIdleStates, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 4);

  checkBoxDimemasVerbose = new wxCheckBox( itemScrolledWindow25, ID_CHECKBOX_VERBOSE, _("Verbose"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxDimemasVerbose->SetValue(false);
  if (RunScript::ShowToolTips())
    checkBoxDimemasVerbose->SetToolTip(_("If unset, Dimemas shows only simulation times"));
  itemBoxSizer26->Add(checkBoxDimemasVerbose, 0, wxALIGN_LEFT|wxALL, 4);

  itemScrolledWindow25->FitInside();
  itemNotebook24->AddPage(itemScrolledWindow25, _("Main"));

  wxScrolledWindow* itemScrolledWindow30 = new wxScrolledWindow( itemNotebook24, ID_SCROLLEDWINDOW_DIMEMAS_ADVANCED, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL|wxTAB_TRAVERSAL );
  itemScrolledWindow30->SetScrollbars(1, 1, 0, 0);
  wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
  itemScrolledWindow30->SetSizer(itemBoxSizer31);

  wxBoxSizer* itemBoxSizer32 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer31->Add(itemBoxSizer32, 0, wxGROW|wxALL, 2);
  wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer32->Add(itemBoxSizer33, 0, wxGROW|wxTOP, 20);
  wxStaticText* itemStaticText34 = new wxStaticText( itemScrolledWindow30, wxID_STATIC, _("Bandwidth (MB/s)"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer33->Add(itemStaticText34, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlDimemasBandwidth = new wxTextCtrl( itemScrolledWindow30, ID_TEXTCTRL_DIMEMAS_BANDWIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer33->Add(textCtrlDimemasBandwidth, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 2);

  wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer32->Add(itemBoxSizer36, 0, wxGROW|wxTOP, 5);
  wxStaticText* itemStaticText37 = new wxStaticText( itemScrolledWindow30, wxID_STATIC, _("Latency (s)"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer36->Add(itemStaticText37, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlDimemasLatency = new wxTextCtrl( itemScrolledWindow30, ID_TEXTCTRL_DIMEMAS_LATENCY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer36->Add(textCtrlDimemasLatency, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 2);

  wxStaticBox* itemStaticBoxSizer39Static = new wxStaticBox(itemScrolledWindow30, wxID_ANY, _(" Tasks Mapping  "));
  wxStaticBoxSizer* itemStaticBoxSizer39 = new wxStaticBoxSizer(itemStaticBoxSizer39Static, wxVERTICAL);
  itemBoxSizer31->Add(itemStaticBoxSizer39, 1, wxALIGN_TOP|wxLEFT|wxRIGHT|wxTOP, 4);
  wxPanel* itemPanel40 = new wxPanel( itemScrolledWindow30, ID_PANEL_DUMMY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  itemStaticBoxSizer39->Add(itemPanel40, 0, wxGROW, 0);
  wxBoxSizer* itemBoxSizer41 = new wxBoxSizer(wxVERTICAL);
  itemPanel40->SetSizer(itemBoxSizer41);

  wxBoxSizer* itemBoxSizer42 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer41->Add(itemBoxSizer42, 0, wxGROW|wxALL, 2);
  radioButtonDimemasDefaultTasksMapping = new wxRadioButton( itemPanel40, ID_RADIOBUTTON_DIMEMAS_DEFAULT_TASKS_MAPPING, _("Default"), wxDefaultPosition, wxDefaultSize, 0 );
  radioButtonDimemasDefaultTasksMapping->SetValue(true);
  radioButtonDimemasDefaultTasksMapping->SetHelpText(_("Don't override the mapping that's defined in the Dimemas Cfg"));
  if (RunScript::ShowToolTips())
    radioButtonDimemasDefaultTasksMapping->SetToolTip(_("Don't override the mapping that's defined in the Dimemas Cfg"));
  itemBoxSizer42->Add(radioButtonDimemasDefaultTasksMapping, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  radioButtonDimemasFillNodes = new wxRadioButton( itemPanel40, ID_RADIOBUTTON_DIMEMAS_FILL_NODES, _("Fill nodes"), wxDefaultPosition, wxDefaultSize, 0 );
  radioButtonDimemasFillNodes->SetValue(false);
  radioButtonDimemasFillNodes->SetHelpText(_("Assign as many tasks as cpus on each node, resulting on first nodes always filled"));
  if (RunScript::ShowToolTips())
    radioButtonDimemasFillNodes->SetToolTip(_("Assign as many tasks as cpus on each node, resulting on first nodes always filled"));
  itemBoxSizer42->Add(radioButtonDimemasFillNodes, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  radioButtonDimemasInterleaved = new wxRadioButton( itemPanel40, ID_RADIOBUTTON_DIMEMAS_INTERLEAVED, _("Interleaved"), wxDefaultPosition, wxDefaultSize, 0 );
  radioButtonDimemasInterleaved->SetValue(false);
  radioButtonDimemasInterleaved->SetHelpText(_("Tasks are interleaved between the CPUs of all the nodes"));
  if (RunScript::ShowToolTips())
    radioButtonDimemasInterleaved->SetToolTip(_("Tasks are interleaved between the CPUs of all the nodes"));
  itemBoxSizer42->Add(radioButtonDimemasInterleaved, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer41->Add(itemBoxSizer46, 0, wxGROW|wxALL, 2);
  radioButtonDimemasTasksPerNode = new wxRadioButton( itemPanel40, ID_RADIOBUTTON_DIMEMAS_TASKS_PER_NODE, _("Tasks per Node:"), wxDefaultPosition, wxDefaultSize, 0 );
  radioButtonDimemasTasksPerNode->SetValue(false);
  radioButtonDimemasTasksPerNode->SetHelpText(_("Tasks assigned to a Node"));
  if (RunScript::ShowToolTips())
    radioButtonDimemasTasksPerNode->SetToolTip(_("Tasks assigned to a Node"));
  itemBoxSizer46->Add(radioButtonDimemasTasksPerNode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  spinCtrlDimemasTasksPerNode = new wxSpinCtrl( itemPanel40, ID_TEXTCTRL_DIMEMAS_TASKS_PER_NODE, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65536, 1 );
  spinCtrlDimemasTasksPerNode->SetHelpText(_("Tasks assigned to a Node"));
  if (RunScript::ShowToolTips())
    spinCtrlDimemasTasksPerNode->SetToolTip(_("Tasks assigned to a Node"));
  itemBoxSizer46->Add(spinCtrlDimemasTasksPerNode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  itemScrolledWindow30->FitInside();
  itemNotebook24->AddPage(itemScrolledWindow30, _("Advanced"));

  dimemasSection->Add(itemNotebook24, 2, wxGROW|wxALL, 4);

  statsSection = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(statsSection, 0, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer50 = new wxBoxSizer(wxHORIZONTAL);
  statsSection->Add(itemBoxSizer50, 0, wxGROW|wxALL, 2);

  statsLabelTextCtrlOutputName = new wxStaticText( itemDialog1, wxID_STATIC, _("Output Prefix"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    statsLabelTextCtrlOutputName->SetToolTip(_("Name given to resulting .dat and .gnuplot files."));
  statsLabelTextCtrlOutputName->SetName(wxT("O"));
  itemBoxSizer50->Add(statsLabelTextCtrlOutputName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  statsTextCtrlOutputName = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_STATS_OUTPUT_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    statsTextCtrlOutputName->SetToolTip(_("Name given to resulting .dat and .gnuplot files."));
  itemBoxSizer50->Add(statsTextCtrlOutputName, 4, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer53 = new wxBoxSizer(wxHORIZONTAL);
  statsSection->Add(itemBoxSizer53, 0, wxGROW|wxALL, 2);

  statsCheckBoxShowBurstsHistogram = new wxCheckBox( itemDialog1, ID_CHECKBOX_STATS_SHOW_BURSTS, _("Generate bursts histogram"), wxDefaultPosition, wxDefaultSize, 0 );
  statsCheckBoxShowBurstsHistogram->SetValue(true);
  if (RunScript::ShowToolTips())
    statsCheckBoxShowBurstsHistogram->SetToolTip(_("Generat _bursts histogram files."));
  itemBoxSizer53->Add(statsCheckBoxShowBurstsHistogram, 1, wxGROW|wxALL, 2);

  statsCheckBoxShowCommsHistogram = new wxCheckBox( itemDialog1, ID_CHECKBOX_STATS_SHOW_COMMS_HISTOGRAM, _("Generate communications histogram"), wxDefaultPosition, wxDefaultSize, 0 );
  statsCheckBoxShowCommsHistogram->SetValue(false);
  if (RunScript::ShowToolTips())
    statsCheckBoxShowCommsHistogram->SetToolTip(_("Generate communications histogram"));
  itemBoxSizer53->Add(statsCheckBoxShowCommsHistogram, 1, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer56 = new wxBoxSizer(wxHORIZONTAL);
  statsSection->Add(itemBoxSizer56, 0, wxGROW|wxALL, 2);

  statsCheckBoxOnlyDatFile = new wxCheckBox( itemDialog1, ID_CHECKBOX_STATS_ONLYGENERATEDATFILE, _("Only generate .dat file"), wxDefaultPosition, wxDefaultSize, 0 );
  statsCheckBoxOnlyDatFile->SetValue(false);
  statsCheckBoxOnlyDatFile->SetHelpText(_("If checked if won't generate .gnuplot, only .dat. If unchecked it generates both."));
  if (RunScript::ShowToolTips())
    statsCheckBoxOnlyDatFile->SetToolTip(_("If checked if won't generate .gnuplot, only .dat. If unchecked it generates both."));
  itemBoxSizer56->Add(statsCheckBoxOnlyDatFile, 1, wxGROW|wxALL, 2);

  statsCheckBoxExclusiveTimes = new wxCheckBox( itemDialog1, ID_CHECKBOX_STATS_EXCLUSIVE_TIMES, _("Exclusive instead of inclusive times"), wxDefaultPosition, wxDefaultSize, 0 );
  statsCheckBoxExclusiveTimes->SetValue(false);
  if (RunScript::ShowToolTips())
    statsCheckBoxExclusiveTimes->SetToolTip(_("Changes how the times of the routine calls are calculated; if unchecked, inclusive times are calculated; if checked, exclusive times are calculated."));
  itemBoxSizer56->Add(statsCheckBoxExclusiveTimes, 1, wxGROW|wxALL, 2);

  clusteringSection = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(clusteringSection, 0, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer60 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSection->Add(itemBoxSizer60, 0, wxGROW|wxALL, 2);

  wxStaticText* itemStaticText61 = new wxStaticText( itemDialog1, wxID_STATIC, _("XML File"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer60->Add(itemStaticText61, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  textCtrlClusteringXML = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_CLUSTERING_XML, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer60->Add(textCtrlClusteringXML, 9, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  fileBrowserButtonClusteringXML = new FileBrowserButton( itemDialog1, ID_BUTTON_CLUSTERING_XML, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer60->Add(fileBrowserButtonClusteringXML, 2, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  buttonClusteringXML = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_CLUSTERING_XML, itemDialog1->GetBitmapResource(wxT("app_edit.xpm")), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  itemBoxSizer60->Add(buttonClusteringXML, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxBoxSizer* itemBoxSizer65 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSection->Add(itemBoxSizer65, 0, wxGROW|wxALL, 2);

  itemBoxSizer65->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  checkBoxClusteringUseSemanticWindow = new wxCheckBox( itemDialog1, ID_CHECKBOX_CLUSTERING_USE_SEMANTIC_WINDOW, _("Use semantic window as bursts divider"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxClusteringUseSemanticWindow->SetValue(false);
  itemBoxSizer65->Add(checkBoxClusteringUseSemanticWindow, 4, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 2);

  wxBoxSizer* itemBoxSizer68 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSection->Add(itemBoxSizer68, 0, wxGROW|wxALL, 2);

  itemBoxSizer68->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  checkBoxClusteringCSVValueAsDimension = new wxCheckBox( itemDialog1, ID_CHECKBOX_CLUSTERING_SEMVAL_AS_CLUSTDIMENSION, _("Use semantic values as clustering dimensions"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxClusteringCSVValueAsDimension->SetValue(false);
  itemBoxSizer68->Add(checkBoxClusteringCSVValueAsDimension, 4, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 2);

  wxBoxSizer* itemBoxSizer71 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSection->Add(itemBoxSizer71, 0, wxGROW|wxALL, 2);

  itemBoxSizer71->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  checkBoxClusteringNormalize = new wxCheckBox( itemDialog1, ID_CHECKBOX_CLUSTERING_NORMALIZE, _("Apply logarithmic normalization to semantic value"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxClusteringNormalize->SetValue(false);
  itemBoxSizer71->Add(checkBoxClusteringNormalize, 4, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 2);

  wxBoxSizer* itemBoxSizer74 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSection->Add(itemBoxSizer74, 0, wxGROW|wxALL, 2);

  itemBoxSizer74->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  checkBoxClusteringGenerateSeq = new wxCheckBox( itemDialog1, ID_CHECKBOX_CLUSTERING_GENERATE_SEQUENCES, _("Generate sequences file"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBoxClusteringGenerateSeq->SetValue(false);
  checkBoxClusteringGenerateSeq->SetHelpText(_("Generate a file containing the cluster sequences"));
  if (RunScript::ShowToolTips())
    checkBoxClusteringGenerateSeq->SetToolTip(_("Generate a file containing the cluster sequences"));
  itemBoxSizer74->Add(checkBoxClusteringGenerateSeq, 2, wxGROW|wxLEFT|wxTOP|wxBOTTOM, 2);

  clusteringRadioGenerateSeqNumbered = new wxRadioButton( itemDialog1, ID_RADIOBUTTON_CLUSTERING_GEN_SEQ_NUMBERED, _("Numbered"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  clusteringRadioGenerateSeqNumbered->SetValue(true);
  clusteringRadioGenerateSeqNumbered->SetHelpText(_("Generate a file containing the cluster sequences"));
  if (RunScript::ShowToolTips())
    clusteringRadioGenerateSeqNumbered->SetToolTip(_("Generate a file containing the cluster sequences"));
  clusteringRadioGenerateSeqNumbered->Enable(false);
  itemBoxSizer74->Add(clusteringRadioGenerateSeqNumbered, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);

  clusteringRadioGenerateSeqFASTA = new wxRadioButton( itemDialog1, ID_RADIOBUTTON_CLUSTERING_GEN_SEQ_FASTA, _("FASTA format"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringRadioGenerateSeqFASTA->SetValue(false);
  clusteringRadioGenerateSeqFASTA->SetHelpText(_("Generate FASTA aminoacid sequences"));
  if (RunScript::ShowToolTips())
    clusteringRadioGenerateSeqFASTA->SetToolTip(_("Generate FASTA aminoacid sequences"));
  clusteringRadioGenerateSeqFASTA->Enable(false);
  itemBoxSizer74->Add(clusteringRadioGenerateSeqFASTA, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);

  clusteringSizerAlgorithm = new wxStaticBox(itemDialog1, wxID_ANY, _(" Algorithm "));
  wxStaticBoxSizer* itemStaticBoxSizer79 = new wxStaticBoxSizer(clusteringSizerAlgorithm, wxVERTICAL);
  clusteringSection->Add(itemStaticBoxSizer79, 0, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer80 = new wxBoxSizer(wxHORIZONTAL);
  itemStaticBoxSizer79->Add(itemBoxSizer80, 0, wxGROW|wxALL, 2);

  clusteringRadioXMLDefined = new wxRadioButton( itemDialog1, ID_RADIOBUTTON_CLUSTERING_XMLDEFINED, _("XML defined"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  clusteringRadioXMLDefined->SetValue(true);
  itemBoxSizer80->Add(clusteringRadioXMLDefined, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringRadioDBScan = new wxRadioButton( itemDialog1, ID_RADIOBUTTON_CLUSTERING_DBSCAN, _("DBScan"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringRadioDBScan->SetValue(false);
  itemBoxSizer80->Add(clusteringRadioDBScan, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringRadioRefinement = new wxRadioButton( itemDialog1, ID_RADIOBUTTON_CLUSTERING_REFINEMENT, _("Aggregative refinement"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringRadioRefinement->SetValue(false);
  itemBoxSizer80->Add(clusteringRadioRefinement, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringAlgorithmLineSeparator = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  itemStaticBoxSizer79->Add(clusteringAlgorithmLineSeparator, 0, wxGROW|wxALL, 2);

  clusteringSizerDBScan = new wxBoxSizer(wxHORIZONTAL);
  itemStaticBoxSizer79->Add(clusteringSizerDBScan, 0, wxGROW|wxALL, 2);

  wxStaticText* itemStaticText86 = new wxStaticText( itemDialog1, wxID_STATIC, _("Epsilon"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringSizerDBScan->Add(itemStaticText86, 2, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringTextBoxDBScanEpsilon = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_CLUSTERING_DBSCAN_EPSILON, _("0.01"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringSizerDBScan->Add(clusteringTextBoxDBScanEpsilon, 4, wxGROW|wxALL, 2);

  clusteringSizerDBScan->Add(5, 5, 1, wxGROW|wxALL, 2);

  wxStaticText* itemStaticText89 = new wxStaticText( itemDialog1, wxID_STATIC, _("Min Points"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringSizerDBScan->Add(itemStaticText89, 2, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringTextBoxDBScanMinPoints = new wxSpinCtrl( itemDialog1, ID_TEXTCTRL_DBSCAN_MIN_POINTS, wxT("4"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 4 );
  clusteringSizerDBScan->Add(clusteringTextBoxDBScanMinPoints, 4, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringSizerRefinement = new wxBoxSizer(wxVERTICAL);
  itemStaticBoxSizer79->Add(clusteringSizerRefinement, 0, wxGROW|wxALL, 2);

  clusteringCheckBoxRefinementPrintData = new wxCheckBox( itemDialog1, ID_CHECKBOX_CLUSTERING_REFINEMENT_PRINT_DATA, _("Print intermediate data"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringCheckBoxRefinementPrintData->SetValue(false);
  clusteringSizerRefinement->Add(clusteringCheckBoxRefinementPrintData, 0, wxALIGN_LEFT|wxALL, 2);

  clusteringCheckBoxRefinementTune = new wxCheckBox( itemDialog1, ID_CHECKBOX_CLUSTERING_REFINEMENT_TUNE, _("Tune manually"), wxDefaultPosition, wxDefaultSize, 0 );
  clusteringCheckBoxRefinementTune->SetValue(false);
  clusteringSizerRefinement->Add(clusteringCheckBoxRefinementTune, 0, wxALIGN_LEFT|wxALL, 2);

  wxBoxSizer* itemBoxSizer94 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSizerRefinement->Add(itemBoxSizer94, 0, wxGROW|wxALL, 2);

  clusteringLabelRefinementEpsilon = new wxStaticText( itemDialog1, wxID_STATIC, _("Epsilon"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer94->Add(clusteringLabelRefinementEpsilon, 2, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringLabelRefinementEpsilonMin = new wxStaticText( itemDialog1, wxID_STATIC, _("Min"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer94->Add(clusteringLabelRefinementEpsilonMin, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringTextBoxRefinementEpsilonMin = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_CLUSTERING_REFINEMENT_EPSILON_MIN, _("0.001"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer94->Add(clusteringTextBoxRefinementEpsilonMin, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringLabelRefinementEpsilonMax = new wxStaticText( itemDialog1, wxID_STATIC, _("Max"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer94->Add(clusteringLabelRefinementEpsilonMax, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringTextBoxRefinementEpsilonMax = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_CLUSTERING_REFINEMENT_EPSILON_MAX, _("0.01"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer94->Add(clusteringTextBoxRefinementEpsilonMax, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  itemBoxSizer94->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringLabelRefinementSteps = new wxStaticText( itemDialog1, wxID_STATIC, _("Steps"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer94->Add(clusteringLabelRefinementSteps, 1, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  clusteringTextBoxRefinementSteps = new wxSpinCtrl( itemDialog1, ID_TEXTCTRL_CLUSTERING_REFINEMENT_STEPS, wxT("10"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 30, 10 );
  itemBoxSizer94->Add(clusteringTextBoxRefinementSteps, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxBoxSizer* itemBoxSizer103 = new wxBoxSizer(wxHORIZONTAL);
  clusteringSizerRefinement->Add(itemBoxSizer103, 0, wxGROW|wxALL, 2);

  clusteringLabelRefinementMinPoints = new wxStaticText( itemDialog1, wxID_STATIC, _("Min Points"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer103->Add(clusteringLabelRefinementMinPoints, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  clusteringTextBoxRefinementMinPoints = new wxSpinCtrl( itemDialog1, ID_TEXTCTRL_CLUSTERING_REFINEMENT_MIN_POINTS, wxT("4"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 4 );
  itemBoxSizer103->Add(clusteringTextBoxRefinementMinPoints, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  itemBoxSizer103->Add(5, 5, 9, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  foldingSection = new wxBoxSizer(wxVERTICAL);
  clusteringSection->Add(foldingSection, 0, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer108 = new wxBoxSizer(wxHORIZONTAL);
  foldingSection->Add(itemBoxSizer108, 0, wxGROW|wxBOTTOM, 2);

  itemBoxSizer108->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  checkboxFoldingUseSemanticValues = new wxCheckBox( itemDialog1, ID_CHECKBOX_FOLDING_USE_SEMANTIC_VALUE, _("Use semantic values as region delimiter"), wxDefaultPosition, wxDefaultSize, 0 );
  checkboxFoldingUseSemanticValues->SetValue(false);
  if (RunScript::ShowToolTips())
    checkboxFoldingUseSemanticValues->SetToolTip(_("If checked, a precomputed CSV containing timeline semantic values will be used as region delimiter.\nIf unchecked the above event  type/name will be used."));
  itemBoxSizer108->Add(checkboxFoldingUseSemanticValues, 4, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 2);

  wxBoxSizer* itemBoxSizer111 = new wxBoxSizer(wxHORIZONTAL);
  foldingSection->Add(itemBoxSizer111, 0, wxGROW|wxBOTTOM, 2);

  wxStaticText* itemStaticText112 = new wxStaticText( itemDialog1, wxID_STATIC, _("Model"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
  if (RunScript::ShowToolTips())
    itemStaticText112->SetToolTip(_("Combine the trace-file hardware counters for the selected architecture"));
  itemBoxSizer111->Add(itemStaticText112, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxArrayString comboboxFoldingModelStrings;
  comboboxFoldingModelStrings.Add(_("none"));
  comboboxFoldingModelStrings.Add(_("basic"));
  comboboxFoldingModelStrings.Add(_("ibm-power5"));
  comboboxFoldingModelStrings.Add(_("ibm-power7"));
  comboboxFoldingModelStrings.Add(_("ibm-power8"));
  comboboxFoldingModelStrings.Add(_("intel-haswell"));
  comboboxFoldingModelStrings.Add(_("intel-nehalem"));
  comboboxFoldingModelStrings.Add(_("intel-sandybridge"));
  comboboxFoldingModelStrings.Add(_("samsung-exynos5-armv7"));
  comboboxFoldingModel = new wxComboBox( itemDialog1, ID_COMBOBOX_FOLDING_MODEL, _("None"), wxDefaultPosition, wxDefaultSize, comboboxFoldingModelStrings, wxCB_DROPDOWN );
  comboboxFoldingModel->SetStringSelection(_("None"));
  if (RunScript::ShowToolTips())
    comboboxFoldingModel->SetToolTip(_("Combine the trace-file hardware counters for the selected architecture"));
  itemBoxSizer111->Add(comboboxFoldingModel, 12, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 2);

  wxStaticLine* itemStaticLine114 = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  itemBoxSizer2->Add(itemStaticLine114, 0, wxGROW|wxALL, 5);

  wxBoxSizer* itemBoxSizer115 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer115, 1, wxGROW, 5);

  wxStaticText* itemStaticText116 = new wxStaticText( itemDialog1, wxID_STATIC, _("Preview:"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    itemStaticText116->SetToolTip(_("Command to execute"));
  itemBoxSizer115->Add(itemStaticText116, 1, wxALIGN_TOP|wxALL, 5);

  labelCommandPreview = new wxTextCtrl( itemDialog1, wxID_LABELCOMMANDPREVIEW, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
  if (RunScript::ShowToolTips())
    labelCommandPreview->SetToolTip(_("Command to execute"));
  itemBoxSizer115->Add(labelCommandPreview, 4, wxGROW|wxALL, 5);

  wxBoxSizer* itemBoxSizer118 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer118, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT, 5);

  buttonHelpScript = new wxButton( itemDialog1, ID_BUTTON_HELP_SCRIPT, _("Help"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    buttonHelpScript->SetToolTip(_("Shows the application '--help' message if available"));
  buttonHelpScript->Show(false);
  itemBoxSizer118->Add(buttonHelpScript, 0, wxGROW|wxALL, 5);

  buttonRun = new wxButton( itemDialog1, ID_BUTTON_RUN, _("Run"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    buttonRun->SetToolTip(_("Runs the application"));
  itemBoxSizer118->Add(buttonRun, 0, wxGROW|wxALL, 5);

  buttonKill = new wxButton( itemDialog1, ID_BUTTON_KILL, _("Kill"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer118->Add(buttonKill, 0, wxGROW|wxALL, 5);

  buttonClearLog = new wxButton( itemDialog1, ID_BUTTON_CLEAR_LOG, _("Clear Log"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    buttonClearLog->SetToolTip(_("Clears accumulated messages"));
  itemBoxSizer118->Add(buttonClearLog, 0, wxGROW|wxALL, 5);

  listboxRunLog = new wxHtmlWindow( itemDialog1, ID_LISTBOX_RUN_LOG, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxHSCROLL|wxVSCROLL );
  if (RunScript::ShowToolTips())
    listboxRunLog->SetToolTip(_("Execution messages"));
  itemBoxSizer2->Add(listboxRunLog, 3, wxGROW|wxALL, 7);

  wxBoxSizer* itemBoxSizer124 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer124, 0, wxALIGN_RIGHT|wxALL, 5);

  wxBoxSizer* itemBoxSizer125 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer124->Add(itemBoxSizer125, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  buttonExit = new wxButton( itemDialog1, ID_BUTTON_EXIT, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
  if (RunScript::ShowToolTips())
    buttonExit->SetToolTip(_("Close window but don't run the selected application."));
  itemBoxSizer125->Add(buttonExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

////@end RunScript content construction

#if wxMAJOR_VERSION<3
  listboxRunLog->SetWindowStyleFlag( wxALWAYS_SHOW_SB|wxHW_SCROLLBAR_AUTO|wxHSCROLL|wxVSCROLL );
#else
  listboxRunLog->ShowScrollbars( wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS );
#endif
  
  for ( int i = DIMEMAS_WRAPPER; i <= USER_DEFINED; i++ )
  {
    choiceApplication->Append( applicationLabel[ TExternalApp(i) ] );
  }
  
  // Trace browser
  fileBrowserButtonTrace->SetTextBox( textCtrlTrace );
  fileBrowserButtonTrace->SetDialogMessage( _( "Load Trace" ) );
  wxString tmpWildCard = wxT( "Paraver trace (*.prv;*.prv.gz)|*.prv;*.prv.gz|All files (*.*)|*.*" );
  fileBrowserButtonTrace->SetFileDialogWildcard( tmpWildCard );
  fileBrowserButtonTrace->Enable();

  fileBrowserButtonDimemasCFG->SetTextBox( textCtrlDimemasCFG ); 
  fileBrowserButtonDimemasCFG->SetDialogMessage( _( "Load Dimemas Configuration" ) );
  tmpWildCard = wxT( "Dimemas configuration file (*.cfg)|*.cfg|All files (*.*)|*.*" );
  fileBrowserButtonDimemasCFG->SetFileDialogWildcard( tmpWildCard );
  fileBrowserButtonDimemasCFG->Enable(); 

  fileBrowserButtonClusteringXML->SetTextBox( textCtrlClusteringXML );
  fileBrowserButtonClusteringXML->SetDialogMessage( _( "Load Clustering Configuration" ) );
  tmpWildCard = wxT( "Clustering configuration file (*.xml)|*.xml|All files (*.*)|*.*" );
  fileBrowserButtonClusteringXML->SetFileDialogWildcard( tmpWildCard );
  fileBrowserButtonClusteringXML->Enable();
  
  // Filter forbidden chars
  wxArrayString forbidden;
  forbidden.Add( _( "-" ) );
  wxTextValidator validator( wxFILTER_NUMERIC | wxFILTER_EXCLUDE_CHAR_LIST );
  validator.SetExcludes( forbidden );
  textCtrlDimemasBandwidth->SetValidator( validator );
  textCtrlDimemasLatency->SetValidator( validator );
  clusteringTextBoxDBScanEpsilon->SetValidator( validator );
  clusteringTextBoxRefinementEpsilonMin->SetValidator( validator );
  clusteringTextBoxRefinementEpsilonMax->SetValidator( validator );
  
  int appNumber = DIMEMAS_WRAPPER; // Default is 0
  choiceApplication->Select( appNumber );

  adaptWindowToApplicationSelection();
  
  if ( !clusteringXML.IsEmpty() )
  {
    fileBrowserButtonClusteringXML->SetPath( clusteringXML );
  }
  clusteringRadioGenerateSeqNumbered->SetValue(true);

}


/*!
 * Should we show tooltips?
 */

bool RunScript::ShowToolTips()
{
  return true;
}


/*!
 * Get bitmap resources
 */

wxBitmap RunScript::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin RunScript bitmap retrieval
  wxUnusedVar(name);
  if (name == wxT("app_edit.xpm"))
  {
    wxBitmap bitmap(app_edit_xpm);
    return bitmap;
  }
  return wxNullBitmap;
////@end RunScript bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RunScript::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin RunScript icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end RunScript icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_EXIT
 */
void RunScript::OnButtonExitClick( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}


wxString RunScript::GetCommand( wxString &command, wxString &parameters, TExternalApp selectedApp )
{
  wxFileName tmpFilename;
  wxString tmpPath;
  wxString tmpNameWOExtension;
  wxString tmpParams;
  wxString fullCommand;
  wxString tmpValue;
  
  command.Clear();
  parameters.Clear();
 
  if ( selectedApp == DEFAULT )
  {
    selectedApp = (TExternalApp)choiceApplication->GetSelection();
  }
 
  switch ( selectedApp )
  {
    case DIMEMAS_WRAPPER:
      command = application[ DIMEMAS_WRAPPER ];

      parameters = doubleQuote( fileBrowserButtonTrace->GetPath() );      // Source trace
      parameters += wxString( wxT( " " ) ) + doubleQuote( fileBrowserButtonDimemasCFG->GetPath() ); // Dimemas cfg
      parameters += wxString( wxT( " " ) ) + doubleQuote( textCtrlOutputTrace->GetValue() ); // Final trace
      if ( checkBoxReuseDimemasTrace->IsChecked() )
      {
        parameters += wxString( wxT( " 1" ) );
      }
      else
      {
        parameters += wxString( wxT( " 0" ) );
      }
      
      if ( !checkBoxDimemasVerbose->IsChecked() )
      {
        parameters += wxString( wxT( " -t" ) );
      }
      
      if ( checkBoxDontTranslateIdleStates->IsChecked() )
      {
        parameters += wxString( wxT( " -n" ) );
      }

      tmpValue.Clear();
      tmpValue = textCtrlDimemasBandwidth->GetValue();
      if ( !tmpValue.IsEmpty() )
      {
        parameters += wxString( wxT( " --bw" ) );
        parameters += wxString( wxT( " " ) ) + tmpValue;
      }

      tmpValue.Clear();
      tmpValue = textCtrlDimemasLatency->GetValue();
      if ( !tmpValue.IsEmpty() )
      {
        parameters += wxString( wxT( " --lat" ) );
        parameters += wxString( wxT( " " ) ) + tmpValue;
      }

      if ( !radioButtonDimemasDefaultTasksMapping->GetValue() ) // default and common
      {
        if ( radioButtonDimemasTasksPerNode->GetValue() )
        {
          parameters += wxString( wxT( " --ppn" ) );
          tmpValue.Clear();
          tmpValue << spinCtrlDimemasTasksPerNode->GetValue();
          parameters += wxString( wxT( " " ) ) + tmpValue;
        }
        else if ( radioButtonDimemasFillNodes->GetValue() )
        {
          parameters += wxString( wxT( " --fill" ) );
        }
        else //  radioButtonDimemasInterleaved->GetValue()
        {
          parameters += wxString( wxT( " --interlvd" ) );
        }
      }
      
      parameters += wxString( wxT( " " ) ) + expandVariables( textCtrlDefaultParameters->GetValue() ); // Extra params
      
      if ( textCtrlDefaultParameters->GetValue() == wxString( wxT( "--help" ) ))
      {
        helpOption = true;
      }

      break;
    
    case DIMEMAS_GUI:
      command = application[ DIMEMAS_GUI ];
    
      break;
      
    case STATS_WRAPPER:
    case STATS:
    
      if ( textCtrlDefaultParameters->GetValue() == wxString( wxT( "--help" ) ))
      {
        command  = application[ STATS ];
        parameters = textCtrlDefaultParameters->GetValue();
        helpOption = true;
      }
      else
      {
        // TODO: DEFAULT VALUES?
        command  = application[ STATS_WRAPPER ];

        parameters = doubleQuote( fileBrowserButtonTrace->GetPath() );      // Source trace
        parameters += wxString( wxT( " -o " ) ) + doubleQuote( statsTextCtrlOutputName->GetValue() ); // Final name
        if ( statsCheckBoxShowBurstsHistogram->IsChecked() )
        {
          parameters += wxString( wxT( " -bursts_histo" ) );
        }
        if ( statsCheckBoxShowCommsHistogram->IsChecked() )
        {
          parameters += wxString( wxT( " -comms_histo" ) );
        }
        if ( statsCheckBoxOnlyDatFile->IsChecked() )
        {
          parameters += wxString( wxT( " -only_dat_file" ) );
        }
        if ( statsCheckBoxExclusiveTimes->IsChecked() )
        {
          parameters += wxString( wxT( " -exclusive_times" ) );
        }
        parameters += wxString( wxT( " " ) ) +
                expandVariables( textCtrlDefaultParameters->GetValue() ); // Extra params
      }
      
      break;
      
    case CLUSTERING:
      command = application[ CLUSTERING ];
      
      parameters = wxString( wxT( " -p" ) );
      
      if ( checkBoxClusteringCSVValueAsDimension->IsEnabled() && 
           checkBoxClusteringCSVValueAsDimension->IsChecked() )
      {
        parameters += wxString( wxT( " -c" ) );
        if ( checkBoxClusteringNormalize->IsEnabled() &&
             checkBoxClusteringNormalize->IsChecked() )
        {
          parameters += wxString( wxT( "l" ) );
        }
      }
      
      if ( checkBoxClusteringGenerateSeq->IsChecked() )
      {
        if( clusteringRadioGenerateSeqNumbered->GetValue() )
          parameters += wxString( wxT( " -a" ) );
        else
          parameters += wxString( wxT( " -af" ) );
      }
      
      parameters += wxString( wxT( " -d " ) ) + doubleQuote( fileBrowserButtonClusteringXML->GetPath() );
      
      if ( clusteringRadioDBScan->GetValue() )
      {
        parameters += wxString( wxT( " -dbscan " ) );
        parameters += clusteringTextBoxDBScanEpsilon->GetValue() + wxString( wxT( "," ) );
        tmpValue.Clear();
        tmpValue << clusteringTextBoxDBScanMinPoints->GetValue();
        parameters += tmpValue;
      }
      else if ( clusteringRadioRefinement->GetValue() )
      {
        parameters += wxString( wxT( " -ra" ) );
        
        if ( clusteringCheckBoxRefinementPrintData->GetValue() )
        {
          parameters += wxString( wxT( "p" ) );
        }
        parameters += wxString( wxT( " " ) );

        
        if  ( clusteringCheckBoxRefinementTune->GetValue() )
        { 
          tmpValue.Clear();
          tmpValue << clusteringTextBoxRefinementMinPoints->GetValue();
          parameters += tmpValue;
          parameters += wxString( wxT( "," ) );
          parameters += clusteringTextBoxRefinementEpsilonMax->GetValue();
          parameters += wxString( wxT( "," ) );
          parameters += clusteringTextBoxRefinementEpsilonMin->GetValue();
          parameters += wxString( wxT( "," ) );
          tmpValue.Clear();
          tmpValue << clusteringTextBoxRefinementSteps->GetValue();
          parameters += tmpValue;
        }
      }
      
      parameters += wxString( wxT( " -i " ) );
      
      // !clusteringCSV.IsEmpty() ==> checkBoxClusteringUseSemanticWindow->IsEnabled()
      if ( !clusteringCSV.IsEmpty() && 
            checkBoxClusteringUseSemanticWindow->IsChecked() )
      {
        parameters += doubleQuote( clusteringCSV + wxString( wxT( "," ) ) + fileBrowserButtonTrace->GetPath() ) ;
      }
      else
      {
        parameters += doubleQuote( fileBrowserButtonTrace->GetPath() );
      }
      
      parameters += wxString( wxT(" -o ") );
      tmpFilename = wxFileName( fileBrowserButtonTrace->GetPath() );
      tmpPath = tmpFilename.GetPath( wxPATH_GET_SEPARATOR );
      tmpNameWOExtension = tmpFilename.GetName();
      parameters += doubleQuote( tmpPath + tmpNameWOExtension + wxString( wxT( ".clustered.prv" )));

      break;
      
    case FOLDING:
      command = application[ FOLDING ];

      if ( comboboxFoldingModel->GetValue() != wxString( wxT("none") ) )
      {
        parameters += wxString( wxT( " -model " ) ) + doubleQuote( comboboxFoldingModel->GetValue() );
      }

      parameters += wxString( wxT(" ") ) + doubleQuote( fileBrowserButtonTrace->GetPath() );
      
      if ( checkboxFoldingUseSemanticValues->IsChecked() )
      {
        if ( !foldingCSV.IsEmpty() )
        {
          parameters += wxString( wxT(" ") );
          parameters += doubleQuote( foldingCSV );
        }
      }
      else
      {
        parameters += wxString( wxT( " " ) );
        parameters += doubleQuote( expandVariables( textCtrlDefaultParameters->GetValue() ) ); // Event type
      }
      
      
      if ( textCtrlDefaultParameters->GetValue() == wxString( wxT( "--help" ) ))
      {
        helpOption = true;
      }
      
      break;
      
    case USER_DEFINED:
      
      tmpParams = expandVariables( textCtrlDefaultParameters->GetValue() );
      command = tmpParams.BeforeFirst( ' ' );
      parameters = tmpParams.AfterFirst( ' ' );
      
      break;
      
    default:
      // Third kind: registered application doesn't need a wrapper; app has the same name
      command = choiceApplication->GetString( choiceApplication->GetSelection() );
      
      break;
  }
  
  fullCommand = command;
  if ( !command.IsEmpty() && !parameters.IsEmpty() )
  { 
    // extend with parameters
    fullCommand += wxString( wxT( " " ) ) + parameters;
  }

  return ( fullCommand );
}


void RunScript::ShowWarning( wxString whichMessage )
{
  wxMessageDialog message( this, whichMessage, _( "Warning" ), wxOK | wxICON_EXCLAMATION );
  message.ShowModal();
}


void RunScript::ShowWarningUnreachableProgram( wxString program, TEnvironmentVar envVar, bool alsoPrintPath )
{
  wxString auxMessage;
  
  if ( envVar == PATH && alsoPrintPath )
  {
    alsoPrintPath = false;
  }
  
  auxMessage = wxString( wxT( "Unable to find:" ) );
  auxMessage += wxString( wxT( "\n\n\t" ) ) + program + wxString( wxT( "\n\n" ) );
  auxMessage += wxString( wxT("Please check that the program is reachable through the environment variable" ) );
  if ( alsoPrintPath )
  {
    auxMessage += wxString( wxT( " $PATH or " ) )  + environmentVariable[ envVar ];
  }
  auxMessage += wxString( wxT( " $" ) )  + environmentVariable[ envVar ] + wxString( wxT( "." ) );
  
  ShowWarning( auxMessage );
}
  

wxString RunScript::GetReachableCommand( TExternalApp selectedApp )
{
  wxString program, parameters;
  wxString readyCommand;
  wxString pathToProgram;

  wxString candidateCommand = GetCommand( program, parameters, selectedApp );
  if ( candidateCommand.IsEmpty() )
  {
    ShowWarning( wxString( wxT( "Empty command." )) );
  }
  else
  {
    if ( selectedApp == DEFAULT )
    {
      selectedApp = (TExternalApp)choiceApplication->GetSelection();
    } 
   
    switch ( selectedApp )
    {
        
      case DIMEMAS_GUI:
        pathToProgram = getEnvironmentPath( DIMEMAS_HOME );
        if ( !pathToProgram.IsEmpty() )
        {
          readyCommand = doubleQuote( pathToProgram + candidateCommand );
        }
        else
        {
          ShowWarningUnreachableProgram( program, DIMEMAS_HOME );
        }

        break;
        
      case DIMEMAS_WRAPPER:
        pathToProgram = getEnvironmentPath( PATH, program );
        if ( !pathToProgram.IsEmpty() )
        {
          readyCommand =  pathToProgram + candidateCommand;
        }
        else
        {
          pathToProgram = getEnvironmentPath( PARAVER_HOME );
          if ( !pathToProgram.IsEmpty() )
          {
            readyCommand =  pathToProgram + candidateCommand;
          }
          else
          {
            ShowWarningUnreachableProgram( program, PARAVER_HOME, true );
          }
        }
        
        break;
        
      case STATS_WRAPPER:
        pathToProgram = getEnvironmentPath( PATH, program );
        if ( !pathToProgram.IsEmpty() )
        {
          readyCommand =  pathToProgram + candidateCommand;
        }
        else
        {
          pathToProgram = getEnvironmentPath( PARAVER_HOME );
          if ( !pathToProgram.IsEmpty() )
          {
            readyCommand =  pathToProgram + candidateCommand;
          }
          else
          {
            ShowWarningUnreachableProgram( program, PARAVER_HOME, true );
          }
        }
        
        break;
        
      case CLUSTERING:
      case FOLDING:
      case USER_DEFINED:
      default:
        pathToProgram = getEnvironmentPath( PATH, program );
        if ( !pathToProgram.IsEmpty() )
        {
          readyCommand =  pathToProgram + candidateCommand;
        }
        else
        {
          ShowWarningUnreachableProgram( program, PATH );
        }      
        
        break;
    }
  }
  
  return ( readyCommand );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RUN
 */
void RunScript::OnButtonRunClick( wxCommandEvent& event )
{
  buttonRun->Enable( false );
  helpOption = false;  

  wxString readyCommand = GetReachableCommand();
  if ( !readyCommand.IsEmpty() )
  {
    if ( choiceApplication->GetSelection() == CLUSTERING )
    {
      // PRECOND: not empty
      clusteringXML = fileBrowserButtonClusteringXML->GetPath();
    }
    
    //executionStatus = -2;
    myProcess = new RunningProcess( this, readyCommand );
    myProcessPid = wxExecute( readyCommand, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, myProcess );

    if( !myProcessPid )
    {
      OnProcessTerminated( myProcessPid );
    }
  }

  buttonRun->Enable( true );
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_BUTTON_RUN
 */
void RunScript::OnButtonRunUpdate( wxUpdateUIEvent& event )
{
  // Check parameters
  bool active = ( myProcess == NULL );
  
  TExternalApp selectedApp = (TExternalApp)choiceApplication->GetSelection();
   
  switch ( selectedApp )
  {
    case DIMEMAS_WRAPPER:
      active &= !fileBrowserButtonTrace->GetPath().IsEmpty();
      active &= !fileBrowserButtonDimemasCFG->GetPath().IsEmpty();
      active &= !textCtrlOutputTrace->GetValue().IsEmpty();      
      break;
      
    case STATS_WRAPPER:
      active &= !fileBrowserButtonTrace->GetPath().IsEmpty();      
      break;
      
    case CLUSTERING:
      active &= !fileBrowserButtonTrace->GetPath().IsEmpty();
      active &= !fileBrowserButtonClusteringXML->GetPath().IsEmpty();      
      break;

    case FOLDING:
      active &= !fileBrowserButtonTrace->GetPath().IsEmpty();      
      break;

    case USER_DEFINED:
      active &= !textCtrlDefaultParameters->GetValue().IsEmpty();
      break;

    default:
      break;
  }
          
  buttonRun->Enable( active );
}


void RunScript::OnProcessTerminated( int pid )
{
  myProcessPid = 0;
  
  if ( pid != 0 && pid == pidDimemasGUI )
    pidDimemasGUI = 0;
    
  delete myProcess;
  myProcess = NULL;
}


void RunScript::AppendToLog( wxString msg, bool formatOutput )
{
  if ( !helpOption || formatOutput )
  {
    TExternalApp selectedApp = (TExternalApp)choiceApplication->GetSelection();
   
    switch ( selectedApp )
    {
 //     case DIMEMAS_WRAPPER:
 //       msg = insertLog( msg, extensionsDimemas );
 //       break;
        
      default:
        msg = insertLog( msg, extensions );
        break;
    }
  }
  
  if( formatOutput )
    listboxRunLog->AppendToPage( wxT("<TT>") + msg + wxT("</TT><BR>") );
  else
    listboxRunLog->AppendToPage( wxT("<TT>") + msg + wxT("</TT>") );
  
  //std::cout << msg << std::endl;
  
  int x, y;
  listboxRunLog->GetVirtualSize( &x, &y );
  listboxRunLog->Scroll( -1, y );
  listboxRunLog->Update();
}


/*!
 * wxEVT_IDLE event handler for ID_RUN_APPLICATION
 */

void RunScript::OnIdle( wxIdleEvent& event )
{
  if ( myProcess != NULL && myProcess->HasInput() )
  {
    event.RequestMore();
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLEAR_LOG
 */

void RunScript::OnButtonClearLogClick( wxCommandEvent& event )
{
  listboxRunLog->SetPage( wxT( "" ) );
}


wxString RunScript::doubleQuote( const wxString& path )
{
  return wxString( wxT( "\"" ) ) + path + wxString( wxT( "\"" ) );
}


void RunScript::adaptWindowToApplicationSelection()
{
  wxString toolTip;
  int currentChoice = choiceApplication->GetSelection();
  
  switch ( currentChoice )
  {
    case DIMEMAS_WRAPPER:
      toolTip = wxString( wxT( "Extra parameters passed to the script\n"
                               "%TRACE refers to input trace" ) );

      labelTextCtrlDefaultParameters->SetLabel( wxT( "Parameters" ) ); 
      labelTextCtrlDefaultParameters->SetToolTip( toolTip );
                                                  
      textCtrlDefaultParameters->SetValidator( wxTextValidator( wxFILTER_NONE ));
      textCtrlDefaultParameters->SetToolTip( toolTip );

      labelTextCtrlDefaultParameters->Show();
      textCtrlDefaultParameters->Show();
      break;
      
    case STATS_WRAPPER:
      toolTip = wxString( wxT( "Extra parameters passed to 'stats'\n"
                               "-events_histo[:type1[-type2],...]\n"
                               "-thread_calls[:type1[-type2],...]\n" ) );
      
      labelTextCtrlDefaultParameters->SetLabel( wxT( "Parameters" ) ); 
      labelTextCtrlDefaultParameters->SetToolTip( toolTip );
                                                  
      textCtrlDefaultParameters->SetValidator( wxTextValidator( wxFILTER_NONE ));
      textCtrlDefaultParameters->SetToolTip( toolTip );

      statsTextCtrlOutputName->SetValue( fileBrowserButtonTrace->GetPath() );
                                                  
      labelTextCtrlDefaultParameters->Show();
      textCtrlDefaultParameters->Show();
      break;
      
    case CLUSTERING:
      labelTextCtrlDefaultParameters->Hide();
      textCtrlDefaultParameters->Hide();
      if ( clusteringCSV.IsEmpty() )
      {
        checkBoxClusteringUseSemanticWindow->Enable( false );
        
        // This two others are chained by their own Update_UI with previous one
        // Anyway, it's made here
        checkBoxClusteringCSVValueAsDimension->Enable( false );
        checkBoxClusteringNormalize->Enable( false );
      }
      break;
      
    case FOLDING:
      toolTip = wxString( wxT( "Event type that determines the folded regions."
                               " Allowed formats include either numerical or string"
                               " (i.e. 90000001 or 'Cluster ID')." ) );  
    
      labelTextCtrlDefaultParameters->SetLabel( wxT( "Event type/name" ) ); 
      labelTextCtrlDefaultParameters->SetToolTip( toolTip );

      textCtrlDefaultParameters->SetValidator( wxTextValidator( wxFILTER_NONE ));
      textCtrlDefaultParameters->SetToolTip( toolTip );
      textCtrlDefaultParameters->SetValue( wxString( wxT( "Cluster ID" ) ) );
      
      checkboxFoldingUseSemanticValues->Enable( !foldingCSV.empty() );
      comboboxFoldingModel->SetStringSelection(_("intel-sandybridge"));

      labelTextCtrlDefaultParameters->Show();
      textCtrlDefaultParameters->Show();
      break;
      
    case USER_DEFINED:
    default:
      toolTip = wxString( wxT( "Command and parameters to execute\n"
                               "%TRACE refers to input trace" ) );
                               
      labelTextCtrlDefaultParameters->SetLabel( wxT( "Command" ) );
      labelTextCtrlDefaultParameters->SetToolTip( toolTip );
                                                 
      textCtrlDefaultParameters->SetValidator( wxTextValidator( wxFILTER_NONE ));
      textCtrlDefaultParameters->SetToolTip( toolTip );
                                                  
      labelTextCtrlDefaultParameters->Show();
      textCtrlDefaultParameters->Show();
      break;
  }

  tunePrvLinksForFolding = ( currentChoice == FOLDING );

  dimemasSection->Show( currentChoice == DIMEMAS_WRAPPER );
  statsSection->Show( currentChoice == STATS_WRAPPER );
  clusteringSection->Show( currentChoice == CLUSTERING );
  adaptClusteringAlgorithmParameters();
  foldingSection->Show( currentChoice == FOLDING );

  Layout();
}


/*!
 * wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_CHOICE_APPLICATION
 */
void RunScript::OnChoiceApplicationSelected( wxCommandEvent& event )
{
  adaptWindowToApplicationSelection();
}


wxString RunScript::expandVariables( wxString command )
{
  command.Replace( wxT( "%TRACE" ), doubleQuote( fileBrowserButtonTrace->GetPath() ) );

  return command;
}


struct gthan
{
  bool operator()( const std::pair< int, wxString > &a, const std::pair< int, wxString > &b )
  {   
    return ( a.first > b.first ) ||
           ( a.first == b.first && a.second.Len() > b.second.Len() );
  }   
} greaterThan;


bool RunScript::readFoldingTag( wxString rawLine )
{
  int initTag = rawLine.Find( tagFoldingOutputDirectory );
  if ( initTag != wxNOT_FOUND )
  {
    int endLine = rawLine.Len();
    int initPath = initTag + tagFoldingOutputDirectory.Len();
    
    foldingOutputDirectory = rawLine.Mid( initPath, endLine - initPath );
  }
  
  return ( initTag != wxNOT_FOUND );
}


// Turn spaces to &nsbp for all the line
wxString RunScript::rawFormat( wxString rawLine )
{
  rawLine.Replace( wxT( " " ), wxT( "&nbsp;" ) );

  return rawLine;
}


wxString RunScript::insertLinks( wxString rawLine, wxArrayString extensions )
{
  // Detect all the ocurrences of every given extension
  std::vector< std::pair< int, wxString > > extensionsPositions; // {(4,.cfg),(6,.prv),(6.prv.gz)}
  
  for ( size_t i = 0; i < extensions.Count(); ++i )
  {
    int endLine = rawLine.Len();
    int initSubStr = 0; // Start from the begin
    while ( initSubStr < endLine )
    {
      wxString subStr = rawLine.Mid( initSubStr, endLine - initSubStr );
      int initSuffixPos = subStr.Find( extensions[i] );

      if ( initSuffixPos != wxNOT_FOUND )
      {
        // Remember (position, extension)
        int globalPos = initSubStr + initSuffixPos;

        extensionsPositions.push_back( std::make_pair( globalPos, extensions[i] ) );
        
        // Advance: compute new end of the substring 
        initSubStr = globalPos + extensions[i].Len();
      }
      else
      {
        // No more current extensions; exit loop to try next extension
        initSubStr = endLine;
      }
    }
  }
  
  if ( extensionsPositions.size() == 0 )
  {
    // No match found for any extension!!
    rawLine = rawFormat( rawLine );
  }
  else 
  {  
    // Sort extensionsPosition by
    //   1. position, descending
    //   2. longest suffix first
    // Ex:
    //   Before :{ (4,.cfg), (6,.prv), (25,.prv), (6,.prv.gz) }
    //   After  :{ (25,.prv), (6,.prv.gz), (6,.prv), (4,.cfg) }
   
    std::sort( extensionsPositions.begin(), extensionsPositions.end(), greaterThan );
    
    // Clean extensionsPosition
    //   1. with unique positions, only longest suffix is kept (.prv.gz vs .prv)
    // Ex:
    //   Before :{ (25,.prv), (6,.prv.gz), (6,.prv), (4,.cfg) }
    //   After  :{ (25,.prv), (6,.prv.gz), (4,.cfg) }
    std::vector< std::pair< int, wxString > > auxExtensionsPositions;
    for ( std::vector< std::pair< int, wxString > >::iterator it = extensionsPositions.begin();
            it != extensionsPositions.end(); ++it )
    {
      if ( auxExtensionsPositions.empty() )
      {
        auxExtensionsPositions.push_back( *it );
      }
      else if ( it->first != auxExtensionsPositions.back().first )
      {
        auxExtensionsPositions.push_back( *it );
      }
    }
    
    std::swap( extensionsPositions, auxExtensionsPositions );
    

    // If available, get trace path from wxFilePickerCtrl
    wxString selectedTracePath =
        wxFileName( fileBrowserButtonTrace->GetPath() ).GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    // Now, traverse rawLine using extensionPositions.
    wxString auxLine;  // where output line is being built, starting from the tail.
    int endLine = rawLine.Len();
    int endSubStr = endLine;
    int oldEndSubStr = endSubStr;
    size_t i = 0;
    while ( i < extensionsPositions.size() )
    {
      // New end of the substring
      int currentEndSubStr = extensionsPositions[i].first + 
                             extensionsPositions[i].second.Len();
      
      // Anything after it must be nbspaced; for sure it doesn't belong to a link
      wxString trashTail = rawFormat(
              rawLine.Mid( currentEndSubStr, endSubStr - currentEndSubStr ) );
        
      // Advance endSubStr, keeping the old one
      oldEndSubStr = endSubStr;
      endSubStr = currentEndSubStr;
      
      // Find trace candidate
      wxFileName candidateFile;
      int currentPos = 0; // always start from the beginning
      bool candidateFound = false;
      int initSuffixPos = extensionsPositions[i].first;
      wxString candidateName;
      bool TRIM_LEFT = false;
      while ( currentPos < initSuffixPos && !candidateFound )
      {
        // Normalize
        candidateName = rawLine.Mid( currentPos, endSubStr - currentPos );
        candidateFile = wxFileName( candidateName );
        //candidateFound = ( candidateFile.Normalize() && candidateFile.FileExists() );
        
        if ( tunePrvLinksForFolding )
        {
          candidateFound = candidateFile.MakeAbsolute( foldingOutputDirectory );
        }
        else
        {
          candidateFound = candidateFile.Normalize();
        }
     
        candidateFound = candidateFound &&
                         wxFileName::FileExists( candidateFile.GetFullPath().Trim( TRIM_LEFT ) ); 

        if ( !candidateFound )
        {
          candidateFile = wxFileName( candidateName );
          candidateFound =
              ( candidateFile.Normalize( wxPATH_NORM_ALL, selectedTracePath ) &&
                candidateFile.FileExists() );
        }

        // At this moment, always filter cfgs from Dimemas
        if ( candidateFound && 
             CFGLoader::isDimemasCFGFile (
                    std::string( candidateFile.GetFullPath().Trim( TRIM_LEFT ).mb_str() )))
          candidateFound = false;
    
        ++currentPos;
      }

      if ( candidateFound )
      {
        --currentPos;

        wxString linkName = rawFormat( candidateName );
        wxString linkFullPath = candidateFile.GetFullPath().Trim( TRIM_LEFT );
        
        wxString currentLink;
        if ( tunePrvLinksForClustering && 
              extensionsPositions[i].second.Cmp( wxString( wxT( ".prv" ))) == 0 )
        {
          currentLink = linkName + wxString( wxT(" ") );
          currentLink += wxT("<A HREF=\"") + linkFullPath + wxT("\">") +
                         wxString( wxT("(analyze ClusterIds)") ) +
                         wxT("</A>");
        }
        else
        {
          currentLink = wxT("<A HREF=\"") + linkFullPath + wxT("\">") + linkName + wxT("</A>");
        }
        
        auxLine = currentLink + trashTail + auxLine;
        endSubStr = currentPos;
        
        // Advance vector of positions-extensions to next useful (position, ext)
        // Ex: After succesful detection of link, any extension inside must be ignored.
        //   Good link:                    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
        //   Dangerous line: (...) bla bla /home/user/traces/current.cfgs.chop.prv.gz
        //   Extension to avoid!!:                                  ^^^^^
        while ( i < extensionsPositions.size() && extensionsPositions[i].first > currentPos )
        {
          ++i;
        }
      }
      else
      {
        // Advance vector positions-extensions to next useful (position, ext)
        // It's the next one, because we didn't consume any rawLine character (no links!)
        ++i;
        endSubStr = oldEndSubStr;
      }
    }

    if ( endSubStr > 0 )
    {
      wxString trashHead = rawLine.Mid( 0, endSubStr );
      auxLine = rawFormat( trashHead ) + auxLine;
    }

    // Finally we copy it.
    rawLine = auxLine;
  }
  
  return rawLine;
}


wxString RunScript::insertLog( wxString rawLine, wxArrayString extensions )
{
  wxString formattedLine;

  if (( choiceApplication->GetSelection() == FOLDING ) && readFoldingTag( rawLine ) )
  {
    formattedLine = rawFormat( rawLine );
  }
  else
  {
    formattedLine = insertLinks( rawLine, extensions );
  }
  
  return formattedLine;
}


std::string RunScript::getHrefFullPath( wxHtmlLinkEvent &event )
{
  std::string hrefFullPath = std::string( event.GetLinkInfo().GetHref().mb_str() );

  return hrefFullPath;
}


bool RunScript::matchHrefExtension( wxHtmlLinkEvent &event, const wxString extension )
{
  return ( event.GetLinkInfo().GetHref().Right( extension.Len() ).Cmp( extension ) == 0 );
}


/*!
 * wxEVT_COMMAND_HTML_LINK_CLICKED event handler for ID_LISTBOX_RUN_LOG
 */

void RunScript::OnListboxRunLogLinkClicked( wxHtmlLinkEvent& event )
{
  wxString auxCommand;
  
  if ( tunePrvLinksForClustering && matchHrefExtension( event, wxT(".clustered.prv") ) )
  {
    paraverMain::myParaverMain->DoLoadTrace( getHrefFullPath( event ) );
    std::vector< Trace * > loadedTraces = paraverMain::myParaverMain->GetLoadedTraces();
    Trace *clusteredTrace = loadedTraces.back();
    
    // Create cluster id window
    Window *sourceWindow = paraverMain::myParaverMain->GetClusteringWindow();
    Window *newWindow = paraverMain::myParaverMain->createBaseWindow( wxString( wxT( "ClusterId" ) ) );
    TTime beginZoomTime = sourceWindow->getWindowBeginTime() - clusteredTrace->getCutterOffset();

    TTime endZoomTime = sourceWindow->getWindowEndTime() - clusteredTrace->getCutterOffset();
    newWindow->setWindowBeginTime( beginZoomTime );
    newWindow->setWindowEndTime( endZoomTime );
    newWindow->addZoom( beginZoomTime, endZoomTime, 0, newWindow->getWindowLevelObjects() - 1 );

    Filter *filter = newWindow->getFilter();
    filter->insertEventType( (TEventType)90000001 );
    filter->setEventTypeFunction( std::string( "=" ) ); // how to get name?

    newWindow->setLevelFunction( THREAD, "Last Evt Val" );

    newWindow->setDrawCommLines( false );
    newWindow->setDrawFlags( false );
    newWindow->setCodeColorMode();
    newWindow->setDrawModeObject( DRAW_MAXIMUM );
    newWindow->setDrawModeTime( DRAW_MAXIMUM );
    
    newWindow->setWidth( sourceWindow->getWidth() ); 
    newWindow->setHeight( sourceWindow->getHeight() );
    newWindow->setPosX( sourceWindow->getPosX() );
    newWindow->setPosY( sourceWindow->getPosY() +
                        sourceWindow->getHeight() +
                        paraverMain::myParaverMain->GetDefaultTitleBarHeight() );

    newWindow->setComputeYMaxOnInit( true );
    
    paraverMain::myParaverMain->insertInTree( newWindow );
  }
  else if ( matchHrefExtension( event, wxT(".prv") ) || matchHrefExtension( event, wxT(".prv.gz")))
  {
    paraverMain::myParaverMain->DoLoadTrace( getHrefFullPath( event ) );
  }
  else if ( matchHrefExtension( event, wxT(".dat" )))
  {
    wxString command = wxString( wxT( "libreoffice --calc " ) ) +
                       doubleQuote( wxString( getHrefFullPath( event ).c_str(), wxConvUTF8 ) );
    runDetachedProcess( command );    
  }
  else if ( matchHrefExtension( event, wxT(".gnuplot" )))
  {
    // gnuplot needs to be executed in the same dir where .csv is
    
    wxString currentWorkingDir = wxGetCwd(); // keep the old
    
    // change to dir pointed by link
    wxString tmpFile = wxString( getHrefFullPath( event ).c_str(), wxConvUTF8 );
    wxFileName tmpDir( tmpFile );
    tmpDir = wxFileName( tmpDir.GetPathWithSep() );
    tmpDir.SetCwd();

    // prepare command
    wxString command = wxString( wxT( "gnuplot -persist " ) ) +
                       doubleQuote( tmpFile );
    
    runDetachedProcess( command );
    
    wxFileName::SetCwd( currentWorkingDir ); // restore the old
  }
  else if ( matchHrefExtension( event, _(".cfg")))
  {
    if ( CFGLoader::isCFGFile( getHrefFullPath( event ) ) )
    {
      if ( paraverMain::myParaverMain->GetLoadedTraces().size() > 0 )
      {
        paraverMain::myParaverMain->DoLoadCFG( getHrefFullPath( event )  );
      }
      else
      {
        ShowWarning( wxString( wxT( "No trace loaded." ) ) );
      }
    }
  }
  else if ( matchHrefExtension( event, _(".xml")))
  {
    std::string traceName = std::string( wxFileName( fileBrowserButtonTrace->GetPath() ).GetFullPath().mb_str() );

    bool loadTrace = true;
    std::string strXmlFile = getHrefFullPath( event );

    paraverMain::myParaverMain->ShowCutTraceWindow( traceName, loadTrace, strXmlFile );
  }
  else
  {
    event.Skip();
  }
}


void RunScript::runDetachedProcess( wxString command, bool checkPidDimemasGUI )
{
  //executionStatus = -2;
  RunningProcess *localProcess = new RunningProcess( this, command );

  int myProcessPid = wxExecute( command, wxEXEC_ASYNC, localProcess );
  if( !myProcessPid )
  {
    ShowWarning( wxT( "Unable to execute command. Please check it and rerun" ) );
    
    delete localProcess;
    localProcess = NULL;
  }
  else
  {
    if ( checkPidDimemasGUI )
      pidDimemasGUI = myProcessPid;
    
    localProcess->HasInput();
    localProcess->Detach();
    
    // After Detach is done, localProcess shouldn't be deleted!
  }  
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DIMEMAS_GUI
 */
void RunScript::OnButtonDimemasGuiClick( wxCommandEvent& event )
{
  wxString command = GetReachableCommand( DIMEMAS_GUI );
  if( !command.IsEmpty() && pidDimemasGUI == 0 )
  {  
    bool checkPidDimemasGUI = true;
    runDetachedProcess( command, checkPidDimemasGUI );
  }
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_BUTTON_DIMEMAS_GUI
 */

void RunScript::OnButtonDimemasGuiUpdate( wxUpdateUIEvent& event )
{
  // bool active = ( myProcess == NULL );
  bool active = ( pidDimemasGUI == 0 );
  
  if ( active && !buttonDimemasGUI->IsEnabled() )
    buttonDimemasGUI->SetToolTip( _("Edit Dimemas Configuration File using DimemasGUI.") );
    
  if ( !active && buttonDimemasGUI->IsEnabled() )
    buttonDimemasGUI->SetToolTip( _("DimemasGUI instance running...") );    
          
  buttonDimemasGUI->Enable( active );
}


/*!
 * wxEVT_UPDATE_UI event handler for wxID_LABELCOMMANDPREVIEW
 */

void RunScript::OnLabelcommandpreviewUpdate( wxUpdateUIEvent& event )
{
  wxString dummyCommand, dummyParameter;
  event.SetText( GetCommand( dummyCommand, dummyParameter ) );
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_CHECKBOX_CLUSTERING_SEMVAL_AS_CLUSTDIMENSION
 */

void RunScript::OnCheckboxClusteringSemvalAsClustdimensionUpdate( wxUpdateUIEvent& event )
{
  checkBoxClusteringCSVValueAsDimension->Enable(
          checkBoxClusteringUseSemanticWindow->IsEnabled() &&
          checkBoxClusteringUseSemanticWindow->IsChecked() );
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_CHECKBOX_CLUSTERING_NORMALIZE
 */

void RunScript::OnCheckboxClusteringNormalizeUpdate( wxUpdateUIEvent& event )
{
  checkBoxClusteringNormalize->Enable(
          checkBoxClusteringCSVValueAsDimension->IsEnabled() &&
          checkBoxClusteringCSVValueAsDimension->IsChecked() );
}


void RunScript::setApp( TExternalApp whichApp )
{
  choiceApplication->Select( whichApp ); 
  adaptWindowToApplicationSelection();
}


void RunScript::setDimemas()
{
  tunePrvLinksForClustering = false;
  tunePrvLinksForFolding = false;

  setApp( DIMEMAS_WRAPPER );
}


void RunScript::setStats()
{
  tunePrvLinksForClustering = false;
  tunePrvLinksForFolding = false;

  setApp( STATS_WRAPPER );
}


void RunScript::setClustering( wxString whichClusteringCSV )
{
  clusteringCSV = whichClusteringCSV;
  tunePrvLinksForClustering = true;
  tunePrvLinksForFolding = false;

  setApp( CLUSTERING );
}


void RunScript::setFolding( wxString whichFoldingCSV )
{
  foldingCSV = whichFoldingCSV;
  tunePrvLinksForClustering = false;
  tunePrvLinksForFolding = true;

  setApp( FOLDING );
}


void RunScript::setUserDefined()
{
  tunePrvLinksForClustering = false;
  tunePrvLinksForFolding = false;

  setApp( USER_DEFINED );
}


void RunScript::adaptClusteringAlgorithmParameters()
{
  // Refinement parameters remain visible, but enabled/disabled
  bool tuneByHand = clusteringCheckBoxRefinementTune->IsChecked();
  clusteringLabelRefinementEpsilon->Enable( tuneByHand );
  clusteringLabelRefinementEpsilonMin->Enable( tuneByHand );
  clusteringTextBoxRefinementEpsilonMin->Enable( tuneByHand );
  clusteringLabelRefinementEpsilonMax->Enable( tuneByHand );
  clusteringTextBoxRefinementEpsilonMax->Enable( tuneByHand );
  clusteringLabelRefinementSteps->Enable( tuneByHand );
  clusteringTextBoxRefinementSteps->Enable( tuneByHand );
  clusteringLabelRefinementMinPoints->Enable( tuneByHand );
  clusteringTextBoxRefinementMinPoints->Enable( tuneByHand );

  bool clusteringSelected = ( choiceApplication->GetSelection() == CLUSTERING );
  clusteringSizerDBScan->Show( clusteringSelected && clusteringRadioDBScan->GetValue() );
  clusteringSizerRefinement->Show( clusteringSelected && clusteringRadioRefinement->GetValue() );
  clusteringAlgorithmLineSeparator->Show( clusteringSelected && !clusteringRadioXMLDefined->GetValue() );
}


void RunScript::OnRadiobuttonClusteringXmldefinedSelected( wxCommandEvent& event )
{
  adaptClusteringAlgorithmParameters();
  Layout();
}


void RunScript::OnRadiobuttonClusteringDbscanSelected( wxCommandEvent& event )
{
  adaptClusteringAlgorithmParameters();
  Layout();
}


void RunScript::OnRadiobuttonClusteringRefinementSelected( wxCommandEvent& event )
{
  adaptClusteringAlgorithmParameters();
  Layout();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_CLUSTERING_REFINEMENT_TUNE
 */

void RunScript::OnCheckboxClusteringRefinementTuneClick( wxCommandEvent& event )
{
  adaptClusteringAlgorithmParameters();
  Layout();
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_BUTTON_KILL
 */

void RunScript::OnButtonKillUpdate( wxUpdateUIEvent& event )
{
  event.Enable( myProcess != NULL );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_KILL
 */

void RunScript::OnButtonKillClick( wxCommandEvent& event )
{
  if( myProcessPid != 0 )
  {
    if( wxProcess::Kill( myProcessPid, wxSIGKILL, wxKILL_CHILDREN ) != wxKILL_OK )
      AppendToLog( wxT( "Error: Process not killed" ) );
    else
      AppendToLog( wxT( "Process killed!" ) );
  }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_TRACE
 */

void RunScript::OnTextctrlTraceTextUpdated( wxCommandEvent& event )
{
  if ( choiceApplication->GetSelection() == STATS_WRAPPER )
  {
    statsTextCtrlOutputName->SetValue( fileBrowserButtonTrace->GetPath() );
  }
  else if ( choiceApplication->GetSelection() == DIMEMAS_WRAPPER )
  {
    textCtrlOutputTrace->SetValue( wxString( (
            LocalKernel::composeName( std::string( event.GetString().mb_str() ),
                                      std::string( "sim" ) ) + PRV_SUFFIX ).c_str(), wxConvUTF8 ) );
  }
}


//bool RunScript::shellCommand( const wxString& program, const wxString& whichFile )
//{
  //wxString command = _( "/bin/sh -c '" ) + program + _(" ") + whichFile + _(" 1>&- 2>&- ; echo $?'");
/*
  wxString command = program + _(" ") + whichFile + _(" 1>&- 2>&- ; echo $?");

  wxArrayString outputArray;
  int executionPid = wxExecute( command, wxEXEC_ASYNC, outputArray );
  
  std::cout << command << " " << executionPid << std::endl;
  for ( unsigned int i = 0; i < outputArray.GetCount(); ++i )
  {
    std::cout <<  "exit: " << outputArray[ i ] << std::endl;
  }  
  
  return ( outputArray.Last() == _("0") );
*/  
  //wxString command = _( "/bin/sh -c '" ) + program + _(" ") + whichFile + _(" 1>&- 2>&-'");
/*  wxString command = program + _(" ") + whichFile;
  executionStatus = -2;
  wxProcess* tmpmyProcess = new RunningProcess( this, command );
  int tmpmyProcessPid = wxExecute( command, wxEXEC_ASYNC, tmpmyProcess );
std::cout << "Pid: " << command << " " << tmpmyProcessPid 
  << " executionStatus: " << executionStatus << std::endl;


  //return ( executionStatus == 0 );
  */
//}


bool RunScript::existCommand( const wxString& program, const wxString& parameter )
{
  wxString command = _( "/bin/sh -c '" ) + program + _(" ") + parameter + _(" 1>&- 2>&-'");
  return ( wxExecute( command, wxEXEC_SYNC ) == 0 );
}

void RunScript::runCommand( const wxString& program, const wxString& parameter )
{
  wxString command = _( "/bin/sh -c '" ) + program + _(" ") + parameter + _(" 1>&- 2>&-'");
  wxExecute( command ); // ASYNC
}


// Idea taken from wxMakeShellCommand
// TODO Put apart in a class
void RunScript::OnBitmapbuttonClusteringXmlClick( wxCommandEvent& event )
{
  wxString fileToEdit = fileBrowserButtonClusteringXML->GetPath();
  wxString command;
#ifdef WIN32
  command = _( "wordpad.exe " ) + fileToEdit;
  wxExecute( command );
#else
  
  // TODO -> PUT IN CLASSES
  vector< wxString > editor;
  vector< wxString > versionParameter;
  
  editor.push_back( _( "gvim" ) );
  versionParameter.push_back( _( "--version" ) );
  editor.push_back( _( "nedit" ) );
  versionParameter.push_back( _( "-version" ) );
  editor.push_back( _( "gedit" ) );
  versionParameter.push_back( _( "--version" ) );

  size_t i;
  for ( i = 0; i < editor.size(); ++i )
  {
    if ( existCommand( editor[ i ], versionParameter[ i ] ) )
    {
      runCommand( editor[ i ], fileToEdit );
      break;
    }
  }
  
  if ( i == 3 )
  {
    wxMessageBox( _( "Unable to find gvim, nedit or gedit. Please check $PATH variable." ), _( "Edit Clustering Configuration XML" ) );
  }
#endif
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_BITMAPBUTTON_CLUSTERING_XML
 */

void RunScript::OnBitmapbuttonClusteringXmlUpdate( wxUpdateUIEvent& event )
{
  event.Enable( !textCtrlClusteringXML->GetValue().IsEmpty() );
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_CHECKBOX_CLUSTERING_GENERATE_SEQUENCES
 */

void RunScript::OnCheckboxClusteringGenerateSequencesUpdate( wxUpdateUIEvent& event )
{
  clusteringRadioGenerateSeqNumbered->Enable( checkBoxClusteringGenerateSeq->IsChecked() );
  clusteringRadioGenerateSeqFASTA->Enable( checkBoxClusteringGenerateSeq->IsChecked() );
}




/*!
 * wxEVT_UPDATE_UI event handler for ID_CHECKBOX_FOLDING_USE_EVENT_TYPE
 */

void RunScript::OnCheckboxFoldingUseSemanticValueUpdate( wxUpdateUIEvent& event )
{
  labelTextCtrlDefaultParameters->Enable( !checkboxFoldingUseSemanticValues->IsChecked() );
  textCtrlDefaultParameters->Enable( !checkboxFoldingUseSemanticValues->IsChecked() );
}

