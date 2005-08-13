/**
   @page psf PSF Parameterization

   @section scaling Angular Deviation Scaling

   We attempt to characterize the mean angular deviations of the
   reconstructed direction from the true direction as a function of \c
   McEnergy and \c McZDir.  For the GR v4r2 AllGamma data and DC1
   cuts, the angular deviations are defined using \c BestDirErr:

@verbatim
   if (IMvertexProb < 0.5 || VtxAngle == 0.0) {
      BestDirErr = McTkr1DirErr;
   } else {
      BestDirErr = McDirErr;
   }
@endverbatim
   
   First, we fit the shape of the overall mean deviations as a
   function of energy (for the front and back of the LAT separately).
   We fit the profile plots using the form

   \f[
   \tilde\theta({\tt McEnergy}) = 
         {\tt theta0} (1/{\tt McEnergy} + 1/{\tt Eb})^{\tt gamma}
   \f]

   with these results:

   @image html zeroth_order_scaling.png Overall energy scaling of angular deviations

   Fit parameters:
@verbatim
                  Front          Back
   theta0         1.947          3.457
   Eb             4.93e4         3.52e4
   gamma          0.786          0.798
@endverbatim

   By cutting on \c McZDir, we find that the angular deviation scale,
   \c theta0, varies with incident photon inclination.  The parameters
   \c Eb and \c gamma appear to be largely independent of \c McZDir,
   so using the above values for those parameters, we fit \c theta0 as
   a function of \c McZDir.  Here are some example plots of the fits
   to the profiles for the ends of the range of \c McZDir values:

   @image html energy_scale_fit_examples.png Fits of angular deviation for different ranges of \c McZDir

   Now we model \c theta0 as a function of \c McZDir using the
   hyper-parameterization

   \f[
   {\tt theta0}({\tt McZDir}) = {\tt A} ({\tt McZDir} + 1)^2 + {\tt B}
   \f]

   and find

@verbatim
                  Front          Back
   A              3.392          4.884
   B              1.630          2.899
@endverbatim

   @image html energy_scale_normalization.png Hyper-parameterizations of \c theta0.

   For reference, the complete expression for the angular deviation
   scaling is

   \f[
   \tilde\theta({\tt McEnergy}, {\tt McZDir}) 
                 = \left[{\tt A} ({\tt McZDir} + 1)^2 + B\right]
                    \left(\frac{1}{\tt McEnergy} + 
                          \frac{1}{\tt Eb}\right)^{\tt gamma}
   \f]
   
   @section psf_fits Fitting the PSFs

   We define the scaled deviation as
   \f[
   {\tt x} \equiv {\tt BestDirErr}/\tilde\theta
   \f]
   and model the PSF using 
   \f[
   dN/d{\tt x} = p_0 {\tt x} (1 + p_2 {\tt x}^2)^{-p_1}
   \f]

   This form has the same dependence on \c BestDirErr as the expression Toby
   uses,

   \f[
   dN/du = \left(1 - \frac{1}{\gamma}\right)
           \left(1 + \frac{u}{\gamma}\right)^{-\gamma}
   \f]

   where \f$u = 0.5 ({\tt BestDirErr}/\sigma)^2\f$, and his
   \f$\sigma\f$ is equivalent to our \f$\tilde\theta\f$.  We include
   an extra parameter, \f$p_2\f$, for which we will derive a
   functional dependence on \f$p_1\f$ that differs from that found in
   Toby's expression.

   Since the distributions of scaled deviations are nearly lognormal,
   we make histograms of \c log10(x) so that the function that we fit
   to these distributions is actually

   \f[
   dN/d\log {\tt x } =  p_0 {\tt x}^2 (1 + p_2 {\tt x}^2)^{-p_1}
   \f]
   
   Following the practice used for the DC1 IRFs, the AllGamma data are
   partitioned into 8 energy bins (two per decade over range
   \f$31.6\f$ to \f$3.16\times 10^5\f$ MeV) and four inclination
   ranges (partitioning \c McZDir into four equal bins covering from
   -1 to -0.2, which corresponds to inclination ranges 0-37, 37-53,
   53-66, 66-78 degrees).  Plots of the fits to these distributions
   are given in the Appendix.

   Here we plot \f$\log p_1\f$ vs \f$\log p_2\f$:

   @image html p2_hyperparams.png  \c log(p1) vs \c log(p2)

   The y-axis for each plot is log-scale, so we fit

   \f[
   \log p_1 = C \exp(-\log p_2/D)
   \f]

   or equivalently,

   \f[
   p_2 = \exp\left[D\log(C/\log p_1)\right]
   \f]

   The red curves correspond to the fit parameters,
@verbatim
                  Front          Back
   C              0.695          0.688
   D              2.117          2.900
@endverbatim

   The green curves are \f$p_2 = 1/2 p_1\f$, as inferred from Toby'
   expression.

   Finally, we plot \f$p_1\f$ vs energy for the 3-parameter (black points)
   and 2-parameter (red) fits:

   @image html final_hyperparam.png Yet another hyperparameterization?

   Does one need a yet another hyper-parameterization to describe
   \f$p_1\f$ as a function of energy, or can we just take a single
   value, e.g., \f$p_1 = 2\f$, as an adequately representative value?

   @section containment_radius 68% and 95% Containment Radii

   The integral PSF as a function of scaled deviation is

   \f[
   f(x) = \frac{1}{f(\infty)}
          \int_0^x \tilde{x}\,dx (1 + p_2 \tilde{x}^2)^{-p_1}
        = 1 - (1 + p_2x^2)^{1 - p_1}
   \f]

   Given \f$f(x_{68}) = 0.68\f$,

   \f[
   x_{68}(p_1, p_2) = \left[\frac{(1 + 0.68)^{1/(1-p_1)} - 1}{p_2}\right]^{1/2}
   \f]

   so that \f$\theta_{68}=\tilde{\theta}({\tt McEnergy},{\tt McZDir}) \times 
              x_{68}\f$.

   The solid curves in the following plot show the 68% and 95%
   containment radii as a function of energy (on-axis).  The plotted
   points are <a
   href="http://www-glast.slac.stanford.edu/software/AnaGroup/burnett/LAT%20performance.pdf">Toby's
   results</a> from the Jan 3, 2005 Analysis meeting.

   @image html panel1.png 68% (left) and 95% (right) containment radii for front (black) and back (red) of LAT.

   @section appendix Appendix: Plots of fits to scaled deviation distributions.

   The plots in this appendix show the results of fitting the
   distributions of \c log10(x).  The solid black curves are the
   3-parameter representation comprising \f$p_0\f$, \f$p_1\f$ and
   \f$p_2\f$; the dotted black curves are the 2-parameter
   representations comprising \f$p_0\f$ and \f$p_1\f$ and using the
   hyper-parameterization of \f$p_2(p_1)\f$; and the cyan curves are
   the DC1A implementation taking \f$p_1 \equiv 2\f$.  Note that the
   DC1A curves have not been normalized to the distributions.

   The following four sets are for the front part of the LAT:

   @image html psf_fit_thin_0.png "Front, 0 < inc < 37"

   @image html psf_fit_thin_1.png "Front, 37 < inc < 53"

   @image html psf_fit_thin_2.png "Front, 53 < inc < 66"

   @image html psf_fit_thin_3.png "Front, 66 < inc < 78"

   The next four sets show the corresponding fits for the back of the LAT:

   @image html psf_fit_thick_0.png "Back, 0 < inc < 37"

   @image html psf_fit_thick_1.png "Back, 37 < inc < 53"

   @image html psf_fit_thick_2.png "Back, 53 < inc < 66"

   @image html psf_fit_thick_3.png "Back, 66 < inc < 78"
   

*/

