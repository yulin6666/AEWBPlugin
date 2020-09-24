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
/*
	Streamie.cpp
	
	Revision History

	Version		Change																					Engineer	Date
	=======		======																					========	======
		1.0		(from the mists of time)																dmw	
		1.x		Various fixes																			bbb
		1.6		Fix error when opening context menu or Layer menu with camera or light layer selected	zal			4/3/2015

*/

#include "Streamie.h"

static AEGP_PluginID	S_my_id				= 0;

static SPBasicSuite		*sP 				= NULL;

static AEGP_Command		S_Streamie_cmd 		= 0;

		
static A_Err
CheckForEffectStreams(
	AEGP_LayerH			layerH,						/* >> */
	A_Boolean*		has_effect)		/* << */
{
    A_Err 				err 						= A_Err_NONE;
	AEGP_SuiteHandler	suites(sP);
    
	if (layerH){
        
        AEGP_EffectRefH effectPH = NULL;
        ERR(suites.EffectSuite4()->AEGP_GetLayerEffectByIndex(S_my_id,layerH,0,&effectPH));

        if(effectPH){
            
            *has_effect = TRUE;
        }
    }
        
//
	return err;
}		

static A_Err
CommandHook(
	AEGP_GlobalRefcon	plugin_refconPV,		/* >> */
	AEGP_CommandRefcon	refconPV,				/* >> */
	AEGP_Command		command,				/* >> */
	AEGP_HookPriority	hook_priority,			/* >> */
	A_Boolean			already_handledB,		/* >> */
	A_Boolean			*handledPB)				/* << */
{
	A_Err 				err 				= A_Err_NONE, 
						err2 				= A_Err_NONE;

	A_long				num_strokesL	 	= 0,
						new_indexL			= 0;
	
	A_char				nameAC[AEGP_MAX_STREAM_MATCH_NAME_SIZE]  = {'\0'};

	AEGP_LayerH 		layerH						= NULL;

    A_Boolean   has_effect = FALSE;
	AEGP_SuiteHandler	suites(sP);
	
	if (command == S_Streamie_cmd) {
		try {
			ERR(suites.UtilitySuite3()->AEGP_StartUndoGroup(STR(StrID_Name)));
			ERR(suites.LayerSuite5()->AEGP_GetActiveLayer(&layerH));
			if (layerH){
				ERR(CheckForEffectStreams(layerH, &has_effect));
				if (has_effect){
                    AEGP_EffectRefH effectPH = NULL;
                    ERR(suites.EffectSuite4()->AEGP_GetLayerEffectByIndex(S_my_id,layerH,0,&effectPH));

                    if(effectPH){
                        //获取install key
                        AEGP_InstalledEffectKey installed_keyP;
                        ERR(suites.EffectSuite4()->AEGP_GetInstalledKeyFromLayerEffect(effectPH,&installed_keyP));

                        //获取名称
                        A_char nameAC[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                        ERR(suites.EffectSuite4()->AEGP_GetEffectName(installed_keyP,nameAC));

                        //获得参数数量
                        A_long num_param;
                        ERR(suites.StreamSuite5()->AEGP_GetEffectNumParamStreams(effectPH,&num_param));

                        //获得参数名称
                        AEGP_StreamRefH  param_streamH = NULL;
                        ERR(suites.StreamSuite5()->AEGP_GetNewEffectStreamByIndex(S_my_id,effectPH,1,&param_streamH));
                        A_char paramName[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                        ERR(suites.StreamSuite2()->AEGP_GetStreamName(param_streamH,TRUE,paramName));

                        //参数信息
                        AEGP_StreamValue2 value;
                        A_Time timeT = { 0,1 };
                        ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,param_streamH,AEGP_LTimeMode_LayerTime,&timeT,TRUE,&value));
                    }
                    
                }
			}
		} catch (A_Err &thrown_err){
			err = thrown_err;
		}	
		if (err) { // not !err, err!
			ERR2(suites.UtilitySuite3()->AEGP_ReportInfo(S_my_id, STR(StrID_Troubles)));
		}
	}
	return err;
}

static A_Err
UpdateMenuHook(
			   AEGP_GlobalRefcon		plugin_refconPV,	/* >> */
			   AEGP_UpdateMenuRefcon	refconPV,			/* >> */
			   AEGP_WindowType			active_window)		/* >> */
			   {
	A_Err 				err 					=	A_Err_NONE,
	err2					=	A_Err_NONE;
	AEGP_SuiteHandler	suites(sP);
	AEGP_LayerH			layerH 					=	NULL;
    A_Boolean    has_effect = FALSE;
	
	try {
		ERR(suites.LayerSuite5()->AEGP_GetActiveLayer(&layerH));
		
		if (layerH){
			ERR(CheckForEffectStreams(layerH, &has_effect));
			if (has_effect){
				ERR(suites.CommandSuite1()->AEGP_SetMenuCommandName(S_Streamie_cmd, STR(StrID_Name)));
				ERR(suites.CommandSuite1()->AEGP_EnableCommand(S_Streamie_cmd));
			} else	{
				ERR(suites.CommandSuite1()->AEGP_DisableCommand(S_Streamie_cmd));
			}
		} else {
			ERR(suites.CommandSuite1()->AEGP_SetMenuCommandName(S_Streamie_cmd, STR(StrID_CommandName)));
		}
		
	} catch (A_Err &thrown_err){
		err = thrown_err;
	}	
	if (err) { // not !err, err!
		ERR2(suites.UtilitySuite3()->AEGP_ReportInfo(S_my_id, STR(StrID_Troubles)));
	}
	return err;
}
			   
A_Err
EntryPointFunc(
	struct SPBasicSuite		*pica_basicP,		/* >> */
	A_long				 	major_versionL,		/* >> */		
	A_long					minor_versionL,		/* >> */		
	AEGP_PluginID			aegp_plugin_id,		/* >> */
	AEGP_GlobalRefcon		*global_refconP)	/* << */
{
	A_Err 			err					=	A_Err_NONE, 
					err2 				=	A_Err_NONE;
					sP					=	pica_basicP;
					S_my_id				=	aegp_plugin_id;	

	AEGP_SuiteHandler	suites(sP);
	try {
		ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&S_Streamie_cmd));

		ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(	S_Streamie_cmd, 
															STR(StrID_Name), 
															AEGP_Menu_ANIMATION, 
															AEGP_MENU_INSERT_AT_TOP));

		ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(	S_my_id, 
																AEGP_HP_BeforeAE, 
																AEGP_Command_ALL, 
																CommandHook, 
																NULL));

		ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(S_my_id, UpdateMenuHook, NULL));
        
    } catch (A_Err &thrown_err){
		err = thrown_err;
	}	
	if (err) { // not !err, err!
		ERR2(suites.UtilitySuite3()->AEGP_ReportInfo(S_my_id, STR(StrID_Troubles)));
	}
	return err;
}

