/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FrontSearch.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_FrontSearch.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_FrontSearch::BHV_FrontSearch(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  string host = toupper(m_host_community);
  m_report = "UCTD_MSMNT_REPORT";
  addInfoVars(m_report);
  addInfoVars("ANGLE");

  m_desired_course=180;
  m_initial_temp=0;
  m_warming=false;
  m_cooling=false;
  m_osx=0;
  m_osy=0;
  m_osh=0;
  m_steady=false;
  m_previous_temp=0;
  m_counter=0;
  m_host_community="archie";
  m_first_temp=true;
  m_west=true;
  m_east=false;
  m_angle=0;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_FrontSearch::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if(param == "vessel") {
    // Set local member variables here
    m_host_community=val;
    //cout << m_host_community << endl;
    return(true);
  }
  //else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  //}

  // If not handled above, then just return false;
  return(true);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_FrontSearch::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_FrontSearch::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_FrontSearch::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_FrontSearch::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_FrontSearch::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_FrontSearch::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_FrontSearch::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_FrontSearch::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  bool ok1, ok2, ok3, ok4, ok5;
  m_osx=getBufferDoubleVal("NAV_X",ok1);
  m_osy=getBufferDoubleVal("NAV_Y",ok2);
  m_osh=getBufferDoubleVal("NAV_HEADING",ok3);
  string info = getBufferStringVal(m_report,ok4);
  string get_temp = tokStringParse(info,"temp",',','=');
  double temp = strtod(get_temp.c_str(),NULL);
  m_angle=getBufferDoubleVal("ANGLE",ok5);
  if(ok1)
    {
      if(m_osx>120)
        {
          m_east=true;
          m_west=false;
        }
      if(m_osx<10)
        {
          m_west=true;
          m_east=false;
        }
    }
  if(ok4)
    {
      if(m_first_temp)
        {
          m_initial_temp=temp;
          m_previous_temp=temp;
          m_first_temp=false;
          postMessage("TEMP1",m_initial_temp);
        }
      

      
      postMessage("DELTA_T2",temp-m_initial_temp);

      if(temp-m_initial_temp>=4)
        {
          m_warming=true;
          m_steady=false;
          m_cooling=false;
	  m_initial_temp=temp;
	  m_previous_temp=temp;
	  m_counter=0;
          postMessage("WARMING","true");
          postMessage("STEADY1","false");
        }

      else if(temp-m_initial_temp<=-4)
        {
          m_cooling=true;
          m_steady=false;
          m_warming=false;
	  m_initial_temp=temp;
	  m_previous_temp=temp;
	  m_counter=0;
          postMessage("COOLING","true");
          postMessage("STEADY2","false");
        }

      if(m_warming)
	{
	  postMessage("COUNTER",m_counter);
	  postMessage("DELTA_T",abs(temp-m_previous_temp));
	  if(m_counter>10)
	   {
              
	      if(abs(temp-m_previous_temp<=.20))
                {
		  
                  if(m_west)
                    m_desired_course=m_angle+20;
                  else if(m_east)
	            m_desired_course=m_angle+340;
                  m_warming=false;
                  m_steady=false;
                  postMessage("WARMING","false");
                  m_initial_temp=temp;
                  m_previous_temp=temp;
                  m_counter=0;
                  postMessage("TEMP2",m_initial_temp);
                  postMessage("STEADY3","true");
                }
	      else
	        {
		  m_counter=0;
		  m_previous_temp=temp;
		  m_desired_course=m_desired_course;
	        }
	       }
	      else
	      m_desired_course=m_desired_course;
	}

      else if(m_cooling)
	{
	  if(m_counter>10)
	    {
	      if(abs(temp-m_previous_temp<=.2))
                {
                  //m_desired_course=180;
		  if(m_west)
                    m_desired_course=m_angle+160;
                  else if(m_east)
	            m_desired_course=m_angle+200;
		  m_cooling=false;
                  m_steady=false;
                  postMessage("COOLING","false");
                  m_initial_temp=temp;
                  m_previous_temp=temp;
                  m_counter=0;
                  postMessage("TEMP2",m_initial_temp);
                  postMessage("STEADY3","true");
                }
	      else
	        {
		  m_counter=0;
		  m_desired_course=m_desired_course;
	        }
	    }
	  else
	    m_desired_course=m_desired_course;
	}
      else
	m_desired_course=m_desired_course;
  
  
  m_counter+=1;
  //if(m_counter>=3)
  //{
      //  m_previous_temp=temp;
      //m_counter=0;
  //}
    }
  
  ipf = GetCourse();
  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}
  

IvPFunction* BHV_FrontSearch::GetCourse()
{
  IvPFunction *ipf = 0;

  ZAIC_PEAK zaic_peak(m_domain, "course");
  zaic_peak.setSummit(m_desired_course);
  zaic_peak.setMinMaxUtil(0, 100);
  zaic_peak.setBaseWidth(60);
  //IvPFunction *ipf = 0;

  ipf = zaic_peak.extractIvPFunction();
  return(ipf);
}
