/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FrontSearch.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef FrontSearch_HEADER
#define FrontSearch_HEADER

#include <string>
#include "IvPBehavior.h"
#include "ZAIC_PEAK.h"
#include <iostream>
#include <cmath>

class BHV_FrontSearch : public IvPBehavior {
public:
  BHV_FrontSearch(IvPDomain);
  ~BHV_FrontSearch() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  IvPFunction* GetCourse();

protected: // Local Utility functions

protected: // Configuration parameters
  string m_host_community;
  string m_report;
  
protected: // State variables
  double m_desired_course;
  double m_initial_temp;
  bool m_warming;
  bool m_cooling;
  double m_osx;
  double m_osy;
  double m_osh;
  bool m_steady;
  double m_previous_temp;
  double m_counter;
  bool m_first_temp;
  bool m_west;
  bool m_east;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_FrontSearch(domain);}
}
#endif
