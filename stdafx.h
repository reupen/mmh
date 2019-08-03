#pragma once

#include <algorithm>

#include <assert.h>
#include <ppl.h>

// Included before windows.h, because pfc.h includes winsock2.h
#include "../pfc/pfc.h"

#include <windows.h>
#include <VersionHelpers.h>

#include "algorithm.h"
#include "osversion.h"
#include "exception.h"
#include "module.h"
#include "sort.h"
#include "string.h"
#include "thread.h"
#include "istream.h"
#include "comptr.h"
#include "sha1.h"
#include "rand.h"
