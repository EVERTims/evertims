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
 * The socket handling for EVERTims
 *
 * (C) 2007 Lauri Savioja, Rami Ajaj
 * Helsinki University of Technology CNRS / Limsi 
 *
 ***********************************************************************/

#ifndef __SOCKET_HPP
#define __SOCKET_HPP

// ********************************************************
// BEWARE: Never tested under WIN32 !!
// ********************************************************

// ********************************************************
// The includes
// ********************************************************
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <netdb.h>
#include <unistd.h>

#ifdef __Linux
#include <string.h>
#include <stdlib.h>
#endif

typedef int SOCKET;
#define  SOCKADDR_IN struct sockaddr_in

#else
#include <winsock2.h>
#include <Ws2tcpip.h>
WSADATA initialisation_win32; // Variable permettant de récupérer la structure d'information sur l'initialisation
WSADATA initialisation2_win32; // Variable permettant de récupérer la structure d'information sur l'initialisation
#endif


class Socket {
 public:
  Socket(int port);
  Socket(char *hostAndPort);

  ~Socket();

  int read(char *buf);
  void write(int len, char *buf);

 private:
  void openSocket();
  
  int m_socket_id; 
  struct sockaddr_in m_sockaddr; 
};


#endif
