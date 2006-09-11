#include "TH5Dataset.h"

#include<iostream>

#include<H5Cpp.h>

using namespace std;

ClassImp(TH5Dataset)
  
Double_t AddMargin(Double_t& plotMin, Double_t& plotMax, const Double_t margin) {
  // (by default) add 10% at each edge to make a nice plot
  Double_t range = plotMax-plotMin;
  if (range < 1.e-20) {
    plotMin -= 1.;
    plotMax += 1.;
  } else {
    plotMin = plotMin-margin*range;
    plotMax = plotMax+margin*range;
  }
  return(range);
}

TString AddBrackets(TString s) {
  
  TString sb(" [");
  sb += s;
  sb += TString("] ");
  return(sb);

}

TH5Dataset::TH5Dataset(const char * fname, const Int_t subsamplingRate) { // constructor
  Int_t OK = Construct(fname, subsamplingRate);
  if (!OK) Error("TH5Dataset", "Could not create TH5Dataset!");
}

TH5Dataset::TH5Dataset(const TString& fnamestr, const Int_t subsamplingRate) { // alternative constructor
  Int_t OK = Construct(fnamestr.Data(), subsamplingRate);
  if (!OK) Error("TH5Dataset", "Could not create TH5Dataset!");
}

Int_t TH5Dataset::Construct(const char * fname, const Int_t subsamplingRate) { // the real constructor!

  // extract file names; use standard strings for these manipulations
  // (rewrite with TStrings later?)
  string sFilename = string(fname);
  string sFullFilename(sFilename);

  if(sFilename.find("/")!=string::npos) 
    sFilename.erase(0,sFilename.find_last_of("/")+1);  

  string sFilenameNoExt(sFilename);

  if(sFilenameNoExt.find(".")!=string::npos) 
    sFilenameNoExt.erase(sFilenameNoExt.find_last_of("."));  

  fFullFilename = TString(sFullFilename.c_str());
  fFilename = TString(sFilename.c_str());
  fFilenameNoExt = TString(sFilenameNoExt.c_str());

  fH5File = H5PartOpenFile(fFullFilename.Data(),H5PART_READ);

  H5PartSetStep(fH5File,0);
  Int_t nDataset = H5PartGetNumDatasets(fH5File);

  //----------------------------------------------------------------------
  // read in the variable/attribute names
  //----------------------------------------------------------------------

  const Int_t maxLength(64);
  char name[maxLength];

  for(Int_t i = 0; i < 9; i++) {
    fPartVarName[i] = TString("");
    fPartVarUnit[i] = TString("");
  }
    
  if (nDataset > 0) {
    for(Int_t i=0;i<nDataset;i++){ 
      H5PartGetDatasetName(fH5File,i,name,maxLength);
      TString varName = TString(name);
      if (varName == "x")         fPartVarName[0] = varName;
      else if (varName == "y")    fPartVarName[1] = varName;
      else if (varName == "z")    fPartVarName[2] = varName;
      else if (varName == "phiz") fPartVarName[3] = varName;
      else if (varName == "px")   fPartVarName[4] = varName;
      else if (varName == "py")   fPartVarName[5] = varName;
      else if (varName == "pz")   fPartVarName[6] = varName;
      else if (varName == "enez") fPartVarName[7] = varName;
      else if (varName == "id")   fPartVarName[8] = varName;
      else Error("Construct",
		 "Unrecognized variable name %s in file %s.",
		 varName.Data(),fFilename.Data());
    }
  }

  fNStep = H5PartGetNumSteps(fH5File);
 
  cout << "\n\tOpened " << fFilename << ", contains " 
       << fNStep << " steps, " << nDataset << " data sets.\n" << endl;

  fNStepAttr = H5PartGetNumStepAttribs(fH5File); 
  fNFileAttr = H5PartGetNumFileAttribs(fH5File); 

  // list file attributes:
  char an[32];
  TString attrName;
  int* nElem = new int;

  if (fNFileAttr > 0) {
    printf("\tFound %d file attributes:\n",fNFileAttr);
    for (Int_t n = 0; n < fNFileAttr; n++) {

      H5PartGetFileAttribInfo(fH5File,n,an,32,0,nElem);
      attrName = TString(an);
      fFileAttr.AddLast(new TObjString(attrName));

      // nice print out:
      if (n==fNFileAttr-1) { 
	cout << attrName << endl << endl;
      } else if (n==0) {
	cout << "\t" << attrName << ", ";
      } else {
	cout << attrName << ", ";
	if (!((n+1)%5)) cout << endl << "\t";
      }
    }
  } else {
    printf("\tFound no file attributes.\n");
  }

  printf("\tFound %d step attributes:\n",fNStepAttr);

  // retrieve step attributes for step 0 (assume they're the same for all steps!)
  H5PartSetStep(fH5File,0); 
  TString sposName;

  fNScalarAttr=0;
  fNVectorAttr=0;

  // initialize names:
  for (Int_t n=0; n<fNStepAttr; ++n) {
    H5PartGetStepAttribInfo(fH5File,n,an,32,0,nElem);
    //    printf("Attrib name nr %d is \"%s\", it has %d elements \n",n,an,*nElem);
    attrName = TString(an);
    if ((attrName=="Spos"||attrName=="spos"||attrName=="s")&& *nElem==1) {
      sposName = attrName;
    }

    TString type("?");
    if  (*nElem==1 )  { // scalar variable
      fScalarName.AddLast(new TObjString(attrName));
      fNScalarAttr++;
      type = TString(" (scalar)");
    } else if (*nElem==3 )  { // vector variable
      fVectorName.AddLast(new TObjString(attrName));
      fNVectorAttr++;
      type = TString(" (vector)");
    } else {
      type = TString(" (");
      type += *nElem;
      type += TString(" elements)");
      //      Warning("Construct","Step attribute %s with %d elements will be ignored",an,*nElem);
    }
    TString nameAndType = attrName + type;
    if (n==fNStepAttr-1) { 
      cout << nameAndType << endl << endl;
    } else if (n==0) {
      cout << "\t" << nameAndType << ", ";
    } else {
      cout << nameAndType << ", ";
      if (!((n+1)%3)) cout << endl << "\t";
    }
  }

  //----------------------------------------------------------------------
  // read in the units
  //----------------------------------------------------------------------

  for (Int_t i = 0; i < 9; i++) fPartVarUnit[i] = GetUnit(fPartVarName[i]);
  for (Int_t i = 0; i < fNScalarAttr; i++) 
    fScalarUnit.AddLast(new TObjString(GetUnit(static_cast<TObjString*>
					       (fScalarName.At(i))->GetString())));
  for (Int_t i = 0; i < fNVectorAttr; i++) 
    fVectorUnit.AddLast(new TObjString(GetUnit(static_cast<TObjString*>
					       (fVectorName.At(i))->GetString())));

  //----------------------------------------------------------------------
  // read in the data
  //----------------------------------------------------------------------

  // initialize (clones) arrays
  fSpos = new TArrayD(fNStep);
  // add filename to histogram name to avoid memory leak warning
  fScalar = new TH2D((TString("fScalar-")+fFilenameNoExt).Data(),
		     "scalar values",fNStep,0.,1.,fNScalarAttr,0.,1.);
  fVector = new TClonesArray("TClonesArray",fNStep);
  for (Int_t step=0;step<fNStep;++step) {
    TClonesArray &fV = *fVector;
    new(fV[step]) TClonesArray("TVector3",fNVectorAttr);
  }

  fNtuple = new TClonesArray("TNtuple",fNStep);

  // Loop over steps, fill ntuples and vectors (one per step)
  Double_t val;
  Double_t spos;
  Double_t arr[3];
      
  if (nDataset > 0) {

    for (Int_t step=0; step<fNStep; step++) {
      
      char ntupleName[16];
      sprintf(ntupleName,"ntuple%u",step);
      char ntupleDesc[32];
      sprintf(ntupleDesc,"data from step %u",step);
   
      H5PartSetStep(fH5File,step);
      unsigned long n = H5PartGetNumParticles(fH5File);

      // read in scalar attributes
      for (int i=0; i<fNScalarAttr; ++i) {
	TObjString* s = static_cast<TObjString*>(fScalarName.At(i));
	H5PartReadStepAttrib(fH5File,const_cast<char*>(s->GetString().Data()), &val);
	if ( TMath::Abs(val) < 1.e-20) {
	  if (step != 0 && val != 0.) {
	    Warning("Construct","Read in very small value %e for attribute %s, will set to 0.",
		    val,const_cast<char*>(s->GetString().Data()));
	  }
	  val = 0.;
	}
	fScalar->SetBinContent(step,i,val);
      }

      // separately read in spos values:
      H5PartReadStepAttrib(fH5File,const_cast<char*>(sposName.Data()), &spos);
      (*fSpos)[step] = spos;

      // read in vector attributes
      for (int i=0; i<fNVectorAttr; ++i) {
	TObjString* s = static_cast<TObjString*>(fVectorName.At(i));
	H5PartReadStepAttrib(fH5File,const_cast<char*>(s->GetString().Data()), arr);
	TClonesArray* a = static_cast<TClonesArray*>(fVector->At(step));
	if ( TMath::Abs(arr[0]) < 1.e-20 && TMath::Abs(arr[1]) < 1.e-20 && TMath::Abs(arr[2]) < 1.e-20) {
	  Warning("Construct","Read in very small values %e, %e, %e for attribute %s, will set to 0.",
		  arr[0],arr[1],arr[2],const_cast<char*>(s->GetString().Data()));
	  arr[0] = 0.; arr[1] = 0.;arr[2] = 0.;
	}
	(*a)[i] = new TVector3(arr);
      }

      // demonstrate how to read elements from fVector:
      TClonesArray* fV = static_cast<TClonesArray*>(fVector->At(step));
      TVector3* v = static_cast<TVector3*>(fV->At(0));
      cout << "\tStep " << step << ": n = " << n << ", Spos = " << (*fSpos)[step] <<  ",\t" 
	   << static_cast<TObjString*>(fVectorName.At(0))->GetString() << ": "  
	   << v->x() << " " << v->y() << " " << v->z() << endl;

      TClonesArray &fN = *fNtuple;
      new(fN[step]) TNtuple(ntupleName,ntupleDesc,"x:y:z:phiz:px:py:pz:enez:id");

      double *x= new double[n];
      double *y= new double[n];
      double *z= new double[n];
      double *px= new double[n];
      double *py= new double[n];
      double *pz= new double[n];
      long long *id= new long long[n];

      double *phiz= new double[n];
      double *enez= new double[n];

      //      H5PartReadParticleStep(fH5File,step,x,y,z,px,py,pz,id);

      if (fPartVarName[0] == "x") {
	H5PartReadDataFloat64(fH5File,"x",x);
      } else {
	for (unsigned long i = 0; i < n; i++) x[i] = 0.;
      }

      if (fPartVarName[1] == "y") {
	H5PartReadDataFloat64(fH5File,"y",y);
      } else {
	for (unsigned long i = 0; i < n; i++) y[i] = 0.;
      }

      if (fPartVarName[2] == "z") {
	H5PartReadDataFloat64(fH5File,"z",z);
      } else {
	for (unsigned long i = 0; i < n; i++) z[i] = 0.;
      }

      if (fPartVarName[4] == "px") {
	H5PartReadDataFloat64(fH5File,"px",px);
      } else {
	for (unsigned long i = 0; i < n; i++) px[i] = 0.;
      }

      if (fPartVarName[5] == "py") {
	H5PartReadDataFloat64(fH5File,"py",py);
      } else {
	for (unsigned long i = 0; i < n; i++) py[i] = 0.;
      }

      if (fPartVarName[6] == "pz") {
	H5PartReadDataFloat64(fH5File,"pz",pz);
      } else {
	for (unsigned long i = 0; i < n; i++) pz[i] = 0.;
      }

      if (fPartVarName[3] == "phiz") {
	H5PartReadDataFloat64(fH5File,"phiz",phiz);
      } else {
	for (unsigned long i = 0; i < n; i++) phiz[i] = 0.;
      }

      if (fPartVarName[7] == "enez") {
	H5PartReadDataFloat64(fH5File,"enez",enez);
      } else {
	for (unsigned long i = 0; i < n; i++) enez[i] = 0.;
      }

      if (fPartVarName[8] == "id") {
	H5PartReadDataInt64(fH5File,"id",id);
      } else {
	for (unsigned long i = 0; i < n; i++) id[i] = 0;
      }

      for (unsigned long i=0; i<n; i++) {    
	// subsampling: only fill every n-th particle into ntuple:
	if (!(i%subsamplingRate)) {
	  static_cast<TNtuple*>(fNtuple->At(step))->
	    Fill(x[i],y[i],z[i],phiz[i],px[i],py[i],pz[i],enez[i],static_cast<double>(id[i])); 
	}
      }
   
      delete[] x;  delete[] y;  delete[] z;
      delete[] px; delete[] py; delete[] pz;  
      delete[] id;
      delete[] phiz; delete[] enez;
    }// end for loop over steps

  } // end if (nDataset > 0) 

  // set file active:
  fFileState = kActive;

  return(1);
}


TH5Dataset::~TH5Dataset() {

   delete [] fH5File;

   delete fScalar;
   delete fVector;
   delete fNtuple;

} // Destructor


Double_t TH5Dataset::GetSpos(const Int_t step) const {

  if (step < fNStep) {
    return(fSpos->At(step));
  } else {
    Warning("GetSpos", "Step %u is requested, but there are only %u steps!", step, fNStep);
    return(fSpos->At(fNStep-1));
  }
}

Int_t TH5Dataset::GetStep(const Double_t spos) const {

  if (fNStep == 0) return(0);
  Int_t step = 0;

  while (fSpos->At(step) < spos && step < fNStep) step++;
  step -= 1;
  return(step);

}

Double_t TH5Dataset::GetStepVar(TString stepVarName, EH5Coord crd, Int_t step) const {

  if (crd==kScalar) return(GetScalarStepVar(stepVarName, step));
  if (crd==kX) return(GetVectorStepVar(stepVarName, step).x());
  if (crd==kY) return(GetVectorStepVar(stepVarName, step).y());
  if (crd==kZ) return(GetVectorStepVar(stepVarName, step).z());
  return(0.);
}

Double_t TH5Dataset::GetScalarStepVar(TString stepVarName, Int_t step) const {

  if (step >= fNStep) {
    Warning("GetScalarStepVar", "Step %u is requested, but there are only %u steps!", step, fNStep);
    step = fNStep-1;
  }
  Double_t val(0.);
  for (Int_t i=0;i<fNScalarAttr;++i) {
    TObjString* s = static_cast<TObjString*>(fScalarName.At(i));
    if (s->GetString() == stepVarName) val = fScalar->GetBinContent(step,i);
  }
  return (val);
}

TVector3 TH5Dataset::GetVectorStepVar(TString stepVarName, Int_t step) const {

  if (step >= fNStep) {
    Warning("GetVectorStepVar", "Step %u is requested, but there are only %u steps!", step, fNStep);
    step = fNStep-1;
  }
  TVector3 vec = TVector3(0.,0.,0.);
  for (Int_t i=0;i<fNVectorAttr;++i) {
    TObjString* s = static_cast<TObjString*>(fVectorName.At(i));
    if (s->GetString() == stepVarName) {
      TClonesArray* fV = static_cast<TClonesArray*>(fVector->At(step));
      TVector3* pv = static_cast<TVector3*>(fV->At(i));
      vec = *pv;
    }
  }
  return (vec);

}

TH1F* TH5Dataset::Histo1d(const TString* varName, const Int_t step, 
			  const Double_t min, const Double_t max, const Int_t nBin) {
  
  TString vname = *varName;
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  TString hname = vname + TString(" distribution");

  // delete previous histogram to avoid memory leak
  // (caller's responsibility to change name if histo is needed later!)
  TH1F* histVar = (TH1F*) gROOT->FindObject("histVar");
  if (histVar) delete histVar;

  histVar = new TH1F("histVar",hname,nBin,min,max);
  histVar->SetXTitle(vname);
  histVar->SetYTitle("entries");
    
  if (nt) nt->Draw(vname+TString(">>histVar"));

  return(histVar);
}

TH2F* TH5Dataset::Histo2d(const TString* varNameX, const TString* varNameY, const Int_t step, 
			  const Double_t minX, const Double_t maxX, 
			  const Double_t minY, const Double_t maxY, 
			  const Int_t nBinX, const Int_t nBinY) {

  TString vxname = *varNameX;
  TString vyname = *varNameY;
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  TString hname = vxname+TString("-")+vyname+TString(" distribution");
  
  TH2F *histXY = static_cast<TH2F*>(gROOT->FindObject("histXY"));
  if (histXY) delete histXY;

  histXY = new TH2F("histXY",hname,nBinX,minX,maxX,nBinY,minY,maxY);
  histXY->SetXTitle(vxname);
  histXY->SetYTitle(vyname);
    
  if (nt) nt->Draw((vyname+TString(":")+vxname+TString(">>histXY")));

  return(histXY);

}

TH1F* TH5Dataset::Scatter(const TString* varNameX, const TString* varNameY, TCanvas* canv, 
			 const Int_t step, 
			 const Double_t minX, const Double_t maxX, 
			 const Double_t minY, const Double_t maxY) {

  TString vxname = *varNameX;
  TString vyname = *varNameY;
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  TString hname = vxname+TString("-")+vyname+TString(" distribution");

  TH1F *scatPlot = canv->DrawFrame(minX,minY,maxX,maxY);
  scatPlot->SetXTitle(vxname);
  scatPlot->SetYTitle(vyname);

  Int_t origStyle = nt->GetMarkerStyle();
  nt->SetMarkerStyle(1);
  if (nt) nt->Draw(vyname+TString(":")+vxname,"","same");
  nt->SetMarkerStyle(origStyle);

  return(scatPlot);
}

TH3F* TH5Dataset::Histo3d(const TString* varNameX, const TString* varNameY, const TString* varNameZ, 
			  const Int_t step, 
			  const Double_t minX, const Double_t maxX, 
			  const Double_t minY, const Double_t maxY, 
			  const Double_t minZ, const Double_t maxZ, 
			  const Int_t nBinX, const Int_t nBinY, const Int_t nBinZ) {

  TString vxname = *varNameX;
  TString vyname = *varNameY;
  TString vzname = *varNameZ;
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  TString hname = vxname+TString("-")+vyname+TString("-")+vzname+TString(" distribution");
  
  TH3F *histXYZ = static_cast<TH3F*>(gROOT->FindObject("histXYZ"));
  if (histXYZ) delete histXYZ;

  histXYZ = new TH3F("histXYZ",hname,
		     nBinX,minX,maxX,nBinY,minY,maxY,nBinZ,minZ,maxZ);
  histXYZ->SetXTitle(vxname);
  histXYZ->SetYTitle(vyname);
  histXYZ->SetZTitle(vzname);
    
  if (nt) nt->Draw(vzname+TString(":")+vyname+TString(":")+vxname+TString(">>histXYZ"));

  return(histXYZ);

}


Double_t TH5Dataset::FindMinimum(const TString* varName, Int_t step) const {
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  Double_t min = nt->GetMinimum(*varName);
  return(min);
}

Double_t TH5Dataset::FindMaximum(const TString* varName, Int_t step) const {
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  Double_t max = nt->GetMaximum(*varName);
  return(max);
}

Double_t TH5Dataset::PlotRange(const TString* varName, const Int_t step, 
			       Double_t& plotMin, Double_t& plotMax) const {
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  plotMin = nt->GetMinimum(*varName);
  plotMax = nt->GetMaximum(*varName);
  return(AddMargin(plotMin,plotMax));
}

Double_t TH5Dataset::Range(const TString* varName, const Int_t step, 
			   Double_t& min, Double_t& max) const {
  TNtuple* nt = static_cast<TNtuple*>(fNtuple->At(step));
  min = nt->GetMinimum(*varName);
  max = nt->GetMaximum(*varName);
  return(max-min);
}

TString TH5Dataset::GetPartVarName(const Int_t index) const {
  if (index >= 0 && index < 9) {
    return(fPartVarName[index]); 
  } else {
    return(TString(""));
  }
}

TString TH5Dataset::GetScalarStepVarName(const Int_t index) const { 

  return(static_cast<TObjString*>(fScalarName.At(index))->GetString());

}

TString TH5Dataset::GetVectorStepVarName(const Int_t index) const { 

  return(static_cast<TObjString*>(fVectorName.At(index))->GetString());

}

TString TH5Dataset::GetUnit(const TString varName) const { 

  TString unit("");
  TString varNameU = varName;
  varNameU += TString("Unit");

  // loop over file attributes, look for entry var+Unit:
  for (Int_t i = 0; i < fNFileAttr; i++) {
    
    TObjString* s = static_cast<TObjString*>(fFileAttr.At(i));
    if (s->GetString() == varNameU) {
      char u[32];
      H5PartReadFileAttrib(fH5File,const_cast<char*>(s->GetString().Data()), &u);
      unit = TString(u);
    }
  }
  return(unit);

}

