/*********************************************************************************
*  Copyright (c) 2011-2012, Paul Tschirhart
*                             Peter Enns
*                             Jim Stevens
*                             Ishwar Bhati
*                             Mu-Tien Chang
*                             Bruce Jacob
*                             University of Maryland 
*                             pkt3c [at] umd [dot] edu
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/

#ifndef NVINIT_H
#define NVINIT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "FlashConfiguration.h"

using namespace std;

// Uhhh, apparently the #name equals "name" -- HOORAY MACROS!
#define DEFINE_UINT_PARAM(name, paramtype) {#name, &name, UINT, paramtype, false}
#define DEFINE_STRING_PARAM(name, paramtype) {#name, &name, STRING, paramtype, false}
#define DEFINE_FLOAT_PARAM(name,paramtype) {#name, &name, FLOAT, paramtype, false}
#define DEFINE_DOUBLE_PARAM(name,paramtype) {#name, &name, DOUBLE, paramtype, false}
#define DEFINE_BOOL_PARAM(name, paramtype) {#name, &name, BOOL, paramtype, false}
#define DEFINE_UINT64_PARAM(name, paramtype) {#name, &name, UINT64, paramtype, false}

namespace NVDSim 
{

        typedef enum _variableType {STRING, UINT, UINT64, FLOAT, DOUBLE, BOOL} varType;
	typedef enum _paramType {SYS_PARAM, DEV_PARAM} paramType;
	typedef struct _configMap 
	{
		string iniKey; //for example "tRCD"

		void *variablePtr;
		varType variableType;
		paramType parameterType;
		bool wasSet; 
	} ConfigMap;

	class Init 
	{		
		public:
			static void SetKey(string key, string value, bool isSystemParam = false, size_t lineNumber = 0);
			static void OverrideKeys(vector<string> keys, vector<string> values);
			static void ReadIniFile(string filename, bool isSystemParam);
			//static void InitEnumsFromStrings();
			static bool CheckIfAllSet();
			static void WriteValuesOut(std::ofstream &visDataOut);
		private:
			static void Trim(string &str);
	};
}


#endif
