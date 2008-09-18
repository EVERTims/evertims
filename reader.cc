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
 * The Reader class for EVERTims
 *
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology  
 *
 ***********************************************************************/

#include "reader.h"
#include "solver.h"
#include "socket.h"

#include <iostream>
#include <map>
#include <cstring>

#define SCALER 1.f

using namespace std;

#include <pthread.h>

pthread_t update_thread;

Reader::Reader (const char* material_filename, int input_port, float threshold):
  m_input_port (input_port),
  m_threshold ( threshold * threshold )  // Comparison is to the square of the distance
{
  m_materials.readFile(material_filename);
}

void Reader::initializeMembers(EL::Room& room)
{
  m_elements = *(new std::vector<EL::Room::Element>(room.getElements()));

  return;
}

void Reader::getRoom(EL::Room& room) 
{
  room.setElements (m_elements);
}

void Reader::updateElements(const std::string& id, 
			    const EL::Room::Element& element)
{
  //  map<string, EL::Room::Element>::iterator e = emap.find(id);
  
  //  if (e != emap.end())
  //  {
  //    cout << "Updating an existing node: " << id << endl;
  //  }
  //  else
  //  {
  //    cout << "New element: " << id << endl;
  //  }
  emap[id] = element;
  //  emap[id].m_polygon.print();
}

void Reader::createElementList()
{
  m_elements.clear();

  for (map<string, EL::Room::Element>::iterator e = emap.begin(); e != emap.end(); e++)
  {
    //    cout << "Adding element: " << e->first << " to the list: " << endl;
    m_elements.push_back(e->second);
    //    e->second.m_polygon.print();
    //    cout << endl;
  }
}

void parsePosition(std::string msg, std::string& id, EL::Vector3& pos)
{
  char idBuf[512];

  const char *str = msg.c_str();
  
  str = strchr(str, ' ')+1;

  float m00, m01, m02, m10, m11, m12, m20, m21, m22, p0, p1, p2, foo;

  sscanf(str, "%s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
	       &idBuf, 
	       &m00, &m01, &m02, &foo, 
	       &m10, &m11, &m12, &foo, 
	       &m20, &m21, &m22, &foo, 
	       &p0,  &p1,  &p2,  &foo);

  id = string(idBuf);

  p0 = p0*SCALER;
  p1 = p1*SCALER;
  p2 = p2*SCALER;

  std::cout << id << " moved ";
  // from: [" << pos[0] << "," << pos[1] << "," << pos[2] << "]";
  pos.set( p0, p1, p2 );
  std::cout << " to: [" << pos[0] << "," << pos[1] << "," << pos[2] << "]" << std::endl;
}

void parseFace(std::string msg, 
	       MaterialFile& materials, 
	       std::string& id, 
	       EL::Polygon& polygon)
{
  char buf1[256]; 
  char buf2[256]; 

  std::string matrix_items(msg.substr(msg.find(" ")+1));

  //  std::cout << "items: " << matrix_items << endl;

  const char *str = matrix_items.c_str();

  EL::Vector3 p[4];

  sscanf(str, "%s %s %f %f %f %f %f %f %f %f %f %f %f %f", 
	       &buf1, &buf2, 
	       &p[0][0], &p[0][1], &p[0][2],
	       &p[1][0], &p[1][1], &p[1][2],
	       &p[2][0], &p[2][1], &p[2][2],
   	       &p[3][0], &p[3][1], &p[3][2]);

  id = string(buf1);
  Material& m = materials.find(buf2);

   for (int i=0;i<4;i++)
      for (int j=0;j<3;j++)
         p[i][j]=p[i][j]*SCALER;

  if (p[2] == p[3])
  {
    cout << "3";
    polygon = EL::Polygon(&p[0], 3, m, id);
  }
  else
  {
    cout << "4";
    polygon = EL::Polygon(&p[0], 4, m, id);
  }

  //  polygon.print();
  //  cout << endl;

  //  cout << "Vertices of " << id << " made of " << buf2 << ": [";
  //  cout << p0[0] << "," << p0[1] << "," << p0[2] << "] - [";
  //  cout << p1[0] << "," << p1[1] << "," << p1[2] << "] - [";
  //  cout << p2[0] << "," << p2[1] << "," << p2[2] << "] - [";
  //  cout << p3[0] << "," << p3[1] << "," << p3[2] << "]" << endl;
}

void Reader::parseListener ( std::string& msg )
{
  std::string id;
  EL::Vector3 pos;

  parsePosition(msg, id, pos);
  //  pos[0] = 1.64*SCALER;  pos[1] = 8.27*SCALER;  pos[2] = 2.20*SCALER;

  std::map<std::string, EL::Listener>::iterator l = m_listeners.find(id);

  // Do we know the listener already
  if ( l != m_listeners.end() )
    {
      // Yes, we do. So let us check if it moved enough 
      EL::Listener& listener = l->second;
      if ( ( pos - listener.getPosition() ).lengthSqr() > m_threshold )
	{
	  // OK, it did, and we have to update all the solution nodes of this source
	  listener.setPosition ( pos );

	  for (std::map<std::string, EL::Source>::iterator s = m_sources.begin();
	       s != m_sources.end(); s++)
	    m_solver->markListenerMovement ( s->second, listener );
	}
    }
  // No. We got to create a new listener and solution nodes.
  else
    {
      EL::Listener listener;

      listener.setPosition ( pos );
      listener.setName ( id );

      m_listeners[id] = listener;

      cout << "New Listener generated!" << endl;

      for (std::map<std::string, EL::Source>::iterator s = m_sources.begin();
	   s != m_sources.end(); s++)
	m_solver->createNewSolutionNode ( s->second, listener );
    }
}

void Reader::parseSource ( std::string& msg )
{
  std::string id;
  EL::Vector3 pos;

  parsePosition(msg, id, pos);
  //  pos[0] = 0.0*SCALER;  pos[1] = -1.0*SCALER;  pos[2] = 1.50*SCALER;

  std::map<std::string, EL::Source>::iterator s = m_sources.find(id);

  // Do we know the source already
  if ( s != m_sources.end() )
    {
      // Yes, we do. So let us check if the source really moved enough 
      if ( ( pos - s->second.getPosition() ).lengthSqr() > m_threshold )
	{
	  // OK, it did, and we have to update all the solution nodes of this source
	  s->second.setPosition ( pos );

	  for (std::map<std::string, EL::Listener>::iterator l = m_listeners.begin();
	       l != m_listeners.end(); l++)
	    m_solver->markSourceMovement ( s->second, l->second );
	}
    }
  // No. We got to create a new source and solution nodes. Cool!
  else
    {
      EL::Source source;

      source.setPosition ( pos );
      source.setName ( id );

      m_sources[id] = source;

      cout << "New source generated!" << endl;

      for (std::map<std::string, EL::Listener>::iterator l = m_listeners.begin();
	   l != m_listeners.end(); l++)
	m_solver->createNewSolutionNode ( source, l->second );
    }
}

void Reader::printSourcesAndListeners()
{
    cout << "Sources: ";
    for (std::map<std::string, EL::Source>::iterator s = m_sources.begin();
	 s != m_sources.end(); s++)
      cout << s->second.getName () << " ";
    cout << endl << "Listener: ";
    for (std::map<std::string, EL::Listener>::iterator l = m_listeners.begin();
	 l != m_listeners.end(); l++)
      cout << l->second.getName () << " ";
    cout << endl;
}
          
void *update_loop (void *obj)
{
  Reader *re = (Reader *)obj;

  char buffer[1024]; 
  Socket* s = re->initializeInputSocket();

  while (1)
  {
    EL::Vector3 new_listener;
    EL::Vector3 new_source;
    EL::Room::Element element;
    string id;

    //    re->printSourcesAndListeners();

    (void) s->read(buffer);
    
    string message = buffer;
    //    cout << "Message: " << message << endl;

    if (message.find("/face ")==0) 
    {
      //      cout << "Face ";
      parseFace(message, re->getMaterials(), id, element.m_polygon);
      re->updateElements(id, element);
    }
    else if (message.find("/source ")==0) {
      cout << "Source ";
      re->parseSource(message);
    } 
    else if (message.find("/listener ")==0) 
    {
      cout << "Listener ";
      re->parseListener(message);
    } 
    else if (message.find("/facefinished")==0)
    {
      cout << "Geometry modficiations done. Signaling the solver to restart." << endl;
      re->createElementList();
      re->signalSolver ();
    }
  }
}

void Reader::signalSolver ()
{
  m_geometry_initialized = true;

  if (m_solver)
  {
    m_solver->markGeometryChanged();
  }
}

void Reader::start ()
{
  pthread_create (&update_thread, NULL, update_loop, this);
}


