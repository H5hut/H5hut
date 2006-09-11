#include <TApplication.h>
#include <TSystem.h>
#include <TGClient.h>
#include <TGString.h>
#include <TString.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH2.h>
#include <TH3.h>
#include <TRandom.h>
#include <TRootEmbeddedCanvas.h>
#include <TGraph.h>

#include <string>
#include <iostream>

#include "TH5MainFrame.h"
#include "TH5Dataset.h"
#include "TH5Legend.h"

using namespace std;

ClassImp(TH5MainFrame)
  
TH5MainFrame::TH5MainFrame(const TGWindow *p, UInt_t w, UInt_t h, Int_t ssr)
  : TGMainFrame(p, w, h), fSubsamplingRate(ssr) {
  
  //initialize pointers to datasets:
  for (Int_t n=0; n<5; ++n) {
    fDataset[n] = NULL;
  }

  fNStepMax = 0;

  // some predefined layout hints: numbers are left, right, top, bottom
  fLayoutLeftflush = new TGLayoutHints(kLHintsLeft,5,5,5,5);
  fLayoutRightflush = new TGLayoutHints(kLHintsRight,5,5,5,5);
  fLayoutCentered = new TGLayoutHints(kLHintsCenterX,5,5,15,5);

  //--------------------------------------------------------------------------
  // main canvas for plotting:
  fEcanvas = new TRootEmbeddedCanvas ("Ecanvas",this,w,h);
  fLayoutCanvas = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1);
  AddFrame(fEcanvas, fLayoutCanvas);

  //--------------------------------------------------------------------------
  // file entry bar (includes Exit button)
  fFileEntryBar = new TGHorizontalFrame(this, 600,40);

  fFileLabel = new TGLabel(fFileEntryBar,"Enter filename: ");

  fFileEntry = new TGTextEntry(fFileEntryBar, fFilenameBuffer = new TGTextBuffer(100));
  fFileEntry->Resize(400, fFileEntry->GetDefaultHeight());
  fFileEntry->SetToolTipText("Enter the filename, dude!");
  fFileEntry->Connect("ReturnPressed()","TH5MainFrame",this,"LoadFile()");

  fLoad = new TGTextButton(fFileEntryBar," &Load ");
  fLoad->Connect("Clicked()","TH5MainFrame",this,"LoadFile()");

  fExit = new TGTextButton(fFileEntryBar," &Exit ","gApplication->Terminate()");

  fFileEntryBar->AddFrame(fFileLabel, fLayoutLeftflush); 
  fFileEntryBar->AddFrame(fFileEntry, fLayoutLeftflush); 
  fFileEntryBar->AddFrame(fLoad, fLayoutLeftflush); 
  fFileEntryBar->AddFrame(fExit, fLayoutRightflush); 

  AddFrame(fFileEntryBar, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  //--------------------------------------------------------------------------
  // work space: (everything below the file entry bar)
  fWorkspace = new TGHorizontalFrame(this, 600,40);

  //--------------------------------------------------------------------------
  // selection space: for file and step selection
  fSelspace = new TGVerticalFrame(fWorkspace, 300,100);

  // group frame for file selection:
  fFileSelection = new TGGroupFrame(fSelspace, "File selection",kVerticalFrame);
  fFileSelection->SetTitlePos(TGGroupFrame::kLeft);

  for (Int_t n=0; n<5; ++n) {
    fFileButton[n] = new TGCheckButton(fFileSelection, "      no file loaded yet         ",200);
    fFileButton[n]->SetState(kButtonDisabled);
    fFileSelection->AddFrame(fFileButton[n], new TGLayoutHints(kLHintsLeft,5,5,3,3));
    TString slotCommand = TString("ActivateFile(Int_t=");
    slotCommand+=n;
    slotCommand+=TString(")");
    fFileButton[n]->Connect("Clicked()","TH5MainFrame",this,slotCommand.Data());  
  } 
  
  fSelspace->AddFrame(fFileSelection, fLayoutLeftflush); 

  // group frame for step selection:
  fStepSelection = new TGGroupFrame(fSelspace, "Step selection",kVerticalFrame);
  fStepSelection->SetTitlePos(TGGroupFrame::kLeft);

  fStepSlider = new TGHSlider(fStepSelection, 220, kSlider1 | kScaleDownRight, stepSliderID);
  fStepSlider->SetRange(0,100);
  fStepSlider->Connect("PositionChanged(Int_t)", "TH5MainFrame", this, "DoStepSlider(Int_t)");
  fStepSlider->Connect("Released()", "TH5MainFrame", this, "DoLastPlot");

  // various subframes needed for layout below step slider:
  fStepSposTEFrame = new TGHorizontalFrame(fStepSelection);
  fStepTEFrame = new TGVerticalFrame(fStepSposTEFrame);
  fSposTEFrame = new TGVerticalFrame(fStepSposTEFrame);
  fPrevStepFrame = new TGVerticalFrame(fStepSposTEFrame);
  fNextStepFrame = new TGVerticalFrame(fStepSposTEFrame);

  fStepLabel = new TGLabel(fStepTEFrame,"Step");
  fStepSliderTE = new TGTextEntry(fStepTEFrame, fStepSliderTB = new TGTextBuffer(5), stepSliderTEID);
  fStepSliderTB->AddText(0, "0");
  fStepSliderTE->Connect("TextChanged(char*)", "TH5MainFrame", this, "DoStepText(char*)");
  fStepTEFrame->AddFrame(fStepLabel, new TGLayoutHints(kLHintsLeft,2,1,1,0));
  fStepTEFrame->AddFrame(fStepSliderTE, new TGLayoutHints(kLHintsCenterX,2,1,0,0));

  fSposLabel = new TGLabel(fSposTEFrame,"Spos");
  fSposSliderTE = new TGTextEntry(fSposTEFrame, fSposSliderTB = new TGTextBuffer(5), sposSliderTEID);
  fSposSliderTB->AddText(0, "0");
  fSposSliderTE->Connect("TextChanged(char*)", "TH5MainFrame", this, "DoStepText(char*)");
  fSposSliderTE->Connect("ReturnPressed()","TH5MainFrame",this,"DoStepTextReturn(char*)");
  fSposTEFrame->AddFrame(fSposLabel, new TGLayoutHints(kLHintsLeft,2,1,1,0));
  fSposTEFrame->AddFrame(fSposSliderTE, new TGLayoutHints(kLHintsCenterX,2,1,0,0));

  fPrevStepLabel = new TGLabel(fPrevStepFrame,"   ");
  fPrevButton = new TGTextButton(fPrevStepFrame," Pre&v ");
  fPrevButton->Connect("Clicked()","TH5MainFrame",this,"PlotNextStep(Int_t=-1)");
  fPrevStepFrame->AddFrame(fPrevStepLabel, new TGLayoutHints(kLHintsLeft,2,1,1,0));
  fPrevStepFrame->AddFrame(fPrevButton, new TGLayoutHints(kLHintsRight,2,1,0,0));

  fNextStepLabel = new TGLabel(fNextStepFrame,"   ");
  fNextButton = new TGTextButton(fNextStepFrame," &Next ");
  fNextButton->Connect("Clicked()","TH5MainFrame",this,"PlotNextStep(Int_t=1)");
  fNextStepFrame->AddFrame(fNextStepLabel, new TGLayoutHints(kLHintsLeft,2,1,1,0));
  fNextStepFrame->AddFrame(fNextButton, new TGLayoutHints(kLHintsRight,2,1,0,0));

  fStepSposTEFrame->AddFrame(fStepTEFrame, new TGLayoutHints(kLHintsCenterX,2,1,0,0));
  fStepSposTEFrame->AddFrame(fSposTEFrame, new TGLayoutHints(kLHintsCenterX,2,1,0,0));
  fStepSposTEFrame->AddFrame(fNextStepFrame, new TGLayoutHints(kLHintsRight,5,1,0,0));
  fStepSposTEFrame->AddFrame(fPrevStepFrame, new TGLayoutHints(kLHintsRight,10,1,0,0));

  fStepSelection->AddFrame(fStepSlider, new TGLayoutHints(kLHintsCenterX,5,1,0,2));
  fStepSelection->AddFrame(fStepSposTEFrame, new TGLayoutHints(kLHintsCenterX,5,1,2,0));

  fSelspace->AddFrame(fStepSelection, new TGLayoutHints(kLHintsCenterX,1,1,1,1)); 

  fSelspace->MapSubwindows();
  fWorkspace->AddFrame(fSelspace, new TGLayoutHints(kLHintsLeft,1,1,1,1)); 

  //--------------------------------------------------------------------------
  // variable space: for variable selection (also contains print button)
  fVarspace = new TGVerticalFrame(fWorkspace, 300,100);

  // group frame for variable selection
  fVarSelection = new TGGroupFrame(fVarspace, "Var. sel.",kVerticalFrame);
  fVarSelection->SetTitlePos(TGGroupFrame::kLeft);

  // default names for particle variables 
  // (will be overwritten by loaded file)
  fVarNameDefault[0] = TString(" x ");
  fVarNameDefault[1] = TString(" y ");
  fVarNameDefault[2] = TString(" z ");
  fVarNameDefault[3] = TString(" phiz");
  fVarNameDefault[4] = TString(" px ");
  fVarNameDefault[5] = TString(" py ");
  fVarNameDefault[6] = TString(" pz ");
  fVarNameDefault[7] = TString(" enez");

  for (Int_t n=0; n<8; ++n) {
    fVarName[n] = fVarNameDefault[n];
    fVarButton[n] = new TGCheckButton(fVarSelection, fVarName[n],200);
    fVarButton[n]->SetState(kButtonDisabled);
    Int_t l = 5;
    if (n==3||n==7) l = 15;
    Int_t b = 2;
    if (n==3) b = 10;
    fVarSelection->AddFrame(fVarButton[n], new TGLayoutHints(kLHintsLeft,l,0,2,b));
  } 
  
  fVarspace->AddFrame(fVarSelection, new TGLayoutHints(kLHintsLeft,2,2,2,2)); 

  // print space
  fPrintspace = new TGVerticalFrame(fVarspace);

  fPrintButton = new TGTextButton(fPrintspace," &Print ");
  fPrintButton->Connect("Clicked()","TH5MainFrame",this,"PrintCanv()");
  fPrintspace->AddFrame(fPrintButton, fLayoutLeftflush); 

  fPrintBox = new TGComboBox(fPrintspace,"format",100);
  // define string array at the same time (see note at end of PrintCanv() method)
  fPrintBox->AddEntry("pdf",0);   H5PrintFormat[0] = TString("pdf");
  fPrintBox->AddEntry("jpg",1);   H5PrintFormat[1] = TString("jpg");
  fPrintBox->AddEntry("png",2);   H5PrintFormat[2] = TString("png");
  fPrintBox->AddEntry("gif",3);   H5PrintFormat[3] = TString("gif");
  fPrintBox->AddEntry("tiff",4);  H5PrintFormat[4] = TString("tiff");
  fPrintBox->AddEntry("ps",5);    H5PrintFormat[5] = TString("ps");
  fPrintBox->AddEntry("ps-port",6);  H5PrintFormat[6] = TString("Portrait");
  fPrintBox->AddEntry("ps-land",7);  H5PrintFormat[7] = TString("Landscape");
  fPrintBox->AddEntry("eps",8);   H5PrintFormat[8] = TString("eps");
  fPrintBox->AddEntry("eps-pre",9);  H5PrintFormat[9] = TString("Preview");
  fPrintBox->AddEntry("svg",10);  H5PrintFormat[10] = TString("svg");
  fPrintBox->AddEntry("xpm",11);  H5PrintFormat[11] = TString("xpm");
  fPrintBox->AddEntry("xml",12);  H5PrintFormat[12] = TString("xml");
  fPrintBox->AddEntry("cxx",13);  H5PrintFormat[13] = TString("cxx");
  fPrintBox->AddEntry("root",14); H5PrintFormat[14] = TString("root");
  fPrintBox->Select(0);
  fPrintBox->Resize(70,20);
  fPrintspace->AddFrame(fPrintBox, fLayoutLeftflush); 

  fVarspace->AddFrame(fPrintspace, fLayoutLeftflush); 

  fVarspace->MapSubwindows();
  fWorkspace->AddFrame(fVarspace, fLayoutLeftflush); 

  //--------------------------------------------------------------------------
  // button space: for plot buttons
  fButtonspace = new TGVerticalFrame(fWorkspace, 300,100);

  // group frame for histogramming, with three subframes
  fHistoFrame = new TGGroupFrame(fButtonspace, "Histograms",kVerticalFrame);
  f1dFrame = new TGHorizontalFrame(fHistoFrame);
  f2dFrame = new TGHorizontalFrame(fHistoFrame);
  f3dFrame = new TGHorizontalFrame(fHistoFrame);

  // 1d histograms
  fHisto1d = new TGTextButton(f1dFrame,"  &1D  ");
  fHisto1d->Connect("Clicked()","TH5MainFrame",this,"Histo1d()");
  fHisto1d->Resize(20,fHisto1d->GetDefaultHeight());

  fHisto1dBox = new TGComboBox(f1dFrame,"style",100);
  fHisto1dBox->AddEntry("HIST",0);   H5DrawOpt1d[0] = TString("");
  fHisto1dBox->AddEntry("ERRBAR",1); H5DrawOpt1d[1] = TString("e1p");
  fHisto1dBox->AddEntry("ERRBOX",2); H5DrawOpt1d[2] = TString("e2p");
  fHisto1dBox->AddEntry("CURVE",3);  H5DrawOpt1d[3] = TString("c");
  fHisto1dBox->AddEntry("BAR",4);    H5DrawOpt1d[4] = TString("b");
  fHisto1dBox->AddEntry("LINE",5);   H5DrawOpt1d[5] = TString("l");
  fHisto1dBox->AddEntry("LEGO",6);   H5DrawOpt1d[6] = TString("lego");
  fHisto1dBox->Select(0);
  fHisto1dBox->Resize(70,20);

  // 2d histograms
  fHisto2d = new TGTextButton(f2dFrame,"  &2D  ");
  fHisto2d->Connect("Clicked()","TH5MainFrame",this,"Histo2d()");
  fHisto2d->Resize(70,fHisto2d->GetDefaultHeight());

  fHisto2dBox = new TGComboBox(f2dFrame,"style",100);
  fHisto2dBox->AddEntry("NOBIN",0);   H5DrawOpt2d[0] = TString("unbinned");
  fHisto2dBox->AddEntry("SCAT",1);   H5DrawOpt2d[1] = TString("");
  fHisto2dBox->AddEntry("BOX",2);    H5DrawOpt2d[2] = TString("box");
  fHisto2dBox->AddEntry("ARR",3);    H5DrawOpt2d[3] = TString("arr");
  fHisto2dBox->AddEntry("COLZ",4);   H5DrawOpt2d[4] = TString("colz");
  fHisto2dBox->AddEntry("TEXT",5);   H5DrawOpt2d[5] = TString("text");
  fHisto2dBox->AddEntry("CONT",6);   H5DrawOpt2d[6] = TString("cont");
  fHisto2dBox->AddEntry("CONTZ",7);  H5DrawOpt2d[7] = TString("contz");
  fHisto2dBox->AddEntry("CONT1",8);  H5DrawOpt2d[8] = TString("cont1");
  fHisto2dBox->AddEntry("CONT2",9);  H5DrawOpt2d[9] = TString("cont2");
  fHisto2dBox->AddEntry("CONT3",10);  H5DrawOpt2d[10] = TString("cont3");
  fHisto2dBox->AddEntry("LEGO",11);  H5DrawOpt2d[11] = TString("lego");
  fHisto2dBox->AddEntry("LEGO1",12); H5DrawOpt2d[12] = TString("lego1");
  fHisto2dBox->AddEntry("LEGO2",13); H5DrawOpt2d[13] = TString("lego2");
  fHisto2dBox->AddEntry("SURF1",14); H5DrawOpt2d[14] = TString("surf1");
  fHisto2dBox->AddEntry("SURF2z",15);H5DrawOpt2d[15] = TString("surf2z");
  fHisto2dBox->AddEntry("SURF3",16); H5DrawOpt2d[16] = TString("surf3");
  fHisto2dBox->AddEntry("SURF4",17); H5DrawOpt2d[17] = TString("surf4");
  fHisto2dBox->AddEntry("SURF1POL",18); H5DrawOpt2d[18] = TString("surf1pol");
  fHisto2dBox->AddEntry("SURF1CYL",19); H5DrawOpt2d[19] = TString("surf1cyl");
  fHisto2dBox->Select(0);
  fHisto2dBox->Resize(70,20);

  // 3d histograms
  fHisto3d = new TGTextButton(f3dFrame,"  &3D  ");
  fHisto3d->Connect("Clicked()","TH5MainFrame",this,"Histo3d()");
  fHisto3d->Resize(100,fHisto3d->GetDefaultHeight());

  fHisto3dBox = new TGComboBox(f3dFrame,"style",100);
  fHisto3dBox->AddEntry("SCAT",0); H5DrawOpt3d[0] = TString("");
  fHisto3dBox->AddEntry("BOX",1);  H5DrawOpt3d[1] = TString("box");
  fHisto3dBox->Select(0);
  fHisto3dBox->Resize(70,20);

  f1dFrame->AddFrame(fHisto1d, new TGLayoutHints(kLHintsLeft,2,10,2,2));
  f1dFrame->AddFrame(fHisto1dBox, new TGLayoutHints(kLHintsLeft,2,2,2,2)); 

  f2dFrame->AddFrame(fHisto2d, new TGLayoutHints(kLHintsLeft,2,10,2,2)); 
  f2dFrame->AddFrame(fHisto2dBox, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  f3dFrame->AddFrame(fHisto3d, new TGLayoutHints(kLHintsLeft,2,10,2,2)); 
  f3dFrame->AddFrame(fHisto3dBox, new TGLayoutHints(kLHintsLeft,2,2,2,2));  

  fHistoFrame->AddFrame(f1dFrame, new TGLayoutHints(kLHintsLeft,2,2,10,2)); 
  fHistoFrame->AddFrame(f2dFrame, new TGLayoutHints(kLHintsLeft,2,2,2,2)); 
  fHistoFrame->AddFrame(f3dFrame, new TGLayoutHints(kLHintsLeft,2,2,2,2)); 

  fHistoFrame->Resize(2000,500);

  fButtonspace->AddFrame(fHistoFrame, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  // group frame for line plots
  fLinePlotFrame = new TGGroupFrame(fButtonspace, "Line plots",kVerticalFrame);

  fSelLPFrame = new TGHorizontalFrame(fLinePlotFrame);

  fSelLPFrameLeft = new TGVerticalFrame(fSelLPFrame);
  fSelLPLabel1 = new TGLabel(fSelLPFrame,"Independent");
  fSelLPLabel2 = new TGLabel(fSelLPFrame,"variable:");
  fSelLPFrameLeft->AddFrame(fSelLPLabel1, new TGLayoutHints(kLHintsLeft,0,0,0,0)); 
  fSelLPFrameLeft->AddFrame(fSelLPLabel2, new TGLayoutHints(kLHintsRight,0,0,0,0)); 

  fSelLPFrameRight = new TGVerticalFrame(fSelLPFrame);
  fSelLPBox = new TGComboBox(fSelLPFrameRight,"select",100);
  fSelLPBox->Resize(70,20);
  fSelLPFrameRight->AddFrame(fSelLPBox, new TGLayoutHints(kLHintsLeft,5,0,5,0)); 

  fScalarLPFrame = new TGHorizontalFrame(fLinePlotFrame);

  fScalar = new TGTextButton(fScalarLPFrame,"&Scalar");
  fScalar->Connect("Clicked()","TH5MainFrame",this,"ScalarLinePlot()");

  fScalarLPBox = new TGComboBox(fScalarLPFrame,"scalar",100);
  fScalarLPBox->Resize(70,20);

  fVectorLPFrame = new TGHorizontalFrame(fLinePlotFrame);

  fVector = new TGTextButton(fVectorLPFrame,"&Vector");
  fVector->Connect("Clicked()","TH5MainFrame",this,"VectorLinePlot()");

  fVectorLPBox = new TGComboBox(fVectorLPFrame,"vector",100);
  fVectorLPBox->Resize(70,20);

  fSelLPFrame->AddFrame(fSelLPFrameLeft, new TGLayoutHints(kLHintsLeft,0,0,0,0)); 
  fSelLPFrame->AddFrame(fSelLPFrameRight, new TGLayoutHints(kLHintsRight,5,0,2,0)); 

  fScalarLPFrame->AddFrame(fScalar, new TGLayoutHints(kLHintsLeft,2,10,2,2)); 
  fScalarLPFrame->AddFrame(fScalarLPBox, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  fVectorLPFrame->AddFrame(fVector, new TGLayoutHints(kLHintsLeft,2,10,2,2)); 
  fVectorLPFrame->AddFrame(fVectorLPBox, new TGLayoutHints(kLHintsLeft,2,2,2,2)); 

  fRadioLPFrame = new TGHorizontalFrame(fLinePlotFrame);
  fRadioLPFrame->AddFrame(fRadioLP[0] = new TGRadioButton(fRadioLPFrame, "x", linePlotXID), 
			new TGLayoutHints(kLHintsLeft,20,2,2,2)); 
  fRadioLPFrame->AddFrame(fRadioLP[1] = new TGRadioButton(fRadioLPFrame, "y", linePlotYID), 
			new TGLayoutHints(kLHintsLeft,2,2,2,2)); 
  fRadioLPFrame->AddFrame(fRadioLP[2] = new TGRadioButton(fRadioLPFrame, "z", linePlotZID), 
			new TGLayoutHints(kLHintsLeft,2,2,2,2)); 
  fRadioLP[0]->Connect("Pressed()", "TH5MainFrame", this, "DoLinePlotRadioButtons()");
  fRadioLP[1]->Connect("Pressed()", "TH5MainFrame", this, "DoLinePlotRadioButtons()");
  fRadioLP[2]->Connect("Pressed()", "TH5MainFrame", this, "DoLinePlotRadioButtons()");

  fLinePlotFrame->AddFrame(fSelLPFrame, new TGLayoutHints(kLHintsLeft,2,2,10,2));
  fLinePlotFrame->AddFrame(fScalarLPFrame, new TGLayoutHints(kLHintsLeft,2,2,10,2));
  fLinePlotFrame->AddFrame(fVectorLPFrame, new TGLayoutHints(kLHintsLeft,2,2,2,2));
  fLinePlotFrame->AddFrame(fRadioLPFrame, new TGLayoutHints(kLHintsLeft,2,2,2,2));
  fLinePlotFrame->MapSubwindows();

  fButtonspace->AddFrame(fLinePlotFrame, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  
  fButtonspace->MapSubwindows();
  fWorkspace->AddFrame(fButtonspace, fLayoutLeftflush); 
  
  fWorkspace->MapSubwindows();
  AddFrame(fWorkspace, fLayoutCentered);
  
  // Sets window name and shows the main frame
  SetWindowName("H5PartROOT");
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();	
  
  fLastPlot = kNoPlot;
  
}

TH5MainFrame::~TH5MainFrame() {

  delete [] fDataset;

  Cleanup();  // deletes all buttons and frames etc.

}

void TH5MainFrame::LoadFile() {
  if (fFilenameBuffer->GetTextLength()==0) { 
    Error("LoadFile", "No filename entered!");
  } else {
    LoadFile(fFilenameBuffer->GetString());   
  } 
}

void TH5MainFrame::LoadFile(const char * filename) {

  FileStat_t fs;
  Int_t notExist = gSystem->GetPathInfo(filename, fs);
  if (notExist) {
    Error("LoadFile", "There is no file named \"%s\"!",filename);
    return;
  } 

  // find the first available slot:
  Int_t slot(0);
  for (Int_t n=0; n<5; ++n) {
    if (!fDataset[n]) {
      slot = n;
      break;
    }
    if (n==4) {
      Error("LoadFile", "You have already loaded 5 files!");    
      return;
    }  
  }

  fDataset[slot] = new TH5Dataset(filename, fSubsamplingRate);

  // update name of check button (strip path):
  TString fname = fDataset[slot]->GetFilename();
  Int_t nSteps = fDataset[slot]->GetNSteps();

  TString bname(fname+TString(" ("));
  bname+=nSteps;
  bname+=TString(" steps)");
  fFileButton[slot]->SetText(bname);

  // ... and check the box!
  fFileButton[slot]->SetState(kButtonDown);  
  // set file to active:
  fDataset[slot]->SetState(kActive);

  // warn if number of steps is larger than for first file:
  if (nSteps != fDataset[0]->GetNSteps())
    Warning("LoadFile","New file has %d steps, whereas first file has %d!",
	    nSteps,fDataset[0]->GetNSteps());

  UpdateSelections();

}

void TH5MainFrame::ActivateFile(Int_t slot) {
  if (fDataset[slot]->GetState() == kNotExist) {
    fFileButton[slot]->SetState(kButtonDisabled);
    return;
  }
  if (fFileButton[slot]->GetState()==kButtonUp)   fDataset[slot]->SetState(kNotActive);
  if (fFileButton[slot]->GetState()==kButtonDown) fDataset[slot]->SetState(kActive);

  UpdateSelections();

}

void TH5MainFrame::UpdateSelections() {

  // update step and variable selection (according to first selected file)
  TH5Dataset* activeData[5];
  Int_t nFiles(0);
  GetSelFiles(activeData,nFiles);

  if (activeData[0]) {

    fStepSlider->SetRange(0,(activeData[0]->GetNSteps())-1);
    fStepSlider->SetPosition(0);

    for(Int_t i=0;i<8;i++){ 
      EButtonState prevState = fVarButton[i]->GetState();
      fVarName[i] = activeData[0]->GetPartVarName(i);
      if (fVarName[i] != TString("")) {
	if (prevState==kButtonDown) {
	  fVarButton[i]->SetState(prevState);
	} else {
	  fVarButton[i]->SetState(kButtonUp);
	}	  
	fVarButton[i]->SetText(activeData[0]->GetPartVarName(i));
      } else {
	fVarName[i] = fVarNameDefault[i];
	fVarButton[i]->SetState(kButtonDisabled);
      }
    }

    Int_t prevSelEntry = fSelLPBox->GetSelected();
    Int_t prevScalarEntry = fScalarLPBox->GetSelected();
    Int_t prevVectorEntry = fVectorLPBox->GetSelected();

    //    fSelLPBox->RemoveAll();  // only in most recent version??
    //    fScalarLPBox->RemoveAll();  // only in most recent version??
    fSelLPBox->RemoveEntries(0,fSelLPBox->GetNumberOfEntries()-1);
    fScalarLPBox->RemoveEntries(0,fScalarLPBox->GetNumberOfEntries()-1);
    Int_t sposID(0);
    for (Int_t is = 0; is < activeData[0]->GetNScalarAttr(); is++) {
      TString vname = activeData[0]->GetScalarStepVarName(is);
      fSelLPBox->AddEntry(vname,is);      
      fScalarLPBox->AddEntry(vname,is);      
      if (vname=="Spos"||vname=="spos"||vname=="s") sposID = is;
    }

    //    fVectorLPBox->RemoveAll();  // only in most recent version??
    fVectorLPBox->RemoveEntries(0,fVectorLPBox->GetNumberOfEntries()-1);
    for (Int_t iv = 0; iv < activeData[0]->GetNVectorAttr(); iv++) {
      fVectorLPBox->AddEntry(activeData[0]->GetVectorStepVarName(iv),iv);      
    }

    // restore previous setting if possible
    if (activeData[0]->GetNScalarAttr() > prevSelEntry) {
      fSelLPBox->Select(prevSelEntry);
    } else {
      fSelLPBox->Select(sposID);
    }
    if (activeData[0]->GetNScalarAttr() > prevScalarEntry) {
      fScalarLPBox->Select(prevScalarEntry);
    } else {
      fScalarLPBox->Select(0);
    }
    if (activeData[0]->GetNVectorAttr() > prevVectorEntry) {
      fVectorLPBox->Select(prevVectorEntry);
    } else {
      fVectorLPBox->Select(0);      
    }
  }
  
}


void TH5MainFrame::Histo1d() {

  TH5Dataset* dataToPlot[5];
  Int_t nFiles(0);
  GetSelFiles(dataToPlot,nFiles);

  switch (nFiles) {
  case 0:
    Error("Histo1d", "No files have been selected!");
    break;
  case 1:
    Histo1d(dataToPlot[0]);
    break;
  case 2:
    Histo1d(dataToPlot[0],dataToPlot[1]);
    break;
  case 3:
    Error("Histo1d", "Histogramming for %d files not yet implemented",nFiles);
    break;
  case 4:
    Error("Histo1d", "Histogramming for %d files not yet implemented",nFiles);
    break;
  case 5:
    Error("Histo1d", "Histogramming for %d files not yet implemented",nFiles);
    break;
  default:
    cout << "Huh?" << endl;
  } // switch
  
  fLastPlot = kHisto1d;

}

void TH5MainFrame::Histo1d(TH5Dataset* data) {

  //  Int_t step = fStepBox->GetSelected();
  Int_t step = fStepSlider->GetPosition();
  Int_t opt = fHisto1dBox->GetSelected();
  TString drawOpt = H5DrawOpt1d[opt];

  TString legEntry = TString("l");
  if (drawOpt=="e1p") legEntry = TString("lp");
  if (drawOpt=="e2p") legEntry = TString("fp");

  TString* varToPlot[8];
  Int_t nVars;
  GetSelVar(varToPlot,nVars);

  TCanvas *canv = fEcanvas->GetCanvas();

  Double_t plotMin(0.);
  Double_t plotMax(0.);

  switch (nVars) {
  case 0:
    Error("Histo1d", "No variable has been selected!");
    break;

  case 1:  // nVars == 1
      
    {
      TH1F* hist;
      if (data) {
	data->PlotRange(varToPlot[0], step, plotMin, plotMax);
	hist = data->Histo1d(varToPlot[0],step,plotMin,plotMax);
        if (data->GetUnit(*varToPlot[0]) != "") 
	  hist->SetXTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
      }
      canv->Clear();
      canv->SetRightMargin(0.25);

      if (drawOpt=="e2p") hist->SetFillColor(17);
      if (drawOpt=="b") {
	hist->SetBarOffset(0.2);
	hist->SetBarWidth(0.75);
	hist->SetFillColor(kBlack);
      }
      hist->Draw(drawOpt);
      
      TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
      TString header(*varToPlot[0]+TString( "distribution, step "));
      header+=step;
      leg->AddEntry(hist, data->GetFilename(),legEntry);
      leg->SetHeader(header);
      leg->Draw();
      
      canv->Update();
    }
    break;
  case 2:  // nVars == 2

    TH1F* hist0 = (TH1F*) gROOT->FindObject("hist0");
    if (hist0) delete hist0;
    TH1F* hist1 = (TH1F*) gROOT->FindObject("hist1");
    if (hist1) delete hist1;

    if (data) {
      data->PlotRange(varToPlot[0], step, plotMin, plotMax);
      hist0 = data->Histo1d(varToPlot[0],step,plotMin,plotMax);
      hist0->SetName("hist0");
      if (data->GetUnit(*varToPlot[0]) != "") 
	hist0->SetXTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
      data->PlotRange(varToPlot[1], step, plotMin, plotMax);
      hist1 = data->Histo1d(varToPlot[1],step,plotMin,plotMax);
      hist1->SetName("hist1");
      if (data->GetUnit(*varToPlot[1]) != "") 
	hist1->SetXTitle(*varToPlot[1] + AddBrackets(data->GetUnit(*varToPlot[1])));
    }

    canv->Clear();
    canv->Divide(2,1);

    if (drawOpt=="e2p") {
      hist0->SetFillColor(17);hist1->SetFillColor(17);
    }
    if (drawOpt=="b") {
      hist0->SetBarOffset(0.2);hist1->SetBarOffset(0.2);
      hist0->SetBarWidth(0.75);hist1->SetBarWidth(0.75);
      hist0->SetFillColor(kBlack);hist1->SetFillColor(kBlack);
    }

    canv->cd(1);
    hist0->Draw(drawOpt);
    TH5Legend* leg0 = new TH5Legend(0.7,0.75,.9,0.9);
    leg0->AddEntry(hist0, data->GetFilename(),legEntry);
    leg0->SetHeader(*varToPlot[0]+TString(" distribution"));
    leg0->Draw();

    canv->cd(2);
    hist1->Draw(drawOpt);
    TH5Legend* leg1 = new TH5Legend(0.7,0.75,.9,0.9);
    leg1->AddEntry(hist1, data->GetFilename(),legEntry);
    leg1->SetHeader(*varToPlot[1]+TString(" distribution"));
    leg1->Draw();

    break;
  case 3:  // nVars == 3
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  case 4:  // nVars == 4
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  case 5:  // nVars == 5
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  case 6:  // nVars == 6
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  default:
    cout << "Huh?" << endl;
  }//switch

  canv->Update();

}

void TH5MainFrame::Histo1d(TH5Dataset* data0, TH5Dataset* data1) {

  Int_t step = fStepSlider->GetPosition();
  Int_t opt = fHisto1dBox->GetSelected();

  TString drawOpt = H5DrawOpt1d[opt];
  TString drawOptSame = H5DrawOpt1d[opt]+TString("same");

  TString legEntry = TString("l");
  if (drawOpt=="e1p") legEntry = TString("lp");
  if (drawOpt=="e2p") legEntry = TString("fp");

  if (opt!=0) Warning("Histo1d", "Drawing option %s may give weird results for two files!", drawOpt.Data());

  TString* varToPlot[8];
  Int_t nVars;
  GetSelVar(varToPlot,nVars);

  TCanvas *canv = fEcanvas->GetCanvas();
  canv->Clear();
  canv->SetRightMargin(0.25);
  
  Double_t plotMin(0.);
  Double_t plotMax(0.);
  Double_t min0(0.);
  Double_t max0(0.);
  Double_t min1(0.);
  Double_t max1(0.);

  switch (nVars) {
  case 0:
    Error("Histo1d", "No variables have been selected");
    break;

  case 1:  { // nVars == 1
    TH1F* hist0 = (TH1F*) gROOT->FindObject("hist0");
    if (hist0) delete hist0;
    TH1F* hist1 = (TH1F*) gROOT->FindObject("hist1");
    if (hist1) delete hist1;

    if (data0) data0->Range(varToPlot[0], step, min0, max0);
    if (data1) data1->Range(varToPlot[0], step, min1, max1);
    plotMin =  min0  <? min1;  
    plotMax =  max0  >? max1; 
    AddMargin(plotMin,plotMax);

    if (data0) hist0 = data0->Histo1d(varToPlot[0], step, plotMin, plotMax);
    hist0->SetName("hist0");
    if (data1) hist1 = data1->Histo1d(varToPlot[0], step, plotMin, plotMax);
    hist1->SetName("hist1");    

    if (data0->GetUnit(*varToPlot[0]) == data1->GetUnit(*varToPlot[0])) {
      if (data0->GetUnit(*varToPlot[0]) != "") {
	hist0->SetXTitle(*varToPlot[0] + AddBrackets(data0->GetUnit(*varToPlot[0])));
      }
    } else {
      Warning("Histo1d","Inconsistent units between files %s and %s",
	      data0->GetFilename().Data(),data1->GetFilename().Data());
      if (data0->GetUnit(*varToPlot[0]) != "") {
	hist0->SetXTitle(*varToPlot[0] + AddBrackets(data0->GetUnit(*varToPlot[0])));
      } else if (data0->GetUnit(*varToPlot[0]) != "") {
	hist0->SetXTitle(*varToPlot[0] + AddBrackets(data0->GetUnit(*varToPlot[0])));
      }
    }

    if (drawOpt=="e2p") {
      hist0->SetFillColor(15);hist1->SetFillColor(18);
    }

    hist1->SetLineStyle(kDashed);
    hist1->SetMarkerStyle(25);

    if (hist0->GetMaximum() > hist1->GetMaximum()) {
      hist0->Draw(drawOpt); hist1->Draw(drawOptSame);
    } else {
      hist1->Draw(drawOpt); hist0->Draw(drawOptSame);
    }

    TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
    leg->AddEntry(hist0, data0->GetFilename(),legEntry);
    leg->AddEntry(hist1, data1->GetFilename(),legEntry);
    leg->SetHeader(*varToPlot[0]+TString(" distribution"));
    leg->Draw();
    
    break; }

  case 2:  { // nVars == 2

    TH1F* hist00 = (TH1F*) gROOT->FindObject("hist00");
    if (hist00) delete hist00;
    TH1F* hist01 = (TH1F*) gROOT->FindObject("hist01");
    if (hist01) delete hist01;
    TH1F* hist10 = (TH1F*) gROOT->FindObject("hist10");
    if (hist10) delete hist10;
    TH1F* hist11 = (TH1F*) gROOT->FindObject("hist11");
    if (hist11) delete hist11;

    if (data0) data0->Range(varToPlot[0], step, min0, max0);
    if (data1) data1->Range(varToPlot[0], step, min1, max1);
    plotMin =  min0  <? min1;  
    plotMax =  max0  >? max1;  
    AddMargin(plotMin,plotMax);

    if (data0) hist00 = data0->Histo1d(varToPlot[0], step, plotMin, plotMax);
    hist00->SetName("hist00");
    if (data1) hist10 = data1->Histo1d(varToPlot[0], step, plotMin, plotMax);
    hist10->SetName("hist10");

    if (data0->GetUnit(*varToPlot[0]) == data1->GetUnit(*varToPlot[0])) {
      if (data0->GetUnit(*varToPlot[0]) != "") {
	hist00->SetXTitle(*varToPlot[0] + AddBrackets(data0->GetUnit(*varToPlot[0])));
      }
    } else {
      Warning("Histo1d","Inconsistent units between files %s and %s",
	      data0->GetFilename().Data(),data1->GetFilename().Data());
      if (data0->GetUnit(*varToPlot[0]) != "") {
	hist00->SetXTitle(*varToPlot[0] + AddBrackets(data0->GetUnit(*varToPlot[0])));
      } else if (data0->GetUnit(*varToPlot[0]) != "") {
	hist00->SetXTitle(*varToPlot[0] + AddBrackets(data0->GetUnit(*varToPlot[0])));
      }
    }

    if (data0) data0->Range(varToPlot[1], step, min0, max0);
    if (data1) data1->Range(varToPlot[1], step, min1, max1);
    plotMin =  min0  <? min1;  
    plotMax =  max0  >? max1;  
    AddMargin(plotMin,plotMax);

    if (data0) hist01 = data0->Histo1d(varToPlot[1], step, plotMin, plotMax);
    hist01->SetName("hist01");
    if (data1) hist11 = data1->Histo1d(varToPlot[1], step, plotMin, plotMax);
    hist11->SetName("hist11");

    if (data0->GetUnit(*varToPlot[1]) == data1->GetUnit(*varToPlot[1])) {
      if (data0->GetUnit(*varToPlot[1]) != "") {
	hist01->SetXTitle(*varToPlot[1] + AddBrackets(data0->GetUnit(*varToPlot[1])));
      }
    } else {
      Warning("Histo1d","Inconsistent units between files %s and %s",
	      data0->GetFilename().Data(),data1->GetFilename().Data());
      if (data0->GetUnit(*varToPlot[1]) != "") {
	hist01->SetXTitle(*varToPlot[1] + AddBrackets(data0->GetUnit(*varToPlot[1])));
      } else if (data0->GetUnit(*varToPlot[1]) != "") {
	hist01->SetXTitle(*varToPlot[1] + AddBrackets(data0->GetUnit(*varToPlot[1])));
      }
    }

    if (drawOpt=="e2p") {
      hist00->SetFillColor(15);hist10->SetFillColor(18);
      hist01->SetFillColor(15);hist11->SetFillColor(18);
    }

    canv->Clear();
    canv->Divide(2,1);

    canv->cd(1);

    hist10->SetLineStyle(kDashed);
    hist10->SetMarkerStyle(25);

    if (hist00->GetMaximum() > hist10->GetMaximum()) {
      hist00->Draw(drawOpt); hist10->Draw(drawOptSame);
    } else {
      hist10->Draw(drawOpt); hist00->Draw(drawOptSame);
    }

    TH5Legend* leg0 = new TH5Legend(0.7,0.75,.9,0.9);
    leg0->AddEntry(hist00, data0->GetFilename(),legEntry);
    leg0->AddEntry(hist10, data1->GetFilename(),legEntry);
    leg0->SetHeader(*varToPlot[0]+TString(" distribution"));
    leg0->Draw();

    canv->cd(2);

    hist11->SetLineStyle(kDashed);
    hist11->SetMarkerStyle(25);

    if (hist01->GetMaximum() > hist11->GetMaximum()) {
      hist01->Draw(drawOpt); hist11->Draw(drawOptSame);
    } else {
      hist11->Draw(drawOpt); hist01->Draw(drawOptSame);
    }

    TH5Legend* leg1 = new TH5Legend(0.7,0.75,.9,0.9);
    leg1->AddEntry(hist01, data0->GetFilename(),legEntry);
    leg1->AddEntry(hist11, data1->GetFilename(),legEntry);
    leg1->SetHeader(*varToPlot[1] + TString(" distribution"));
    leg1->Draw();

    break; }
  case 3:  // nVars == 3
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  case 4:  // nVars == 4
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  case 5:  // nVars == 5
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  case 6:  // nVars == 6
    Error("Histo1d", "Histogramming for %d variables not yet implemented",nVars);
    break;
  default:
    cout << "Huh?" << endl;
  } //switch
  
  canv->Update();


}

void TH5MainFrame::Histo2d() {

  TH5Dataset* dataToPlot[5];
  Int_t nFiles(0);
  GetSelFiles(dataToPlot,nFiles);

  switch (nFiles) {
  case 0:
    Error("Histo2d", "No files have been selected!");
    break;
  case 1:
    Histo2d(dataToPlot[0]);
    break;
  case 2:
    Error("Histo2d", "2d histogram for %d files not yet implemented",nFiles);
    break;
  case 3:
    Error("Histo2d", "2d histogram for %d files not yet implemented",nFiles);
    break;
  case 4:
    Error("Histo2d", "2d histogram for %d files not yet implemented",nFiles);
    break;
  case 5:
    Error("Histo2d", "2d histogram for %d files not yet implemented",nFiles);
    break;
  default:
    cout << "Huh?" << endl;
  } // switch
  
  fLastPlot = kHisto2d;

}

void TH5MainFrame::Histo2d(TH5Dataset* data) {

  Int_t step = fStepSlider->GetPosition();
  Int_t opt = fHisto2dBox->GetSelected();

  TString* varToPlot[8];
  Int_t nVars;
  GetSelVar(varToPlot,nVars);

  TCanvas *canv = fEcanvas->GetCanvas();

  canv->Clear();
  if (H5DrawOpt2d[opt]=="colz"||H5DrawOpt2d[opt]=="contz"||H5DrawOpt2d[opt]=="surf2z") {
    canv->SetRightMargin(0.35);
  } else {
    canv->SetRightMargin(0.25);
  }

  Double_t plotMinX(0.);
  Double_t plotMaxX(0.);
  Double_t plotMinY(0.);
  Double_t plotMaxY(0.);

  switch (nVars) {
  case 0:
    Error("Histo2d", "No variable has been selected!");
    break;

  case 1:  // nVars == 1
      
    {
      TH2F* hist;
      if (data) {
	data->PlotRange(varToPlot[0], step, plotMinX, plotMaxX);
	hist = data->Histo2d(varToPlot[0],varToPlot[0],step,plotMinX,plotMaxX,plotMinX,plotMaxX);
        if (data->GetUnit(*varToPlot[0]) != "") {
	  hist->SetXTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
	  hist->SetYTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
	}
      }
      hist->SetMarkerStyle(1);
      hist->Draw(H5DrawOpt2d[opt]);
      
      TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
      TString header(*varToPlot[0]+TString("-") + *varToPlot[0] + 
		     TString(" distr., step "));
      header+=step;
      leg->AddEntry(hist, data->GetFilename(),"p");
      leg->SetHeader(header);
      leg->Draw();
      
      canv->Update();
    }
    break;
  case 2:  // nVars == 2

    {
      TH2F* hist;
      if (data) {
	data->PlotRange(varToPlot[0], step, plotMinX, plotMaxX);
	data->PlotRange(varToPlot[1], step, plotMinY, plotMaxY);
	hist = data->Histo2d(varToPlot[0],varToPlot[1],step,plotMinX,plotMaxX,plotMinY,plotMaxY);
        if (data->GetUnit(*varToPlot[0]) != "") 
	  hist->SetXTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
        if (data->GetUnit(*varToPlot[1]) != "") 
	  hist->SetYTitle(*varToPlot[1] + AddBrackets(data->GetUnit(*varToPlot[1])));
      }
      hist->SetMarkerStyle(1);
      
      if (H5DrawOpt2d[opt]=="unbinned") { // true scatter plot
	canv->Clear();
	TH1F* shist = data->Scatter(varToPlot[0],varToPlot[1],canv,step,plotMinX,plotMaxX,plotMinY,plotMaxY);
        if (data->GetUnit(*varToPlot[0]) != "") 
	  shist->SetXTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
        if (data->GetUnit(*varToPlot[1]) != "") 
	  shist->SetYTitle(*varToPlot[1] + AddBrackets(data->GetUnit(*varToPlot[1])));
      } else {
	hist->Draw(H5DrawOpt2d[opt]);
      }

      TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
      TString header(*varToPlot[0] + TString("-") + *varToPlot[1] + 
		     TString(" distr., step "));
      header+=step;
      leg->AddEntry(hist, data->GetFilename(),"p");
      leg->SetHeader(header);
      leg->Draw();
            
      canv->Update();
    }
    break;
  case 3:  // nVars == 3
    Error("Histo2d", "Cannot produce 2d histogram with %d variables!",nVars);
    break;
  case 4:  // nVars == 4
    Error("Histo2d", "2d histogram for %d variables not yet implemented",nVars);
    break;
  case 5:  // nVars == 5
    Error("Histo2d", "Cannot produce 2d histogram with %d variables!",nVars);
    break;
  case 6:  // nVars == 6
    Error("Histo2d", "2d histogram for %d variables not yet implemented",nVars);
    break;
  default:
    cout << "Huh?" << endl;
  }//switch

}

void TH5MainFrame::Histo3d() {

  TH5Dataset* dataToPlot[5];
  Int_t nFiles(0);
  GetSelFiles(dataToPlot,nFiles);

  switch (nFiles) {
  case 0:
    Error("Histo3d", "No files have been selected!");
    break;
  case 1:
    Histo3d(dataToPlot[0]);
    break;
  case 2:
    Error("Histo3d", "3d histogram for %d files not yet implemented",nFiles);
    break;
  case 3:
    Error("Histo3d", "3d histogram for %d files not yet implemented",nFiles);
    break;
  case 4:
    Error("Histo3d", "3d histogram for %d files not yet implemented",nFiles);
    break;
  case 5:
    Error("Histo3d", "3d histogram for %d files not yet implemented",nFiles);
    break;
  default:
    cout << "Huh?" << endl;
  } // switch
  
  fLastPlot = kHisto3d;

}

void TH5MainFrame::Histo3d(TH5Dataset* data) {

  Int_t step = fStepSlider->GetPosition();
  Int_t opt = fHisto3dBox->GetSelected();

  TString* varToPlot[8];
  Int_t nVars;
  GetSelVar(varToPlot,nVars);

  TCanvas *canv = fEcanvas->GetCanvas();

  canv->Clear();
  canv->SetRightMargin(0.50);  // is ignored by TH3F...

  Double_t plotMinX(0.);
  Double_t plotMaxX(0.);
  Double_t plotMinY(0.);
  Double_t plotMaxY(0.);
  Double_t plotMinZ(0.);
  Double_t plotMaxZ(0.);

  switch (nVars) {
  case 0:
    Error("Histo3d", "No variable has been selected!");
    break;

  case 1:  // nVars == 1
    Error("Histo3d", "Cannot produce 3d histogram with %d variables",nVars);
    break;
  case 2:  // nVars == 2
    Error("Histo3d", "Cannot produce 3d histogram with %d variables",nVars);
    break;
  case 3:  // nVars == 3
    {

      TH3F* hist;
      if (data) {
	data->PlotRange(varToPlot[0], step, plotMinX, plotMaxX);
	data->PlotRange(varToPlot[1], step, plotMinY, plotMaxY);
	data->PlotRange(varToPlot[2], step, plotMinZ, plotMaxZ);
	hist = data->Histo3d(varToPlot[0],varToPlot[1],varToPlot[2],step,
			     plotMinX,plotMaxX,plotMinY,plotMaxY,plotMinZ,plotMaxZ);
        if (data->GetUnit(*varToPlot[0]) != "") 
	  hist->SetXTitle(*varToPlot[0] + AddBrackets(data->GetUnit(*varToPlot[0])));
        if (data->GetUnit(*varToPlot[1]) != "") 
	  hist->SetYTitle(*varToPlot[1] + AddBrackets(data->GetUnit(*varToPlot[1])));
        if (data->GetUnit(*varToPlot[2]) != "") 
	  hist->SetZTitle(*varToPlot[2] + AddBrackets(data->GetUnit(*varToPlot[2])));
      }
      hist->SetMarkerStyle(6);
      hist->Draw(H5DrawOpt3d[opt]);

      TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
      TString header(*varToPlot[0] + TString("-") + *varToPlot[1] +
		     TString("-") + *varToPlot[2] + TString(" distr., step ")); 
      header += step;
      leg->AddEntry(hist, data->GetFilename(),"p");
      leg->SetHeader(header);
      leg->Draw();
      
      canv->Update();
    }
    break;
  case 4:  // nVars == 4
    Error("Histo3d", "3d histogram for %d variables not yet implemented",nVars);
    break;
  case 5:  // nVars == 5
    Error("Histo3d", "3d histogram for %d variables not yet implemented",nVars);
    break;
  case 6:  // nVars == 6
    Error("Histo3d", "3d histogram for %d variables not yet implemented",nVars);
    break;
  default:
    cout << "Huh?" << endl;
  }//switch

}

void TH5MainFrame::ScalarLinePlot() {

  // determine active datafiles
  TH5Dataset* dataToPlot[5];
  Int_t nFiles(0);
  GetSelFiles(dataToPlot,nFiles);
    
  // get selected variables
  TString stepVarNameX;
  TString stepVarNameY;
  if (nFiles) {
    stepVarNameX = TString(fSelLPBox->GetSelectedEntry()->GetTitle());
    stepVarNameY = TString(fScalarLPBox->GetSelectedEntry()->GetTitle());
  }

  switch (nFiles) {
  case 0:
    Error("ScalarLinePlot", "No files have been selected!");
    break;
  case 1:
    LinePlot(dataToPlot[0],stepVarNameX,stepVarNameY,kScalar);
    break;
  case 2:
    LinePlot(dataToPlot[0],dataToPlot[1],stepVarNameX,stepVarNameY,kScalar);
    break;
  case 3:
    Error("ScalarLinePlot", "LinePlot for %d files not yet implemented",nFiles);
    break;
  case 4:
    Error("ScalarLinePlot", "LinePlot for %d files not yet implemented",nFiles);
    break;
  case 5:
    Error("ScalarLinePlot", "LinePlot for %d files not yet implemented",nFiles);
    break;
  default:
    cout << "Huh?" << endl;
  } // switch

  fLastPlot = kScalarLinePlot;
}

void TH5MainFrame::VectorLinePlot() {

  // determine active datafiles
  TH5Dataset* dataToPlot[5];
  Int_t nFiles(0);
  GetSelFiles(dataToPlot,nFiles);
    
  // get selected variables
  TString stepVarNameX;
  TString stepVarNameY;
  if (nFiles) {
    stepVarNameX = TString(fSelLPBox->GetSelectedEntry()->GetTitle());
    stepVarNameY = TString(fVectorLPBox->GetSelectedEntry()->GetTitle());
  }

  // get selected coordinate:
  EH5Coord crd;
  if (kButtonDown == fRadioLP[2]->GetState()) crd = kZ; 
  if (kButtonDown == fRadioLP[1]->GetState()) crd = kY; 
  if (kButtonDown == fRadioLP[0]->GetState()) crd = kX; 

  switch (nFiles) {
  case 0:
    Error("VectorLinePlot", "No files have been selected!");
    break;
  case 1:
    LinePlot(dataToPlot[0],stepVarNameX,stepVarNameY,crd);
    break;
  case 2:
    LinePlot(dataToPlot[0],dataToPlot[1],stepVarNameX,stepVarNameY,crd);
    break;
  case 3:
    Error("VectorLinePlot", "LinePlot for %d files not yet implemented",nFiles);
    break;
  case 4:
    Error("VectorLinePlot", "LinePlot for %d files not yet implemented",nFiles);
    break;
  case 5:
    Error("VectorLinePlot", "LinePlot for %d files not yet implemented",nFiles);
    break;
  default:
    cout << "Huh?" << endl;
  } // switch

  fLastPlot = kVectorLinePlot;
}

void TH5MainFrame::LinePlot(const TH5Dataset* data, const TString stepVarNameX, 
			    const TString stepVarNameY, const EH5Coord coord) {

  TString coordName;
  if (coord==kScalar) coordName = TString("");
  if (coord==kX) coordName = TString(" (x) ");
  if (coord==kY) coordName = TString(" (y) ");
  if (coord==kZ) coordName = TString(" (z) ");

  Int_t nStep = data->GetNSteps();

  Double_t varX[nStep];
  Double_t varY[nStep];

  Double_t varXMin = 1.e20; 
  Double_t varXMax = -1.e20;
  Double_t varYMin = 1.e20;
  Double_t varYMax = -1.e20;

  TCanvas *canv = fEcanvas->GetCanvas();
  canv->SetRightMargin(0.25);

  for(Int_t step = 0; step < nStep; step++){ 
    varX[step] = data->GetStepVar(stepVarNameX,kScalar,step);
    if (varX[step] < varXMin) varXMin = varX[step];
    if (varX[step] > varXMax) varXMax = varX[step];
    
    varY[step] = data->GetStepVar(stepVarNameY,coord,step);

    if (varY[step] < varYMin) varYMin = varY[step];
    if (varY[step] > varYMax) varYMax = varY[step];
  }
  
  AddMargin(varXMin,varXMax);
  AddMargin(varYMin,varYMax);

  TH2F *linePlot = static_cast<TH2F*>(gROOT->FindObject("linePlot"));
  if (linePlot) delete linePlot;

  linePlot = new TH2F("linePlot","line plot",
		      200,varXMin,varXMax,200,varYMin,varYMax);
  canv->Clear();
  linePlot->SetStats(kFALSE);
  linePlot->Draw();

  TGraph* varGraph = new TGraph(nStep,varX,varY);
  varGraph->Draw("LP");
  
  TString varXTitle = stepVarNameX;
  TString varYTitle = stepVarNameY+coordName;

  if (data->GetUnit(stepVarNameX) != "") 
    varXTitle += AddBrackets(data->GetUnit(stepVarNameX));
  if (data->GetUnit(stepVarNameY) != "") 
    varYTitle += AddBrackets(data->GetUnit(stepVarNameY));

  TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
  leg->AddEntry(varGraph, data->GetFilename(),"lp");
  leg->SetHeader("line plot");
  leg->Draw();
  
  linePlot->SetYTitle(varYTitle);
  linePlot->SetXTitle(varXTitle);
  canv->Update();

}

void TH5MainFrame::LinePlot(const TH5Dataset* data0, const TH5Dataset* data1, 
			    const TString stepVarNameX, const TString stepVarNameY, 
			    const EH5Coord coord) {

  TString coordName;
  if (coord==kScalar) coordName = TString("");
  if (coord==kX) coordName = TString(" (x) ");
  if (coord==kY) coordName = TString(" (y) ");
  if (coord==kZ) coordName = TString(" (z) ");

  Int_t nStep0 = data0->GetNSteps();
  Int_t nStep1 = data1->GetNSteps();

  Double_t varY0[nStep0];
  Double_t varX0[nStep0];
  Double_t varY1[nStep1];
  Double_t varX1[nStep1];

  Double_t varXMin0 = 1.e20; 
  Double_t varXMax0 = -1.e20;
  Double_t varYMin0 = 1.e20;
  Double_t varYMax0 = -1.e20;
  Double_t varXMin1 = 1.e20; 
  Double_t varXMax1 = -1.e20;
  Double_t varYMin1 = 1.e20;
  Double_t varYMax1 = -1.e20;

  TCanvas *canv = fEcanvas->GetCanvas();
  canv->SetRightMargin(0.25);

  for(Int_t step = 0; step < nStep0; step++){ 
    varX0[step] = data0->GetStepVar(stepVarNameX,kScalar,step);
    if (varX0[step] < varXMin0) varXMin0 = varX0[step];
    if (varX0[step] > varXMax0) varXMax0 = varX0[step];
    
    varY0[step] = data0->GetStepVar(stepVarNameY,coord,step);
    if (varY0[step] < varYMin0) varYMin0 = varY0[step];
    if (varY0[step] > varYMax0) varYMax0 = varY0[step];
  }
  
  for(Int_t step = 0; step < nStep1; step++){ 
    varX1[step] = data1->GetStepVar(stepVarNameX,kScalar,step);
    if (varX1[step] < varXMin1) varXMin1 = varX1[step];
    if (varX1[step] > varXMax1) varXMax1 = varX1[step];
    
    varY1[step] = data1->GetStepVar(stepVarNameY,coord,step);
    if (varY1[step] < varYMin1) varYMin1 = varY1[step];
    if (varY1[step] > varYMax1) varYMax1 = varY1[step];
  }

  Double_t varXMin = varXMin0 <? varXMin1;
  Double_t varXMax = varXMax0 >? varXMax1;
  Double_t varYMin = varYMin0 <? varYMin1;
  Double_t varYMax = varYMax0 >? varYMax1;
  
  AddMargin(varXMin,varXMax);
  AddMargin(varYMin,varYMax);
  
  TH2F *linePlot = static_cast<TH2F*>(gROOT->FindObject("linePlot"));
  if (linePlot) delete linePlot;

  linePlot = new TH2F("linePlot","line plot",
			    200,varXMin,varXMax,200,varYMin,varYMax);
  canv->Clear();
  linePlot->SetStats(kFALSE);
  linePlot->Draw();

  TGraph* varGraph0 = new TGraph(nStep0,varX0,varY0);
  varGraph0->SetMarkerStyle(20);
  varGraph0->SetLineStyle(kSolid);
  
  TGraph* varGraph1 = new TGraph(nStep1,varX1,varY1);
  varGraph1->SetMarkerStyle(25);
  varGraph1->SetLineStyle(kDashed);
  
  varGraph0->Draw("LP");
  varGraph1->Draw("LPsame");

  TString varXTitle = stepVarNameX;
  TString varYTitle = stepVarNameY+coordName;

  if (data0->GetUnit(stepVarNameX) == data1->GetUnit(stepVarNameX)) {
    if (data0->GetUnit(stepVarNameX) != "") {
      varXTitle += AddBrackets(data0->GetUnit(stepVarNameX));
    }
  } else {
    Warning("LinePlot","Inconsistent units between files %s and %s",
	    data0->GetFilename().Data(),data1->GetFilename().Data());
    if (data0->GetUnit(stepVarNameX) != "") {
      varXTitle += AddBrackets(data0->GetUnit(stepVarNameX));
    } else if (data1->GetUnit(stepVarNameX) != "") {
      varXTitle += AddBrackets(data1->GetUnit(stepVarNameX));
    }
  }
      
  if (data0->GetUnit(stepVarNameY) == data1->GetUnit(stepVarNameY)) {
    if (data0->GetUnit(stepVarNameY) != "") {
      varYTitle += AddBrackets(data0->GetUnit(stepVarNameY));
    }
  } else {
    Warning("LinePlot","Inconsistent units between files %s and %s",
	    data0->GetFilename().Data(),data1->GetFilename().Data());
    if (data0->GetUnit(stepVarNameY) != "") {
      varYTitle += AddBrackets(data0->GetUnit(stepVarNameY));
    } else if (data1->GetUnit(stepVarNameY) != "") {
      varYTitle += AddBrackets(data1->GetUnit(stepVarNameY));
    }
  }
      

  TH5Legend* leg = new TH5Legend(0.8,0.8,1.,0.95);
  leg->AddEntry(varGraph0, data0->GetFilename(),"lp");
  leg->AddEntry(varGraph1, data1->GetFilename(),"lp");
  leg->SetHeader("line plot");
  leg->Draw();
  
  linePlot->SetYTitle(varYTitle);
  linePlot->SetXTitle(varXTitle);
  canv->Update();

}

void TH5MainFrame::GetSelVar(TString* selVarName[8], Int_t& nv) {

  nv=0;
  Int_t iter(0);
  for (Int_t n=0; n<8; ++n) {
    selVarName[n] = NULL;
    if (fVarName[n]) {
      if (fVarButton[n]->GetState()==kButtonDown) {
	nv++;
	selVarName[iter] = &fVarName[n];
	iter++;
      }
    }
  }
}

void TH5MainFrame::GetSelFiles(TH5Dataset* selFile[5], Int_t& nf) {

  nf=0;
  Int_t iter(0);

  for (Int_t n=0; n<5; ++n) {
    selFile[n] = NULL;
    if (fDataset[n]) {
      if (fDataset[n]->GetState()==kActive)  {
	nf++;
	selFile[iter] = fDataset[n];
	iter++;
      }
    }
  }
}

void TH5MainFrame::PrintCanv() {

  Int_t format = fPrintBox->GetSelected();

  TCanvas *canv = fEcanvas->GetCanvas();

  // construct filename from active files and plotted variables:
  TString PrintFilename;

  Int_t nFiles(0);
  for (Int_t n=0; n<5; ++n) {
    if (fDataset[n]) {
      if (fDataset[n]->GetState()==kActive)  {
	if (nFiles > 0) {
	  PrintFilename+= TString("-");
	  PrintFilename+= fDataset[n]->GetFilenameNoExt();
	} else {
	  PrintFilename = fDataset[n]->GetFilenameNoExt();
	}
	nFiles++;
      }
    }
  }
    
  Int_t nVars(0);

  if (fLastPlot==kHisto1d || fLastPlot==kHisto2d || fLastPlot==kHisto3d) {
    for (Int_t n=0; n<8; ++n) {
      if (fVarName[n]) {
	if (fVarButton[n]->GetState()==kButtonDown) {
	  PrintFilename += TString("-");
	  PrintFilename += fVarName[n];
	  nVars++;
	}
      }
    }
  }

  if (fLastPlot==kScalarLinePlot) {
    PrintFilename += TString("-");
    PrintFilename += fScalarLPBox->GetSelectedEntry()->GetTitle();
    PrintFilename += TString("-");
    PrintFilename += fSelLPBox->GetSelectedEntry()->GetTitle();
  }

  if (fLastPlot==kVectorLinePlot) {
    TString coordName;
    if (kButtonDown == fRadioLP[2]->GetState()) coordName = TString("z"); 
    if (kButtonDown == fRadioLP[1]->GetState()) coordName = TString("y");
    if (kButtonDown == fRadioLP[0]->GetState()) coordName = TString("x"); 
  
    PrintFilename += TString("-");
    PrintFilename += fVectorLPBox->GetSelectedEntry()->GetTitle();
    PrintFilename += TString("-");
    PrintFilename += coordName;
    PrintFilename += TString("-");
    PrintFilename += fSelLPBox->GetSelectedEntry()->GetTitle();
  }

  PrintFilename+= ".";
  PrintFilename+= H5PrintFormat[format];
  canv->Print(PrintFilename,H5PrintFormat[format]);

  // note: could do this directly like this:
  //  PrintFilename+= ".";
  //  PrintFilename+= fPrintBox->GetSelectedEntry()->GetTitle();
  //  canv->Print(PrintFilename,fPrintBox->GetSelectedEntry()->GetTitle());
  // but then we would not have the opportunity to choose option labels in the 
  // GUI that are different from the sometimes cryptic ROOT option strings!

}

void TH5MainFrame::DoStepSlider(Int_t step) {
  
  Int_t id;
  TGFrame *frm = (TGFrame *) gTQSender;
  if (frm->IsA()->InheritsFrom(TGSlider::Class())) {
    TGSlider *sl = (TGSlider*) frm;
    id = sl->WidgetId();
  } else {
    TGDoubleSlider *sd = (TGDoubleSlider *) frm;
    id = sd->WidgetId();
  }
  
  char bufStep[32];
  sprintf(bufStep, "%d", step);
  
  Double_t spos = Spos(step);
  char bufSpos[32];
  sprintf(bufSpos, "%g", spos);
  
  switch (id) {
  case stepSliderID:
    fStepSliderTB->Clear();
    fStepSliderTB->AddText(0, bufStep);
    // Re-align the cursor with the characters.
    fStepSliderTE->SetCursorPosition(fStepSliderTE->GetCursorPosition());
    fStepSliderTE->Deselect();
    gClient->NeedRedraw(fStepSliderTE);
    
    fSposSliderTB->Clear();
    fSposSliderTB->AddText(0, bufSpos);
    // Re-align the cursor with the characters.
    fSposSliderTE->SetCursorPosition(fSposSliderTE->GetCursorPosition());
    fSposSliderTE->Deselect();
    gClient->NeedRedraw(fSposSliderTE);

    break;
  default:
    break;
  }
}


void TH5MainFrame::DoStepText(const char * ) {

  TGTextEntry *te = (TGTextEntry *) gTQSender;
  Int_t id = te->WidgetId();
  
  Double_t spos;
  Int_t step;
  char bufSpos[32];
  char bufStep[32];

  switch (id) {
  case stepSliderTEID:
    step = atoi(fStepSliderTB->GetString());
    fStepSlider->SetPosition(step);
    // update spos field:
    spos = Spos(step);
    sprintf(bufSpos, "%g", spos);
    fSposSliderTB->Clear();
    fSposSliderTB->AddText(0, bufSpos);
    fSposSliderTE->SetCursorPosition(fSposSliderTE->GetCursorPosition());
    fSposSliderTE->Deselect();
    gClient->NeedRedraw(fSposSliderTE);
    break;
  case sposSliderTEID:
    spos = atof(fSposSliderTB->GetString());
    step = Step(spos);
    fStepSlider->SetPosition(step);
    // update step field:
    sprintf(bufStep, "%d", step);
    fStepSliderTB->Clear();
    fStepSliderTB->AddText(0, bufStep);
    fStepSliderTE->SetCursorPosition(fStepSliderTE->GetCursorPosition());
    fStepSliderTE->Deselect();
    gClient->NeedRedraw(fStepSliderTE);
    break;
  default:
    break;
  }
}

void TH5MainFrame::DoStepTextReturn(const char * ) {

  TGTextEntry *te = (TGTextEntry *) gTQSender;
  Int_t id = te->WidgetId();
  
  Double_t spos;
  Int_t step;
  char bufSpos[32];
  char bufStep[32];

  switch (id) {
  case sposSliderTEID:
    spos = atof(fSposSliderTB->GetString());
    step = Step(spos);
    fStepSlider->SetPosition(step);
    // update step field:
    sprintf(bufStep, "%d", step);
    fStepSliderTB->Clear();
    fStepSliderTB->AddText(0, bufStep);
    fStepSliderTE->SetCursorPosition(fStepSliderTE->GetCursorPosition());
    fStepSliderTE->Deselect();
    gClient->NeedRedraw(fStepSliderTE);
    // update spos field: (jump to next smaller spos)
    spos = Spos(step);
    sprintf(bufSpos, "%g", spos);
    fSposSliderTB->Clear();
    fSposSliderTB->AddText(0, bufSpos);
    fSposSliderTE->SetCursorPosition(fSposSliderTE->GetCursorPosition());
    fSposSliderTE->Deselect();
    gClient->NeedRedraw(fSposSliderTE);
    break;
  default:
    break;
  }
}

void TH5MainFrame::PlotNextStep(Int_t jump) {

  // update step slider:
  Int_t step = fStepSlider->GetPosition();
  Int_t stepMax = fStepSlider->GetMaxPosition();
  step += jump;
  if (step > stepMax) step = stepMax;
  if (step < 0) step = 0;
  fStepSlider->SetPosition(step);
  fStepSlider->PositionChanged(step);  // triggers DoStepSlider!

  DoLastPlot();

}
 
void TH5MainFrame::DoLastPlot() {

  // redo last plot:
  if (fLastPlot==kHisto1d) Histo1d();
  if (fLastPlot==kHisto2d) Histo2d();
  if (fLastPlot==kHisto3d) Histo3d();
  if (fLastPlot==kScalarLinePlot) ScalarLinePlot();
  if (fLastPlot==kVectorLinePlot) VectorLinePlot();

}

void TH5MainFrame::DoLinePlotRadioButtons()
{
  
  TGButton *btn = (TGButton *) gTQSender;
  Int_t id = btn->WidgetId();
  
  switch (id) {
  case linePlotXID:  
    fRadioLP[1]->SetState(kButtonUp);
    fRadioLP[2]->SetState(kButtonUp);
    break;
  case linePlotYID:  
    fRadioLP[0]->SetState(kButtonUp);
    fRadioLP[2]->SetState(kButtonUp);
    break;
  case linePlotZID:  
    fRadioLP[0]->SetState(kButtonUp);
    fRadioLP[1]->SetState(kButtonUp);
    break;
  default:
    break;
   }
}


Int_t TH5MainFrame::Step(const Double_t spos ) {
  
  TH5Dataset* activeData[5];
  Int_t nFiles(0);
  GetSelFiles(activeData,nFiles);

  Int_t step;
  if (activeData[0]) {
    step = activeData[0]->GetStep(spos);
  } else {
    step = TMath::Nint(0.5*spos); // dummy
  }
  return(step);
}

Double_t TH5MainFrame::Spos(const Int_t step ) {

  TH5Dataset* activeData[5];
  Int_t nFiles(0);
  GetSelFiles(activeData,nFiles);

  Double_t spos;
  if (activeData[0]) {
    spos = activeData[0]->GetSpos(step);
  } else {
    spos = 2.*step;   // dummy
  }
  return (spos);
}

void TH5MainFrame::Test() {
  // try to change name of test button!
  if (*(fTestButton->GetText())==TGHotString("   Test   ")) {
    fTestButton->SetText("Why");
  } else if (*(fTestButton->GetText())==TGHotString("Why")) {
    fTestButton->SetText("the");
  } else if (*(fTestButton->GetText())==TGHotString("the")) {
    fTestButton->SetText("hell");
  } else if (*(fTestButton->GetText())==TGHotString("hell")) {
    fTestButton->SetText("do");
  } else if (*(fTestButton->GetText())==TGHotString("do")) {
    fTestButton->SetText("you");
  } else if (*(fTestButton->GetText())==TGHotString("you")) {
    fTestButton->SetText("keep");
  } else if (*(fTestButton->GetText())==TGHotString("keep")) {
    fTestButton->SetText("clicking?");
  } else if (*(fTestButton->GetText())==TGHotString("clicking?")) {
    fTestButton->SetText("   Test   ");
  } else {
    cout << "Huh?" << endl;
  }
}

