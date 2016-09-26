/***********************************************************************
 *
 * This file is part of the EVERT beam-tracing library.
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
 * The Material and MaterialFile class for EVERT
 *
 * (C) 2007 Lauri Savioja, Samuel Siltanen
 * Helsinki University of Technology
 *
 * (C) 2008-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ***********************************************************************/

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <map>
#include <string>

class Material
{
public:
  float absorption[10];
  float diffusion[10];
  float transmission[10];
};

class MaterialFile
{
public:
    
  void readFile (const char* filename);
  Material& find (const char* material_name);
  void addMaterial (const char* material_name, Material a);
    
private:
  std::map<std::string, Material> m_lib;
};

#endif
