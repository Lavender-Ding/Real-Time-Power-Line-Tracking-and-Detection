#pragma once
#include "stdafx.h"
#include <cfloat>
#include <cmath>
#include <iostream>
// SLIC.h: interface for the SLIC class.
//===========================================================================
// This code implements the superpixel method described in:
//
// Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Susstrunk,
// "SLIC Superpixels",
// EPFL Technical Report no. 149300, June 2010.
//===========================================================================
//	Copyright (c) 2012 Radhakrishna Achanta [EPFL]. All rights reserved.
//===========================================================================
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
#if !defined(_SLIC_H_INCLUDED_)
#define _SLIC_H_INCLUDED_

class SLIC  
{
public:
	SLIC();
	virtual ~SLIC();
	//============================================================================
	// sRGB to CIELAB conversion for 2-D images
	//=========================================================================
	void DoRGBtoLABConversionSup(
        vector <double> &           r1,
		vector <double> &           g1,
		vector <double> &           b1,
		double*&					lvec,
		double*&					avec,
		double*&					bvec,
		const int &                 numSup);
	//======================================================================
	//mean rgb in each superpixel
	//=======================================================================
	void SLIC::DoMeanSup( double *  &                    m_rr,
	                      double *  &                    m_gg,
                  	      double *  &                    m_bb,
	                      int &                          numlabels,
	                      int &                          NumPixel ,
	                      double *&                      outlabel,
	                      vector <double> &                meanSupl,
					      vector <double> &                meanSupa,
					      vector <double> &                meanSupb);
	//============================================================================
	// Superpixel segmentation for a given step size (superpixel size ~= step*step)
	//============================================================================
        void DoSuperpixelSegmentation_ForGivenSuperpixelSize(
          double *   &          r,
		  double *   &          g,
		  double *   &          b,
	      const int					width,
	      const int					height,
		vector <int> &		    	klabels,
		int&						numlabels,
        const int&					superpixelsize,
        const double&               compactness,
		double *&                   outlabel,
		const int&                   sz);
	//============================================================================
	// Superpixel segmentation for a given number of superpixels
	//============================================================================
        void DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(
        double *    &              r,
		double *    &              g,
		double *    &              b,
		const int					width,
		const int					height,
		//vector <int> &						klabels,
		int&						numlabels,
       const int&					K,         //required number of superpixels
       const double&                compactness,
	   double *&                    outlabel,
	   const int&                   NumPixel);

private:
	//============================================================================
	// The main SLIC algorithm for generating superpixels
	//============================================================================
	void PerformSuperpixelSLIC(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector <int> &			    klabels,
		const int&					STEP,
        const vector<double>&		edgemag,
		const double&				m ,
		const int &                 sz,
		const int &                 numk);
	
	//============================================================================
	// Pick seeds for superpixels when step size of superpixels is given.
	//============================================================================
	void GetLABXYSeeds_ForGivenStepSize(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const int&					STEP,
		const bool&					perturbseeds,
		const vector<double>&		edgemag,
		int &                       numk);
	
	//============================================================================
	// Move the superpixel seeds to low gradient positions to avoid putting seeds
	// at region boundaries.
	//============================================================================
	void PerturbSeeds(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const vector<double>&		edges);
	//============================================================================
	// Detect color edges, to help PerturbSeeds()
	//============================================================================
	void DetectLabEdges(
		const double*				lvec,
		const double*				avec,
		const double*				bvec,
		const int&					width,
		const int&					height,
		vector<double>&				edges,
		const int &                 sz);
	//============================================================================
	// sRGB to XYZ conversion; helper for RGB2LAB()
	//============================================================================
	void RGB2XYZ(
		const double &					sR,
		const double &					sG,
		const double &					sB,
		double&						X,
		double&						Y,
		double&						Z);
	//============================================================================
	// sRGB to CIELAB conversion (uses RGB2XYZ function)
	//============================================================================
	void RGB2LAB(
		const double &					sR,
		const double &					sG,
		const double &					sB,
		double&						lval,
		double&						aval,
		double&						bval);
	//============================================================================
	// sRGB to CIELAB conversion for 2-D images
	//============================================================================
	void DoRGBtoLABConversion(
         double *  &           r1,
		 double *  &           g1,
		 double *  &           b1,
		double*&					lvec,
		double*&					avec,
		double*&					bvec,
		const int &                 sz);
	//===============================================================================
	//  convert mean rgb of superpixel to lab
	//=================================================================================
	void RGB2XYZSup(
		const double &					sR,
		const double &					sG,
		const double &					sB,
		double&						X,
		double&						Y,
		double&						Z);
	//============================================================================
	// sRGB to CIELAB conversion (uses RGB2XYZ function)
	void RGB2LABSup(
		const  double &					sR,
		const double &					sG,
		const double &					sB,
		double&						lval,
		double&						aval,
		double&						bval);
	
	//============================================================================
	// Post-processing of SLIC segmentation, to avoid stray labels.
	//============================================================================
	void EnforceLabelConnectivity(
		vector <int>&				labels,
		const int					width,
		const int					height,
		double *&					outlabelout,      //input labels that need to be corrected to remove stray labels
		int&						numlabels,        //the number of labels changes in the end if segments are removed
		const int&					K,                //the number of superpixels desired by the user
		const int&                  sz);
	

private:
	int										m_width;
	int										m_height;
	int										m_depth;

	double*									m_lvec;
	double*									m_avec;
	double*									m_bvec;

	double**								m_lvecvec;
	double**								m_avecvec;
	double**								m_bvecvec;
};

#endif // !defined(_SLIC_H_INCLUDED_)