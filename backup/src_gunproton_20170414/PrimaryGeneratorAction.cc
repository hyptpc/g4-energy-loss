// ====================================================================
//   PrimaryGeneratorAction.cc
//
// ====================================================================
#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

// ====================================================================
//
// class description
//
// ====================================================================

////////////////////////////////////////////////
PrimaryGeneratorAction::PrimaryGeneratorAction()
////////////////////////////////////////////////
{
  particleGun= new G4ParticleGun;

 G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
 G4String particleName;
 G4ParticleDefinition* particle
   = particleTable->FindParticle(particleName="proton");
 // G4ParticleDefinition* particle
 //   = particleTable->FindParticle(particleName="kaon-");

  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
  particleGun->SetParticleEnergy(50.*MeV);
  //  G4double position = -0.5*(ExN03Detector->GetWorldSizeX());
  // particleGun->SetParticlePosition(G4ThreeVector(position,0.*cm,0.*cm));

}

/////////////////////////////////////////////////
PrimaryGeneratorAction::~PrimaryGeneratorAction()
/////////////////////////////////////////////////
{

  delete particleGun;
}

////////////////////////////////////////////////////////////////
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
////////////////////////////////////////////////////////////////
{ 
  
  double pion_mass = 139.57;
  double proton_mass = 938.2;
  double electron_mass = 0.5109989;


 
  G4double mom_abs = 1900. + G4UniformRand()*200.;//for proton
  //G4double mom_abs = 1780. + G4UniformRand()*40.;//for kaon beam
  
  G4ThreeVector mom(0.,0.,mom_abs);
  
  particleGun->SetParticleMomentum(mom * MeV);
  
  //  G4double z0 = -0.5*(ExN03Detector->GetWorldSizeX());
  //G4double y0 = 0.*cm, z0 = 0.*cm;
  

  G4double x0 = 0. *cm;
  G4double y0 = 0. *cm;
  G4double z0 = G4UniformRand()*10. *cm;
  //       z0 = (ExN03Detector->GetCalorSizeYZ())*(G4UniformRand()-0.5);
  
  particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  particleGun -> GeneratePrimaryVertex(anEvent);

}

