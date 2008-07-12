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
 * The MaterialFile class for EVERTims
 *
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology  
 *
 ***********************************************************************/

#include <iostream>

#include "material.h"

using namespace std;

void MaterialFile::readFile (const char* filename)
{
  FILE* f = fopen(filename, "r");
  if (!f)
  {
    std::cerr << "No such material file: " << filename << std::endl;
    return;
  }

  char name[256];
  Material m;
  
  while(!feof(f))
  {
    for (int i=0; i < 10; i++)
      fscanf(f, " %f", &m.absorption[i]);

    fscanf(f, "%f", &m.diffusion[0]);
    fscanf(f, "%s", &name);
    //    std::cout << "name = " << name << m.absorption[0] << std::endl;

    addMaterial(name, m);
  }
  fclose(f);
}


//std::map<std::string, Material> MaterialFile::lib;

Material& MaterialFile::find (const char* material_name) 
{
  std::string name(material_name);
  //  std::cout << "Looking for material: " << name;

  std::map<std::string, Material>::iterator it = m_lib.find (name);
  if (it != m_lib.end ())
  {
    //    std::cout << " and found it: " << it->second.absorption[0] << std::endl;
    return it->second;
  }
  else
  {    
    std::cout << "Unknown material: " << name << std::endl;
  }
}

void MaterialFile::addMaterial (const char *material_name, Material a)
{
  std::string name(material_name);

  std::map<std::string,Material>::iterator it = m_lib.find (name);
  if (it != m_lib.end ())
  {    
    std::cerr << "Material: " << name << " already in the database." << std::endl;
  }
  else
  {
    m_lib[name] = a;
    //    std::cerr << "Material: " << name << " added in the database:" << a.absorption[0] << " = " << m_lib[name].absorption[0] << std::endl;
  }
}

