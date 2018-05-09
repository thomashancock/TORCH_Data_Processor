#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <iomanip>
#include <string>
#include <ios>

#define FCNNAMELENGTH 45

//------------------------------------------------------------------------------
/***< LOCAL_DEBUG Code */
//------------------------------------------------------------------------------
#ifdef LOCAL_DEBUG
	#define STD_LOG(x) std::clog << "\033[2;32m" << x << "\033[0m" << std::endl;
#else
	#define STD_LOG(x)
#endif /* LOCAL_DEBUG */

#ifdef LOCAL_DEBUG
	#define STD_LOG_VAR(x) std::clog << "\033[2;32m" << #x << ": " << x << "\033[0m" << std::endl;
#else
	#define STD_LOG_VAR(x)
#endif /* LOCAL_DEBUG */

#ifdef LOCAL_DEBUG
	#define STD_LOG_VAR_HEX(x) std::clog << "\033[2;32m" << #x << ": " << std::hex << x << std::dec << "\033[0m" << std::endl;
#else
	#define STD_LOG_VAR_HEX(x)
#endif /* LOCAL_DEBUG */

inline std::string trimmedString(
	const std::string& thing,
	const unsigned int& length = FCNNAMELENGTH
) {
  size_t pos = thing.find("(");
  if( pos != std::string::npos ){
    return pos < length ? thing.substr(0,pos) : thing.substr(0,length);
  }
  return thing.size() < length ? thing : thing.substr(0,length)+"...";
  if( thing.size() < length ) return thing;
}

#define  STD_ERR(x) std::cerr << "\033[1;31m" << std::left << std::setw(FCNNAMELENGTH)   \
<< trimmedString(__PRETTY_FUNCTION__) << "  ERROR        " << "\033[0m" << x << std::endl;

#define  WARNING(x) std::cout << "\033[1;35m" << std::left << std::setw(FCNNAMELENGTH) \
<<  trimmedString(__PRETTY_FUNCTION__) << "  WARNING      "    << "\033[0m" << x << "\n"

inline void forceStop(std::string fileName, unsigned lineNo) {
  fflush(NULL);
	std::cerr << "\033[1;31m" << "\nAssertion failed: Line: " << lineNo << ", File: " << fileName << "\033[0m" << std::endl;
	exit(0);
}

#ifdef LOCAL_DEBUG
	#define ASSERT(x) \
		if (x) \
			{} \
		else \
			forceStop(__FILE__,__LINE__)
#else
	#define ASSERT(x)
	// If LOCAL_DEBUG is not defined, the assertions will be "switched off".
#endif /* LOCAL_DEBUG */

#endif /* DEBUG_H */
