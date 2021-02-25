/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

#include "GLator.h"


typedef struct {
    unsigned long    index;
    char            str[256];
} TableString;


TableString        g_strs[StrID_NUMTYPES] = {
    StrID_NONE,                        "",
    StrID_Amount,                      "scatter amount",
    StrID_Particle,                    "particle",
    StrID_Random,                      "Scattered randomness",
    StrID_Random2,                      "random",
    StrID_Choices,                      "both |"
                                        "level |"
                                        "vertical ｜",

    StrID_Description,                "A very basic OpenGL implementation, within a plug-in.\rCopyright 2007 Adobe Systems Incorporated.",
    StrID_Color_Param_Name,            "Color",
    StrID_Checkbox_Param_Name,        "Use Downsample Factors",
    StrID_Checkbox_Description,        "Correct at all resolutions",
    StrID_DependString1,            "All Dependencies requested.",
    StrID_DependString2,            "Missing Dependencies requested.",
    StrID_Err_LoadSuite,            "Error loading suite.",
    StrID_Err_FreeSuite,            "Error releasing suite.",
    StrID_3D_Param_Name,            "Use lights and cameras",
    StrID_3D_Param_Description,        ""
};


char    *GetStringPtr(int strNum)
{
    return g_strs[strNum].str;
}

    
