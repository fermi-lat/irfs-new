{
   Int_t npts = 11;
   Float_t x[npts];
   Float_t y[npts];
   for (Int_t i=0; i < npts; i++) {
      x[i] = i/10.;
      y[i] = i/10.;
      printf("%f  %f\n", x[i], y[i]);
   }
   TH2F foo("my_hist", "my_hist", npts-1, x, npts-1, y);

   for (Int_t i=0; i < npts; i++) {
      for (Int_t j=0; j < npts; j++) {
         foo.SetBinContent(i+1, j+1, i);
      }
   }
   Int_t bin;
   Float_t yy = 0.5;
   for (Float_t xx = 0; xx < 1.5; xx += 0.05) {
      bin = foo.FindBin(xx, yy);
      printf("%f  %f  %f\n", xx, yy, foo.GetBinContent(bin));
   }
   Float_t xx = 0.5;
   for (Float_t yy = 0; yy < 1.5; yy += 0.05) {
      bin = foo.FindBin(xx, yy);
      printf("%f  %f  %f\n", xx, yy, foo.GetBinContent(bin));
   }

   bin = foo.FindBin(0.9999, 1.5);
   printf("%f\n", foo.GetBinContent(bin));
}
