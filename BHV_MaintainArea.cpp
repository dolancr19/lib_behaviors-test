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
  m_first_calc=true;
  m_turn_required=false;
  m_west=true;
  m_east=false;
  m_heading=0;
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
  if(ok1)
    {
      if(m_osx>0)
        {
          m_east=true;
          m_west=false;
        }
      if(m_osx<0)
        {
          m_west=true;
          m_east=false;
        }
    }
  if(m_eta<20)
    m_turn_required=true;
  if(m_turn_required)
    {
      m_priority_wt=100;
      if(m_first_calc)
	{
	  m_heading=m_osh;
	  if((m_osh>120) && (m_osh<240) && (m_west))
	    m_desired_course=m_osh-150;
	  else if((m_osh>120) && (m_osh<240) && (m_east))
	    m_desired_course=m_osh+150;
	  else if((m_osh>300) && (m_osh<60) && (m_west))
	    m_desired_course=m_osh+150;
	  else if((m_osh>300) && (m_osh<60) && (m_east))
	    m_desired_course=m_osh-150;
	  else
	    m_desired_course=m_osh+150;
	  if(m_desired_course>=360)
	    m_desired_course=m_desired_course-360;
	  if(m_desired_course<0)
	    m_desired_course=m_desired_course+360;
	  m_first_calc=false;
	}
      
      ipf = GetCourse(m_desired_course);
      postMessage("m_osh",m_osh);
      
      postMessage("m_desired_course",m_desired_course);
      
    }
  else
    {
      m_first_calc=true;
      m_priority_wt=0;
      m_desired_course=m_osh;
    }
  double turn = fabs(((m_heading+360)-(m_osh+360))-360);
  postMessage("turn",turn);
  if((turn>120)&&(m_eta>20))
	 m_turn_required=false;
  if((fabs(m_osh-m_desired_course))<5)
  m_turn_required=false;
  
  ipf = GetCourse(m_desired_course);
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
