#ifndef __TH5Dataset__
#define __TH5Dataset__

#include <TObject.h>
#include <TNtuple.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include <TVector3.h>
#include <TObjString.h>
#include <TString.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TArrayD.h>
#include <TCanvas.h>

#include <iostream>
#include <string>

#include "H5Part.hh"

enum EH5State { kNotExist=-1, kNotActive=0, kActive=1};
enum EH5Coord { kScalar, kX, kY, kZ };

class H5PartFile;

using namespace std;

class TH5Dataset : public TObject
{
 private:
  
  H5PartFile* fH5File;      // H5 file
  TString fFullFilename;     // full filename of H5file
  TString fFilename;         // filename of H5file (without path)
  TString fFilenameNoExt;    // filename of H5file (without path, no extension)
  EH5State fFileState;      // flag for active/loaded...
  Int_t fNStep;             // number of time steps
  Int_t fNStepAttr;         // number of step attributes
  Int_t fNFileAttr;         // number of file attributes
  Int_t fNScalarAttr;       // number of scalar step attributes
  Int_t fNVectorAttr;       // number of vector step attributes

  TObjArray fFileAttr;

  TString fPartVarName[9];
  TString fPartVarUnit[9];

  TObjArray fScalarName;     
  TObjArray fScalarUnit;     
  TObjArray fVectorName;    
  TObjArray fVectorUnit;    

  TArrayD* fSpos;           // spos values 
  TH2D* fScalar;            // 2D histogram to emulate 2D array
  TClonesArray*  fVector;   // vector attributes are TClonesArray (TClonesArray of 
                            // TClonesArray of TVector3's...

  TClonesArray*  fNtuple;   // array of ntuples (one per step)

 public:

  TH5Dataset(const char * filename, const Int_t subsamplingRate = 1); // constructor
  TH5Dataset(const TString& filename, const Int_t subsamplingRate = 1); // alternative constructor
  Int_t Construct(const char * filename, const Int_t subsamplingRate); // the real constructor!
  
  ~TH5Dataset(); // Destructor
  
  Int_t GetNSteps() const { return(fNStep); }
  Int_t GetNScalarAttr() const { return(fNScalarAttr); }
  Int_t GetNVectorAttr() const { return(fNVectorAttr); }

  Double_t GetSpos(const Int_t step) const ;  
  Int_t GetStep(const Double_t spos) const ;  

  Double_t GetStepVar(TString stepVarName, EH5Coord crd, Int_t step) const ;
  Double_t GetScalarStepVar(TString stepVarName, Int_t step) const ;
  TVector3 GetVectorStepVar(TString stepVarName, Int_t step) const ;

  TString GetFullFilename() const { return(fFullFilename); }
  TString GetFilename() const { return(fFilename); }
  TString GetFilenameNoExt() const { return(fFilenameNoExt); }

  EH5State GetState() const { return(fFileState); }
  void SetState(const EH5State s) { fFileState=s; }

  TString GetPartVarName(const Int_t index) const ; 
  TString GetScalarStepVarName(const Int_t index) const ;
  TString GetVectorStepVarName(const Int_t index) const ;

  TString GetUnit(const TString varName) const ;

  TH1F* Histo1d(const TString* varName, const Int_t step=0, 
		const Double_t min=-1., const Double_t max = 1., 
		const Int_t nBin=30);

  TH2F* Histo2d(const TString* varNameX, const TString* varNameY, const Int_t step=0, 
		const Double_t minX=-1., const Double_t maxX = 1., 
		const Double_t minY=-1., const Double_t maxY = 1., 
		const Int_t nBinX=50, Int_t nBinY=50);
  
  TH1F* Scatter(const TString* varNameX, const TString* varNameY, TCanvas* canv, 
		const Int_t step=0, 
		const Double_t minX=-2., const Double_t maxX = 2., 
		const Double_t minY=-2., const Double_t maxY = 2.); 
  
  TH3F* Histo3d(const TString* varNameX, const TString* varNameY, const TString* varNameZ, 
		const Int_t step=0, 
	        const Double_t minX=-1., const Double_t maxX = 1., 
		const Double_t minY=-1., const Double_t maxY = 1., 
		const Double_t minZ=-1., const Double_t maxZ = 1., 
		const Int_t nBinX=30, const Int_t nBinY=30, const Int_t nBinZ=30);

  Double_t FindMinimum(const TString* varName, Int_t step=0) const ;
  Double_t FindMaximum(const TString* varName, Int_t step=0) const ;
  Double_t PlotRange(const TString* varName, const Int_t step, 
		     Double_t& plotMin, Double_t& plotMax) const ;
  Double_t Range(const TString* varName, const Int_t step, 
		 Double_t& min, Double_t& max) const ;

  ClassDef(TH5Dataset,1);
    
};

Double_t AddMargin(Double_t& plotMin, Double_t& plotMax, const Double_t margin=0.1);

TString AddBrackets(const TString s);;

#endif
