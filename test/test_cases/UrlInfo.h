/*
    Copyright (c) 2014, McAfee, Inc.
    
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    
    Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.
    
    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.
    
    Neither the name of McAfee, Inc. nor the names of its contributors may be used
    to endorse or promote products derived from this software without specific prior
    written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef URL_INFO_H
#define URL_INFO_H

#ifdef __cplusplus 
extern "C" {
#endif

#define MAX_CATEGORIES 8

#define URL_0_0 "http://www.testingmcafeesites.com/testrep_red.html"
#define URL_1_0 "http://www.testingmcafeesites.com"
#define URL_2_0 "http://www.targetingnow.com/delivery"
#define URL_3_0 "http://www.testingmcafeesites.com/testrep_red.html"
#define URL_4_0 "http://www.testingmcafeesites.com/testrep_yellow.html"

TWPCategories CATEGORIES_0_0_1[2] =
{
    TWP_Malicioussites,
    TWP_OverallRiskHigh
};

TWPCategories CATEGORIES_0_0_0[2] =
{
    TWP_Pharmacy,
    TWP_Restaurants
};

TWPCategories CATEGORIES_0_0_2[2] =
{
    TWP_Malicioussites,
    TWP_OverallRiskHigh
};

TWPCategories CATEGORIES_0_0_3[1] =
{
    TWP_OverallRiskHigh
};

TWPCategories VIOLATIONS_0_0_2[1] =
{
    TWP_OverallRiskHigh
};

#define SCORE_0_0 127
#define CATEGORY_0_0_0 TWP_Pharmacy
#define CATEGORY_0_0_1 TWP_OverallRiskHigh

#ifdef __cplusplus 
}
#endif

#endif /* URL_INFO_H */

