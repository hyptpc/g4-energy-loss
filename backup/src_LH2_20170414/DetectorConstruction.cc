
#include "DetectorConstruction.hh"


#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4PVParameterised.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "RCParameterisation.hh"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include "G4SDManager.hh"
#include "RCSD.hh"
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:AbsorberMaterial(0),GapMaterial(0),defaultMaterial(0),
 solidWorld(0),logicWorld(0),physiWorld(0),
 solidCalor(0),logicCalor(0),physiCalor(0),
 solidLayer(0),logicLayer(0),physiLayer(0),
 solidAbsorber(0),logicAbsorber(0),physiAbsorber(0),
 solidGap (0),logicGap (0),physiGap (0),
 magField(0)
{

  
  // default parameter values of the calorimeter
  AbsorberThickness =  0.*mm; // %%%%%%%%%%%%%%%%%
  GapThickness      =  0.*mm; // %%%%%%%%%%%%%%%%%
  NbOfLayers        =  10;    // %%%%%%%%%%%%%%%%%
  CalorSizeYZ       = 10.*cm;
  ComputeCalorParameters();
  
  // materials
  DefineMaterials();
  SetAbsorberMaterial("Vacuum");
  SetGapMaterial("Vacuum"); // %%%%%%%%%%%%%%
  //  SetRCMaterial("CH2"); // %%%%%%%%%%%%%%
  //SetRCMaterial("Carbon"); // %%%%%%%%%%%%%%
  SetRCMaterial("LH2"); // %%%%%%%%%%%%%%

  // create commands for interactive definition of the calorimeter
  //detectorMessenger = new DetectorMessenger(this);


  //  std::ifstream det_param_file;
  // det_param_file.open("DetPar/det_param1.par");
  // det_param_file >> NbOfLayers;
  
  //  det_param_file.close();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  // Clean old geometry, if any
  //
  
  
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // complete the Calor parameters definition
  ComputeCalorParameters();
  
  //     
  // World
  //
  solidWorld = new G4Box("World",				//its name
			 //                   WorldSizeX/2,WorldSizeYZ/2,WorldSizeYZ/2);	//its size
			 200 *cm,200 *cm,200 *cm);	//its size
                         
  logicWorld = new G4LogicalVolume(solidWorld,		//its solid
                                   defaultMaterial,	//its material
                                   "World");		//its name
                                   
  physiWorld = new G4PVPlacement(0,			//no rotation
  				 G4ThreeVector(),	//at (0,0,0)
                                 logicWorld,		//its logical volume				 
                                 "World",		//its name
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number
  
  //
  solidRC = new G4Box("RC", 100*cm, 100*cm, 100*cm);
  logicRC = new G4LogicalVolume(solidRC,RCMaterial,"RC",0,0,0);
  

  RCParam = new RCParameterisation();
  NbOfLayers = RCParam->GetNbOfLayers();
  //ConstructCalorimeter();
  
  //  physiRC = new G4PVPlacement(0,			//no rotation
  //		      G4ThreeVector(100,100,100),	//at (0,0,0)
  //		      logicRC,	//its logical volume
  //		      "RC",	//its name
    //		      logicWorld,	//its mother  volume
  //		      false,		//no boolean operation
    //		      0);		//copy number
  
  
  G4cout <<" *************** "<<NbOfLayers << G4endl;
  physiRC = new G4PVParameterised(
				  "RC",       // their name                                                                                                     
				  logicRC,    // their logical volume                                                                                           
				  logicWorld,    // Mother logical volume                                                                                          
				  kZAxis,          // Are placed along this axis                                                                                     
				  NbOfLayers,    // Number of chambers                                                                                                     
				  RCParam);   // The parametrisation                                                                                            



  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // define senstive detector
  G4SDManager* SDmanager= G4SDManager::GetSDMpointer();
  RCSD* rcsd= new RCSD("/rangecounterr");
  SDmanager-> AddNewDetector(rcsd);
  logicRC-> SetSensitiveDetector(rcsd);
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //define visualization
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);
  
  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.5,1.5));
  
  simpleBoxVisAtt->SetVisibility(true);
  
  logicRC->SetVisAttributes(simpleBoxVisAtt);


  
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{ 
 //This function illustrates the possible ways to define materials
 
G4String symbol;             //a=mass of a mole;
G4double a, z, density;      //z=mean number of protons;  
G4int iz, n;                 //iz=number of protons  in an isotope; 
                             // n=number of nucleons in an isotope;

G4int ncomponents, natoms;
G4double abundance, fractionmass;

//
// define Elements
//

G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
G4Element* Si = new G4Element("Silicon",symbol="Si" , z= 14., a= 28.09*g/mole);

//
// define an Element from isotopes, by relative abundance 
//

G4Isotope* U5 = new G4Isotope("U235", iz=92, n=235, a=235.01*g/mole);
G4Isotope* U8 = new G4Isotope("U238", iz=92, n=238, a=238.03*g/mole);

G4Element* U  = new G4Element("enriched Uranium",symbol="U",ncomponents=2);
U->AddIsotope(U5, abundance= 90.*perCent);
U->AddIsotope(U8, abundance= 10.*perCent);

//
// define simple materials
//

new G4Material("Aluminium", z=13., a=26.98*g/mole, density=2.700*g/cm3);
new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
new G4Material("Lead"     , z=82., a= 207.19*g/mole, density= 11.35*g/cm3);
new G4Material("Carbon", z=6., a=12.01*g/mole, density=1.7274*g/cm3);

//
// define a material from elements.   case 1: chemical molecule
//


G4Material* LH2 = 
new G4Material("LH2", z=1., a=1.008*g/mole, density=0.07099*g/cm3);


G4Material* H2O = 
new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
H2O->AddElement(H, natoms=2);
H2O->AddElement(O, natoms=1);
// overwrite computed meanExcitationEnergy with ICRU recommended value 
H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

G4Material* Sci = 
new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
Sci->AddElement(C, natoms=9);
Sci->AddElement(H, natoms=10);

G4Material* CH2 = 
new G4Material("CH2", density= 0.9534*g/cm3, ncomponents=2);
CH2->AddElement(C, natoms=1);
CH2->AddElement(H, natoms=2);







G4Material* Myl = 
new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
Myl->AddElement(C, natoms=10);
Myl->AddElement(H, natoms= 8);
Myl->AddElement(O, natoms= 4);

G4Material* SiO2 = 
new G4Material("quartz",density= 2.200*g/cm3, ncomponents=2);
SiO2->AddElement(Si, natoms=1);
SiO2->AddElement(O , natoms=2);

//
// define a material from elements.   case 2: mixture by fractional mass
//

G4Material* Air = 
new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
Air->AddElement(N, fractionmass=0.7);
Air->AddElement(O, fractionmass=0.3);

//
// define a material from elements and/or others materials (mixture of mixtures)
//

G4Material* Aerog = 
new G4Material("Aerogel", density= 0.200*g/cm3, ncomponents=3);
Aerog->AddMaterial(SiO2, fractionmass=62.5*perCent);
Aerog->AddMaterial(H2O , fractionmass=37.4*perCent);
Aerog->AddElement (C   , fractionmass= 0.1*perCent);

//
// examples of gas in non STP conditions
//

G4Material* CO2 = 
new G4Material("CarbonicGas", density= 27.*mg/cm3, ncomponents=2,
                              kStateGas, 325.*kelvin, 50.*atmosphere);
CO2->AddElement(C, natoms=1);
CO2->AddElement(O, natoms=2);
 
G4Material* steam = 
new G4Material("WaterSteam", density= 0.3*mg/cm3, ncomponents=1,
                             kStateGas, 500.*kelvin, 2.*atmosphere);
steam->AddMaterial(H2O, fractionmass=1.);

//
// examples of vacuum
//

G4Material* Vacuum =
new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                           kStateGas, 2.73*kelvin, 3.e-18*pascal);

G4Material* beam = 
new G4Material("Beam", density= 1.e-5*g/cm3, ncomponents=1,
                       kStateGas, STP_Temperature, 2.e-2*bar);
beam->AddMaterial(Air, fractionmass=1.);

G4cout << *(G4Material::GetMaterialTable()) << G4endl;

//default materials of the World
defaultMaterial  = Air;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructCalorimeter(){

//void DetectorConstruction::ConstructCalorimeter(G4VPhysicalVolume *pMother){  
  //                               
  // Calorimeter
  //  
  solidCalor=0; logicCalor=0; physiCalor=0;
  solidLayer=0; logicLayer=0; physiLayer=0;
  

  if (CalorThickness > 0.)  
    { solidCalor = new G4Box("Calorimeter",		//its name
    		       CalorThickness/2,CalorSizeYZ/2,CalorSizeYZ/2);//size
    			     
      logicCalor = new G4LogicalVolume(solidCalor,	//its solid
      				       defaultMaterial,	//its material
      				       "Calorimeter");	//its name
    				       
      physiCalor = new G4PVPlacement(0,			//no rotation
                                     G4ThreeVector(),	//at (0,0,0)
                                     logicCalor,	//its logical volume
                                     "Calorimeter",	//its name
                                     logicWorld,	//its mother  volume
                                     false,		//no boolean operation
                                     0);		//copy number
  
  //                                 
  // Layer
  //
      solidLayer = new G4Box("Layer",			//its name
                       LayerThickness/2,CalorSizeYZ/2,CalorSizeYZ/2); //size
                       
      logicLayer = new G4LogicalVolume(solidLayer,	//its solid
                                       defaultMaterial,	//its material
                                       "Layer");	//its name
      if (NbOfLayers > 1)                                      
        physiLayer = new G4PVReplica("Layer",		//its name
      				     logicLayer,	//its logical volume
      				     logicCalor,	//its mother
                                     kXAxis,		//axis of replication
                                     NbOfLayers,	//number of replica
                                     LayerThickness);	//witdth of replica
      else
        physiLayer = new G4PVPlacement(0,		//no rotation
                                     G4ThreeVector(),	//at (0,0,0)
                                     logicLayer,	//its logical volume				     
                                     "Layer",		//its name
                                     logicCalor,	//its mother  volume
                                     false,		//no boolean operation
                                     0);		//copy number     
    }                                   
  
  //                               
  // Absorber
  //
  solidAbsorber=0; logicAbsorber=0; physiAbsorber=0;  
  
  if (AbsorberThickness > 0.) 
    { solidAbsorber = new G4Box("Absorber",		//its name
                          AbsorberThickness/2,CalorSizeYZ/2,CalorSizeYZ/2); 
                          
      logicAbsorber = new G4LogicalVolume(solidAbsorber,    //its solid
      			                  AbsorberMaterial, //its material
      			                  AbsorberMaterial->GetName()); //name
      			                  
      physiAbsorber = new G4PVPlacement(0,		   //no rotation
      		    G4ThreeVector(-GapThickness/2,0.,0.),  //its position
                                        logicAbsorber,     //its logical volume		    
                                        AbsorberMaterial->GetName(), //its name
                                        logicLayer,        //its mother
                                        false,             //no boulean operat
                                        0);                //copy number
                                        
    }
  
  //                                 
  // Gap
  //
  solidGap=0; logicGap=0; physiGap=0; 
  
  if (GapThickness > 0.)
    { solidGap = new G4Box("Gap",
    			   GapThickness/2,CalorSizeYZ/2,CalorSizeYZ/2);
    			   
      logicGap = new G4LogicalVolume(solidGap,
      				     GapMaterial,
      				     GapMaterial->GetName());
      				     
      physiGap = new G4PVPlacement(0,                      //no rotation
               G4ThreeVector(AbsorberThickness/2,0.,0.),   //its position
                                   logicGap,               //its logical volume	       
                                   GapMaterial->GetName(), //its name
                                   logicLayer,             //its mother
                                   false,                  //no boulean operat
                                   0);                     //copy number
    }
    
  PrintCalorParameters();     


  
  //                                        
  // Visualization attributes
  //

  
 /*
  // Below are vis attributes that permits someone to test / play 
  // with the interactive expansion / contraction geometry system of the
  // vis/OpenInventor driver :
 {G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  simpleBoxVisAtt->SetVisibility(true);
  delete logicCalor->GetVisAttributes();
  logicCalor->SetVisAttributes(simpleBoxVisAtt);}

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  logicLayer->SetVisAttributes(atb);}
  
 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  atb->SetForceSolid(true);
  logicAbsorber->SetVisAttributes(atb);}
  
 {//Set opacity = 0.2 then transparency = 1 - 0.2 = 0.8
  G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.2));
  atb->SetForceSolid(true);
  logicGap->SetVisAttributes(atb);}
  */

  //
  //always return the physical World
  //
  //  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n------------------------------------------------------------"
         << "\n---> The calorimeter is " << NbOfLayers << " layers of: [ "
         << AbsorberThickness/mm << "mm of " << AbsorberMaterial->GetName() 
         << " + "
         << GapThickness/mm << "mm of " << GapMaterial->GetName() << " ] " 
         << "\n------------------------------------------------------------\n";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorberMaterial(G4String materialChoice)
{
  // search the material by its name   
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);     
  if (pttoMaterial) AbsorberMaterial = pttoMaterial;
}

void DetectorConstruction::SetRCMaterial(G4String materialChoice)
{
  // search the material by its name   
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);     
  if (pttoMaterial) RCMaterial = pttoMaterial;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetGapMaterial(G4String materialChoice)
{
  // search the material by its name
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);
  if (pttoMaterial) GapMaterial = pttoMaterial;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorberThickness(G4double val)
{
  // change Absorber thickness and recompute the calorimeter parameters
  AbsorberThickness = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetGapThickness(G4double val)
{
  // change Gap thickness and recompute the calorimeter parameters
  GapThickness = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCalorSizeYZ(G4double val)
{
  // change the transverse size and recompute the calorimeter parameters
  CalorSizeYZ = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfLayers(G4int val)
{
  NbOfLayers = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

void DetectorConstruction::SetMagField(G4double fieldValue)
{
  //apply a global uniform magnetic field along Z axis
  G4FieldManager* fieldMgr
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  if(magField) delete magField;		//delete the existing magn field

  if(fieldValue!=0.)			// create a new one if non nul
  { magField = new G4UniformMagField(G4ThreeVector(0.,0.,fieldValue));
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);
  } else {
    magField = 0;
    fieldMgr->SetDetectorField(magField);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
  //  G4RunManager::GetRunManager()->DefineWorldVolume(physiWorld);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
