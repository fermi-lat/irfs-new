{
/**
 * @file irf_test.C
 * @brief Macro to demonstrate the use of IRF components.
 * @author J. Chiang
 *
 * $Header$
 */

gSystem->Load("rootIrfLoader");

// DC1A::Front are loaded by default.
rootIrfLoader::Psf psf;

// Change it on the fly to DC1A::Back.
psf.setIrfs("DC1A::Back");

Int_t npts = 50;

TVector sep(npts);
TVector psf_val(npts);

Float_t sepmax = 20.;
Float_t sepmin = 1e-2;
Float_t dsep = log(sepmax/sepmin)/Float_t(npts-1);

Float_t energy = 300;
Float_t theta = 0;
Float_t phi = 0;

for (Int_t i = 0; i < npts; i++) {
   sep[i] = sepmin*exp(dsep*i);
   psf_val[i] = psf(sep[i], energy, theta, phi);
//    printf("%e  %e\n", sep[i], psf_val[i]);
}

TGraph psf_plot(sep, psf_val);

TCanvas canvas("c1", "Psf plot", 400, 400);

canvas.SetLogx();
canvas.SetLogy();
psf_plot.GetXaxis()->SetTitle("separation (degrees)");
psf_plot.GetXaxis()->CenterTitle();
psf_plot.GetYaxis()->SetTitle("Psf (sr^-1)");
psf_plot.GetYaxis()->CenterTitle();

psf_plot->Draw("AC");

rootIrfLoader::Aeff aeff_front;

// One can also set the irf in constructor.
rootIrfLoader::Aeff aeff_back("DC1A::Back");

TVector energies(npts);
TVector front(npts);
TVector back(npts);

Float_t emin = 30;
Float_t emax = 2e5;

Float_t de = log(emax/emin)/Float_t(npts-1);
for (Int_t i = 0; i < npts; i++) {
   energies[i] = emin*exp(de*i);
   front[i] = aeff_front(energies[i], theta, phi);
   back[i] = aeff_back(energies[i], theta, phi);
}

TGraph front_plot(energies, front);
TGraph back_plot(energies, back);

TCanvas canvas2("c2", "Aeff plot", 400, 400);

canvas2.SetLogx();
front_plot.GetXaxis()->SetTitle("energy (MeV)");
front_plot.GetXaxis()->CenterTitle();
front_plot.GetYaxis()->SetTitle("Aeff (cm^2)");
front_plot.GetYaxis()->CenterTitle();

front_plot->Draw("AC");
back_plot->SetLineStyle(2);
back_plot->Draw("C");

}

