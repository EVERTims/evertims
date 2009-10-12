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
 * The Reverberation estimation class for EVERTims
 *
 * (C) 2009 Lauri Savioja
 * Helsinki University of Technology  
 *
 ***********************************************************************/

#ifndef _REVERB_ESTIM_H
#define _REVERB_ESTIM_H

#define MAX_BANDS 10

#include "elPathSolution.h"

#include <map>
#include <cstring>

class Response
{
 public:
  Response(double samplingFrequency, double lengthInSecond);
  ~Response();

  void setItem(double time, double value);
  void addItem(double time, double value);

  void  SchroederIntegrate();
  float search           (double val = -0.00001);
  void  getMinMax(double& minValue, double& maxValue);

private:
  double m_samplingFrequency;
  int m_length;

  double* m_signal;
};

class ReverbEstimator
{
public:
  ReverbEstimator (double samplingFrequency, EL::PathSolution *solution, double speedOfSound, double maxTime);
  ~ReverbEstimator () { };
  
  void getMinMax(int band, double& minValue, double& minPosition double& maxValue, double& maxPosition);

  double getEstimate60(int band, double startDecay, double endDecay);

private:
  Response* m_SchroederPlots[MAX_BANDS];
};

#endif
