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
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;
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
        
        A_long effect_num = 0;
        ERR(suites.EffectSuite4()->AEGP_GetLayerNumEffects(layerH,&effect_num));

        
        
        if(effect_num > 0){
            
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
                rapidjson::Document document;
                std::string str1 = "{}"; // 这个是必须的，且不能为""，否则Parse出错
                document.Parse(str1.c_str());

//     -------------------------------------------------------------videoResolution，videoFps，duration信息-------------------------------------------------------------------
                //获取整个comp时间轴
                AEGP_CompH compPH;
                ERR(suites.LayerSuite5()->AEGP_GetLayerParentComp(layerH,&compPH));
                //获取comp的layer数目
                A_long LayerNum;
                ERR(suites.LayerSuite5()->AEGP_GetCompNumLayers(compPH,&LayerNum));
                //获取根目录的合成信息
                AEGP_ItemH  rootItemPH;
                ERR(suites.CompSuite11()->AEGP_GetItemFromComp(compPH,&rootItemPH));
                if(rootItemPH){
                    rapidjson::Value videoResolution(rapidjson::kObjectType);
                    //获取画布宽高
                    A_long width;
                    A_long height;
                    ERR(suites.ItemSuite5()->AEGP_GetItemDimensions(rootItemPH,&width,&height));
                    videoResolution.AddMember("imageHeight", height, document.GetAllocator());
                    videoResolution.AddMember("imageWidth", width, document.GetAllocator());
                    //获取画布像素比
                    A_Ratio pixelRatio;
                    ERR(suites.ItemSuite5()->AEGP_GetItemPixelAspectRatio(rootItemPH,&pixelRatio));
                    rapidjson::Value imagePAR(rapidjson::kObjectType);
                    imagePAR.AddMember("den", pixelRatio.den, document.GetAllocator());
                    imagePAR.AddMember("num", pixelRatio.num, document.GetAllocator());
                    videoResolution.AddMember("imagePAR", imagePAR, document.GetAllocator());
                    document.AddMember("videoResolution", videoResolution, document.GetAllocator());
                    //获取画布的FPS
                    A_FpLong fps;
                    ERR(suites.CompSuite5()->AEGP_GetCompFramerate(compPH,&fps));
                    rapidjson::Value videoFps(rapidjson::kObjectType);
                    videoFps.AddMember("den", 1, document.GetAllocator());
                    videoFps.AddMember("num", fps, document.GetAllocator());
                    document.AddMember("videoFps", videoFps, document.GetAllocator());

                    //获取duration信息
                    A_Time dTime = { 0,1 };
                    ERR(suites.ItemSuite5()->AEGP_GetItemDuration(rootItemPH,&dTime));
                    double duration = (double)(dTime.value*1000/dTime.scale)*1000;
                    document.AddMember("duration", duration, document.GetAllocator());
                }
                rapidjson::Value videoTrackArray(rapidjson::kArrayType);
                rapidjson::Value videoClipArray(rapidjson::kArrayType);
                rapidjson::Value audioTrackArray(rapidjson::kArrayType);
                rapidjson::Value audioClipArray(rapidjson::kArrayType);
                rapidjson::Value timeLineFxs(rapidjson::kArrayType);

                //处理层信息
                for(int i= 0;i<LayerNum;i++){
                    AEGP_LayerH layerH;
                    ERR(suites.LayerSuite5()->AEGP_GetCompLayerByIndex(compPH,i,&layerH));
                    
                    AEGP_LayerFlags layerflags;
                    ERR(suites.LayerSuite5()->AEGP_GetLayerFlags(layerH,&layerflags));
                    
//     -------------------------------------------------------------timelineFXS信息-------------------------------------------------------------------
                    if(layerflags & AEGP_LayerFlag_ADJUSTMENT_LAYER){
                       A_long effect_num;
                       ERR(suites.EffectSuite4()->AEGP_GetLayerNumEffects(layerH,&effect_num));
                       if(effect_num > 0){
                           for(int i = 0;i<effect_num;i++){
                               //每个效果都是一个timelineFx
                               rapidjson::Value timeLineFx(rapidjson::kObjectType);
                               //获取时间轴的inpoint
                               A_Time dTime = { 0,1 };
                               ERR(suites.LayerSuite5()->AEGP_GetLayerDuration(layerH,AEGP_LTimeMode_CompTime,&dTime));
                               double duration = (double)(dTime.value*1000/dTime.scale)*1000;
                               A_Time iTime = { 0,1 };
                               ERR(suites.LayerSuite5()->AEGP_GetLayerInPoint(layerH,AEGP_LTimeMode_CompTime,&iTime));
                               double inPoint = (double)(iTime.value*1000/iTime.scale)*1000;
                               timeLineFx.AddMember("inPoint", inPoint, document.GetAllocator());
                               //获得时间轴的outPoint
                               float outPoint = inPoint + duration;
                               timeLineFx.AddMember("outPoint", outPoint, document.GetAllocator());
                               AEGP_EffectRefH effectPH = NULL;
                               ERR(suites.EffectSuite4()->AEGP_GetLayerEffectByIndex(S_my_id,layerH,i,&effectPH));
                               AEGP_InstalledEffectKey installed_keyP;
                               ERR(suites.EffectSuite4()->AEGP_GetInstalledKeyFromLayerEffect(effectPH,&installed_keyP));
                               A_char nameAC[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                               ERR(suites.EffectSuite4()->AEGP_GetEffectName(installed_keyP,nameAC));
                               if(nameAC[0]=='W' && nameAC[1]=='B'){//微博自研效果
                                   //滤镜名
                                   timeLineFx.AddMember("videoFxPath",rapidjson::Value(nameAC, document.GetAllocator()).Move(), document.GetAllocator());
                                   //滤镜类型
                                   timeLineFx.AddMember("timelineVideoFxType",2, document.GetAllocator());
                                   A_long num_param;
                                   ERR(suites.StreamSuite5()->AEGP_GetEffectNumParamStreams(effectPH,&num_param));
                                   rapidjson::Value properties(rapidjson::kObjectType);
                                   for(int j=1;j<num_param;j++){
                                       //获得参数流
                                       AEGP_StreamRefH  param_streamH = NULL;
                                       ERR(suites.StreamSuite5()->AEGP_GetNewEffectStreamByIndex(S_my_id,effectPH,j,&param_streamH));
                                       A_char paramName[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                                       ERR(suites.StreamSuite2()->AEGP_GetStreamName(param_streamH,TRUE,paramName));
                                       std::string sParamName(paramName);
                                       //参数value
                                       AEGP_StreamValue2 value;
                                       A_Time timeT = { 0,1 };
                                       ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,param_streamH,AEGP_LTimeMode_LayerTime,&timeT,TRUE,&value));
                                       properties.AddMember(rapidjson::Value(paramName, document.GetAllocator()).Move(),value.val.one_d,document.GetAllocator());
                                   }
                                   timeLineFx.AddMember("properties",properties, document.GetAllocator());
                                }
                               timeLineFxs.PushBack(timeLineFx, document.GetAllocator());
                           }
                       }
                    }else{
//     -------------------------------------------------------------clip信息-------------------------------------------------------------------

                       rapidjson::Value clip(rapidjson::kObjectType);
                       //name信息
                       A_char name[100]={"\0"};
                       ERR(suites.LayerSuite5()->AEGP_GetLayerName(layerH,NULL,name));
                       std::string sourceName(name);
                       clip.AddMember("filePath",rapidjson::Value(sourceName.c_str(), document.GetAllocator()).Move(), document.GetAllocator());
                       //获取时间轴的inpoint
                       A_Time dTime = { 0,1 };
                       ERR(suites.LayerSuite5()->AEGP_GetLayerDuration(layerH,AEGP_LTimeMode_CompTime,&dTime));
                       double duration = (double)(dTime.value*1000/dTime.scale)*1000;
                       A_Time iTime = { 0,1 };
                       ERR(suites.LayerSuite5()->AEGP_GetLayerInPoint(layerH,AEGP_LTimeMode_CompTime,&iTime));
                       double inPoint = (double)(iTime.value*1000/iTime.scale)*1000;
                       clip.AddMember("inPoint", inPoint, document.GetAllocator());
                       //获得时间轴的outPoint
                       float outPoint = inPoint + duration;
                       clip.AddMember("outPoint", outPoint, document.GetAllocator());
                       //speed = 1
                       clip.AddMember("speed", 1, document.GetAllocator());
                       //获取trimin信息
                       A_Time triTime = { 0,1 };
                       ERR(suites.LayerSuite5()->AEGP_GetLayerInPoint(layerH,AEGP_LTimeMode_LayerTime,&triTime));
                       double trimIn = triTime.value/triTime.scale;
                       clip.AddMember("trimIn", trimIn, document.GetAllocator());
                       //获取trimout信息
                       double trimOut = trimIn +duration;
                       clip.AddMember("trimOut", trimOut, document.GetAllocator());
                       AEGP_ItemH sourceItem;
                       ERR(suites.LayerSuite5()->AEGP_GetLayerSourceItem(layerH,&sourceItem));
                       AEGP_ItemFlags flags;
                       ERR(suites.ItemSuite5()->AEGP_GetItemFlags(sourceItem,&flags));
                       if(flags & AEGP_ItemFlag_HAS_VIDEO){//图片
                           //     -------------------------------------------------------------video信息-------------------------------------------------------------------
                           //videoType
                           clip.AddMember("videoType", 1, document.GetAllocator());
                           //获取宽高信息
                           A_long width;
                           A_long height;
                           ERR(suites.ItemSuite5()->AEGP_GetItemDimensions(sourceItem,&width,&height));
                           clip.AddMember("imageHeight", height, document.GetAllocator());
                           clip.AddMember("imageWidth", width, document.GetAllocator());
                           //获取像素比
                           A_Ratio pixelRatio;
                           ERR(suites.ItemSuite5()->AEGP_GetItemPixelAspectRatio(sourceItem,&pixelRatio));
                           rapidjson::Value imagePAR(rapidjson::kObjectType);
                           imagePAR.AddMember("den", pixelRatio.den, document.GetAllocator());
                           imagePAR.AddMember("num", pixelRatio.num, document.GetAllocator());
                           clip.AddMember("imagePAR", imagePAR, document.GetAllocator());
                           //特效信息
                           A_long effect_num;
                           ERR(suites.EffectSuite4()->AEGP_GetLayerNumEffects(layerH,&effect_num));
                           rapidjson::Value fxArray(rapidjson::kArrayType);
                           rapidjson::Value TransformArray(rapidjson::kArrayType);
                           for(int i = 0;i<effect_num;i++){
                                   AEGP_EffectRefH effectPH = NULL;
                                   ERR(suites.EffectSuite4()->AEGP_GetLayerEffectByIndex(S_my_id,layerH,i,&effectPH));
                                   
                                   //获取install key
                                   AEGP_InstalledEffectKey installed_keyP;
                                   ERR(suites.EffectSuite4()->AEGP_GetInstalledKeyFromLayerEffect(effectPH,&installed_keyP));
                                   //效果name
                                   A_char nameAC[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                                   ERR(suites.EffectSuite4()->AEGP_GetEffectName(installed_keyP,nameAC));
                                   //特效类型
                                   A_char category[AEGP_MAX_EFFECT_NAME_SIZE]={'\0'};
                                   ERR(suites.EffectSuite4()->AEGP_GetEffectCategory(installed_keyP,category));
                                   string sCategory(category);
                                   if(sCategory =="WBTransition"){
                                      //     -------------------------------------------------------------特效转场-------------------------------------------------------------------
                                      rapidjson::Value transform(rapidjson::kObjectType);
                                      transform.AddMember("fxPath",rapidjson::Value(nameAC, document.GetAllocator()).Move(), document.GetAllocator());
                                      transform.AddMember("videoTransitionType",2, document.GetAllocator());
                                      //duration信息
                                      AEGP_StreamRefH  param_streamH = NULL;
                                      ERR(suites.StreamSuite5()->AEGP_GetNewEffectStreamByIndex(S_my_id,effectPH,1,&param_streamH));
                                      A_long num_keyFrame;
                                      ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(param_streamH,&num_keyFrame));
                                      A_Time startTime = { 0,1 };
                                       A_Time endTime = { 0,1 };
                                      if(num_keyFrame == 2){
                                          ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(param_streamH,0,AEGP_LTimeMode_CompTime,&startTime));
                                          ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(param_streamH,1,AEGP_LTimeMode_CompTime,&endTime));
                                          double duration = (double)((endTime.value-startTime.value)*1000/startTime.scale)*1000;
                                          transform.AddMember("duration", duration, document.GetAllocator());
                                      }
                                      TransformArray.PushBack(transform, document.GetAllocator());
                                   }else{
                                        //     -------------------------------------------------------------特效滤镜-------------------------------------------------------------------
                                       if(nameAC[0]=='W' && nameAC[1]=='B'){//微博自研效果
                                           rapidjson::Value fx(rapidjson::kObjectType);
                                           fx.AddMember("videoFxPath",rapidjson::Value(nameAC, document.GetAllocator()).Move(), document.GetAllocator());
                                           fx.AddMember("timelineVideoFxType",2, document.GetAllocator());
                                           //效果参数
                                           rapidjson::Value properties(rapidjson::kObjectType);
                                            A_long num_param;
                                            ERR(suites.StreamSuite5()->AEGP_GetEffectNumParamStreams(effectPH,&num_param));
                                            for(int j=1;j<num_param;j++){
                                                //获得参数流
                                                AEGP_StreamRefH  param_streamH = NULL;
                                                ERR(suites.StreamSuite5()->AEGP_GetNewEffectStreamByIndex(S_my_id,effectPH,j,&param_streamH));
                                                A_char paramName[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                                                ERR(suites.StreamSuite2()->AEGP_GetStreamName(param_streamH,TRUE,paramName));
                                                std::string sParamName(paramName);
                                                //参数value
                                                AEGP_StreamValue2 value;
                                                A_Time timeT = { 0,1 };
                                                ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,param_streamH,AEGP_LTimeMode_LayerTime,&timeT,TRUE,&value));
                                                properties.AddMember(rapidjson::Value(paramName,document.GetAllocator()).Move(),value.val.one_d,document.GetAllocator());
                                            }
                                           fx.AddMember("properties",properties, document.GetAllocator());
                                           fxArray.PushBack(fx, document.GetAllocator());
                                       }
                                   }
                               }
                          if(fxArray.Size() > 0)
                            clip.AddMember("fxs", fxArray, document.GetAllocator());
                          if(TransformArray.Size()>0)
                            clip.AddMember("transitionFxs", TransformArray, document.GetAllocator());
                          videoClipArray.PushBack(clip, document.GetAllocator());
                       }else if(flags & AEGP_ItemFlag_HAS_AUDIO){//音频
                           //videoType
                           clip.AddMember("videoType", 0, document.GetAllocator());
                           audioClipArray.PushBack(clip, document.GetAllocator());
                       }
                    }
                }
                
                if(videoClipArray.Size() > 0){
                    rapidjson::Value videoTrack(rapidjson::kObjectType);
                    videoTrack.AddMember("clips", videoClipArray, document.GetAllocator());
                    videoTrackArray.PushBack(videoTrack, document.GetAllocator());
                    document.AddMember("videoTracks", videoTrackArray, document.GetAllocator());
                }
               
                if(audioClipArray.Size()>0){
                    rapidjson::Value audioTrack(rapidjson::kObjectType);
                    audioTrack.AddMember("clips", audioClipArray, document.GetAllocator());
                    audioTrackArray.PushBack(audioTrack, document.GetAllocator());
                    document.AddMember("audioTracks", audioTrackArray, document.GetAllocator());
                }
                
                if(timeLineFxs.Size()>0){
                    document.AddMember("timelineVideoFxs", timeLineFxs, document.GetAllocator());
                }
                
               

                rapidjson::StringBuffer buffer1;
                rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
                document.Accept(writer1);
                printf("%s\n", buffer1.GetString());

                std::string json (buffer1.GetString(), buffer1.GetSize());
                std::ofstream of("/tmp/export.json",ios::out);
                of << json;
                if (!of.good()) throw std::runtime_error ("Can't write the JSON string to the file!");
            
            
            
            

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
            has_effect = TRUE;
            //ERR(CheckForEffectStreams(layerH, &has_effect));
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
															AEGP_Menu_FILE,
															AEGP_MENU_INSERT_AT_BOTTOM));

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
