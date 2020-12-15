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
#include <stdio.h>
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
                double totalDuration;

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
                    totalDuration = (double)(dTime.value*1000/dTime.scale)*1000;
                    document.AddMember("duration", totalDuration, document.GetAllocator());
                }

                rapidjson::Value videoTrackArray(rapidjson::kArrayType);
                rapidjson::Value audioTrackArray(rapidjson::kArrayType);
                rapidjson::Value timeLineFxs(rapidjson::kArrayType);

                //处理层信息
                for(int i= 0;i<LayerNum;i++){
                    rapidjson::Value videoClipArray(rapidjson::kArrayType);
                    rapidjson::Value audioClipArray(rapidjson::kArrayType);
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
                               A_char nameMatchAC[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                               ERR(suites.EffectSuite4()->AEGP_GetEffectMatchName(installed_keyP,nameMatchAC));
                               if(nameAC[0]=='W' && nameAC[1]=='B'){//微博自研效果
                                   //滤镜名
                                   A_char packageName[AEGP_MAX_EFFECT_NAME_SIZE]="\0";
                                   strncpy(packageName,nameMatchAC+5,AEGP_MAX_EFFECT_NAME_SIZE-5);
                                   timeLineFx.AddMember("videoFxPath",rapidjson::Value(packageName, document.GetAllocator()).Move(), document.GetAllocator());
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
                       A_char name[900]={"\0"};
                       ERR(suites.LayerSuite5()->AEGP_GetLayerName(layerH,NULL,name));
                       std::string sourceName(name);
                       clip.AddMember("filePath",rapidjson::Value(sourceName.c_str(), document.GetAllocator()).Move(), document.GetAllocator());
                       //获取时间轴的inpoint
                       A_Time dTime = { 0,1 };
                       ERR(suites.LayerSuite5()->AEGP_GetLayerDuration(layerH,AEGP_LTimeMode_CompTime,&dTime));
                       double duration = (double)(dTime.value*1000/dTime.scale)*1000;
                       if(duration > totalDuration)
                           duration = totalDuration;
                       A_Time iTime = { 0,1 };
                       ERR(suites.LayerSuite5()->AEGP_GetLayerInPoint(layerH,AEGP_LTimeMode_CompTime,&iTime));
                        double inPoint;
                        if(iTime.value < 0){//超过左边边境
                            inPoint = 0;
                        }else{
                            inPoint = (double)(iTime.value*1000/iTime.scale)*1000;
                        }
                       clip.AddMember("inPoint", inPoint, document.GetAllocator());
                       //获得时间轴的outPoint
                       float outPoint = inPoint + duration;
                        if(outPoint > totalDuration){//超过右边边界
                            outPoint = totalDuration;
                        }
                       clip.AddMember("outPoint", outPoint, document.GetAllocator());
                       //speed = 1
                       clip.AddMember("speed", 1, document.GetAllocator());
                       //获取trimin信息
                        double trimIn;
                       if(iTime.value < 0){//超过左边边界
                           trimIn = (double)((1-iTime.value)*1000/iTime.scale)*1000;
                       }else{
                           trimIn = 0;
                       }
                       clip.AddMember("trimIn", trimIn, document.GetAllocator());
                       //获取trimout信息
                       float trimOut = trimIn + duration;
                       clip.AddMember("trimOut", trimOut, document.GetAllocator());
                       AEGP_ItemH sourceItem = NULL;
                       ERR(suites.LayerSuite5()->AEGP_GetLayerSourceItem(layerH,&sourceItem));
                       if(sourceItem){
                       AEGP_ItemFlags flags;
                       ERR(suites.ItemSuite5()->AEGP_GetItemFlags(sourceItem,&flags));
                       if(flags & AEGP_ItemFlag_HAS_VIDEO){//视频
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
                           //视频遮罩信息
                           AEGP_LayerTransferMode mode;
                           ERR(suites.LayerSuite8()->AEGP_GetLayerTransferMode(layerH,&mode));
                           int trackMatte = mode.track_matte;
                           clip.AddMember("trackMatte", trackMatte, document.GetAllocator());
                           //transform信息
                           rapidjson::Value transformInfo(rapidjson::kObjectType);
                           rapidjson::Value anchor(rapidjson::kObjectType);
                           rapidjson::Value anchorKeyFrameArray(rapidjson::kArrayType);
                           AEGP_StreamRefH anchorS = NULL;
                           ERR(suites.StreamSuite5()->AEGP_GetNewLayerStream(S_my_id,layerH,AEGP_LayerStream_ANCHORPOINT,&anchorS));
                           A_long num_anchorkeyFrame;
                           ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(anchorS,&num_anchorkeyFrame));
                           rapidjson::Value anchorDeFault(rapidjson::kObjectType);
                           AEGP_StreamValue2 defaultValue;
                           if(num_anchorkeyFrame == 0){
                               A_Time timeT = { 0,1 };
                               ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,anchorS,AEGP_LTimeMode_LayerTime,&timeT,TRUE,&defaultValue));
                               anchorDeFault.AddMember("pointX", defaultValue.val.two_d.x, document.GetAllocator());
                               anchorDeFault.AddMember("pointY", defaultValue.val.two_d.y, document.GetAllocator());
                           }else{
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,anchorS,0,&value));
                               anchorDeFault.AddMember("pointX", value.val.two_d.x, document.GetAllocator());
                               anchorDeFault.AddMember("pointY", value.val.two_d.y, document.GetAllocator());
                           }
                           anchor.AddMember("defaultValue", anchorDeFault, document.GetAllocator());
                           for(int i=0;i<num_anchorkeyFrame;i++){
                               rapidjson::Value anchorKeyFrame(rapidjson::kObjectType);
                               A_Time keyFrameTime = { 0,1 };
                               ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(anchorS,i,AEGP_LTimeMode_CompTime,&keyFrameTime));
                               double time = (double)(keyFrameTime.value*1000/keyFrameTime.scale)*1000;
                               anchorKeyFrame.AddMember("time",time, document.GetAllocator());
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,anchorS,i,&value));
                               anchorKeyFrame.AddMember("pointX", value.val.two_d.x, document.GetAllocator());
                               anchorKeyFrame.AddMember("pointY", value.val.two_d.y, document.GetAllocator());
                               anchorKeyFrameArray.PushBack(anchorKeyFrame, document.GetAllocator());
                           }
                           anchor.AddMember("keyFrame", anchorKeyFrameArray, document.GetAllocator());
                           transformInfo.AddMember("anchor", anchor, document.GetAllocator());
                           
                           rapidjson::Value position(rapidjson::kObjectType);
                           rapidjson::Value positionKeyFrameArray(rapidjson::kArrayType);
                           AEGP_StreamRefH positionS = NULL;
                           ERR(suites.StreamSuite5()->AEGP_GetNewLayerStream(S_my_id,layerH,AEGP_LayerStream_POSITION,&positionS));
                           A_long num_positionKeyFrame;
                           ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(positionS,&num_positionKeyFrame));
                           rapidjson::Value positionDeFault(rapidjson::kObjectType);
                           AEGP_StreamValue2 pdefaultValue;
                           if(num_positionKeyFrame == 0){
                               A_Time ptimeT = { 0,1 };
                               ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,positionS,AEGP_LTimeMode_LayerTime,&ptimeT,TRUE,&pdefaultValue));
                               positionDeFault.AddMember("translateX", pdefaultValue.val.two_d.x, document.GetAllocator());
                               positionDeFault.AddMember("translateY", pdefaultValue.val.two_d.y, document.GetAllocator());
                           }else{
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,positionS,0,&value));
                               positionDeFault.AddMember("translateX", value.val.two_d.x, document.GetAllocator());
                               positionDeFault.AddMember("translateY", value.val.two_d.y, document.GetAllocator());
                           }
                           position.AddMember("defaultValue", positionDeFault, document.GetAllocator());
                           for(int i=0;i<num_positionKeyFrame;i++){
                               rapidjson::Value positionKeyFrame(rapidjson::kObjectType);
                               A_Time keyFrameTime = { 0,1 };
                               ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(positionS,i,AEGP_LTimeMode_CompTime,&keyFrameTime));
                               double time = (double)(keyFrameTime.value*1000/keyFrameTime.scale)*1000;
                               positionKeyFrame.AddMember("time",time, document.GetAllocator());
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,positionS,i,&value));
                               positionKeyFrame.AddMember("translateX", value.val.two_d.x, document.GetAllocator());
                               positionKeyFrame.AddMember("translateY", value.val.two_d.y, document.GetAllocator());
                               A_short dim;
                               ERR(suites.KeyframeSuite4()->AEGP_GetStreamTemporalDimensionality(positionS,&dim));
                               AEGP_KeyframeEase inEase;
                               AEGP_KeyframeEase outEase;
                               ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTemporalEase(positionS,i,dim-1,&inEase,&outEase));
                               positionKeyFrame.AddMember("inEase_speed", inEase.speedF, document.GetAllocator());
                               positionKeyFrame.AddMember("inEase_influenceF", inEase.influenceF, document.GetAllocator());
                               positionKeyFrame.AddMember("outEase_speed", outEase.speedF, document.GetAllocator());
                               positionKeyFrame.AddMember("outEase_influenceF", outEase.influenceF, document.GetAllocator());
                               positionKeyFrameArray.PushBack(positionKeyFrame, document.GetAllocator());
                           }
                           position.AddMember("keyFrame", positionKeyFrameArray, document.GetAllocator());
                           transformInfo.AddMember("translate", position, document.GetAllocator());
                           
                           rapidjson::Value scale(rapidjson::kObjectType);
                           rapidjson::Value scaleKeyFrameArray(rapidjson::kArrayType);
                           AEGP_StreamRefH scaleS = NULL;
                           ERR(suites.StreamSuite5()->AEGP_GetNewLayerStream(S_my_id,layerH,AEGP_LayerStream_SCALE,&scaleS));
                           A_long num_scaleKeyFrame;
                           ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(scaleS,&num_scaleKeyFrame));
                           rapidjson::Value scaleDeFault(rapidjson::kObjectType);
                           AEGP_StreamValue2 sdefaultValue;
                           if(num_scaleKeyFrame == 0){
                               A_Time stimeT = { 0,1 };
                               ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,scaleS,AEGP_LTimeMode_LayerTime,&stimeT,TRUE,&sdefaultValue));
                               scaleDeFault.AddMember("scaleX", sdefaultValue.val.two_d.x, document.GetAllocator());
                               scaleDeFault.AddMember("scaleY", sdefaultValue.val.two_d.y, document.GetAllocator());
                           }else{
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,scaleS,0,&value));
                               scaleDeFault.AddMember("scaleX", value.val.two_d.x, document.GetAllocator());
                               scaleDeFault.AddMember("scaleY", value.val.two_d.y, document.GetAllocator());
                           }
                           scale.AddMember("defaultValue", scaleDeFault, document.GetAllocator());
                           for(int i=0;i<num_scaleKeyFrame;i++){
                               rapidjson::Value scaleKeyFrame(rapidjson::kObjectType);
                               A_Time keyFrameTime = { 0,1 };
                               ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(scaleS,i,AEGP_LTimeMode_CompTime,&keyFrameTime));
                               double time = (double)(keyFrameTime.value*1000/keyFrameTime.scale)*1000;
                               scaleKeyFrame.AddMember("time",time, document.GetAllocator());
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,scaleS,i,&value));
                               scaleKeyFrame.AddMember("scaleX", value.val.two_d.x, document.GetAllocator());
                               scaleKeyFrame.AddMember("scaleY", value.val.two_d.y, document.GetAllocator());
                               scaleKeyFrameArray.PushBack(scaleKeyFrame, document.GetAllocator());
                           }
                           scale.AddMember("keyFrame", scaleKeyFrameArray, document.GetAllocator());
                           transformInfo.AddMember("scale", scale, document.GetAllocator());
                           
                           rapidjson::Value rotate(rapidjson::kObjectType);
                           rapidjson::Value rotateKeyFrameArray(rapidjson::kArrayType);
                           AEGP_StreamRefH rotateS = NULL;
                           ERR(suites.StreamSuite5()->AEGP_GetNewLayerStream(S_my_id,layerH,AEGP_LayerStream_ROTATION,&rotateS));
                           A_long num_rotateKeyFrame;
                           ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(rotateS,&num_rotateKeyFrame));
                           rapidjson::Value rotateDeFault(rapidjson::kObjectType);
                           AEGP_StreamValue2 rdefaultValue;
                           if(num_rotateKeyFrame == 0){
                               A_Time rtimeT = { 0,1 };
                               ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,rotateS,AEGP_LTimeMode_LayerTime,&rtimeT,TRUE,&rdefaultValue));
                               rotateDeFault.AddMember("angle", rdefaultValue.val.one_d, document.GetAllocator());
                           }else{
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,rotateS,0,&value));
                               rotateDeFault.AddMember("angle", value.val.two_d.x, document.GetAllocator());
                           }
                           rotate.AddMember("defaultValue", rotateDeFault, document.GetAllocator());
                           for(int i=0;i<num_rotateKeyFrame;i++){
                               rapidjson::Value rotateKeyFrame(rapidjson::kObjectType);
                               A_Time keyFrameTime = { 0,1 };
                               ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(rotateS,i,AEGP_LTimeMode_CompTime,&keyFrameTime));
                               double time = (double)(keyFrameTime.value*1000/keyFrameTime.scale)*1000;
                               rotateKeyFrame.AddMember("time",time, document.GetAllocator());
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,rotateS,i,&value));
                               rotateKeyFrame.AddMember("angle", value.val.two_d.x, document.GetAllocator());
                               rotateKeyFrameArray.PushBack(rotateKeyFrame, document.GetAllocator());
                           }
                           rotate.AddMember("keyFrame", rotateKeyFrameArray, document.GetAllocator());
                           transformInfo.AddMember("rotate", rotate, document.GetAllocator());
                           
                           rapidjson::Value alpha(rapidjson::kObjectType);
                           rapidjson::Value alphaKeyFrameArray(rapidjson::kArrayType);
                           AEGP_StreamRefH opacityS = NULL;
                           ERR(suites.StreamSuite5()->AEGP_GetNewLayerStream(S_my_id,layerH,AEGP_LayerStream_OPACITY,&opacityS));
                           A_long num_opacityKeyFrame;
                           ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(opacityS,&num_opacityKeyFrame));
                           rapidjson::Value alphaDeFault(rapidjson::kObjectType);
                           AEGP_StreamValue2 adefaultValue;
                           if(num_opacityKeyFrame == 0){
                               A_Time atimeT = { 0,1 };
                               ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,opacityS,AEGP_LTimeMode_LayerTime,&atimeT,TRUE,&adefaultValue));
                               alphaDeFault.AddMember("percent", adefaultValue.val.one_d, document.GetAllocator());
                           }else{
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,opacityS,0,&value));
                               alphaDeFault.AddMember("percent", value.val.one_d, document.GetAllocator());
                           }
                           alpha.AddMember("defaultValue", alphaDeFault, document.GetAllocator());
                           for(int i=0;i<num_opacityKeyFrame;i++){
                               rapidjson::Value alphaKeyFrame(rapidjson::kObjectType);
                               A_Time keyFrameTime = { 0,1 };
                               ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(opacityS,i,AEGP_LTimeMode_CompTime,&keyFrameTime));
                               double time = (double)(keyFrameTime.value*1000/keyFrameTime.scale)*1000;
                               alphaKeyFrame.AddMember("time",time, document.GetAllocator());
                               AEGP_StreamValue2 value;
                               ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,opacityS,i,&value));
                               alphaKeyFrame.AddMember("percent", value.val.one_d, document.GetAllocator());
                               alphaKeyFrameArray.PushBack(alphaKeyFrame, document.GetAllocator());
                           }
                           alpha.AddMember("keyFrame", alphaKeyFrameArray, document.GetAllocator());
                           transformInfo.AddMember("alpha", alpha, document.GetAllocator());
                           
                           clip.AddMember("transformInfo", transformInfo, document.GetAllocator());
                          
                           //特效信息
                           A_long effect_num;
                           ERR(suites.EffectSuite4()->AEGP_GetLayerNumEffects(layerH,&effect_num));
                           rapidjson::Value fxArray(rapidjson::kArrayType);
                           for(int i = 0;i<effect_num;i++){
                                   AEGP_EffectRefH effectPH = NULL;
                                   ERR(suites.EffectSuite4()->AEGP_GetLayerEffectByIndex(S_my_id,layerH,i,&effectPH));
                                   
                                   //获取install key
                                   AEGP_InstalledEffectKey installed_keyP;
                                   ERR(suites.EffectSuite4()->AEGP_GetInstalledKeyFromLayerEffect(effectPH,&installed_keyP));
                                   //效果name
                                   A_char nameAC[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                                   ERR(suites.EffectSuite4()->AEGP_GetEffectName(installed_keyP,nameAC));
                                   A_char nameMatchAC[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                                   ERR(suites.EffectSuite4()->AEGP_GetEffectMatchName(installed_keyP,nameMatchAC));
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
                                      clip.AddMember("transitionFx", transform, document.GetAllocator());

                                   }else{
                                        //     -------------------------------------------------------------特效滤镜-------------------------------------------------------------------
                                       if(nameAC[0]=='W' && nameAC[1]=='B'){//微博自研效果Ï
                                           A_char packageName[AEGP_MAX_EFFECT_NAME_SIZE]="\0";
                                           strncpy(packageName,nameMatchAC+5,AEGP_MAX_EFFECT_NAME_SIZE-5);
                                           rapidjson::Value fx(rapidjson::kObjectType);
                                           fx.AddMember("fxPath",rapidjson::Value(packageName, document.GetAllocator()).Move(), document.GetAllocator());
                                           fx.AddMember("videoFxType",2, document.GetAllocator());
                                           fx.AddMember("filterIntensity",1.0, document.GetAllocator());
                                           //效果参数
                                           rapidjson::Value properties(rapidjson::kObjectType);
                                            A_long num_param;
                                            ERR(suites.StreamSuite5()->AEGP_GetEffectNumParamStreams(effectPH,&num_param));
                                            for(int j=1;j<num_param;j++){
                                                AEGP_StreamRefH  param_streamH = NULL;
                                                ERR(suites.StreamSuite5()->AEGP_GetNewEffectStreamByIndex(S_my_id,effectPH,j,&param_streamH));
                                                //获得参数名
                                                A_char paramName[AEGP_MAX_EFFECT_NAME_SIZE]  = {'\0'};
                                                ERR(suites.StreamSuite2()->AEGP_GetStreamName(param_streamH,TRUE,paramName));
                                                std::string sParamName(paramName);
                                                //关键帧数量
                                                A_long num_keyFrame;
                                                ERR(suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(param_streamH,&num_keyFrame));
                                                AEGP_StreamType type;
                                                ERR(suites.StreamSuite5()->AEGP_GetStreamType(param_streamH,&type));
                                        
                                                if(type == AEGP_StreamType_OneD ){//单键模式
                                                    rapidjson::Value property(rapidjson::kObjectType);
                                                    //默认值
                                                    PF_ParamValue    deFaultValue;
                                                    PF_ParamType      param_typeP;
                                                    PF_ParamDefUnion  uP0;
                                                    ERR(suites.EffectSuite4()->AEGP_GetEffectParamUnionByIndex(S_my_id,effectPH,j,&param_typeP,&uP0));
                                                    if(param_typeP == PF_Param_SLIDER){//滑块类型
                                                        deFaultValue = uP0.sd.value;
                                                        property.AddMember("defaultValue", deFaultValue, document.GetAllocator());
                                                    }else if(param_typeP ==PF_Param_POPUP){//下拉框
                                                        deFaultValue = uP0.pd.value;
                                                        property.AddMember("defaultValue", deFaultValue, document.GetAllocator());
                                                    }else if(param_typeP == PF_Param_ANGLE){//角度
                                                        deFaultValue = (uP0.ad.value/65536)%360;
                                                        property.AddMember("defaultValue", deFaultValue, document.GetAllocator());
                                                    }else if(param_typeP == PF_Param_FLOAT_SLIDER){//float类型滑动块
                                                        float value = uP0.fs_d.value;
                                                        property.AddMember("defaultValue", value, document.GetAllocator());
                                                    }else if(param_typeP == PF_Param_CHECKBOX){//checkBox
                                                        int value = uP0.bd.value;
                                                        property.AddMember("defaultValue", value, document.GetAllocator());
                                                    }
                                            
                                                   
                                                    //关键帧逻辑
                                                    rapidjson::Value protertyKeyFrameArray(rapidjson::kArrayType);
                                                    for(int i=0;i<num_keyFrame;i++){
                                                        rapidjson::Value propertyKeyFrame(rapidjson::kObjectType);
                                                        A_Time proertyFrameTime = { 0,1 };
                                                        ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(param_streamH,i,AEGP_LTimeMode_CompTime,&proertyFrameTime));
                                                        double time = (double)(proertyFrameTime.value*1000/proertyFrameTime.scale)*1000;
                                                        propertyKeyFrame.AddMember("time",time, document.GetAllocator());
                                                        AEGP_StreamValue2 value;
                                                        ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,param_streamH,i,&value));
                                                        propertyKeyFrame.AddMember("value",(int)value.val.one_d, document.GetAllocator());
                                                        protertyKeyFrameArray.PushBack(propertyKeyFrame, document.GetAllocator());
                                                    }
                                                    if(protertyKeyFrameArray.Size()>0){
                                                        property.AddMember("keyFrame",protertyKeyFrameArray, document.GetAllocator());
                                                    }
                                                    properties.AddMember(rapidjson::Value(paramName,document.GetAllocator()).Move(),property,document.GetAllocator());
                                                }else if(type == AEGP_StreamType_TwoD|| type == AEGP_StreamType_TwoD_SPATIAL){
                                                    rapidjson::Value property_x(rapidjson::kObjectType);
                                                    AEGP_StreamValue2    defaultValue;
                                                    A_Time timeT = { 0,1 };
                                                    ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,param_streamH,AEGP_LTimeMode_LayerTime,&timeT,TRUE,&defaultValue));
                                                    property_x.AddMember("defaultValue", defaultValue.val.two_d.x, document.GetAllocator());
                                                    //关键帧逻辑
                                                    rapidjson::Value protertyKeyFrameArray(rapidjson::kArrayType);
                                                    for(int i=0;i<num_keyFrame;i++){
                                                        rapidjson::Value propertyKeyFrame(rapidjson::kObjectType);
                                                        A_Time proertyFrameTime = { 0,1 };
                                                        ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(param_streamH,i,AEGP_LTimeMode_CompTime,&proertyFrameTime));
                                                        double time = (double)(proertyFrameTime.value*1000/proertyFrameTime.scale)*1000;
                                                        propertyKeyFrame.AddMember("time",time, document.GetAllocator());
                                                        AEGP_StreamValue2 value;
                                                        ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,param_streamH,i,&value));
                                                        propertyKeyFrame.AddMember("value", (int)value.val.two_d.x, document.GetAllocator());
                                                        protertyKeyFrameArray.PushBack(propertyKeyFrame, document.GetAllocator());
                                                    }
                                                    if(protertyKeyFrameArray.Size()>0){
                                                        property_x.AddMember("keyFrame",protertyKeyFrameArray, document.GetAllocator());
                                                    }
                                                    string sParamName_x(sParamName.append("_x"));
                                                    properties.AddMember(rapidjson::Value(sParamName_x.c_str(),document.GetAllocator()).Move(),property_x,document.GetAllocator());
                                                    
                                                    rapidjson::Value property_y(rapidjson::kObjectType);
                                                    property_y.AddMember("defaultValue", defaultValue.val.two_d.y, document.GetAllocator());
                                                    //关键帧逻辑
                                                    rapidjson::Value protertyKeyFrameArray_y(rapidjson::kArrayType);
                                                    for(int i=0;i<num_keyFrame;i++){
                                                        rapidjson::Value propertyKeyFrame(rapidjson::kObjectType);
                                                        A_Time proertyFrameTime = { 0,1 };
                                                        ERR(suites.KeyframeSuite4()->AEGP_GetKeyframeTime(param_streamH,i,AEGP_LTimeMode_CompTime,&proertyFrameTime));
                                                        double time = (double)(proertyFrameTime.value*1000/proertyFrameTime.scale)*1000;
                                                        propertyKeyFrame.AddMember("time",time, document.GetAllocator());
                                                        AEGP_StreamValue2 value;
                                                        ERR(suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(S_my_id,param_streamH,i,&value));
                                                        propertyKeyFrame.AddMember("value", (int)value.val.two_d.y, document.GetAllocator());
                                                        protertyKeyFrameArray_y.PushBack(propertyKeyFrame, document.GetAllocator());
                                                    }
                                                    if(protertyKeyFrameArray_y.Size()>0){
                                                        property_y.AddMember("keyFrame",protertyKeyFrameArray_y, document.GetAllocator());
                                                    }
                                                    string sParamName_y(paramName);
                                                    sParamName_y.append("_y");
                                                    properties.AddMember(rapidjson::Value(sParamName_y.c_str(),document.GetAllocator()).Move(),property_y,document.GetAllocator());
                                                }else if(type == AEGP_StreamType_COLOR){//调色板逻辑
                                                    AEGP_StreamValue2    defaultValue;
                                                    A_Time timeT = { 0,1 };
                                                    ERR(suites.StreamSuite5()->AEGP_GetNewStreamValue(S_my_id,param_streamH,AEGP_LTimeMode_LayerTime,&timeT,TRUE,&defaultValue));
                                                    //r
                                                    rapidjson::Value property_r(rapidjson::kObjectType);
                                                    string sParamName_r(paramName);
                                                    sParamName_r.append("_r");
                                                    A_FpLong rValue = defaultValue.val.color.redF * 255.0;
                                                    property_r.AddMember("defaultValue", rValue, document.GetAllocator());
                                                    properties.AddMember(rapidjson::Value(sParamName_r.c_str(),document.GetAllocator()).Move(),property_r,document.GetAllocator());
                                                    //g
                                                    rapidjson::Value property_g(rapidjson::kObjectType);
                                                    string sParamName_g(paramName);
                                                    sParamName_g.append("_g");
                                                    A_FpLong gValue = defaultValue.val.color.greenF * 255.0;
                                                    property_g.AddMember("defaultValue", gValue, document.GetAllocator());
                                                    properties.AddMember(rapidjson::Value(sParamName_g.c_str(),document.GetAllocator()).Move(),property_g,document.GetAllocator());
                                                    //b
                                                    rapidjson::Value property_b(rapidjson::kObjectType);
                                                    string sParamName_b(paramName);
                                                    sParamName_b.append("_b");
                                                    A_FpLong bValue = defaultValue.val.color.blueF * 255.0;
                                                    property_b.AddMember("defaultValue", bValue, document.GetAllocator());
                                                    properties.AddMember(rapidjson::Value(sParamName_b.c_str(),document.GetAllocator()).Move(),property_b,document.GetAllocator());
                                                    //a
                                                    rapidjson::Value property_a(rapidjson::kObjectType);
                                                    string sParamName_a(paramName);
                                                    sParamName_a.append("_a");
                                                    A_FpLong aValue = defaultValue.val.color.alphaF * 255.0;
                                                    property_a.AddMember("defaultValue", aValue, document.GetAllocator());
                                                    properties.AddMember(rapidjson::Value(sParamName_a.c_str(),document.GetAllocator()).Move(),property_a,document.GetAllocator());
                                                }
                                            }
                                           fx.AddMember("fxProperties",properties, document.GetAllocator());
                                           fxArray.PushBack(fx, document.GetAllocator());
                                       }
                                   }
                               }

                           //添加默认变换fx
                            rapidjson::Value transformFx(rapidjson::kObjectType);
                            transformFx.AddMember("fxPath","WBAETransform", document.GetAllocator());
                            transformFx.AddMember("videoFxType",2, document.GetAllocator());
                            transformFx.AddMember("filterIntensity",1.0, document.GetAllocator());
                            rapidjson::Value properties(rapidjson::kObjectType);
                            transformFx.AddMember("properties",properties, document.GetAllocator());
                            fxArray.PushBack(transformFx, document.GetAllocator());
                          if(fxArray.Size() > 0)
                            clip.AddMember("fxs", fxArray, document.GetAllocator());
                          
                          videoClipArray.PushBack(clip, document.GetAllocator());
                       }else if(flags & AEGP_ItemFlag_HAS_AUDIO){//音频
                           //videoType
                           clip.AddMember("videoType", 0, document.GetAllocator());
                           audioClipArray.PushBack(clip, document.GetAllocator());
                       }
                       }
                    }
                
                    if(videoClipArray.Size()>0){
                        rapidjson::Value videoTrack(rapidjson::kObjectType);
                        videoTrack.AddMember("clips", videoClipArray, document.GetAllocator());
                        videoTrackArray.PushBack(videoTrack, document.GetAllocator());
                    }
                    if(audioClipArray.Size()>0){
                        rapidjson::Value audioTrack(rapidjson::kObjectType);
                        audioTrack.AddMember("clips", audioClipArray, document.GetAllocator());
                        audioTrackArray.PushBack(audioTrack, document.GetAllocator());
                    }
                    
                }
                
                if(videoTrackArray.Size()> 0)
                    document.AddMember("videoTracks", videoTrackArray, document.GetAllocator());
                if(audioTrackArray.Size()>0)
                    document.AddMember("audioTracks", audioTrackArray, document.GetAllocator());
                if(timeLineFxs.Size()>0){
                    document.AddMember("timelineVideoFxs", timeLineFxs, document.GetAllocator());
                }
                
               

                rapidjson::StringBuffer buffer1;
                rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
                document.Accept(writer1);
                printf("%s\n", buffer1.GetString());

                std::string json (buffer1.GetString(), buffer1.GetSize());
                std::ofstream of("/tmp/album",ios::out);
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
