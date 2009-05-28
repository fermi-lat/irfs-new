/** @file IrfEval.h
    @brief declare class IrfEval

    $Header$
*/

#ifndef handoff_response_IrfEval_h
#define handoff_response_IrfEval_h

#include <string>

namespace handoff_response {

/** @class IrfEval
    @brief Evaluate the functions


*/
class IrfEval {
public:

    /** @brief ctor
        @param filename name of the file to open
        @param eventclass name of the event class - expect to be of the form name/front, or name/back


    */
    virtual ~IrfEval(){};

    /** effective area
        @param energy energy in MeV
        @param theta polar angle in degrees
        @param phi   azimuthal angle in degrees

    */
    virtual double aeff(double energy, double theta=0, double phi=0)=0;

    virtual double aeffmax()=0;

    /** Point spread function, differential in solid angle
        @param delta  deviation from incoming direction, in degrees
        @param energy energy in MeV
        @param theta polar angle in degrees
        @param phi   azimuthal angle in degrees

    */
    virtual double psf(double delta, double energy, double theta=0, double phi=0)=0;

    /** Energy dispesion function, differential in energy
        @param emeas measured energy in MeV
        @param energy actual energy in MeV
        @param theta polar angle in degrees
        @param phi   azimuthal angle in degrees

    */
    virtual double dispersion(double emeas, double energy, double theta=0, double phi=0)=0;

    const std::string& eventClass()const{return m_type;}

    bool isFront()const{return m_front;}

protected:

    IrfEval(const std::string & eventclass); ///< default for subclasses only
private:
    std::string m_type;

    bool m_front;



};

} // namespace handoff_response
#endif
