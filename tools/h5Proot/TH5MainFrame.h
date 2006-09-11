#ifndef __TH5MainFrame__
#define __TH5MainFrame__

#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>

#include <TGButton.h>
#include <TGTextBuffer.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TClonesArray.h>

#include "TH5Dataset.h"

enum EH5StepVar { kSpos, kEmit, kRms };

enum EWidgetIDs { stepSliderID, stepSliderTEID, sposSliderTEID, 
		  linePlotXID, linePlotYID, linePlotZID };

enum EPlotType { kNoPlot, kHisto1d, kHisto2d, kHisto3d,
		 kScalarLinePlot, kVectorLinePlot };

using namespace std;

class TH5MainFrame : public TGMainFrame {

 public:

  TH5MainFrame(const TGWindow *p, UInt_t w, UInt_t h, Int_t ssr = 1);
  virtual ~TH5MainFrame();

  void LoadFile();
  void LoadFile(const char * filename);

  void ActivateFile(Int_t slot);
  //  void SelectVar(Int_t n);
  void UpdateSelections();

  void Histo1d();
  void Histo1d(TH5Dataset* data);
  void Histo1d(TH5Dataset* data0, TH5Dataset* data1);

  void Histo2d();
  void Histo2d(TH5Dataset* data);

  void Histo3d();
  void Histo3d(TH5Dataset* data);

  void ScalarLinePlot();
  void VectorLinePlot();
  void LinePlot(const TH5Dataset* data, const TString stepVarNameX, 
		const TString stepVarNameY, const EH5Coord coord);
  void LinePlot(const TH5Dataset* data0, const TH5Dataset* data1, 
		const TString stepVarNameX, const TString stepVarNameY, 
		const EH5Coord coord);

  //  void GetSelVar(TH5Variable* selVar[6], Int_t& nv);
  void GetSelVar(TString* selVar[6], Int_t& nv);
  void GetSelFiles(TH5Dataset* selFile[5], Int_t& nf);

  void PrintCanv();

  void Test();

  void DoStepSlider(Int_t pos);
  void DoStepText(const char * );
  void DoStepTextReturn(const char * );
  void DoLinePlotRadioButtons();

  void PlotNextStep(Int_t jump);

  void DoLastPlot();

  Int_t Step(const Double_t spos);
  Double_t Spos(const Int_t step);

  TString H5DrawOpt1d[10];
  TString H5DrawOpt2d[19];
  TString H5DrawOpt3d[2];
  TString H5PrintFormat[15];

 private:
  TH5Dataset* fDataset[5];      // array of H5files 
  TString fVarName[8];          // array of particle variables 
  TString fVarNameDefault[8];   // 
  Int_t fNStepMax;              // maximum number of steps in loaded files
  EPlotType fLastPlot;          // type of last plot
  Int_t fSubsamplingRate;       // subsampling rate

  // frames, canvas
  TRootEmbeddedCanvas *fEcanvas;
  TGHorizontalFrame *fFileEntryBar;
  TGHorizontalFrame *fWorkspace;
  TGVerticalFrame *fSelspace;
  TGVerticalFrame *fVarspace;
  TGVerticalFrame *fButtonspace;
  TGVerticalFrame *fPrintspace;
  TGGroupFrame *fFileSelection;
  TGGroupFrame *fStepSelection;
  TGGroupFrame *fVarSelection;

  TGGroupFrame *fHistoFrame;
  TGHorizontalFrame *f1dFrame;
  TGHorizontalFrame *f2dFrame;
  TGHorizontalFrame *f3dFrame;
  TGGroupFrame *fLinePlotFrame;
  //  TGVerticalFrame *fLinePlotSubFrame1;
  //  TGVerticalFrame *fLinePlotSubFrame2;

  //buttons, labels, etc.
  TGTextButton *fLoad;
  TGTextButton *fExit;
  TGTextButton *fHisto1d;
  TGTextButton *fHisto2d;
  TGTextButton *fHisto3d;
  TGTextButton *fEmittance;
  TGTextButton *fRMS;

  TGTextButton *fPrintButton;

  TGTextButton *fTestButton;

  TGLabel *fFileLabel;
  TGTextEntry *fFileEntry;
  TGTextBuffer *fFilenameBuffer;

  TGCheckButton *fFileButton[5];
  TGCheckButton *fVarButton[8];

  TGComboBox *fStepBox;
  TGComboBox *fHisto1dBox;
  TGComboBox *fHisto2dBox;
  TGComboBox *fHisto3dBox;

  TGComboBox *fPrintBox;

  TGHSlider *fStepSlider;
  TGHorizontalFrame *fStepSposTEFrame;
  TGVerticalFrame *fStepTEFrame; 
  TGVerticalFrame *fSposTEFrame; 
  TGVerticalFrame *fPrevStepFrame; 
  TGVerticalFrame *fNextStepFrame; 
  TGLabel *fStepLabel; 
  TGLabel *fSposLabel; 
  TGLabel *fPrevStepLabel; 
  TGLabel *fNextStepLabel; 
  TGTextEntry *fStepSliderTE;
  TGTextBuffer *fStepSliderTB;
  TGTextEntry *fSposSliderTE;
  TGTextBuffer *fSposSliderTB;
  TGTextButton *fNextButton;
  TGTextButton *fPrevButton;

  TGHorizontalFrame *fSelLPFrame;
  TGVerticalFrame *fSelLPFrameLeft;
  TGVerticalFrame *fSelLPFrameRight;
  TGHorizontalFrame *fScalarLPFrame;
  TGHorizontalFrame *fVectorLPFrame;
  TGHorizontalFrame *fRadioLPFrame;
  TGLabel *fSelLPLabel1; 
  TGLabel *fSelLPLabel2; 
  TGTextButton *fScalar;
  TGTextButton *fVector;
  TGComboBox *fSelLPBox;
  TGComboBox *fScalarLPBox;
  TGComboBox *fVectorLPBox;
  TGRadioButton *fRadioLP[3];
  TGTextButton *fLinePlot;

  //other GUI stuff
  TGLayoutHints* fLayoutCanvas;
  TGLayoutHints* fLayoutLeftflush;
  TGLayoutHints* fLayoutRightflush;
  TGLayoutHints* fLayoutCentered;

  ClassDef(TH5MainFrame,0)
};

#endif
