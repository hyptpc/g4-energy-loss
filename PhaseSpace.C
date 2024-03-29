

void PhaseSpace() {
// example of use of TGenPhaseSpace  
//Author: Valerio Filippini

   if (!gROOT->GetClass("TGenPhaseSpace")) gSystem.Load("libPhysics");

   TLorentzVector target(0.0, 0.0, 0.0, 0.938);
   TLorentzVector beam(0.0, 0.0,1.92 , 1.9250974);
   TLorentzVector W = beam + target;

   //(Momentum, Energy units are Gev/C, GeV)
   Double_t masses[3] = { 0.938, 0.139, 0.139 } ;

   TGenPhaseSpace event;
   event.SetDecay(W, 3, masses);

TH1D *h1 = new TH1D("h1","h1", 100,0.,1.0);
   //   TH2F *h2 = new TH2F("h2","h2", 50,1.1,1.8, 50,1.1,1.8);

   for (Int_t n=0;n<100000;n++) {
      Double_t weight = event.Generate();

      TLorentzVector *pProton = event.GetDecay(0);

      TLorentzVector *pPip    = event.GetDecay(1);
      TLorentzVector *pPim    = event.GetDecay(2);

      TLorentzVector pPPip = *pProton + *pPip;
      TLorentzVector pPPim = *pProton + *pPim;

      if(pPip.Theta()*(180/3.1415)>100&&pPip.Theta()*(180/3.1415)<120){

      h1->Fill( pPip.P()/pPip.E()  ,weight);

      }
   }
   h1->Draw();
}

