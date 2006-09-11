#include <TStyle.h>
#include <TROOT.h>
#include <TPaveText.h>
#include <TText.h>
#include <TLatex.h>

#include <iomanip>
#include <iostream>
using namespace std;

TPaveText *h5Name = new TPaveText(0.65,0.8,0.9,0.9,"BRNDC");
TText *h5Label = new TText();
TLatex *h5Latex = new TLatex();

void h5Style()
{
////////////////////////////////////////////////////////////////////
// PURPOSE:
//
// This header file defines a reasonable style for (black-and-white) 
// "publication quality" ROOT plots. The default settings contain 
// many features that are either not desirable for printing on white 
// paper or impair the general readibility of plots.
//
// USAGE:
//
// Simply include the line
//   #include "H5Style.h"
// at the beginning of your root program (and make sure H5Style.h is 
// in a location accessible to the compiler). Then add the line
//   h5Style(); 
// somewhere at the beginning of your main().
//
// SOME COMMENTS:
//
// Statistics and fit boxes:
//
// "Decorative" items around the histogram are kept to a minimum.
// In particular there is no box with statistics or fit information.
// You can easily change this either by editing your private copy
// of this style file or by calls to "gStyle" in your macro.
// For example, 
//   gStyle->SetOptFit(1011);
// will add some fit information.
//
// Font:
// 
// The font is chosen to be 62, i.e.helvetica-bold-r-normal with
// precision 2. Font is of course a matter of taste, but most people
// will probably agree that Helvetica bold gives close to optimal
// readibility in presentations. It appears to be the ROOT default, 
// and since there are still some features in ROOT that simply won't 
// respond to any font requests, it is the wise choice to avoid 
// ugly font mixtures on the same plot... The precision of the font (2)
// is chosen in order to have a rotatable and scalable font. Be sure
// to use true-type fonts! I.e.
// Unix.*.Root.UseTTFonts: true  in your .rootrc file. 
//
// "Landscape histograms":
//
// The style here is designed for more or less quadratic plots.
// For very long histograms, adjustements are needed. For instance, 
// for a canvas with 1x5 histograms:
//  TCanvas* c1 = new TCanvas("c1", "L0 muons", 600, 800);
//  c1->Divide(1,5);
// adaptions like the following will be needed:
//  gStyle->SetTickLength(0.05,"x");
//  gStyle->SetTickLength(0.01,"y");
//  gStyle->SetLabelSize(0.15,"x");
//  gStyle->SetLabelSize(0.1,"y");
//  gStyle->SetStatW(0.15);
//  gStyle->SetStatH(0.5);
//
////////////////////////////////////////////////////////////////////

cout << "\n\tloading H5PartROOT style file\n " << endl;

TStyle *h5Style= new TStyle("h5Style","H5Part plots style");

// use helvetica-bold-r-normal, precision 2 (rotatable)
Int_t h5Font = 62;

// line thickness
Width_t h5Width = 2;

// use plain black on white colors
h5Style->SetFrameBorderMode(0);
h5Style->SetCanvasBorderMode(0);
h5Style->SetPadBorderMode(0);
h5Style->SetPadColor(0);
h5Style->SetCanvasColor(0);
h5Style->SetStatColor(0);
h5Style->SetPalette(1);
//h5Style->SetTitleColor(0);
//h5Style->SetFillColor(0);

// set the paper & margin sizes
h5Style->SetPaperSize(20,26);
h5Style->SetPadTopMargin(0.05);
h5Style->SetPadRightMargin(0.05); // increase for colz plots!!
h5Style->SetPadBottomMargin(0.16);
h5Style->SetPadLeftMargin(0.12);

// use large fonts
h5Style->SetTextFont(h5Font);
h5Style->SetTextSize(0.08);
h5Style->SetLabelFont(h5Font,"x");
h5Style->SetLabelFont(h5Font,"y");
h5Style->SetLabelFont(h5Font,"z");
h5Style->SetLabelSize(0.05,"x");
h5Style->SetLabelSize(0.05,"y");
h5Style->SetLabelSize(0.05,"z");
h5Style->SetTitleFont(h5Font);
h5Style->SetTitleSize(0.06,"x");
h5Style->SetTitleSize(0.06,"y");
h5Style->SetTitleSize(0.06,"z");

// use bold lines and markers
h5Style->SetLineWidth(h5Width);
h5Style->SetFrameLineWidth(h5Width);
h5Style->SetHistLineWidth(h5Width);
h5Style->SetFuncWidth(h5Width);
h5Style->SetGridWidth(h5Width);
h5Style->SetLineStyleString(2,"[12 12]"); // postscript dashes
h5Style->SetMarkerStyle(8);
h5Style->SetMarkerSize(1.0);

// label offsets
h5Style->SetLabelOffset(0.015);

// statistics box:
h5Style->SetOptStat(1110);  // show only nent, mean, rms
h5Style->SetStatBorderSize(h5Width);
h5Style->SetStatFont(h5Font);
h5Style->SetStatFontSize(0.05);
h5Style->SetStatFormat("6.4g");  
h5Style->SetStatX(0.99);  // (1.,1.) is upper right corner 
h5Style->SetStatY(0.6);  // (0.,0.) is lower left corner
h5Style->SetStatW(0.22);
h5Style->SetStatH(0.15);

h5Style->SetOptTitle(0);
h5Style->SetOptFit(0);
//h5Style->SetOptFit(1011); // show probability, parameters and errors

// put tick marks on top and RHS of plots
h5Style->SetPadTickX(1);
h5Style->SetPadTickY(1);

h5Style->SetTickLength(0.04,"x");
h5Style->SetTickLength(0.02,"y");

// histogram divisions: only 7 in x to avoid label overlaps
h5Style->SetNdivisions(507,"x");
h5Style->SetNdivisions(510,"y");

// paper size for printing
h5Style->SetPaperSize(TStyle::kA4);
//h5Style->SetPaperSize(TStyle::kUSLetter);

gROOT->SetStyle("h5Style");
gROOT->ForceStyle();

h5Name->SetFillColor(0);
h5Name->SetTextAlign(12);
h5Name->SetBorderSize(0);
h5Name->AddText("H5");

h5Label->SetTextFont(h5Font);
h5Label->SetTextColor(1);
h5Label->SetTextSize(0.04);
h5Label->SetTextAlign(12);

h5Latex->SetTextFont(h5Font);
h5Latex->SetTextColor(1);
h5Latex->SetTextSize(0.04);
h5Latex->SetTextAlign(12);

}
