/***********************************************************************
 *
 * This file is part of the EVERTims room acoustic modeling software.
 * It is released under the MIT License. 
 * For details, see the LICENSE file
 *
 * The Writer class for EVERTims
 *
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology  
 *
 ***********************************************************************/

#ifndef _WRITER_H
#define _WRITER_H

#include "evert/elPathSolution.h"
#include "OSC/OSC-client.h"
#include "socket.h"

#include <map>
#include <cstring>
#include <regex.h>

#define BUF_SIZE 16384

class Writer
{
public:
  Writer (char *addr);
  ~Writer () { disconnect(); };

  bool match ( const char* id );

  void connect ();
  void disconnect ();

  virtual const char* getType() { return "Base"; };
  virtual void write (EL::PathSolution *solution) { return; };

 protected:

  Socket *m_socket;
  char m_writeBuf[BUF_SIZE];

private:
  char *m_host;
  char *m_pattern;
  regex_t m_preq;
};

class MarkusWriter : public Writer
{
 public:
  MarkusWriter (char *host);

  virtual const char* getType() { return "Markus"; };
  void write (EL::PathSolution *solution);

 private:

  enum PathState
  {
    FADE_IN,
    UPDATE,
    FADE_OUT
  };

  std::string    getPathName              ( const EL::PathSolution::Path& p );
  int            getPathIDandState        ( const EL::PathSolution::Path& p, enum PathState& state );
  float          getPathLength            ( const EL::PathSolution::Path& p );
  int            getNewID                 ( );

  void           createSourceMessage      ( const EL::Source& source );
  void           createListenerMessage    ( const EL::Listener& listener );
  void           createReflectionMessage  ( int pathID, 
					    enum PathState state,
					    const EL::Vector3& p0, 
					    const EL::Vector3& pN, 
					    float len, 
					    int order, 
					    float *reflectance );
  void           createInvisMessage       ( int pathID );

  void           markExistingPaths        ( EL::PathSolution *solution );
  void           releaveLeftOverPaths     ( );

  OSCbuf m_oscbuf;

  struct PathNode
  {
    PathState    m_state;
    int          m_id;
  };

  std::map<std::string, struct PathNode> m_pathMap;
  std::vector<int> m_releaved;
};

class VirchorWriter : public Writer
{
 public:
  VirchorWriter (char *host): Writer(host), m_numLines(0) {};

  virtual const char* getType() { return "VirChor"; };
  void write (EL::PathSolution *solution);

 private:
  int m_numLines;
};


class PrintWriter : public Writer
{
 public:
  PrintWriter (char *addr): Writer(addr) {};

  virtual const char* getType() { return "Print"; };
  void write (EL::PathSolution *solution);
};

#endif
