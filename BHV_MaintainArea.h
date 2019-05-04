/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_MaintainArea.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef MaintainArea_HEADER
#define MaintainArea_HEADER

#include <string>
#include "IvPBehavior.h"
#include "ZAIC_PEAK.h"

class BHV_MaintainArea : public IvPBehavior {
public:
  BHV_MaintainArea(IvPDomain);
  ~BHV_MaintainArea() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  IvPFunction* GetCourse(double desired_course);

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
  double m_osx;
  double m_osy;
  double m_osh;
  double m_eta;
  double m_desired_course;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_MaintainArea(domain);}
}
#endif
