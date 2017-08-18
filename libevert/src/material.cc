/*************************************************************************
 *
 * This file is part of the EVERT Library / EVERTims program for room 
 * acoustics simulation.
 *
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or any later version.
 *
 * THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL; BUT WITHOUT 
 * ANY WARRANTY; WITHIOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, see https://www.gnu.org/licenses/gpl-2.0.html or write 
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301, USA.
 *
 * Copyright
 *
 * (C) 2004-2005 Samuli Laine
 * Helsinki University of Technology
 *
 * (C) 2008-2017 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ************************************************************************/
 
 
#include <stdio.h>
#include <iostream>

#include "material.h"

using namespace std;

void MaterialFile::readFile (const char* filename)
{
    FILE* f = fopen(filename, "r");
    if( !f )
    {
        std::cerr << "No such material file: " << filename << std::endl;
        return;
    }
    
    char name[256];
    Material m;
    
    while( !feof(f) )
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
    if( it != m_lib.end () )
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
    if( it != m_lib.end () )
    {
        std::cerr << "Material: " << name << " already in the database." << std::endl;
    }
    else
    {
        m_lib[name] = a;
        //    std::cerr << "Material: " << name << " added in the database:" << a.absorption[0] << " = " << m_lib[name].absorption[0] << std::endl;
    }
}

