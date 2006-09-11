#include <TApplication.h>
//#include <TRint.h>

#include "H5Style.h"
#include "TH5MainFrame.h"

using namespace std;

int main(int argc, char **argv) {

  h5Style();

  Int_t ssr = 1;  // subsampling rate
  Int_t nOpt = 0;

  // check command line for option strings
  if (argc > 2) {
    if (strstr(argv[1],"-ss")||strstr(argv[1],"-subSample")) {
      ssr = atoi(argv[2]);
      if (ssr < 2) {
	printf("\tBad or trivial subsampling rate %d will be ignored.\n",ssr);
	ssr = 1;
      } else {
	printf("\tSubsampling option specified. Will read in only every %dth particle.\n",ssr);
      }
      nOpt++;
    }
  }

  // parse command line for files to be loaded
  Int_t shift = 1 + 2*nOpt;
  Int_t nFiles = argc - shift;
  char *File[5];

  if (nFiles < 6) {
    for (Int_t n = 0; n < nFiles; n++) {
      File[n] = argv[n+shift];
    }
  } else {
    printf("\tError: cannot load %d files!\n\n",nFiles);
    return(0);
  }

  TApplication theApp("App",&argc,argv);

  // if a command line prompt is desired, use TRint:
  //  TRint *theApp = new TRint("Rint", &argc, argv);

  TH5MainFrame* mainGUI = new TH5MainFrame(gClient->GetRoot(),700,495,ssr);
  for (Int_t n = 0; n < nFiles; n++) mainGUI->LoadFile(File[n]);

  theApp.Run();

  return(0);
}
