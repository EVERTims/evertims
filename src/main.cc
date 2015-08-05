/***********************************************************************
 *
 * This file is part of the EVERTims room acoustic modeling software.
 * It is released under the MIT License. You should have received a
 * copy of the MIT License along with EVERTims.  If not, see
 * http://www.opensource.org/licenses/mit-license.php
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * For details, see the LICENSE file
 *
 * The EVERTims
 *
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology 
 *
 * (C) 2007-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ***********************************************************************/

#include <iostream>
#include <vector>

#include "writer.h"
#include "solver.h"
#include "reader.h"

#include <getopt.h>
#include <time.h>

using namespace std;

void printUsage ()
{
  cout << "Usage:\t\t./ims [-s inputport] [-v visualizationHost:port]";
  cout << "[-a auralizationHost:port] [-g]" << endl;
}

int main (int argc, char **argv)
{
  char  room_file[256];
  char  material_file[256];
  bool  graphics = false;
  int   input_socket = 1979;
  char  *auralization_addr = 0;
  char  *virchor_addr = 0;
  char  *print_addr = 0;
  char  tmp[256];
  float threshold = 0.1;

  strcpy (room_file, "sigyn.room");
  strcpy (material_file, "materials.dat");

  int mindepth = 3;
  int maxdepth = 5;

  int c, level;
  while ((c = getopt (argc, argv, "f:gv:a:s:p:m:d:D:t:")) != EOF)
  {
    switch (c)
    {
    case 'g':
      graphics = true;
      break;
    case 'f':
      sscanf ( optarg, "%s", room_file );
      break;
    case 's':
      sscanf ( optarg, "%d", &input_socket );
      break;
    case 'a':
      sscanf ( optarg, "%s", tmp );
      auralization_addr = strdup ( tmp );
      break;
    case 'm':
      sscanf ( optarg, "%s", material_file );
      break;
    case 'd':
      sscanf ( optarg, "%d", &mindepth );
      break;
    case 'D':
      sscanf ( optarg, "%d", &maxdepth );
      break;
    case 'v':
      sscanf ( optarg, "%s", tmp );
      virchor_addr = strdup(tmp);
      break;
    case 'p':
      sscanf ( optarg, "%s", tmp );
      print_addr = strdup(tmp);
      break;
    case 't':
      sscanf ( optarg, "%f", &threshold );
      break;
    case '?':
      cout << "Command line option is not specified!" << endl;
      printUsage ();
      break;
    default:
      printUsage ();
      break;
    }
  }
  if (optind < argc)
  	cout << "Abandoned command line parsing at " << argv[optind] << endl;

  Reader *re = new Reader ( material_file, input_socket, threshold );
  Solver *s = new Solver ( mindepth, maxdepth, graphics );

  s->attachReader (re);
  re->attachSolver (s);

  re->start ();
  cout << "Reader object started." << endl;

  VisualizationWriter *vw;
  if (virchor_addr)
  {
    vw = new VisualizationWriter(virchor_addr);
    vw->connect();
    s->addWriter (vw);
  }

  AuralizationWriter *mw;
  if (auralization_addr)
  {
    mw = new AuralizationWriter(auralization_addr);
    mw->connect();
    s->addWriter (mw);
  }

  PrintWriter *pw;
  if (print_addr)
  {
    pw = new PrintWriter(print_addr);
    s->addWriter (pw);
  }

  while (!re->geometryInitialized ()) ;
  cout << "Got some geometry!" << endl;

  //  s->readRoomDescription (room_file, materials_file);
  //  cout << "Room read." << endl;

  while (1)
  {
    s->update ();
    usleep ( 2000 );
  }

  delete re;
  delete s;
  delete mw;
  delete vw;

  return 0;
}
