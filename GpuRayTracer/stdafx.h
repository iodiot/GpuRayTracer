// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _SECURE_SCL 0										// Disable STL checked iterators
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL_THROWS 0
#define _SCL_SECURE_NO_WARNINGS					
#define _CRT_SECURE_NO_WARNINGS					// Turn off compiler warnings about unsafe calls of old stdlib funcs

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// STL
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

#define FREEGLUT_STATIC

#include <glload/gl_3_1.h> 
#include <glload/gll.h>  
#include <GL/freeglut.h>
#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;
#include <glm/gtc/matrix_transform.hpp>

typedef unsigned int uint;
typedef unsigned char byte;