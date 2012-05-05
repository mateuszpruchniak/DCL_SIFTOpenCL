/**@file
Functions and structures for dealing with image features

Copyright (C) 2006-2010  Rob Hess <hess@eecs.oregonstate.edu>

@version 1.1.2-20100521
*/

#ifndef IMGFEATURES_H
#define IMGFEATURES_H

#include "cxcore.h"




/** FEATURE_OXFD <BR> FEATURE_LOWE */
enum feature_type
{
	FEATURE_OXFD,
	FEATURE_LOWE
};

/** FEATURE_FWD_MATCH <BR> FEATURE_BCK_MATCH <BR> FEATURE_MDL_MATCH */
enum feature_match_type
{
	FEATURE_FWD_MATCH,
	FEATURE_BCK_MATCH,
	FEATURE_MDL_MATCH
};


/* colors in which to display different feature types */
#define FEATURE_OXFD_COLOR CV_RGB(255,255,0)
#define FEATURE_LOWE_COLOR CV_RGB(255,0,255)

/** max feature descriptor length */
#define FEATURE_MAX_D 128


/**
Structure to represent an affine invariant image feature.  The fields
x, y, a, b, c represent the affine region around the feature:

a(x-u)(x-u) + 2b(x-u)(y-v) + c(y-v)(y-v) = 1
*/

//typedef struct feature feature;

typedef struct feature
{
	double x;                      /**< x coord */
	double y;                      /**< y coord */
	double a;                      /**< Oxford-type affine region parameter */
	double b;                      /**< Oxford-type affine region parameter */
	double c;                      /**< Oxford-type affine region parameter */
	double scl;                    /**< scale of a Lowe-style feature */
	double ori;                    /**< orientation of a Lowe-style feature */
	int d;                         /**< descriptor length */
	double descr[FEATURE_MAX_D];   /**< descriptor */
	int type;                      /**< feature type, OXFD or LOWE */
	int category;                  /**< all-purpose feature category */
	struct feature* fwd_match;     /**< matching feature from forward image */
	//struct feature* bck_match;     /**< matching feature from backmward image */
	//struct feature* mdl_match;     /**< matching feature from model */
	CvPoint2D64f img_pt;           /**< location in image */
	CvPoint2D64f mdl_pt;           /**< location in model */
	void* feature_data;            /**< user-definable data */
} feature;


typedef struct
{
	float x;                      /**< x coord */
	float y;                      /**< y coord */
	float a;                      /**< Oxford-type affine region parameter */
	float b;                      /**< Oxford-type affine region parameter */
	float c;                      /**< Oxford-type affine region parameter */
	float scl;                    /**< scale of a Lowe-style feature */
	float ori;                    /**< orientation of a Lowe-style feature */
	int d;                         /**< descriptor length */
	float descr[FEATURE_MAX_D];   /**< descriptor */
	int type;                      /**< feature type, OXFD or LOWE */
	int category;                  /**< all-purpose feature category */

	//CvPoint2D64f img_pt;           /**< location in image */
	//CvPoint2D64f mdl_pt;           /**< location in model */
	//void* feature_data;            /**< user-definable data */
} featureGPU;




#endif
