// ====================================================================
//   CalorimeterSD.cc
//
// ====================================================================
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "RCSD.hh"
#include "RCHit.hh"


//////////////////////////////////////////////////
RCSD::RCSD(const G4String& name)
  : G4VSensitiveDetector(name)
//////////////////////////////////////////////////
{
  collectionName.insert("rangecounter");
}

///////////////////////////////
RCSD::~RCSD()
///////////////////////////////
{
}

/////////////////////////////////////////////////////
void RCSD::Initialize(G4HCofThisEvent* HCTE)
/////////////////////////////////////////////////////
{
  // create hit collection(s)
  hitsCollection= new RCHitsCollection(SensitiveDetectorName,
                                        collectionName[0]); 
  
  // push H.C. to "Hit Collection of This Event"
  G4int hcid= GetCollectionID(0);
  HCTE-> AddHitsCollection(hcid, hitsCollection);
  
  // clear energy deposit buffer
  for (G4int i=0; i<NCHANNEL; i++){
    edepbuf[i]=0.;
    mombuf[i]=0.;
  }
}

///////////////////////////////////////////////////////
G4bool RCSD::ProcessHits(G4Step* astep, 
                                  G4TouchableHistory* )
///////////////////////////////////////////////////////
{
  // get step information from "PreStepPoint"
  const G4StepPoint* preStepPoint= astep-> GetPreStepPoint();
  G4TouchableHistory* touchable=
    (G4TouchableHistory*)(preStepPoint-> GetTouchable());
  G4Track *atrack = astep ->GetTrack();
  G4ThreeVector momV = atrack->GetMomentum();
  
  
  // accumulate energy deposit in each scintillator
  G4int id= touchable-> GetReplicaNumber();
  mombuf[id] = momV.mag();
  edepbuf[id]+= astep-> GetTotalEnergyDeposit();
  
  return true;
}

/////////////////////////////////////////////////
void RCSD::EndOfEvent(G4HCofThisEvent* )
/////////////////////////////////////////////////
{
  // make hits and push them to "Hit Coleltion"
  for (G4int id=0; id< NCHANNEL; id++) {
    if(edepbuf[id] > 0. ) {
      RCHit* ahit= new RCHit(id, edepbuf[id], mombuf[id]);
      hitsCollection-> insert(ahit);
    }
  }
}

/////////////////////////////
void RCSD::DrawAll()
/////////////////////////////
{
}

//////////////////////////////
void RCSD::PrintAll()
//////////////////////////////
{
  hitsCollection-> PrintAllHits();
}
