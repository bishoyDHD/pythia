// Function definitions (not found in the header) for the ProcessLevel class.
// Copyright C 2007 Torbjorn Sjostrand

#include "ProcessLevel.h"

namespace Pythia8 {
 
//**************************************************************************

// Main routine to initialize generation process.

bool ProcessLevel::init( Info* infoPtrIn, BeamParticle* beamAPtrIn, 
  BeamParticle* beamBPtrIn, bool hasPythia6In, bool hasLHAin, 
  LHAinit* lhaInitPtrIn, LHAevnt* lhaEvntPtrIn) {

  // Store input pointers for future use. 
  infoPtr     = infoPtrIn;
  beamAPtr    = beamAPtrIn;
  beamBPtr    = beamBPtrIn;

  // Store pointers to Les Houches Accord input if any.
  hasLHA      = hasLHAin;
  lhaInitPtr  = lhaInitPtrIn;
  lhaEvntPtr  = lhaEvntPtrIn;
  strategyLHA = (hasLHA) ? lhaInitPtr->strategy() : 0;  

  // Initialize event generation from Pythia 6.3.
  hasPythia6  = hasPythia6In;
  if (hasPythia6) initPythia6( infoPtr->idA(), infoPtr->idB(), 
    infoPtr->eCM() );

  // If not Les Houches or Pythia 6.3 then internal machinery.
  hasInternal = !hasLHA && !hasPythia6;
  if (hasInternal) return initInternal();

  // Done. (Check return values from other classes??)
  return true;
}

//*********

// Main routine to generate the hard process.
// Currently rather primitive.

  bool ProcessLevel::next( Event& process) {

  // Generate the next internal event. 
  if (hasInternal) return getInternalEvnt( process);

  // Generate the next Pythia 6.4 event. 
  if (hasPythia6) Pythia6::pyupev();

  // Read in a simple event in the LHAevnt format. Default info.
  if (strategyLHA >= 10) {
    infoPtr->setType( "Simple LHA process", 0, 0, false, true, false, false);
    infoPtr->setTypeMI( 0, 0.);
    return getSimpleLHAevnt( process);
  }

  // Read in an event in the LHAevnt format.
  if (hasPythia6 || hasLHA) return getLHAevnt( process);

  // Done.
  return true;
}

//*********

// Print statistics on cross sections and number of events.

void ProcessLevel::statistics(ostream& os) {

  // Internal statistics.
  if (hasInternal) {
    
    // Header.
    os << "\n *-------  PYTHIA Event and Cross Section Statistics  ------"
       << "---------------------------------------*\n"
       << " |                                                            "
       << "                                     |\n" 
       << " | Subprocess                               Code |       Numbe"
       << "r of events |      sigma +- delta    |\n" 
       << " |                                               |       Tried"
       << "   Accepted |     (estimated) (mb)   |\n"
       << " |                                               |            "
       << "            |                        |\n"
       << " |------------------------------------------------------------"
       << "-------------------------------------|\n"
       << " |                                               |            "
       << "            |                        |\n";

    // Reset sum counters.
    int    nTrySum   = 0; 
    int    nAccSum   = 0;
    double sigmaSum  = 0.;
    double delta2Sum = 0.;

    // Loop over existing processes.
    for (int i = 0; i < int(containerPtrs.size()); ++i) 
    if (containerPtrs[i]->sigmaMax() != 0.) {

      // Read info for process. Sum counters.
      int    nTry  = containerPtrs[i]->nTried();
      int    nAcc  = containerPtrs[i]->nAccepted();
      double sigma = containerPtrs[i]->sigmaMC();
      double delta = containerPtrs[i]->deltaMC(); 
      nTrySum   += nTry;
      nAccSum   += nAcc; 
      sigmaSum  += sigma;
      delta2Sum += pow2(delta);    

      // Print individual process info.
      os << " | " << left << setw(40) << containerPtrs[i]->name() 
         << right << setw(5) << containerPtrs[i]->code() << " | " 
         << setw(11) << nTry << " " << setw(10) << nAcc << " | " 
         << scientific << setprecision(3) << setw(11) << sigma 
         << setw(11) << delta << " |\n";
    }

    // Print summed process info.
    os << " |                                               |            "
       << "            |                        |\n"
       << " | " << left << setw(45) << "sum" << right << " | " << setw(11) 
       << nTrySum << " " << setw(10) << nAccSum << " | " << scientific 
       << setprecision(3) << setw(11) << sigmaSum << setw(11) 
       << sqrtpos(delta2Sum) << " |\n";

    // Listing finished.
    os << " |                                                            "
       << "                                     |\n"
       << " *-------  End PYTHIA Event and Cross Section Statistics -----"
       << "-------------------------------------*" << endl;
     
  }

  // Pythia6 statistics.
  if (hasPythia6) Pythia6::pystat(0);

}

//*********

// Initialize the internal event generation machinery.
  
bool ProcessLevel::initInternal( ostream& os) {

  // Set up SigmaTotal.
  int    idA = infoPtr->idA();
  int    idB = infoPtr->idB();
  double eCM = infoPtr->eCM();
  sigmaTot.init( idA, idB, eCM);

  // Send cross section and beam info to influx, processes and phase space.
  double mA = beamAPtr->m();
  double mB = beamBPtr->m();
  InFlux::setBeamPtr( beamAPtr, beamBPtr); 
  SigmaProcess::setSigmaTotalPtr( &sigmaTot, idA, idB, mA, mB);
  PhaseSpace::setBeamSigmaPtr( beamAPtr, beamBPtr, &sigmaTot);

  // Sets up containers for all the hard processes.
  SetupContainers setupContainers;
  setupContainers.init(containerPtrs);

  // If no processes found then refuse to do anything.
  if ( int(containerPtrs.size()) == 0) {
    ErrorMsg::message("Error in ProcessLevel::initInternal: "
      "no processes switched on"); 
    return false;
  }

  // Initialize each process. 
  int numberOn = 0;
  for (int i = 0; i < int(containerPtrs.size()); ++i)
    if (containerPtrs[i]->init()) ++numberOn;

  // Sum maxima for Monte Carlo choice.
  sigmaMaxSum = 0.;
  for (int i = 0; i < int(containerPtrs.size()); ++i)
    sigmaMaxSum += containerPtrs[i]->sigmaMax();

  // Construct string with incoming beams and for cm energy.
  string collision = "We collide " + ParticleDataTable::name(idA)
    + " with " + ParticleDataTable::name(idB) + " at a CM energy of "; 
  string pad( 46 - collision.length(), ' ');

  // Print initialization information: header.
  os << "\n *-------  PYTHIA Process Initialization  ---------------------*\n"
     << " |                                                             |\n" 
     << " | " << collision << scientific << setprecision(3)<< setw(9) << eCM 
     << " GeV" << pad << " |\n"
     << " |                                                             |\n"
     << " |-------------------------------------------------------------|\n"
     << " |                                               |             |\n"
     << " | Subprocess                               Code |   Estimated |\n" 
     << " |                                               |    max (mb) |\n"
     << " |                                               |             |\n"
     << " |-------------------------------------------------------------|\n"
     << " |                                               |             |\n";


  // Loop over existing processes: print individual process info.
  for (int i = 0; i < int(containerPtrs.size()); ++i) {
    os << " | " << left << setw(40) << containerPtrs[i]->name() 
       << right << setw(5) << containerPtrs[i]->code() << " | " 
       << scientific << setprecision(3) << setw(11)  
       << containerPtrs[i]->sigmaMax() << " |\n";
  }

  // Listing finished.
  os << " |                                                             |\n" 
     << " *-------  End PYTHIA Process Initialization ------------------*" 
     << endl;

  // If sum of maxima vanishes then refuse to do anything. Done.
  if ( numberOn == 0  || sigmaMaxSum <= 0.) {
    ErrorMsg::message("Error in ProcessLevel::initInternal: "
      "all processes have vanishing cross sections"); 
    return false;
  }
  return true;

}

//*********

// Initialize event generation from Pythia 6.3.

void ProcessLevel::initPythia6( int idA, int idB, double eCM) {

  // Simplified translation into input accessible by Pythia 6.3,
  // assuming we are in the cm frame, and only have p/pbar/e+/e- beams.
  string frame = "cms";
  string beam = "p";
  if (idA == -2212) beam = "pbar";
  if (idA == 11) beam = "e-";
  if (idA == -11) beam = "e+";
  string target = "p";
  if (idB == -2212) target = "pbar";
  if (idB == 11) target = "e-";
  if (idB == -11) target = "e+";

  // Initialize Pythia 6.3, without multiple interactions.
  Pythia6::pygive("mstp(81)=0");
  Pythia6::pyinit(frame, beam, target, eCM);

  // Create pointer to LHAevnt. Warning: delete it somewhere??
  lhaEvntPtr = new LHAevntFortran;  

  // Done.
}

//*********

// Generate the next internal event.
  
bool ProcessLevel::getInternalEvnt( Event& process) {

  // Loop over tries until trial event succeeds.
  int iNow;
  for ( ; ; ) {

    // Pick one of the subprocesses.
    double sigmaMaxNow = sigmaMaxSum * Rndm::flat();
    iNow = -1;
    do sigmaMaxNow -= containerPtrs[++iNow]->sigmaMax();
    while (sigmaMaxNow > 0. && iNow < int(containerPtrs.size()) - 1);
    
    // Do a trial event of this subprocess; accept or not.
    if (containerPtrs[iNow]->trialProcess()) break;
  }

  // Construct kinematics of acceptable process.
  containerPtrs[iNow]->constructProcess( process);

  // Do all resonance decays. First draft??
  resonanceDecays.next( process);
   
  // Add any junctions to the process event record list.
  findJunctions( process);

  // Provide current generated cross section estimate.
  int nTrySum = 0; 
  int nAccSum = 0;
  double sigmaSum = 0.;
  double delta2Sum = 0.;
  for (int i = 0; i < int(containerPtrs.size()); ++i) 
  if (containerPtrs[i]->sigmaMax() != 0.) {
     nTrySum += containerPtrs[i]->nTried();
     nAccSum += containerPtrs[i]->nAccepted();
     sigmaSum += containerPtrs[i]->sigmaMC();
     delta2Sum += pow2(containerPtrs[i]->deltaMC()); 
   }
   infoPtr->setSigma( nTrySum, nAccSum, sigmaSum, sqrt(delta2Sum)); 

  // Done.
  return true;
}

//*********

// Read in the hard process from the Les Houches Accord.
// Many more checks to be done for valid input??

bool ProcessLevel::getLHAevnt( Event& process) {

  // Generate the next Les Houches event.
  if (!lhaEvntPtr->set()) return false;
     
  // Let hard process record begin with the event as a whole and
  // the two incoming beam particles.  
  process.append( 90, -11, 0, 0, 1, 2, 0, 0, 
    Vec4(0., 0., 0., infoPtr->eCM()), infoPtr->eCM(), 0. ); 
  process.append( infoPtr->idA(), -12, 0, 0, 3, 0, 0, 0, 
    Vec4(0., 0., infoPtr->pzA(), infoPtr->eA()), infoPtr->mA(), 0. ); 
  process.append( infoPtr->idB(), -12, 0, 0, 4, 0, 0, 0, 
    Vec4(0., 0., infoPtr->pzB(), infoPtr->eB()), infoPtr->mB(), 0. ); 

  // Since LHA partons may be out of order, determine correct one.
  // (Recall that zeroth particle is empty.) 
  vector<int> newPos;
  newPos.reserve(lhaEvntPtr->size());
  newPos.push_back(0);
  for (int iNew = 0; iNew < lhaEvntPtr->size(); ++iNew) {
    // For iNew == 0 look for the two incoming partons, then for
    // partons having them as mothers, and so on layer by layer.
    for (int i = 1; i < lhaEvntPtr->size(); ++i)
      if (lhaEvntPtr->mother1(i) == newPos[iNew]) newPos.push_back(i);
    if (int(newPos.size()) <= iNew) break;
  } 

  // Find scale from which to begin evolution.
  double scale = lhaEvntPtr->scale();
  process.scale( scale);

  // Copy over info from LHA event to process, in proper order.
  for (int i = 1; i < lhaEvntPtr->size(); ++i) {
    int iOld = newPos[i];
    int id = lhaEvntPtr->id(iOld);

    // Translate from LHA status codes.
    int lhaStatus =  lhaEvntPtr->status(iOld);
    int status = -21;
    if (lhaStatus == 2 || lhaStatus == 3) status = -22;
    if (lhaStatus == 1) status = 23;

    // Find where mothers have been moved by reordering.
    int mother1Old = lhaEvntPtr->mother1(iOld);   
    int mother2Old = lhaEvntPtr->mother2(iOld);   
    int mother1 = 0;
    int mother2 = 0; 
    for (int im = 1; im < i; ++im) {
      if (mother1Old == newPos[im]) mother1 = im + 2; 
      if (mother2Old == newPos[im]) mother2 = im + 2; 
    } 
    if (i <= 2) mother1 = i;

    // Find daughters and where they have been moved by reordering. 
    // (Values shifted two steps to account for inserted beams.)
    int daughter1 = 0;
    int daughter2 = 0;
    for (int im = i + 1; im < lhaEvntPtr->size(); ++im) { 
      if (lhaEvntPtr->mother1(newPos[im]) == iOld
        || lhaEvntPtr->mother2(newPos[im]) == iOld) {
        if (daughter1 == 0 || im + 2 < daughter1) daughter1 = im + 2;
        if (daughter2 == 0 || im + 2 > daughter2) daughter2 = im + 2;
      }
    }
    // For 2 -> 1 hard scatterings reset second daughter to 0.
    if (daughter2 == daughter1) daughter2 = 0;

    // Colour trivial, except reset irrelevant colour indices.
    int colType = ParticleDataTable::colType(id);
    int col1 = (colType == 1 || colType == 2) ? lhaEvntPtr->col1(iOld) : 0;   
    int col2 = (colType == -1 || colType == 2) ?  lhaEvntPtr->col2(iOld) : 0; 

    // Momentum trivial.
    double px = lhaEvntPtr->px(iOld);  
    double py = lhaEvntPtr->py(iOld);  
    double pz = lhaEvntPtr->pz(iOld);  
    double e  = lhaEvntPtr->e(iOld);  
    double m  = lhaEvntPtr->m(iOld);

    // For resonance decay products use resonance mass as scale.
    double scaleNow = scale;
    if (mother1 > 4) scaleNow = process[mother1].m();
    process.append( id, status, mother1, mother2, daughter1, daughter2, 
      col1, col2, Vec4(px, py, pz, e), m, scaleNow);
  }  

  // Add any junctions to the process event record list.
  findJunctions( process);

  // Extract information that is guaranteed available.
  string name = (hasPythia6) ? "Pythia6 LHA process" : "External LHA process"; 
  int code = lhaEvntPtr->idProc();
  int nFinal = 0;
  for (int i = 5; i < process.size(); ++i) 
    if (process[i].mother1() == 3) ++nFinal;
  int    id1     =  process[3].id(); 
  int    id2     =  process[4].id(); 
  double Q2Fac   = pow2(lhaEvntPtr->scale());
  double alphaEM = lhaEvntPtr->alphaQED();
  double alphaS  = lhaEvntPtr->alphaQCD();
  double Q2Ren   = Q2Fac;
  double x1      = 2. * process[3].e() / infoPtr->eCM();
  double x2      = 2. * process[4].e() / infoPtr->eCM();
  Vec4   pSum    = process[3].p() + process[4].p();
  double sHat    = pSum*pSum;

  // Reset quantities that may or may not be known.
  double pdf1  = 0.;
  double pdf2  = 0.;
  double tHat  = 0.;
  double uHat  = 0.;
  double pTHat = 0.;
  double m3    = 0.;
  double m4    = 0.;
  double theta = 0.;
  double phi   = 0.;

  // Read info om parton densities if provided.
  if (lhaEvntPtr->pdfIsSet()) {
    pdf1  = lhaEvntPtr->xpdf1();
    pdf2  = lhaEvntPtr->xpdf2();
    Q2Fac = pow2(lhaEvntPtr->scalePDF());
    x1    = lhaEvntPtr->x1();
    x2    = lhaEvntPtr->x2();
  }

  // Reconstruct kinematics of 2 -> 2 processes from momenta.
  if (nFinal == 2) {
    Vec4 pDifT = process[3].p() - process[5].p();
    tHat = pDifT*pDifT;    
    Vec4 pDifU = process[3].p() - process[6].p();
    uHat = pDifU*pDifU;
    pTHat = process[5].pT();
    m3 = process[5].m();    
    m4 = process[6].m(); 
    Vec4 p5 = process[5].p();
    p5.bstback(pSum);
    theta = p5.theta();   
    phi = process[5].phi();   
  }

  // Store information in Info object.
  infoPtr->setType( name, code, nFinal, false, true, false, false);
  infoPtr->setPDFalpha( id1, id2, pdf1, pdf2, Q2Fac, alphaEM, alphaS, Q2Ren);
  infoPtr->setKin( x1, x2, sHat, tHat, uHat, pTHat, m3, m4, theta, phi);  
  infoPtr->setTypeMI( code, pTHat);

  // Done.
  return true;
}

//*********

// Read in the hard process, special case if all partons already given.

bool ProcessLevel::getSimpleLHAevnt( Event& process) {

  // Generate the next Les Houches event.
  if (!lhaEvntPtr->set()) return false;

  // Calculate the total four-momentum of the event.
  // Isolate original system, i.e. those with mother = 0.
  int nTot = lhaEvntPtr->size();
  int nDaughter = 0;
  Vec4 pSum;
  for (int i = 1; i < nTot; ++i) {
    if (lhaEvntPtr->status(i) == 1) pSum += Vec4( lhaEvntPtr->px(i),
      lhaEvntPtr->py(i), lhaEvntPtr->pz(i), lhaEvntPtr->e(i));
    if (lhaEvntPtr->mother1(i) == 0) nDaughter = i;
  }
  double eCM = pSum.mCalc();
    
  // Let hard process record begin with the event as a whole.
  process.append( 90, -11, 0, 0, 1, nDaughter, 0, 0, pSum, eCM, 0. ); 

  // Copy over info from LHA event to process, keeping the given order.
  for (int i = 1; i < nTot; ++i) {

    // Translate from LHA status codes.
    int lhaStatus =  lhaEvntPtr->status(i);
    int status = -21;
    if (lhaStatus == 2 || lhaStatus == 3) status = -22;
    if (lhaStatus == 1) status = 23;

    // Read id and mother information.
    int id = lhaEvntPtr->id(i);
    int mother1 = lhaEvntPtr->mother1(i);   
    int mother2 = lhaEvntPtr->mother2(i);   

    // Find daughters. 
    int daughter1 = 0;
    int daughter2 = 0;
    for (int im = 1; im < nTot; ++im)  
    if (lhaEvntPtr->mother1(im) == i || lhaEvntPtr->mother2(im) == i) {
      if (daughter1 == 0 || im < daughter1) daughter1 = im;
      if (daughter2 == 0 || im > daughter2) daughter2 = im;
    }
    // For 2 -> 1 hard scatterings reset second daughter to 0.
    if (daughter2 == daughter1) daughter2 = 0;

    // Colour trivial, except reset irrelevant colour indices.
    int colType = ParticleDataTable::colType(id);
    int col1 = (colType == 1 || colType == 2) ? lhaEvntPtr->col1(i) : 0;   
    int col2 = (colType == -1 || colType == 2) ?  lhaEvntPtr->col2(i) : 0; 

    // Momentum trivial.
    double px = lhaEvntPtr->px(i);  
    double py = lhaEvntPtr->py(i);  
    double pz = lhaEvntPtr->pz(i);  
    double e  = lhaEvntPtr->e(i);  
    double m  = lhaEvntPtr->m(i);

    // Store the information, particle by particle.
    process.append( id, status, mother1, mother2, daughter1, daughter2, 
      col1, col2, Vec4(px, py, pz, e), m, 0.);
  }  

  // Add any junctions to the process event record list.
  findJunctions( process);

  // Done.
  return true;
}

//*********

// Add any junctions to the process event record list.
// First try, so still incomplete. ??

void ProcessLevel::findJunctions( Event& process) {

  // Loop though event; isolate all uncoloured particles.
  for (int i = 0; i < process.size(); ++i) 
  if ( process[i].col() == 0 && process[i].acol() == 0) {

    // Find all daughters and store daughter colours and anticolours.
    vector<int> daughters = process.daughterList(i);
    vector<int> cols, acols;
    for (int j = 0; j < int(daughters.size()); ++j) {
      int colDau  = process[ daughters[j] ].col();
      int acolDau = process[ daughters[j] ].acol();
      if (colDau > 0)  cols.push_back( colDau);      
      if (acolDau > 0) acols.push_back( acolDau);      
    }

    // Remove all matching colour-anticolour pairs.
    bool foundPair = true;
    while (foundPair && cols.size() > 0 && acols.size() > 0) {
      foundPair = false;
      for (int j = 0; j < int(cols.size()); ++j) {
        for (int k = 0; k < int(acols.size()); ++k) {
	  if (acols[k] == cols[j]) { 
            cols[j]  = cols.back();  cols.pop_back();     
            acols[k] = acols.back(); acols.pop_back();     
            foundPair = true; break;
	  }
	} if (foundPair) break;
      }
    } 

    // Store an (anti)junction when three (anti)coloured daughters.
    if (cols.size() == 3 && acols.size() == 0) 
      process.appendJunction( 1, cols[0], cols[1], cols[2]);
    if (acols.size() == 3 && cols.size() == 0) 
      process.appendJunction( 2, acols[0], acols[1], acols[2]);
  }

  // Done.
}

//**************************************************************************

} // end namespace Pythia8
