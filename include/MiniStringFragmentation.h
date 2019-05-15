// MiniStringFragmentation.h is a part of the PYTHIA event generator.
// Copyright (C) 2008 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This file contains the class for "cluster" fragmentation.
// MiniStringFragmentation: handle the fragmentation of low-mass systems.

#ifndef Pythia8_MiniStringFragmentation_H
#define Pythia8_MiniStringFragmentation_H

#include "Basics.h"
#include "Event.h"
#include "FragmentationFlavZpT.h"
#include "FragmentationSystems.h"
#include "Info.h"
#include "ParticleData.h"
#include "PythiaStdlib.h"
#include "Settings.h"

namespace Pythia8 {

//**************************************************************************

// The MiniStringFragmentation class contains the routines to fragment 
// occasional low-mass colour singlet partonic systems, where the string 
// approach is not directly applicable (for technical reasons).

class MiniStringFragmentation {

public:

  // Constructor. 
  MiniStringFragmentation() {}

  // Initialize and save pointers.
  void init(Info* infoPtrIn, StringFlav* flavSelPtrIn);

  // Do the fragmentation: driver routine.
  bool fragment( int iSub, ColConfig& colConfig, Event& event, 
    bool isDiff = false);

private: 

  // Constants: could only be changed in the code itself.
  static const int    NTRYDIFFRACTIVE, NTRYLASTRESORT, NTRYFLAV;
  static const double SIGMAMIN;

  // Pointer to various information on the generation.
  Info*       infoPtr;

  // Pointer to class for flavour generation.
  StringFlav* flavSelPtr;

  // Initialization data, read from Settings.
  int    nTryMass;
  double sigma, sigma2Had, bLund;

  // Data members.
  bool   isClosed;
  double mSum, m2Sum;
  Vec4   pSum;
  vector<int> iParton;
  FlavContainer flav1, flav2;

  // Attempt to produce two particles from a cluster.
  bool ministring2two( int nTry, Event& event);

  // Attempt to produce one particle from a cluster.
  bool ministring2one( int iSub, ColConfig& colConfig, Event& event);

};

//**************************************************************************

} // end namespace Pythia8

#endif // Pythia8_MiniStringFragmentation_H
