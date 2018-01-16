/*
 * libfsc.h
 *
 *  Created on: 2013年9月25日 下午7:23:05
 *      Author: xuzewen
 */

#ifndef LIBFSC_H_
#define LIBFSC_H_

#define __LIBFSCH__

#if !defined (__LIBFSCH__) && !defined (LIBFSC)
#error only libfsc.h can be included directly.
#endif

#include "core/Fsc.h"
#include "stmp/StmpFusr.h"
#include "stmp/StmpNet.h"
#include "stmp/StmpN2H.h"
#include "stmp/StmpTransPassive.h"

#endif /* LIBFSC_H_ */
