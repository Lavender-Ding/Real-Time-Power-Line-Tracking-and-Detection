//===========================================================================
//	Saliency_MC.cpp:implementation of the Saliency_MC class.
//  This code implements the Saliency_Absorb_MC 
//	MATLAB code of Saliency detection via absorb Markov chain of ICCV 2013.
//
//  Input:	 
//			 imgI :     the input image 
//           Mask :     the Mask obtained from Saliency_MC
//           box  :     the bounding box
//	Copyright (c) 2014 Shen Tan [SJTU]. All rights reserved.
//	Email: tanshen@sjtu.edu.cn
//	Date: 2014/10/26
//  The code is run on VS2012 + OpenCV2.48
//===========================================================================
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "dataManager.h"

#include "Saliency_MC.h"
#include "time.h"

double thresholdForSal = -1;

Mat Saliency_MC::get_WconFirst(int &k,Mat &ConPix2, double *Supll,double *Supaa,double *Supbb)
{
	int NumInit=0;
	for(int j=0;j<k;j++)
		for(int z=j+1;z<k;z++)
			if(ConPix2.at<double>(j,z)>0)NumInit++;

	Mat Dcol= Mat::zeros(NumInit,3,CV_64F);
	int mm=0;
	double DcolTem;
	for(int j=0;j<k-1;j++)
		for(int z=j+1;z<k;z++)
			if(ConPix2.at<double>(j,z)>0){
				DcolTem = sqrt( ( Supll[j]-Supll[z] )*( Supll[j]-Supll[z] ) 
						+ ( Supaa[j]-Supaa[z] )*( Supaa[j]-Supaa[z] ) 
						+ ( Supbb[j]-Supbb[z] )*( Supbb[j]-Supbb[z]));
				Dcol.at<double>(mm,0)=j;
				Dcol.at<double>(mm,1)=z; 
				Dcol.at<double>(mm,2)=DcolTem;
				++mm;
			}

	Mat DcolNor(NumInit,1,CV_64F);
	//cout<<Dcol.at<double>(2)<<endl;
	Mat Dcol3=Dcol.col(2).clone();
	normalize(Dcol3,DcolNor,1.0,0.0,NORM_MINMAX);
	
	Mat weight(NumInit,1,CV_64F);
	for (int r = 0; r <NumInit ; r++)  
		weight.at<double>(r) = exp( -10*DcolNor.at<double>(r) ) + .00001;

	Mat Dcoltmp1=Dcol.col(0).clone();
	Dcoltmp1.push_back(Dcol.col(1).clone());
	Mat Dcoltmp2=Dcol.col(1).clone();
	Dcoltmp2.push_back(Dcol.col(0).clone());
	Mat weight2=weight.clone();
	weight2.push_back(weight);
	//cout<<weight2.size()<<endl;
	Mat WconFirst=Mat::zeros(k,k,CV_64F);
	//int cnt2=0;
	for(int i=0;i<NumInit*2;i++){
			int m=Dcoltmp1.at<double>(i);
			int n=Dcoltmp2.at<double>(i);
			WconFirst.at<double>(m,n)=WconFirst.at<double>(m,n)+weight2.at<double>(i);
		}
	Mat eyes= Mat::eye(k,k,CV_64F);
	WconFirst=WconFirst+eyes;
	return WconFirst;
}

vector <Mat> Saliency_MC::find_connect_superpixel_DoubleIn_Opposite(int &k,int &height,int &width,Mat &Label)
{
	vector <Mat> Vec_Mat;
	Mat ConPix=Mat::zeros(k,k,CV_64F);
	Mat ConPix0=Mat::zeros(k,k,CV_64F);
	Mat ConPix2=Mat::zeros(k,k,CV_64F);
	for(int i=0;i<height-1;i++){
		for(int j=0;j<width-1;j++)
		{
			if(Label.at<double>(i,j)!=Label.at<double>(i,j+1)) 			
				ConPix.at<double>(Label.at<double>(i,j) ,Label.at<double>(i,j+1))=1;
			if(Label.at<double>(i,j)!=Label.at<double>(i+1,j))
				ConPix.at<double>(Label.at<double>(i,j),Label.at<double>(i+1,j))=1;
		}
		if(Label.at<double>(i,width-1)!=Label.at<double>(i+1,width-1))
			ConPix.at<double>(Label.at<double>(i,width-1),Label.at<double>(i+1,width-1))=1;
	}

	for(int j=0;j<width-1;j++)
		if(Label.at<double>(height-1,j)!=Label.at<double>(height-1,j+1))
			ConPix.at<double>(Label.at<double>(height-1,j),Label.at<double>(height-1,j+1))=1;
	
	for(int i=0;i<height-1;i++)
		for(int j=0;j<width-1;j++)
			if(Label.at<double>(i,j)!=Label.at<double>(i+1,j+1))
				ConPix.at<double>(Label.at<double>(i,j),Label.at<double>(i+1,j+1))=1;

	for(int i=0;i<height-1;i++)
		for(int j=1;j<width-1;j++)
			if(Label.at<double>(i,j)!=Label.at<double>(i+1,j-1))
				ConPix.at<double>(Label.at<double>(i,j),Label.at<double>(i+1,j-1))=1;

	ConPix0=ConPix+ConPix.t();

	for(int j=0;j<width;j++)
		ConPix.at<double>(Label.at<double>(0,j),Label.at<double>(height-1,j))=1;
	for(int i=0;i<height;i++)
		ConPix.at<double>(Label.at<double>(i,0),Label.at<double>(i,width-1))=1;

	ConPix=ConPix+ConPix.t();
	ConPix2=ConPix.clone();
	vector <int> siteline;
	for(int i=0;i<k;i++)
	{
		for(int j=0;j<k;j++)
			if(ConPix.at<double>(i,j)>0)
				siteline.push_back(j);
		for(int kk=0;kk<siteline.size();kk++)
			for(int j=0;j<k;j++)
				ConPix2.at<double>(i,j)= ConPix2.at<double>(i,j)+ConPix.at<double>(siteline[kk],j);
		siteline.clear();		
	}
	Vec_Mat.push_back(ConPix);
	Vec_Mat.push_back(ConPix2);
	return Vec_Mat;
}

Mat Saliency_MC::Find_Edge_Superpixels(Mat &Label, int &k, int &height,int &width , Mat &WconFirst,Mat &ConPix)
{
	double Check=0;
	Mat EdgSup=Mat::zeros(k,1,CV_64F);
	Mat EdgSupSecond(k,1,CV_64F);
	for (int i=0;i<height;i++){
		EdgSup.at<double>(Label.at<double>(i,0)) =1;
		EdgSup.at<double>(Label.at<double>(i,width-1))=1;
		}
	for (int i=0;i<width;i++){
		EdgSup.at<double>(Label.at<double>(0,i)) =1;
		EdgSup.at<double>(Label.at<double>(height-1,i))=1;
		}
	EdgSupSecond = EdgSup.clone();

	for(int j=0;j<k ;j++){
		if(EdgSup.at<double>(j)==1 ){
			for(int z=0;z<k;z++){
				if(ConPix.at<double>(j,z)>0 && EdgSup.at<double>(z)==0){
					Check=Check+WconFirst.at<double>(j,z);
					EdgSupSecond.at<double>(z) = 1; 
				}
			}
			if(Check>13)return EdgSup;
		}
	}
	EdgSup =  EdgSup + EdgSupSecond;
	return EdgSup;
}

//modified here
vector <Mat> Saliency_MC::absorb_MC(int &NumIn,int &NumEdg,int &k,Mat & EdgSup,Mat &EdgWcon, Mat &Discard, Mat &WconFirst)
{
	vector <Mat> Vector_Mat;
	double alph = 1;  
	Mat W=Mat::zeros(k,k,CV_64F);
	Mat BaseEdg=Mat::zeros(NumIn,1,CV_64F);
	Mat Sal(NumIn,1,CV_64F);
	Mat Wcon(NumIn,NumIn,CV_64F);
	Mat D(NumIn,NumIn,CV_64F);
	Mat I = Mat::eye(NumIn,NumIn,CV_64F);
	Mat y = Mat::ones( NumIn, 1,CV_64F);
	Mat N(NumIn,NumIn,CV_64F);
	Mat sumD=Mat::zeros(NumIn,1,CV_64F);
	//modified
	double *dataBaseEdg,*dataEdgWcon,*dataW,*dataWconFirst,*dataWmid,*dataEdgSup;

	if(NumIn==k)
	{
		
		for(int i=0;i<k;i++)
		{
			dataBaseEdg = BaseEdg.ptr<double>(i);
			dataEdgWcon = EdgWcon.ptr<double>(i);
			for(int j=0;j<NumEdg;j++)
			{
				*dataBaseEdg = *dataBaseEdg + *(dataEdgWcon + j);
			}
		}

		D=Mat::diag(Discard + BaseEdg);
		Wcon =  D.inv() * WconFirst;	
		N = I - alph* Wcon;
		Sal = N.inv() * y;
		normalize(Sal,Sal,1.0,0.0,NORM_MINMAX);
	}

	else
	{
		int mm=0;
		for(int j=0;j<k;j++){

			if(EdgSup.at<double>(j)<2){
				for(int i=0;i<NumEdg;i++)
					BaseEdg.at<double>(mm)=BaseEdg.at<double>(mm)
						+EdgWcon.at<double>(j,i);
				sumD.at<double>(mm)=Discard.at<double>(j);
				for(int i=0;i<k;i++)
					W.at<double>(mm,i)=WconFirst.at<double>(j,i);
				mm++;
			}
		}
		mm=0;
		for(int j=0;j<k;j++){
			if(EdgSup.at<double>(j)<2){
				for(int i=0;i<k;i++)
					W.at<double>(i,mm)=W.at<double>(i,j);
				mm++;
			}
		}
		Mat Wmid(NumIn,NumIn,CV_64F);

#pragma omp parallel for
		for(int i=0;i<NumIn;i++)
			for(int j=0;j<NumIn;j++)Wmid.at<double>(i,j)=W.at<double>(i,j);
		D = Mat::diag( BaseEdg + sumD );
		Wmid=D.inv() *Wmid;
		N =I-alph* Wmid;
		Sal = N.inv() * y;
		normalize(Sal,Sal,1.0,0.0,NORM_MINMAX);
	}
	Vector_Mat.push_back(Sal);
	Vector_Mat.push_back(N);
	Vector_Mat.push_back(sumD);
	return Vector_Mat;
}

double *Saliency_MC::sup2pixel(int &PixNum, Mat &Label, Mat &sup)
{
	double *SalLine=new double[PixNum];
	for ( int j = 0; j < PixNum; j++ )
		SalLine[j] = sup.at<double>(Label.at<double>(j)) ;
	return SalLine;
}

Mat Saliency_MC::reshape(double * &labelline,int &height,int &width)
{
	Mat Label(height,width,CV_64F);
	for (int r = 0; r < height; r++)  
	{  
		for (int c = 0; c <width ; c++)  
		{ 
			Label.at<double>(r,c)=labelline[c+r*width];
		} 
	}
	return Label;
}

Saliency_MC::Saliency_MC(IplImage * imgI,IplImage *Mask, double *box, bool &ifBg, double thresh)
{	
	double& x_maxSal = dataManager::getInstance()->d_maxSalEver;
	thresholdForSal = thresh;
	
	//int width0 =imgI->width;
	//int height0= imgI->height;
	////int PixNum=width*height;
	CvSize size= cvSize(box[2],box[3]);//区域大小
	CvRect rect=cvRect(box[0],box[1],size.width,size.height);
	cvSetImageROI(imgI,rect);//设置源图像ROI
	IplImage* imgICut = cvCreateImage(size,imgI->depth,imgI->nChannels);//创建目标图像
	cvCopy(imgI,imgICut); //复制图像	
	cvResetImageROI(imgI);//源图像用完后，清空ROI

	//MeanShiftSmooth(imgICut,imgICut,4,4,30);

	cvSaveImage("Roi.jpg",imgICut);//保存目标图像
	
	int width =imgICut->width;
	int height= imgICut->height;
	int PixNum=width*height;
//=====================================slic=============================================================
	SLIC slic;
	int m=20,k=250;
	double *ImgVecB=new double[PixNum];
	double *ImgVecG=new double[PixNum];
	double *ImgVecR=new double[PixNum];	
	double *outlabel=new double[PixNum];

	int cnt=0;
	for(int i=0;i<height;i++)  
		for(int j=0;j<width;j++){  
			ImgVecB[cnt]=cvGet2D(imgICut,i,j).val[0];  
			ImgVecG[cnt]=cvGet2D(imgICut,i,j).val[1];  
			ImgVecR[cnt]=cvGet2D(imgICut,i,j).val[2];  
			cnt++;
		} 
	
	int numlabels(0);
	slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(ImgVecR, ImgVecG, ImgVecB, width, height, numlabels, k, m, outlabel, PixNum);

	double *Supll=new double[numlabels];
	double *Supaa=new double[numlabels];
	double *Supbb=new double[numlabels];
	k = numlabels;
	//cout<<k<<endl;

	vector <double> meanSuprr( numlabels );
	vector <double> meanSupgg( numlabels );
	vector <double> meanSupbb( numlabels );

	slic.DoMeanSup( ImgVecR, ImgVecG, ImgVecB, numlabels, PixNum, outlabel, meanSuprr, meanSupgg, meanSupbb );	
	
	slic.DoRGBtoLABConversionSup(meanSuprr, meanSupgg, meanSupbb, Supll, Supaa, Supbb, numlabels); 

	Mat Label=reshape(outlabel,height,width);

//==========================find_connect_superpixel_DoubleIn_Opposite========================================================
	vector <Mat> Vec_Mat=find_connect_superpixel_DoubleIn_Opposite(k,height,width,Label);
	Mat ConPix=Vec_Mat[0];
	Mat ConPix2=Vec_Mat[1];

//===================================get_WconFirst===============================================
	

	Mat WconFirst=get_WconFirst(k,ConPix2,Supll,Supaa,Supbb);
	Mat Discard=Mat::zeros(k,1,CV_64F);
	for(int i=0;i<k;i++)		
		for(int j=0;j<k;j++)
			Discard.at<double>(i)=Discard.at<double>(i)+WconFirst.at<double>(i,j);
	

//==================================Find_Edge_Superpixels=================================

	vector <int> DiscardPos;
	for(int i=0;i<k;i++)
		if(Discard.at<double>(i) < 1.1)
			DiscardPos.push_back(i);
	int LenDiscardPos=DiscardPos.size();

	Mat EdgSup=Find_Edge_Superpixels( Label, k,  height, width , WconFirst, ConPix);

	for(int j=0;j<LenDiscardPos;j++)
		EdgSup.at<double>(DiscardPos[j]) = 2;

	int NumIn=0,NumIntmp=0;
	int NumEdg=0;
	for(int j=0;j<k;j++){
		if(EdgSup.at<double>(j) ==2)
			NumIntmp++;
		if(EdgSup.at<double>(j) ==1)
			NumEdg++;
	}
	NumIn=k-NumIntmp;

	Mat EdgWcon= Mat::zeros(k,NumEdg,CV_64F);
	int mm=0;
	for(int j=0;j<k;j++){
		if(EdgSup.at<double>(j)==1){
			for(int i=0;i<k;i++)
				EdgWcon.at<double>(i,mm) = WconFirst.at<double>(i,j);
			mm = mm + 1;
		}
	}


//==============================absorb MC====================================================
	
	//test here
	Mat salOld;

	Mat Sal,N,sumD;
	vector <Mat>Vec_absorb_MC=absorb_MC(NumIn,NumEdg,k,EdgSup,EdgWcon, Discard, WconFirst);
	Sal=Vec_absorb_MC[0];
	N=Vec_absorb_MC[1];
	sumD=Vec_absorb_MC[2];

//=============================entropy decide 2===================================================
	Mat Entro = Mat::zeros(11, 1,CV_64F);
	double entroT;
	double Entropy = 0;
	for(int j=0;j< NumIn;j++){
		entroT =  floor( Sal.at<double>(j) * 10 );
		Entro.at<double>(entroT) = Entro.at<double>(entroT) + 1;
	}
	Entro.at<double>(9) = Entro.at<double>(9) + Entro.at<double>(10);
	Entro = Entro / NumIn;
	for(int j=0;j < 10;j++)
		Entropy = Entropy + Entro.at<double>(j) * min((j+1),(10-j));
	//cout<<Entropy;
//=============================output saliency map from absorb MC==========================================

	Mat SalAll=Mat::zeros(k,1,CV_64F);;
	double *SalLine;
	Mat Salpix;
	Mat rW;
	if(Entropy < 2){
		if(NumIn < k){
			int mm=0;

			//here to notice
			/*double showEleMax = -1000;
			for(int shit = 0; shit < Sal.cols; shit ++){
				double* pp = Sal.ptr<double>(shit);
				for (int fuck = 0; fuck < Sal.rows; fuck ++){
					showEleMax = pp[fuck] > showEleMax ? pp[fuck] : showEleMax;
				}
			}*/


			double maxV = 0;
			if(!drawHistS(Sal, 1.0, maxV, x_maxSal)){ //<0.20
				ifBg = true; 
				return;
			}

			for(int j=0;j<k;j++)
				if(EdgSup.at<double>(j)<2){
					SalAll.at<double>(j)=Sal.at<double>(mm);
					mm++;
				}

			for(int j=0;j<LenDiscardPos;j++)
				for(int z=0;z<k;z++)
					if(ConPix.at<double>(DiscardPos[j], z)>0)
						if(SalAll.at<double>(z)>0.3){
							SalAll.at<double>(DiscardPos[j])=1;
							break;
						}
			SalLine = sup2pixel(PixNum, Label, SalAll );  // to convey the saliency value from super pixel to pixel
			Salpix = reshape(SalLine,height,width);
		}
		else{ //NumIn >= k

			//here to notice
			/*double showEleMax = -1000;
			for(int shit = 0; shit < Sal.cols; shit ++){
				double* pp = Sal.ptr<double>(shit);
				for (int fuck = 0; fuck < Sal.rows; fuck ++){
					showEleMax = pp[fuck] > showEleMax ? pp[fuck] : showEleMax;
				}
			}*/

			double maxV = 0;
			if(!drawHistS(Sal, 1.0, maxV, x_maxSal)){ //<0.20
				ifBg = true; 
				return;
			}

			SalLine = sup2pixel(PixNum, Label, Sal);
			Salpix = reshape(SalLine,height,width);
		}
	}
//===========================equilibrium post-process==================================================
	else{ //===entropy < 2===//

		if (NumIn == k){
			double sumDiscard =0;
			for(int i=0;i<k;i++)
				sumDiscard+= Discard.at<double>(i);
			Mat c = Discard / sumDiscard;
			rW = 1 / c;
			double sumrW = 0;
			for(int i=0;i<k;i++)
				sumrW+= rW.at<double>(i);
			rW = rW / sumrW;
			Sal = N.inv()* rW;

			//here to notice
			/*double showEleMax = -1000;
			for(int shit = 0; shit < Sal.cols; shit ++){
			double* pp = Sal.ptr<double>(shit);
			for (int fuck = 0; fuck < Sal.rows; fuck ++){
			showEleMax = pp[fuck] > showEleMax ? pp[fuck] : showEleMax;
			}
			}*/
			double maxV = 0;
			if(!drawHistS(Sal, 1.0, maxV, x_maxSal)){ //<0.20
				ifBg = true; 
				return;
			}

			normalize(Sal,Sal,1.0,0.0,NORM_MINMAX);
		}
		else{
			double sumsumD = 0;
			for(int i=0;i<NumIn;i++)
				sumsumD+= sumD.at<double>(i);
			Mat c = sumD / sumsumD;
			Mat rW = 1 / c;
			double sumrW = 0;
			for(int i=0;i<NumIn;i++)
				sumrW+= rW.at<double>(i);
			rW = rW / sumrW;
			Sal = N.inv()* rW;

			//here to notice
			//double showEleMax = -1000;
			//for(int shit = 0; shit < Sal.cols; shit ++){
			//	double* pp = Sal.ptr<double>(shit);
			//	for (int fuck = 0; fuck < Sal.rows; fuck ++){
			//		showEleMax = pp[fuck] > showEleMax ? pp[fuck] : showEleMax;
			//	}
			//}
			double maxV = 0;
			if(!drawHistS(Sal, 1.0, maxV, x_maxSal)){ //<0.20
				ifBg = true; 
				return;
			}

			normalize(Sal,Sal,1.0,0.0,NORM_MINMAX);
		}
		if (NumIn < k){
			int mm=0;
			for(int j=0;j<k;j++){
				if(EdgSup.at<double>(j) < 2){
					SalAll.at<double>(j) = Sal.at<double>(mm);
					mm=mm+1;
				}
			}
			for(int j=0;j<LenDiscardPos;j++)       //to descide the saliency of outlier based on neighbour's saliency
				for(int z=0;z<k;z++)
					if(ConPix.at<double>(DiscardPos[j], z) > 0)
						if(SalAll.at<double>(z) >0.3){
							SalAll.at<double>(DiscardPos[j]) = 1;
							break;
						}
			SalLine = sup2pixel(PixNum,Label,SalAll);
			Salpix = reshape(SalLine,height,width);
		}
		else{      //NumIn > k
			SalLine = sup2pixel(PixNum,Label,Sal);
			Salpix = reshape(SalLine,height,width);
		}
	}

	IplImage *SalMap;
	SalMap = &IplImage(Salpix);//Salpix is the Mat form of Saliency_MC result,SalMap is IplImage form.
	
	int t_size = Salpix.elemSize();
	int t_cn = Salpix.channels();
//====================================Mask================================================
	IplImage *BinSalMap=cvCreateImage(cvGetSize(SalMap),IPL_DEPTH_8U,1);
	cvCvtScale(SalMap, BinSalMap, 255, 0);//covert to IPL_DEPTH_8U depth
	//cvSaveImage("test_SalMap.png",BinSalMap);
	//IplImage *SalMap2=cvCreateImage(cvGetSize(SalMap),IPL_DEPTH_8U,1);
	//cvCvtScale(SalMap, SalMap2, 255, 0);
	cvSaveImage("BinSalMap.png",BinSalMap);
	
	cvThreshold(BinSalMap,BinSalMap,222,255,CV_THRESH_OTSU);
	  
	


	IplImage *imgIMask = cvCreateImage(cvGetSize(imgI),IPL_DEPTH_8U,1);
	cvZero(imgIMask); 
	cvSetImageROI(imgIMask,rect);
	cvCopy(BinSalMap,imgIMask);
	cvResetImageROI(imgIMask);//源图像用完后，清空ROI
	//cvSaveImage("test_mask.png",imgIMask);

	cvCopy(imgIMask,Mask);
	
	/*cvShowImage("shit", Mask);
	cvWaitKey(0);*/

	cvReleaseImage(&BinSalMap);
	cvReleaseImage(&imgIMask);
	cvReleaseImage(&imgICut);
	//system("pause");
	//return 0;  
}

//IplImage Saliency_MC::*GetMask(IplImage * imgI,double *box)
//{
//	
//	Saliency_MC Saliency_MC(IplImage * imgI,double *box);
//	IplImage * Mask=imgMask;
//	return Mask;
//}
void myTest(Mat& src, Mat mask);

//int main(int argc,char *argv[])
//{	
//
//	//Saliency_MC Saliency_MC;
//	
//
//	
//	
//	char filename[50];
//	
//	sprintf(filename,"weird1.jpg");	
//	//sprintf(filename,"../seqs/seq6/frame0%d.jpg",i);
//	//sprintf(filename,"demo.jpg");
//	IplImage * imgI = cvLoadImage(filename);
//	Mat shit(imgI, true), ass;
//	GaussianBlur(shit, shit, Size(3,3),0,0);
//	/*Canny(shit,ass,50, 100);
//	namedWindow("edge");
//	imshow("edge",ass);
//	waitKey(0);*/
//
//	Scalar color = shit.at<Vec3b>(30,10);
//
//	/*floodFill(shit,Point(10,30),color);
//	namedWindow("floodfill");
//	imshow("floodfill",shit);
//	waitKey(0);*/
//	//pyrMeanShiftFiltering(shit, shit, 10, 10);
//
//
//	IplImage imgI1 = IplImage(shit);
//
//	imgI = &imgI1;
//	
//	/*
//	Mat srcImg(imgI,1),imgUsed;
//	pyrMeanShiftFiltering(srcImg,imgUsed,20,20);
//	Mat edges;
//	Canny(imgUsed,edges,50,100,3);
//	vector<Vec4i> lines;
//	HoughLinesP(edges,lines,1,CV_PI/180,50,100,5);
//	for(int i=0;i<lines.size();i++)
//	line(edges,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(0,0,0),2);
//	namedWindow("lines",1);
//	namedWindow("shitTest",1);
//	namedWindow("shitTest1",1);
//	imshow("shitTest",srcImg);
//	imshow("shitTest1",imgUsed);
//	imshow("lines",edges);
//	waitKey(0);
//	IplImage imgBoutToUsed=imgUsed;
//	IplImage *pimgI=&imgBoutToUsed;
//	*/
//	double *box=new double[4];
//	//box[0]=431;box[1]=173;box[2]=54;box[3]=108;//431 173 54 108
//	//box[0]=526;box[1]=176;box[2]=37;box[3]=84;//526 176 37 84------1576
//	//box[0]=547;box[1]=158;box[2]=22;box[3]=86;//77 143 137 226-----0826
//	//box[0]=348;box[1]=149;box[2]=51;box[3]=101;//348 149 51 101-----1741
//	//box[0]=77;box[1]=143;box[2]=137;box[3]=226;//77 143 137 226----0436
//	
//	box[0]=15;box[1]=3;box[3]=imgI->height-3;box[2]=imgI->width-15;
//	IplImage *Mask= cvCreateImage(cvGetSize(imgI),IPL_DEPTH_8U,1);
//	bool bullshit;
//	Saliency_MC Saliency_MC(imgI,Mask,box,bullshit,0.1);	
//	//Saliency_MC.MeanShiftSmooth2(Mask,Mask,8,8,25);
//	//cvDilate(Mask,Mask, NULL,1);
//	//cvNamedWindow("Mask");
//	//cvShowImage("Mask",Mask);
//	cvSaveImage("frame01741_Mask1.png", Mask);
//	Mat m_Mask(Mask,0);
//	
//	
//
//	//myTest(dst,m_Mask);
//	imshow("shit",m_Mask);
//	waitKey(0);
//	cvReleaseImage(&Mask);
//	
//	
//	//system("pause");
//	return 0;
//
//}



/////////////////////////////////////////////////////////////////////////
///////==========================================================////////
/////////////////////////////////////////////////////////////////////////

Mat SaliencyPro(Mat& src, double thresh)
{

	//debugShow1(src, 0);

	


	//compensate
	Scalar bgColor = dataManager::getInstance()->d_bgColor;
	Vec3b bgColorVec = Vec3b((uchar)bgColor[0], (uchar)bgColor[1], (uchar)bgColor[2]);
	Mat srcMask = checkcolor1(&IplImage(src));
	int maskSize = 18000;//300*60
	uchar* pMask = srcMask.ptr<uchar>(0);
	Vec3b* pSrc = src.ptr<Vec3b>(0);

	CV_Assert(srcMask.isContinuous() && src.isContinuous());
	for(int i=0;i<maskSize;++i)
	{
		if(pMask[i*srcMask.channels()] != 0)
			pSrc[i] = bgColorVec;
	}

	copyMakeBorder(src, src, 5, 0, 0, 0, BORDER_CONSTANT, bgColor);
	GaussianBlur(src, src, Size(3,3), 0);
	
	

	IplImage shit = IplImage(src);
	IplImage* doge;
	doge = &shit;

	double *box = new double[4];

	box[0]=0;
	box[1]=0;
	box[3]=doge->height-1;
	box[2]=doge->width-1;

	IplImage *Mask= cvCreateImage(cvGetSize(doge),IPL_DEPTH_8U,1);

	bool ifBg = false;
	
	

	Saliency_MC Saliency_MC(doge, Mask, box, ifBg, thresh);
	if(ifBg)
		return Mat();
		//Saliency_MC.MeanShiftSmooth2(Mask,Mask,8,8,25);
		//cvDilate(Mask,Mask, NULL,1);
		//cvNamedWindow("Mask");
		//cvShowImage("Mask",Mask);
	cvSaveImage("frame01741_Mask1.png", Mask);
	Mat m_Mask(Mask, true);
	//debugShow1(m_Mask, 0);
	cvReleaseImage(&Mask);
	
	return m_Mask;
}



void myTest(Mat& src, Mat mask)
{
	Scalar colorSum = mean(src, mask);
	
	Scalar fuck = sum(mask);
	threshold(mask, mask, 125, 255, THRESH_BINARY_INV);

	imshow("shit",mask);
	waitKey(0);
	
	Mat adder(Size(src.cols, src.rows),CV_8UC3,colorSum);
	cv::add(adder, src, src, mask);

}

