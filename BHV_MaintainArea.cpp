/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_MaintainArea.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_MaintainArea.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_MaintainArea::BHV_MaintainArea(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
  addInfoVars("OPREG_TRAJECTORY_PERIM_ETA", "no_warning");

  m_osx=0;
  m_osy=0;
  m_osh=0;
  m_eta=0;
  m_desired_course=0;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_MaintainArea::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "foo") && isNumber(val)) {
    // Set local member variables here
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_MaintainArea::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_MaintainArea::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_MaintainArea::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_MaintainArea::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_MaintainArea::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_MaintainArea::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_MaintainArea::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_MaintainArea::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  bool ok1, ok2, ok3, ok4;
  m_osx=getBufferDoubleVal("NAV_X",ok1);
  m_osy=getBufferDoubleVal("NAV_Y",ok2);
  m_osh=getBufferDoubleVal("NAV_HEADING",ok3);
  m_eta=getBufferDoubleVal("OPREG_TRAJECTORY_PERIM_ETA",ok4);

  if(m_eta<4)
    {
      m_priority_wt=100;
      double desired_course=m_osh+180;
      ipf = GetCourse(desired_course);
      return(ipf);
    }
  else
    m_priority_wt=0;
  ipf = GetCourse(m_osh);
  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

IvPFunction* BHV_MaintainArea::GetCourse(double desired_course)
{
  IvPFunction *ipf = 0;

  ZAIC_PEAK zaic_peak(m_domain, "course");
  zaic_peak.setSummit(desired_course);
  zaic_peak.setMinMaxUtil(0, 100);
  zaic_peak.setBaseWidth(60);
  //IvPFunction *ipf = 0;

  ipf = zaic_peak.extractIvPFunction();
  return(ipf);
}
