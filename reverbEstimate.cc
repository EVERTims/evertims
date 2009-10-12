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
 * The Reverberation estimation class implementation for EVERTims
 *
 * (C) 2009 Lauri Savioja
 * Helsinki University of Technology  
 *
 ***********************************************************************/

#include "reverbEstimate.h"

#define SPEED_OF_SOUND 340

Response::Response(double samplingFrequency, double lengthInSeconds):
  m_samplingFrequency(samplingFrequency),
  m_length((int)(lengthInSeconds * samplingFrequency))
{
  m_signal = new double[m_length];
}

Response::~Response()
{
  delete m_signal;
}

void Response::setItem(double time, double value)
{
  int idx = time * m_samplingFrequency;
  if (idx < m_length)
    m_signal[idx] = value;
}

void Response::addItem(double time, double value)
{
  int idx = time * m_samplingFrequency;
  if (idx < m_length)
    m_signal[idx] += value;
}

void Response::SchroederIntegrate()
{
  int idx = m_length;
  double sum = 0;

  while (idx) 
    {
      sum += m_signal[idx];
      m_signal[idx] = sum;
    }
}

float Response::search           (double val)
{
  int idx = 0;

  for (; idx < m_length; idx++)
    if (m_signal[idx] <= val)
      break;

  return (float)idx / m_samplingFrequency;
}

void Response::getMinMax(double& minValue, double& maxValue)
{
  minValue = m_signal[0];
  maxValue = m_signal[m_length-1];
}

ReverbEstimator::ReverbEstimator (double samplingFrequency, EL::PathSolution *solution, double speedOfSound, double maxTime)
{
  int idx = 0;

  for (int i = 0; i < MAX_BANDS; i++)
    m_SchroederPlots[i] = new Response(samplingFrequency, maxTime);

  double len;
  double time;
  float reflectance[MAX_BANDS];

  for (int i = 0; i < solution->numPaths(); i++)
    {
      const EL::PathSolution::Path& path = solution->getPath(i);

      len = solution->getLength( path );
      time = len / speedOfSound;
      
      for (int k = 0; k < MAX_BANDS; k++) reflectance[k] = 1.0;
      for (int j = 0; j < path.m_order; j++)
	{
	  const EL::Polygon* p = path.m_polygons[j];
	  const Material& m = p->getMaterial ();
	  for (int k = 0; k < MAX_BANDS; k++) reflectance[k] *= ( 1 - m.absorption[k] );
	}
      for (int k = 0; k < MAX_BANDS; k++) 
	m_schroederPlots.addItem(time, reflectance[k] / len);
    }
  for (int k = 0; k < MAX_BANDS; k++) 
    m_schroederPlots.SchroederIntegrate();
}


ReverbEstimator::~ReverbEstimator() 
{ 
  for (int i = 0; i < MAX_BANDS; i++)
    delete m_SchroedePlots[i];
}

float ReverbEstimator::getEstimate60(int band, double startDecay, double endDecay)
{
  Response* r = m_schroederPlots[band];

  float realStart = r->search();
  float startTime = r->search(startDecay);
  float endTime   = r->search(endDecay);

  float totalDecay realStart + (60 * (endTime-startTime)/(endDecay - startDecay));

  return totalDecay;
}

void ReverbEstimator::getMinMax(int band, double& minValue, double& maxValue)
{
  Response* r = m_schroederPlots[band];

  r->getMinMax(minValue, maxValue);
}

