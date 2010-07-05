/* h5ToVtk.cc
   Andreas Adelmann

*/

#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <string.h>
#include <hdf5.h>
#include "H5Part.h"
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cassert>
using namespace std;

#define MAX_LEN 100

/* Function headers */
int get_option(int argc, const char **argv, const char *opts, const struct long_options *l_opts);
static void print_help();
static void variable_assign(int argc, const char *argv[]);

/* Global variables */
static char* input_name      = NULL;
static char* output_name      = NULL;
static bool flg_alive = false;
static double z_pos = 0.0; 
static int print_all = 0;

/* `get_option' variables */
int         opt_err = 1;    /*get_option prints errors if this is on */
int         opt_ind = 1;    /*token pointer                          */
const char *opt_arg = NULL;        /*flag argument (or value)               */

/* indication whether the flag (option) requires an argument or not */
enum {
    no_arg = 0,         /* doesn't take an argument     */
    require_arg,        /* requires an argument	        */
};

/* struct for flags (options) */
typedef struct long_options
{
    const char  *name;          /* name of the long option              */
    int          has_arg;       /* whether we should look for an arg    */
    char         shortval;      /* the shortname equivalent of long arg
                                 * this gets returned from get_option   */
} long_options;

/* List of options in single characters */
static const char *s_opts = "h1:2:i:o:a:";

/* List of options in full words */
static struct long_options l_opts[] =
    {
	{ "help", no_arg, 'h' },         // Print help page
	{ "input", require_arg, 'i' },        // Takes input file name
	{ "output", require_arg, 'o' },   // Takes output file name (without this flag, the program will print to stdout)
	{ "alive ", no_arg , 'a' }, // also generate the alive dark current to display the dark current source
	{ NULL, 0, '\0' }
    };



/************************************************************************************
 ***********************************  FUNCTIONS  *************************************
 *************************************************************************************/


string convert2Int(int number) {
    stringstream ss;
    ss <<   setw(5) << setfill('0') <<  number; 
    return ss.str(); 
}


/* get_option is the parsing function that was majorly ported from h5dump utility */
int get_option(int argc, const char **argv, const char *opts, const struct long_options *l_opts) {
    static int sp = 1;    /* character index in current token */
    int opt_opt = '?';    /* option character passed back to user */

    if (sp == 1) 
	{
	    /* check for more flag-like tokens */
	    if (opt_ind >= argc || argv[opt_ind][0] != '-' || argv[opt_ind][1] == '\0') 
		{
		    return EOF;
		}
	    else if (strcmp(argv[opt_ind], "--") == 0)
		{
		    opt_ind++;
		    return EOF;
		}
	}

    if (sp == 1 && argv[opt_ind][0] == '-' && argv[opt_ind][1] == '-') 
	{
	    /* long command line option */
	    const char *arg = &argv[opt_ind][2];
	    int i;

	    for (i = 0; l_opts && l_opts[i].name; i++)
		{
		    size_t len = strlen(l_opts[i].name);

		    if (strncmp(arg, l_opts[i].name, len) == 0)
			{
			    /* we've found a matching long command line flag */
			    opt_opt = l_opts[i].shortval;

			    if (l_opts[i].has_arg != no_arg)
				{
				    if (arg[len] == '=')
					{
					    opt_arg = &arg[len + 1];
					}
				    else if (opt_ind < (argc - 1) && argv[opt_ind + 1][0] != '-')
					{
					    opt_arg = argv[++opt_ind];
					}
				    else if (l_opts[i].has_arg == require_arg)
					{
					    if (opt_err)
						fprintf(stderr, "%s: option required for \"--%s\" flag\n", argv[0], arg);

					    opt_opt = '?';
					}
				}
			    else
				{
				    if (arg[len] == '=')
					{
					    if (opt_err)
						fprintf(stderr, "%s: no option required for \"%s\" flag\n", argv[0], arg);

					    opt_opt = '?';
					}

				    opt_arg = NULL;
				}

			    break;
			}
		}

	    if (l_opts[i].name == NULL)
		{
		    /* exhausted all of the l_opts we have and still didn't match */
		    if (opt_err)
			fprintf(stderr, "%s: unknown option \"%s\"\n", argv[0], arg);

		    opt_opt = '?';
		}

	    opt_ind++;
	    sp = 1;
	}
    else
	{
	    register char *cp;    /* pointer into current token */

	    /* short command line option */
	    opt_opt = argv[opt_ind][sp];

	    if (opt_opt == ':' || (cp = strchr(opts, opt_opt)) == 0)
		{

		    if (opt_err)
			fprintf(stderr, "%s: unknown option \"%c\"\n", argv[0], opt_opt);
		    /* if no chars left in this token, move to next token */
		    if (argv[opt_ind][++sp] == '\0')
			{
			    opt_ind++;
			    sp = 1;
			}

		    return '?';
		}

	    if (*++cp == ':')
		{

		    /* if a value is expected, get it */
		    if (argv[opt_ind][sp + 1] != '\0')
			{
			    /* flag value is rest of current token */
			    opt_arg = &argv[opt_ind++][sp + 1];
			}
		    else if (++opt_ind >= argc)
			{
			    if (opt_err)
				{
				    fprintf(stderr, "%s: value expected for option \"%c\"\n", argv[0], opt_opt);
				}
			    opt_opt = '?';
			}
		    else
			{
			    /* flag value is next token */
			    opt_arg = argv[opt_ind++];
			}

		    sp = 1;
		}
	    else 
		{
		    /* set up to look at next char in token, next time */
		    if (argv[opt_ind][++sp] == '\0')
			{
			    /* no more in current token, so setup next token */
			    opt_ind++;
			    sp = 1;
			}

		    opt_arg = NULL;
		}
	}

    /* return the current flag character found */
    return opt_opt;
}

/* Assigns functions according to the parsed result */
static void variable_assign(int argc, const char *argv[])
{
    int option;

    /* set options according to the command line */
    while ((option = get_option(argc, argv, s_opts, l_opts)) != EOF)
	{
	    switch ((char)option)
		{
		case 'h': // Print help page
		    print_help();
		    exit(1);
		case 'o': // Print number of steps
		    output_name = strdup(opt_arg);
		    break;
		case 'i': // Print shorter version without the values
		    input_name = strdup(opt_arg);
		    break;
		case 'a': // 
                    { flg_alive = true;
                        z_pos = atof(strdup(opt_arg));
                    }
		    break;  
		default:
		    print_help();
		    exit(1);
		}
	}
}

/* For printing help page */
static void print_help()
{
    fflush(stdout);
    fprintf(stdout, "\nusage: h5ToVtk  -i INPUTFILE -o OUTPUTFILE [OPTIONAL_FLAGS] -a ZVALUE\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "  FLAGS\n");
    fprintf(stdout, "   -h, --help                 Print help page\n");
    fprintf(stdout, "   -i file, --input file      (REQUIRED) Takes input base file name to \"file\" (extension h5 is assumed \n");
    fprintf(stdout, "   -o file, --output file     (REQUIRED) Takes output base file name to \"file\" (extension vtk is added)\n");
    fprintf(stdout, "   -a zvalue                  Only display particles which have servived and reached z value  \n");

    fprintf(stdout, "\n");
    fprintf(stdout, "  Examples:\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "        /h5ToVtk -i ctf3-injector-darkcurrent-1 -o ctf3-injector-darkcurrent-1- \n");
    fprintf(stdout, "\n");
    fprintf(stdout, "        /h5ToVtk -i ctf3-injector-darkcurrent-1 -o ctf3-injector-darkcurrent-1- -a 0.19 \n");
    fprintf(stdout, "\n");
}


int main(int argc, const char *argv[])
{
    H5PartFile           *h5file = NULL;

    std::ofstream of, ofalive;
   
    int j;

    int num_dataset;

    int ntime_step = 0;

    variable_assign(argc, argv);

    if(input_name == NULL)  {
	fprintf(stdout, "missing input file name\n");
	print_help();
	exit(1);
    }
   
    if(output_name == NULL) {
	fprintf(stdout, "missing output file name\n");
	print_help();
	exit(1);
    }
   
    string ifn = string(input_name) + string(".h5");
   
    h5file = H5PartOpenFile(ifn.c_str(), H5PART_READ);
   
    if( h5file == NULL ) {
	fprintf(stdout, "unable to open file %s\n", input_name);
	print_help();
	exit(1);
    }
   
    ntime_step = H5PartGetNumSteps(h5file);

    if (flg_alive) {
       
	set<h5part_int64_t> idSet;

	H5PartSetStep(h5file, ntime_step-1);
	num_dataset = H5PartGetNumDatasets(h5file);
	h5part_int64_t nparticles = H5PartGetNumParticles(h5file);
     
	h5part_int64_t* larray = (h5part_int64_t*)malloc(sizeof(h5part_int64_t)*nparticles);
	H5PartReadDataInt64(h5file, "id", larray);

	h5part_float64_t* z = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	H5PartReadDataFloat64(h5file, "z", z);

	for(unsigned long int n = 0; n < nparticles; ++n) {
	    if (z[n] >= z_pos)
		idSet.insert(larray[n]);
	}
       
	cout << "Last timestep contains " << nparticles << " particles" << endl;
       
	for (size_t j = 0; j<ntime_step ; j ++) {

	    H5PartSetStep(h5file,j);

	    num_dataset = H5PartGetNumDatasets(h5file);

	    h5part_int64_t nparticles = H5PartGetNumParticles(h5file);
	    h5part_int64_t* larray = (h5part_int64_t*)malloc(sizeof(h5part_int64_t)*nparticles);
	    H5PartReadDataInt64(h5file, "id", larray);
	   
	    h5part_float64_t* x = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	    H5PartReadDataFloat64(h5file, "x", x);
	    vector<h5part_float64_t> x_alive;

	    h5part_float64_t* y = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	    H5PartReadDataFloat64(h5file, "y", y);
	    vector<h5part_float64_t> y_alive;

	    h5part_float64_t* z = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	    H5PartReadDataFloat64(h5file, "z", z);
	    vector<h5part_float64_t> z_alive;

	    h5part_int64_t* ptype = (h5part_int64_t*)malloc(sizeof(h5part_int64_t)*nparticles);
	    H5PartReadDataInt64(h5file, "ptype", ptype);
	    vector<h5part_int64_t> ptype_alive;

	    for (size_t i = 0; i < nparticles ; i ++) {
		if ( idSet.find(larray[i]) != idSet.end() ) {
		    x_alive.push_back(x[i]);
		    y_alive.push_back(y[i]);
		    z_alive.push_back(z[i]);
		    ptype_alive.push_back(ptype[i]);
		}
	    }
	   
	    string ffnlive = string("vtk/") + string(output_name) + string("-alive-") + convert2Int(j) + string(".vtk");
	    ofalive.open(ffnlive.c_str());
	    assert(ofalive.is_open());
	    ofalive.precision(6);

	    size_t alive_num =  x_alive.size();           

	    ofalive  << setprecision(5)
		     << "# vtk DataFile Version 2.0" << endl
		     << "unstructured grid and vector field on the nodes" << endl
		     << "ASCII" << endl
		     << "DATASET UNSTRUCTURED_GRID" << endl
		     << "POINTS " <<  alive_num << " float" << endl;
           
	    // Particle positions


	    for(size_t i = 0; i < alive_num; i++)
		ofalive << x_alive[i] << "  " << y_alive[i] << "  " << z_alive[i] << endl;
           
	    ofalive << endl;            // defining VTK_poly_vertex
           
	    ofalive << "CELLS " << alive_num << " " << 2 * alive_num << endl;
           
	    for(size_t i = 0; i < alive_num; i++)
		ofalive << "1 " << i << endl;
	    ofalive << endl;
           
	    // defining Cell_types
	    ofalive << "CELL_TYPES " << alive_num << endl;
	    for(size_t i = 0; i < alive_num; i++)
		ofalive << "2" << endl;
           
	    // defining Cell_types
	    ofalive << "POINT_DATA " << alive_num << endl;
	    ofalive << "SCALARS " << "Pointtype" << " float " << "1" << endl;
	    ofalive << "LOOKUP_TABLE " << "mytable" << endl ;
	    for(size_t i = 0; i < alive_num ; i ++) {
		if (ptype_alive[i] == 0) {
		    ofalive << 0.0 << endl;
		}
		else if (ptype_alive[i] == 1) {
		    ofalive << 0.5 << endl;
		}
		else if (ptype_alive[i] == 2) {
		    ofalive << 1.0 << endl;
		}
	    }
           
	    ofalive << "LOOKUP_TABLE " << "mytable " << 3 << endl ;
	    ofalive << 1.0 <<" "<< 0.0 <<" "<< 0.0 <<" "<< 1.0 << endl;
	    ofalive << 0.0 <<" "<< 1.0 <<" "<< 0.0 <<" "<< 1.0 << endl;
	    ofalive << 0.0 <<" "<< 0.0 <<" "<< 1.0 <<" "<< 1.0 << endl;
	    ofalive << endl;
	    ofalive.close();

	    free(x);
	    free(y);
	    free(z);
	    free(ptype);

	    x_alive.clear();
	    y_alive.clear();
	    z_alive.clear();
	    ptype_alive.clear();

	    cout <<"Done time step "<< j << endl;
	}
    }

       
    else {

	for (size_t j=0; j<ntime_step; j++) {
      
	    H5PartSetStep(h5file,j);
     
	    num_dataset = H5PartGetNumDatasets(h5file);
	    h5part_int64_t nparticles = H5PartGetNumParticles(h5file);
       
	    cout << "Working on timestep " << j << " expecting " << nparticles << " particles " << endl;
	   
	    h5part_float64_t* x = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	    H5PartReadDataFloat64(h5file, "x", x);

	    h5part_float64_t* y = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	    H5PartReadDataFloat64(h5file, "y", y);

	    h5part_float64_t* z = (h5part_float64_t*)malloc(sizeof(h5part_float64_t)*nparticles);
	    H5PartReadDataFloat64(h5file, "z", z);

	    h5part_int64_t* ptype = (h5part_int64_t*)malloc(sizeof(h5part_int64_t)*nparticles);
	    H5PartReadDataInt64(h5file, "ptype", ptype);

	    string ffn = string("vtk/") + string(output_name)  + convert2Int(j) + string(".vtk");
      
	    of.open(ffn.c_str());
	    assert(of.is_open());
	    of.precision(6);
       
	    of  << setprecision(5)
		<< "# vtk DataFile Version 2.0" << endl
		<< "unstructured grid and vector field on the nodes" << endl
		<< "ASCII" << endl
		<< "DATASET UNSTRUCTURED_GRID" << endl
		<< "POINTS " << nparticles << " float" << endl;
       
	    // Particle positions
	    for(size_t i = 0; i < nparticles; i++)
		of << x[i] << "  " << y[i] << "  " << z[i] << endl;

	    of << endl;            // defining VTK_poly_vertex
       
	    of << "CELLS " << nparticles << " " << 2 * nparticles << endl;

	    for(size_t i = 0; i < nparticles; i++)
		of << "1 " << i << endl;
	    of << endl;
       
	    // defining Cell_types
	    of << "CELL_TYPES " << nparticles << endl;
	    for(size_t i = 0; i < nparticles; i++)
		of << "2" << endl;

	    // defining Cell_types
	    of << "POINT_DATA " << nparticles << endl;
	    of << "SCALARS " << "Pointtype" << " float " << "1" << endl;
	    of << "LOOKUP_TABLE " << "mytable" << endl ;
	    for(size_t i = 0; i < nparticles ; i ++) {
		if (ptype[i] == 0) {
		    of << 0.0 << endl;
		}
		else if (ptype[i] == 1) {
		    of << 0.5 << endl;
		}
		else if (ptype[i] == 2) {
		    of << 1.0 << endl;
		}
	    }

	    of << "LOOKUP_TABLE " << "mytable " << 3 << endl ;
	    of << 1.0 <<" "<< 0.0 <<" "<< 0.0 <<" "<< 1.0 << endl;
	    of << 0.0 <<" "<< 1.0 <<" "<< 0.0 <<" "<< 1.0 << endl;
	    of << 0.0 <<" "<< 0.0 <<" "<< 1.0 <<" "<< 1.0 << endl;
	    of << endl;
	    of.close();
	    free(x);
	    free(y);
	    free(z);
	    free(ptype);

	}
    }

    H5PartCloseFile(h5file);   
    return 0;
}
