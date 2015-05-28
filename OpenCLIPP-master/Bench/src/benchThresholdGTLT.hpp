////////////////////////////////////////////////////////////////////////////////
//! @file	: ThresholdGTLTBench.hpp
//! @date   : Feb 2014
//!
//! @brief  : Creates classes for image thresholding
//! 
//! Copyright (C) 2014 - CRVI
//!
//! This file is part of OpenCLIPP.
//! 
//! OpenCLIPP is free software: you can redistribute it and/or modify
//! it under the terms of the GNU Lesser General Public License version 3
//! as published by the Free Software Foundation.
//! 
//! OpenCLIPP is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//! GNU Lesser General Public License for more details.
//! 
//! You should have received a copy of the GNU Lesser General Public License
//! along with OpenCLIPP.  If not, see <http://www.gnu.org/licenses/>.
//! 
////////////////////////////////////////////////////////////////////////////////

#define THRESHLT 50
#define VALUELOWER 20
#define THRESHGT 100
#define VALUEHIGHER 150

#define USHORT_THRESHLT 5000
#define USHORT_VALUELOWER 4000
#define USHORT_THRESHGT 8000
#define USHORT_VALUEHIGHER 10000

#define FLOAT_THRESHLT -0.2f
#define FLOAT_VALUELOWER -0.5f
#define FLOAT_THRESHGT 0.2f
#define FLOAT_VALUEHIGHER 0.5f

template<typename DataType>
class ThresholdGTLTBench : public BenchUnaryBase<DataType>
{
public:
   void RunIPP();
   void RunNPP();
   void RunCL();

   bool HasCVTest()   const { return false; }
};

//-----------------------------------------------------------------------------------------------------------------------------
template<>
void ThresholdGTLTBench<unsigned char>::RunIPP()
{
   IPP_CODE(
      ippiThreshold_LTValGTVal_8u_C1R( this->m_ImgSrc.Data(), this->m_ImgSrc.Step,
                           this->m_ImgDstIPP.Data(), this->m_ImgDstIPP.Step, 
                           this->m_IPPRoi, 
                           THRESHLT, VALUELOWER, THRESHGT, VALUEHIGHER);
   )
}
template<>
void ThresholdGTLTBench<unsigned short>::RunIPP()
{
   IPP_CODE(
      ippiThreshold_LTValGTVal_16u_C1R( (Ipp16u*) this->m_ImgSrc.Data(), this->m_ImgSrc.Step,
                            (Ipp16u*) this->m_ImgDstIPP.Data(), this->m_ImgDstIPP.Step, 
                            this->m_IPPRoi, 
                            USHORT_THRESHLT, USHORT_VALUELOWER, USHORT_THRESHGT, USHORT_VALUEHIGHER);
   )
}
template<>
void ThresholdGTLTBench<float>::RunIPP()
{
   IPP_CODE(
      ippiThreshold_LTValGTVal_32f_C1R( (Ipp32f*) this->m_ImgSrc.Data(), this->m_ImgSrc.Step,
                            (Ipp32f*) this->m_ImgDstIPP.Data(), this->m_ImgDstIPP.Step, 
                            this->m_IPPRoi, 
                            FLOAT_THRESHLT, FLOAT_VALUELOWER, FLOAT_THRESHGT, FLOAT_VALUEHIGHER);
   )
}
//-----------------------------------------------------------------------------------------------------------------------------
template<typename DataType>
void ThresholdGTLTBench<DataType>::RunCL()
{
   float threshLT = THRESHLT;
   float threshGT = THRESHGT;
   float valueGT  = VALUEHIGHER;
   float valueLT  = VALUELOWER;

   if (is_same<DataType, unsigned short>::value)
   {
      threshLT = USHORT_THRESHLT;
      threshGT = USHORT_THRESHGT;
      valueGT  = USHORT_VALUEHIGHER;
      valueLT  = USHORT_VALUELOWER;
   }

   if (is_same<DataType, float>::value)
   {
      threshLT = FLOAT_THRESHLT;
      threshGT = FLOAT_THRESHGT;
      valueGT  = FLOAT_VALUEHIGHER;
      valueLT  = FLOAT_VALUELOWER;
   }

   ocipThresholdGTLT(this->m_CLSrc, this->m_CLDst, threshLT, valueLT, threshGT, valueGT);
}

//-----------------------------------------------------------------------------------------------------------------------------
template<>
void ThresholdGTLTBench<unsigned char>::RunNPP()
{
   NPP_CODE(
      nppiThreshold_LTValGTVal_8u_C1R( (Npp8u*) this->m_NPPSrc, this->m_NPPSrcStep,
                           (Npp8u*) this->m_NPPDst, this->m_NPPDstStep,
                           this->m_NPPRoi, 
                           THRESHLT, VALUELOWER, THRESHGT, VALUEHIGHER);
   )
}
template<>
void ThresholdGTLTBench<unsigned short>::RunNPP()
{
   NPP_CODE(
      nppiThreshold_LTValGTVal_16u_C1R( (Npp16u*) this->m_NPPSrc, this->m_NPPSrcStep,
                            (Npp16u*) this->m_NPPDst, this->m_NPPDstStep,
                            this->m_NPPRoi, 
                            USHORT_THRESHLT, USHORT_VALUELOWER, USHORT_THRESHGT, USHORT_VALUEHIGHER);
   )
}
template<>
void ThresholdGTLTBench<float>::RunNPP()
{
   NPP_CODE(
      nppiThreshold_LTValGTVal_32f_C1R( (Ipp32f*) this->m_NPPSrc, this->m_NPPSrcStep,
                            (Ipp32f*) this->m_NPPDst, this->m_NPPDstStep,
                            this->m_NPPRoi, 
                            FLOAT_THRESHLT, FLOAT_VALUELOWER, FLOAT_THRESHGT, FLOAT_VALUEHIGHER);
   )
}
