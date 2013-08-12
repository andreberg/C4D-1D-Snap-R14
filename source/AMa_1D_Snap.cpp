#include "general.h"
#include "lib_modeling.h"
#include "lib_ngon.h"
#include "ge_math.h"
#include "ge_prepass.h"
#include "c4d_descriptiondialog.h"
#include "tool_AMa_1D_Snap.h"

#define ID_AMa_1D_SNAP 1020238 // registered as AMa_GameTrooper_Left
Bool chk;
const Real AXIS_LEN = 0.03;
const Real AXIS_ACTIVE_DIST = 4.0;
const LONG NEAREST_POINT_DIST = 12L;
const Real UNSEL_COLOR_1 = 0.4;
const Real UNSEL_COLOR_2 = 0.2;
const Real SEL_COLOR_2 = 0.1;
const Real WIRE_COLOR_2 = 0.7;
const Real SMALL_AXIS_RELATION = 0.33;

struct PonitDataStru
{
	LONG	ind;
	Vector	pos;
};

struct ObjectsDataStru
{
	PointObject*	pob;
	LONG			sel_cnt;
	LONG			selDirt;
	BaseSelect*		bs;
	PonitDataStru*	poiData;
	Vector*			v_Pnt;
};

//1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn
//1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn1DSn
class D1Snap : public DescriptionToolData
{
public:
	D1Snap();
	~D1Snap();
	virtual LONG			GetToolPluginId() { return ID_AMa_1D_SNAP; }
	virtual const String	GetResourceSymbol() { return String( "tool_AMa_1D_Snap"); }
	virtual Bool			InitTool( BaseDocument* doc, BaseContainer& data, BaseThread* bt);
	virtual void			FreeTool( BaseDocument* doc, BaseContainer& data);
	virtual Bool			MouseInput( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg);
	virtual LONG			GetState( BaseDocument *doc);
	virtual Bool			GetCursorInfo(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, Real x, Real y, BaseContainer &bc);
	virtual void			InitDefaultSettings( BaseDocument *doc, BaseContainer &data);
	virtual TOOLDRAW		Draw( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, BaseDrawHelp *bh, BaseThread *bt, TOOLDRAWFLAGS flags);
	virtual Bool			GetDEnabling( BaseDocument *doc, BaseContainer &data, const DescID &id,const GeData &t_data,DESCFLAGS_ENABLE flags,const BaseContainer *itemdesc);
	virtual Bool			Message( BaseDocument* doc, BaseContainer& data, LONG type, void* t_data);

protected:
	void DrawIt();
	bool Init_My_Data( BaseDocument *doc, bool recalcCenter, AtomArray *preArr = NULL);
	bool CheckForDirty( BaseDocument *doc);
	void FindSnapObjsData( LONG x, LONG y);
	void ComputeMatr( BaseContainer *data, BaseDocument *doc);
	void ComputeAxisVectorsInScreen( BaseDocument *doc, BaseDraw *bd);
	bool ScenePixelPrepare( BaseDocument *doc, BaseDraw *bd, Bool visible_only, bool andActive = false);
	void ScenePixelFree();
	void ActivePixelFree();
	void GetCacheRucurcive_tObjects( BaseObject* bo);
	void GetCacheRucurcive_tCaches( BaseObject* Cache);
	Bool Drag_Interactive( BaseDocument *doc, BaseContainer *data, EditorWindow *win, BaseDraw *bd, const BaseContainer* msg);
	void Free_My_Data();
	
	bool isdragging, overR, overG, overB, overR_old, overG_old, overB_old, draw_from_GetCursorInfo, need_to_reinit_VPixels, aimed, AxFree_Now;
	//BaseDocument *docum;
	PointObject *obj;
	ObjectsDataStru *objs;
	AtomArray *aaObjs;
	LONG objsCnt;
	BaseObject *aimObj;
	PointObject *aimPoObj, *setOrigObj;
	Vector vcCntrScr, vecCentrGlo, veAxX, veAxY, veAxZ, vcX_Scr, vcY_Scr, vcZ_Scr, vcAim, veDifferGlobal;
	Matrix objMg, M_Axis, M_Axis_Inv;
	Real AxLen3d;
	LONG overPix, overPix_old, crX, crY, selectedAxis;
	ViewportSelect *ViSe, *ViSeAll, *ViSeAct;
	ViewportPixel *ViPx;
	BaseDraw *bd_old;
	AtomArray *sceneAArC;
};

//SnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSn
//SnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSnSn
D1Snap::D1Snap()
{
	obj =			NULL;
	objs =			NULL;
	aaObjs =		NULL;
	aimObj =		NULL;
	aimPoObj =		NULL;
	setOrigObj =	NULL;
	ViSe =			NULL;
	ViSeAll =		NULL;
	ViSeAct =		NULL;
	ViPx =			NULL;
	bd_old =		NULL;
	sceneAArC =		NULL;
}

D1Snap::~D1Snap()
{
	Free_My_Data();
}

Bool D1Snap::InitTool( BaseDocument *doc, BaseContainer &data, BaseThread *bt)
{
	chk = DescriptionToolData::InitTool( doc, data, bt);
	if( ! chk) return FALSE;
	isdragging = false;
	ViSe = NULL;
	bd_old = NULL;
	need_to_reinit_VPixels = true;
	draw_from_GetCursorInfo = false;
	
	if( data.GetLong( AMa_1DSNAP_AX) == AMa_1DSNAP_AX_FREE)
	{
		chk = Init_My_Data( doc, false);
		if( ! chk) return FALSE;
		vecCentrGlo = obj->GetModelingAxis( doc).off;
		AxFree_Now = true;
	}
	else
	{
		chk = Init_My_Data( doc, true);
		if( ! chk) return FALSE;
		AxFree_Now = false;
	}
	return TRUE;
}
void D1Snap::FreeTool( BaseDocument *doc, BaseContainer &data)
{
	Free_My_Data();
	DescriptionToolData::FreeTool( doc, data);
}
void D1Snap::InitDefaultSettings( BaseDocument *doc, BaseContainer &data)
{
	DescriptionToolData::InitDefaultSettings( doc, data);
	data.SetBool( AMa_1DSNAP_TO_INVIS, FALSE);
	data.SetLong( AMa_1DSNAP_AX, AMa_1DSNAP_AX_LOCAL);
	AxFree_Now = false;
}
LONG D1Snap::GetState( BaseDocument *doc)
{
	if (!doc)						return 0L;
	if( doc->GetMode() != Mpoints)	return 0L;
	AutoAlloc <AtomArray> arr;
	if( ! arr)						return 0L;
	doc->GetActiveObjects( *arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	LONG cnt =						arr->GetCount();
	for (LONG i = 0L; i != cnt; ++i)
	{
		if( arr->GetIndex( i)->IsInstanceOf( Opoint))
			return CMD_ENABLED;
	}
	return 0L;
}
Bool D1Snap::Message( BaseDocument* doc, BaseContainer& data, LONG type, void* t_data)
{
	//GePrint(  LongToString( type));
	if( type == MSG_DESCRIPTION_VALIDATE)
	{
		CheckForDirty( doc);
		ComputeMatr( &data, doc);
		if( data.GetLong( AMa_1DSNAP_AX) == AMa_1DSNAP_AX_FREE)
		{
			if( ! AxFree_Now)
			{
				vecCentrGlo = obj->GetModelingAxis( doc).off;
				AxFree_Now = true;
			}
		}
		else
			AxFree_Now = false;
	}
	return DescriptionToolData::Message( doc, data, type, t_data);
}
Bool D1Snap::GetCursorInfo( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, Real x, Real y, BaseContainer &bc)
{
	if( bc.GetId() == BFM_CURSORINFO_REMOVE) return TRUE;
	bc.SetString( RESULT_BUBBLEHELP, GeLoadString( IDS_AMa_1DSNAP_BBL));
	
	//GePrint( "GetCursorInfo");
	if( isdragging)	return TRUE;
	
	/////////////////////////////////axisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxisaxis
	overR = overG = overB = false;
	
	ComputeAxisVectorsInScreen( doc, bd);
	Real dX = x - vcCntrScr.x;
	Real dY = y - vcCntrScr.y;
	
	Real Xe = vcX_Scr.x - vcCntrScr.x;
	Real Ye = vcX_Scr.y - vcCntrScr.y;
	Real alp = ATan( Xe / Ye);
	Real casAlp = Cos( alp);
	Real sinAlp = Sin( alp);
	Real Xs = dX * casAlp - dY * sinAlp;
	Real Ys = dY * casAlp - dX * sinAlp;
	Real Yos = Ye * casAlp - Xe * sinAlp;
	if( Abs( Xs) < AXIS_ACTIVE_DIST && ( ( Yos > 0 && Ys > 0 && Ys < Yos) || ( Yos < 0 && Ys < 0 && Ys > Yos)))
	{
		overR = true;
		DrawIt();
		return TRUE;
	}

	alp = ATan( ( vcY_Scr.x - vcCntrScr.x) / ( vcY_Scr.y - vcCntrScr.y));
	Xs = dX * Cos( alp) - dY * Sin( alp);
	Ys = dY * Cos( alp) - dX * Sin( alp);
	Yos = vcY_Scr.y * Cos( alp) - vcY_Scr.x * Sin( alp);

	Xe = vcY_Scr.x - vcCntrScr.x;
	Ye = vcY_Scr.y - vcCntrScr.y;
	alp = ATan( Xe / Ye);
	casAlp = Cos( alp);
	sinAlp = Sin( alp);
	Xs = dX * casAlp - dY * sinAlp;
	Ys = dY * casAlp - dX * sinAlp;
	Yos = Ye * casAlp - Xe * sinAlp;
	if( Abs( Xs) < AXIS_ACTIVE_DIST && ( ( Yos > 0 && Ys > 0 && Ys < Yos) || ( Yos < 0 && Ys < 0 && Ys > Yos)))
	{
		overG = true;
		DrawIt();
		return TRUE;
	}

	Xe = vcZ_Scr.x - vcCntrScr.x;
	Ye = vcZ_Scr.y - vcCntrScr.y;
	alp = ATan( Xe / Ye);
	casAlp = Cos( alp);
	sinAlp = Sin( alp);
	Xs = dX * casAlp - dY * sinAlp;
	Ys = dY * casAlp - dX * sinAlp;
	Yos = Ye * casAlp - Xe * sinAlp;
	if( Abs( Xs) < AXIS_ACTIVE_DIST && ( ( Yos > 0 && Ys > 0 && Ys < Yos) || ( Yos < 0 && Ys < 0 && Ys > Yos)))
	{
		overB = true;
		DrawIt();
		return TRUE;
	}
	
	//////////////////////////////pointspointspointspointspointspointspointspointspointspointspointspointspointspointspoints
	if( bd != bd_old)	need_to_reinit_VPixels = true;
	
	if( need_to_reinit_VPixels)
	{
		//GePrint( "reinit ViewPixels");
		LONG left, top, right, bottom;
		bd->GetFrame(&left,  &top, &right, &bottom);
		LONG	width = right - left + 1L;
		LONG	height = bottom - top + 1L;
		ViewportSelect::Free( ViSe);
		ViSe = ViewportSelect::Alloc();
		if( ! ViSe) return FALSE;
		chk = ViSe->Init( width, height, bd, aaObjs, Mpoints, data.GetBool( AMa_1DSNAP_TO_INVIS));
		if( ! chk) return FALSE;
		need_to_reinit_VPixels = false;
		bd_old = bd;
	}
	overPix = -1L;
	FindSnapObjsData( x, y);

	DrawIt();
	return TRUE;
}
void D1Snap::DrawIt()
{
	if( overPix != overPix_old || overR != overR_old || overG != overG_old || overB != overB_old)
	{
		draw_from_GetCursorInfo = true;
		//GePrint( "from GetCursorInfo");
		DrawViews( DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_ANIMATION|DRAWFLAGS_NO_THREAD|DRAWFLAGS_ONLY_HIGHLIGHT);
		draw_from_GetCursorInfo = false;
	}
	
	overR_old = overR;
	overG_old = overG;
	overB_old = overB;
	overPix_old = overPix;
}

TOOLDRAW D1Snap::Draw( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, BaseDrawHelp *bh, BaseThread *bt, TOOLDRAWFLAGS flags)
{
	//GePrint( "d");
	if( ! ( flags & TOOLDRAWFLAGS_HIGHLIGHT) || flags & TOOLDRAWFLAGS_INVERSE_Z)	return TOOLDRAW_0;
	//GePrint( "draw");
	chk =		CheckForDirty( doc);
	if( ! chk)	return TOOLDRAW_0;
	///////////////////////////+++++++++++++++++++++++++++++++++++++++++++++++++++++
	if( ! draw_from_GetCursorInfo)	overR = overG = overB = false;
	//docum = doc;
	ComputeMatr( &data, doc);
	ComputeAxisVectorsInScreen( doc, bd);
	
	Vector r_color( 1.0, 0.0, 0.0), g_color( 0.0, 1.0, 0.0), b_color( 0.0, 0.0, 1.0), wire_color;
#ifdef	C4D_R12
	Vector highlight_color = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
#else
	Vector highlight_color = GetGuiWorldColor( 39);//COLOR_SELECTION_PREVIEW);
#endif
	if( isdragging)
	{
		switch( selectedAxis)
		{
			case 0:
				r_color = Vector( 1.0, SEL_COLOR_2, SEL_COLOR_2);
				g_color = Vector( UNSEL_COLOR_2, UNSEL_COLOR_1, UNSEL_COLOR_2);
				b_color = Vector( UNSEL_COLOR_2, UNSEL_COLOR_2, UNSEL_COLOR_1);
				wire_color = Vector( 1.0, WIRE_COLOR_2 ,WIRE_COLOR_2);
				break;
			case 1:
				r_color = Vector( UNSEL_COLOR_1, UNSEL_COLOR_2, UNSEL_COLOR_2);
				g_color = Vector( SEL_COLOR_2, 1.0, SEL_COLOR_2);
				b_color = Vector( UNSEL_COLOR_2, UNSEL_COLOR_2, UNSEL_COLOR_1);
				wire_color = Vector( WIRE_COLOR_2, 1.0, WIRE_COLOR_2);
				break;
			case 2:
				r_color = Vector( UNSEL_COLOR_1, UNSEL_COLOR_2, UNSEL_COLOR_2);
				g_color = Vector( UNSEL_COLOR_2, UNSEL_COLOR_1, UNSEL_COLOR_2);
				b_color = Vector( SEL_COLOR_2, SEL_COLOR_2, 1.0);
				wire_color = Vector( WIRE_COLOR_2, WIRE_COLOR_2 ,1.0);
				break;
		}
	}
	else
	{
		if( overR)
			r_color = highlight_color;
		else if( overG)
			g_color = highlight_color;
		else if( overB)
			b_color = highlight_color;
	}

	bd->SetDrawParam( DRAW_PARAMETER_LINEWIDTH, GeData( 2));
	bd->SetPen( r_color);
	bd->DrawLine2D( vcCntrScr, vcX_Scr);
	bd->SetPen( g_color);
	bd->DrawLine2D( vcCntrScr, vcY_Scr);
	bd->SetPen( b_color);
	bd->DrawLine2D( vcCntrScr, vcZ_Scr);
	bd->SetDrawParam( DRAW_PARAMETER_LINEWIDTH, GeData( 1));
	
	///////////////////////////+++++++++++++++++++++++++++++++++++++++++++++++++++++
	if( ! draw_from_GetCursorInfo)
	{
		need_to_reinit_VPixels = true;
	}
	else if( overPix != -1L)
	{
		bd->SetMatrix_Matrix( setOrigObj, setOrigObj->GetMg());
		bd->LineZOffset( 4L);
		bd->SetPen( highlight_color);
		bd->DrawHandle( setOrigObj->GetPointR()[overPix], DRAWHANDLE_BIG, 0L);
		bd->LineZOffset( 0L);
	}
	
	///////////////////////////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if( isdragging)
	{
		//Real AxLen3d_small = AxLen3d * SMALL_AXIS_RELATION;
		Vector veSmallAxis1, veSmallAxis2;
		switch( selectedAxis)
		{
			case 0:
				bd->SetPen( Vector( 1.0, 0.0, 0.0));
				veSmallAxis1 = veAxY * SMALL_AXIS_RELATION;
				veSmallAxis2 = veAxZ * SMALL_AXIS_RELATION; break;
			case 1:
				bd->SetPen( Vector( 0.0, 1.0, 0.0));
				veSmallAxis1 = veAxX * SMALL_AXIS_RELATION;
				veSmallAxis2 = veAxZ * SMALL_AXIS_RELATION; break;
			case 2:
				bd->SetPen( Vector( 0.0, 0.0, 1.0));
				veSmallAxis1 = veAxX * SMALL_AXIS_RELATION;
				veSmallAxis2 = veAxY * SMALL_AXIS_RELATION; break;
		}
		bd->DrawLine2D( bd->WS( vecCentrGlo + veSmallAxis1), bd->WS( vecCentrGlo - veSmallAxis1));
		bd->DrawLine2D( bd->WS( vecCentrGlo + veSmallAxis2), bd->WS( vecCentrGlo - veSmallAxis2));
		bd->DrawLine2D( bd->WS( vecCentrGlo + veDifferGlobal + veSmallAxis1), bd->WS( vecCentrGlo + veDifferGlobal - veSmallAxis1));
		bd->DrawLine2D( bd->WS( vecCentrGlo + veDifferGlobal + veSmallAxis2), bd->WS( vecCentrGlo + veDifferGlobal - veSmallAxis2));
		Vector vcAimProjScr = bd->WS( vecCentrGlo + veDifferGlobal);
		bd->DrawLine2D( vcCntrScr, vcAimProjScr);
		/////// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ rubber line
		bd->SetDrawParam( DRAW_PARAMETER_LINEWIDTH, GeData( 2));
		bd->SetPen( wire_color);
		if( aimed)
		{
			bd->DrawLine2D( vcAimProjScr, bd->WS( vcAim));
			/////// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ point on aim
			bd->SetMatrix_Matrix( NULL, Matrix());
			bd->LineZOffset( 4L);
			bd->SetPen( highlight_color);
			bd->DrawHandle( vcAim, DRAWHANDLE_MIDDLE, 0L);
			bd->LineZOffset( 0L);
		}
		else
			bd->DrawLine2D( vcAimProjScr, Vector( crX, crY, 0.0));
		bd->SetDrawParam( DRAW_PARAMETER_LINEWIDTH, GeData( 1));
	}
	return TOOLDRAW_AXIS|TOOLDRAW_HANDLES;
}
Bool D1Snap::GetDEnabling( BaseDocument *doc, BaseContainer &data, const DescID &id,const GeData &t_data,DESCFLAGS_ENABLE flags,const BaseContainer *itemdesc)
{
	switch( id[0].id)
	{
		case AMa_1DSNAP_LINK:
			if( data.GetLong( AMa_1DSNAP_AX) != AMa_1DSNAP_AX_FROM_O)
				return FALSE;
			break;
		/*case AMa_MSLIDE_IS_LIMIT:
			if( data.GetLong( AMa_MSLIDE_REG) != AMa_MSLD_M_STANDARD)
				return FALSE;
			break;
		case AMa_MSLD_PRES_ANG:
			if( data.GetLong( AMa_MSLIDE_REG) != AMa_MSLD_M_NORMAL)
				return FALSE;
			break;*/
	}
	return DescriptionToolData::GetDEnabling(doc,data,id,t_data,flags,itemdesc);
}
void D1Snap::FindSnapObjsData( LONG x, LONG y)
{
	ViPx = NULL;
	ViewportPixel *ViewpPx_tmp =	NULL;
	LONG minDist_pow2 = 1234567L;
	LONG dist_pow2;
	LONG Xsnap_tmp = x;
	LONG Ysnap_tmp = y;
	for( LONG i = 0L; i != objsCnt; ++i)
	{
		ViewpPx_tmp =		ViSe->GetNearestPoint( objs[i].pob, Xsnap_tmp, Ysnap_tmp, NEAREST_POINT_DIST);
		if(!ViewpPx_tmp)	continue;
		dist_pow2 =			( Xsnap_tmp - x) * ( Xsnap_tmp - x) + ( Ysnap_tmp - y) * ( Ysnap_tmp - y);
		if( dist_pow2 < minDist_pow2)
		{
			minDist_pow2 =	dist_pow2;
			ViPx =			ViewpPx_tmp;
			setOrigObj =	objs[i].pob;
			overPix =		ViPx->i;
		}
	}
}
void D1Snap::ComputeAxisVectorsInScreen( BaseDocument *doc, BaseDraw *bd)
{
	AxLen3d = AXIS_LEN;
	
	LONG cl, ct, cr, cb;
	bd->GetFrame( &cl, &ct, &cr, &cb);
	Real perim = cr - cl + cb - ct;
		
	Vector vcOff, vcSc, vcScZ;
	bd->GetViewParameter( &vcOff, &vcSc, &vcScZ);
	
	bool isPersp = (bd->GetProjection() == 0L);
	if( isPersp)
	{
		Real minSide;
		if( cr - cl > cb - ct)
			minSide = cb - ct;
		else
			minSide = cr - cl;
		
		Vector vcCntrCam = bd->WC( vecCentrGlo);
		
		BaseObject* cam = bd->GetSceneCamera( doc);
		if( ! cam)
			cam = bd->GetEditorCamera();
		
		BaseContainer* bc = cam->GetDataInstance();
		AxLen3d *= bc->GetReal( CAMERAOBJECT_APERTURE, 36.0) / bc->GetReal( CAMERA_FOCUS, 36.0) * vcCntrCam.z * perim / minSide;
	}
	else
	{
		AxLen3d *= perim / vcSc.x;
	}
	//vecCentrGlob = vecCentr * objMg; // temp
	vcCntrScr = bd->WS( vecCentrGlo);
	veAxX = Vector( AxLen3d, 0.0 ,0.0) ^ M_Axis;
	veAxY = Vector( 0.0, AxLen3d ,0.0) ^ M_Axis;
	veAxZ = Vector( 0.0, 0.0 ,AxLen3d) ^ M_Axis;
	vcX_Scr = bd->WS( vecCentrGlo + veAxX);
	vcY_Scr = bd->WS( vecCentrGlo + veAxY);
	vcZ_Scr = bd->WS( vecCentrGlo + veAxZ);
}

void D1Snap::ComputeMatr( BaseContainer *data, BaseDocument* doc)
{
	switch( data->GetLong( AMa_1DSNAP_AX))
	{
		case AMa_1DSNAP_AX_GLOB:
			M_Axis = Matrix();
			break;
		case AMa_1DSNAP_AX_ROOT:
		{
			BaseObject* roo = obj->GetUp();
			if( roo)
			{
				BaseObject *roo_tmp =	NULL;
				while( (roo_tmp = roo->GetUp()))
					roo = roo_tmp;
				M_Axis = roo->GetMg();
			}
			else
				M_Axis = objMg;
		}
		break;
		case AMa_1DSNAP_AX_PARE:
		{
			BaseObject *pare = obj->GetUp();
			if( pare)
				M_Axis = pare->GetMg();
			else
				M_Axis = Matrix();
		}
		break;
		case AMa_1DSNAP_AX_CAM:
		{
			/*BaseDraw *basedr = doc->GetActiveBaseDraw();
			BaseObject *cam = basedr->GetSceneCamera( doc);
			if( cam)
				M_Axis = cam->GetMg();
			else*/
			M_Axis = doc->GetActiveBaseDraw()->GetMg();
		}
		break;
		case AMa_1DSNAP_AX_FROM_O:
		{
			BaseObject *LObj = data->GetObjectLink( AMa_1DSNAP_LINK, doc);
			if( LObj)
				M_Axis = LObj->GetMg();
			else
				M_Axis = objMg;
		}
		break;
		case AMa_1DSNAP_AX_FREE:
			M_Axis = obj->GetModelingAxis( doc);
		break;
		default: // and AMa_1DSNAP_AX_LOCAL
			M_Axis = objMg;
	}
	M_Axis.off = Vector();
	M_Axis_Inv = ! M_Axis;
}

void D1Snap::GetCacheRucurcive_tObjects( BaseObject *bo)
{
	BaseObject* Cache =	NULL;
	while( bo)
	{
		if( ! bo->GetBit( BIT_ACTIVE))
		{
			Cache = bo;
			if( bo->GetCache())
			{
				Cache = bo->GetCache();
				GetCacheRucurcive_tCaches( Cache->GetDown());
			}
			if( Cache->GetDeformCache())
				Cache = Cache->GetDeformCache();
			sceneAArC->Append( Cache);
		}
		GetCacheRucurcive_tObjects( bo->GetDown());
		bo = bo->GetNext();
	}
}

void D1Snap::GetCacheRucurcive_tCaches( BaseObject *Cache)
{
	BaseObject* CacheDef =	NULL;
	while( Cache)
	{
		CacheDef = Cache;
		if( Cache->GetDeformCache())
			CacheDef = Cache->GetDeformCache();
			
		sceneAArC->Append( CacheDef);
		GetCacheRucurcive_tObjects( Cache->GetDown());
		
		Cache = Cache->GetNext();
	}
}

bool D1Snap::ScenePixelPrepare( BaseDocument *doc, BaseDraw *bd, Bool visible_only, bool andActive)
{
	sceneAArC = AtomArray::Alloc();
	if( ! sceneAArC) return false;
	
	GetCacheRucurcive_tObjects( doc->GetFirstObject());
	
	LONG left, top, right, bottom;
	bd->GetFrame(&left,  &top, &right, &bottom);
	LONG width = right - left + 1;
	LONG height = bottom - top + 1;
	ViSeAll = ViewportSelect::Alloc();
	if( ! ViSeAll) return false;
	if( sceneAArC->GetCount())
	{
		chk = ViSeAll->Init( width, height, bd, sceneAArC, Mpoints, visible_only);
		if( ! chk) return false;
	}
	
	if( andActive)
	{
		ViSeAct = ViewportSelect::Alloc();
		if( ! ViSeAct) return false;
		chk = ViSeAct->Init( width, height, bd, aaObjs, Mpoints, visible_only);
		if( ! chk) return false;
	}
	return true;
}

void D1Snap::ScenePixelFree()
{
	ViewportSelect::Free( ViSeAll);
	AtomArray::Free( sceneAArC);
}

void D1Snap::ActivePixelFree()
{
	ViewportSelect::Free( ViSeAct);
	//AtomArray::Free( actAArC);
}

void D1Snap::Free_My_Data()
{
	for( LONG i = 0L; i != objsCnt; ++i)
		GeFree( objs[i].poiData);
	objsCnt = 0L;
	GeFree( objs);
	
	ViewportSelect::Free( ViSe);
	ViPx = NULL;
	
	AtomArray::Free( aaObjs);
}

bool D1Snap::Init_My_Data( BaseDocument *doc, bool recalcCenter, AtomArray *arr)
{
	if( ! arr)
	{
		arr = AtomArray::Alloc();
		if( ! arr) return false;
		doc->GetActiveObjects( *arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	}
	aaObjs = AtomArray::Alloc();
	if( ! aaObjs) return false;
	LONG cnt = arr->GetCount();
	if (!cnt)	return false;
	objs = (ObjectsDataStru*)GeAlloc( sizeof(ObjectsDataStru) * cnt);
	if( ! objs) return false;
	objsCnt = 0L;
	LONG totalSelCnt = 0L;
	if( recalcCenter)
		vecCentrGlo = Vector();
	Vector vecCentr_tmp = Vector();
	LONG r, p, seg_start, seg_end, i_in_data;
	for(LONG i=0L; i != cnt; ++i)
	{
		if( arr->GetIndex( i)->IsInstanceOf( Opoint))
		{
			vecCentr_tmp =			Vector();
			obj =					ToPoint(arr->GetIndex(i));
			aaObjs->Append( obj);
			objs[objsCnt].pob =		obj;
			objs[objsCnt].bs =		obj->GetPointS();
			objs[objsCnt].selDirt =	objs[objsCnt].bs->GetDirty();
			objs[objsCnt].sel_cnt =	objs[objsCnt].bs->GetCount();
			objs[objsCnt].v_Pnt =	const_cast<Vector*>(obj->GetPointR());
			if( objs[objsCnt].sel_cnt > 0L)
			{
				objs[objsCnt].poiData = (PonitDataStru*)GeAlloc( sizeof( PonitDataStru) * objs[objsCnt].sel_cnt);
				if( ! objs[objsCnt].poiData) return false;
				r = 0L;
				i_in_data = 0L;
				if( recalcCenter)
				{
					while( obj->GetPointS()->GetRange( r, MAXLONGl, &seg_start, &seg_end))
					{
						++r;
						for( p=seg_start; p<=seg_end; p++)
						{
							vecCentr_tmp += ( objs[objsCnt].v_Pnt)[p];
							objs[objsCnt].poiData[ i_in_data].ind = p;
							objs[objsCnt].poiData[ i_in_data].pos = objs[objsCnt].v_Pnt[p];
							i_in_data++;
						}
					}
				}
				else
				{
					while( obj->GetPointS()->GetRange( r, MAXLONGl, &seg_start, &seg_end))
					{
						++r;
						for( p=seg_start; p<=seg_end; p++)
						{
							objs[objsCnt].poiData[ i_in_data].ind = p;
							objs[objsCnt].poiData[ i_in_data].pos = objs[objsCnt].v_Pnt[p];
							i_in_data++;
						}
					}
				}
			}
			if( recalcCenter)
			{
				vecCentrGlo += ( vecCentr_tmp ^ obj->GetMg()) + obj->GetMg().off * objs[objsCnt].sel_cnt;
				totalSelCnt += objs[objsCnt].sel_cnt;
			}
			++objsCnt;
		}
	}
	if( recalcCenter)
		vecCentrGlo /= (Real)totalSelCnt;
	AtomArray::Free( arr);

	if( ! objsCnt)
		return false;

	obj = objs[0].pob;
	
	objMg = obj->GetMg();
	
	vcCntrScr = vcX_Scr = vcY_Scr = vcZ_Scr = Vector();
	overR = overG = overB = false;
	overPix = -1L;
	overR_old = overG_old = overB_old = false;
	draw_from_GetCursorInfo = false;
	ViSe = NULL;
	setOrigObj = NULL;
	aimed = false;
	
	return true;
}

bool D1Snap::CheckForDirty( BaseDocument *doc)
{
	AtomArray *arr =	AtomArray::Alloc();
	if (!arr)			return false;
	doc->GetActiveObjects( *arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	LONG io = 0L;
	LONG cnt = arr->GetCount();
	bool ok = true;
	C4DAtom*	atom = NULL;
	for(LONG i=0L; i != cnt; ++i)
	{
		atom =	arr->GetIndex(i);
		if(!atom->IsInstanceOf( Opoint))	continue;
		if((io < objsCnt) && (atom == objs[io].pob) && (objs[io].selDirt == objs[io].bs->GetDirty()))
		{
			io++;
			continue;
		}
		ok = false;
		break;
	}
	if( io != objsCnt)
		ok = false;
	if( ok)
	{
		AtomArray::Free( arr);
		return true;
	}
	//GePrint( "dirt found"); // temp
	Free_My_Data();
	return Init_My_Data( doc, false, arr);
}

Bool D1Snap::Drag_Interactive( BaseDocument *doc, BaseContainer *data, EditorWindow *win, BaseDraw *bd, const BaseContainer *msg)
{
	if( overR)
		selectedAxis = 0L;
	else if( overG)
		selectedAxis = 1L;
	else if( overB)
		selectedAxis = 2L;

	isdragging =			true;
	aimed =					false;
	Vector vecNewCentrGlo =	vecCentrGlo;
	vcAim =					vecCentrGlo;
	veDifferGlobal =		Vector();
	
	chk = ScenePixelPrepare( doc, bd, data->GetBool( AMa_1DSNAP_TO_INVIS), true);
	if( ! chk)
	{
		isdragging = false;
		Free_My_Data();
		return FALSE;
	}

	LONG i, j;
	BaseContainer device;
	Real dx, dy;
	bool cacheChanged = false;
	BaseObject *aimObj_prev = NULL;
	LONG ind_prev = -1L;

	Vector veDiffer;
	Vector veDifferLocal;
	Vector veTmp;
	Matrix maAxI_m_Mg, maMgI_m_Ax;
	Real rAim_m_AxI_val;
	LONG minDist_pow2;
	ViewportPixel *ViPx_tmp =	NULL;
	BaseObject *aimObj_tmp = NULL;
	LONG crXsnap_tmp;
	LONG crYsnap_tmp;
	LONG dist_pow2;
	LONG objInData;
	Bool ctrl, ctrl_old = FALSE;
	crX = msg->GetLong( BFM_INPUT_X);
	crY = msg->GetLong( BFM_INPUT_Y);
	win->MouseDragStart( KEY_MLEFT, crX, crY, MOUSEDRAGFLAGS_NOMOVE);
	while( win->MouseDrag( &dx, &dy, &device) == MOUSEDRAGRESULT_CONTINUE)
	{
		ctrl = ( device.GetLong( BFM_INPUT_QUALIFIER) & QCTRL);
		if( dx == 0 && dy == 0 && ctrl == ctrl_old)
			continue;
		
		if( cacheChanged)
		{
			ScenePixelFree();
			chk = ScenePixelPrepare( doc, bd, data->GetBool( AMa_1DSNAP_TO_INVIS));		
			if( ! chk) 
			{
				win->MouseDragEnd();
				ActivePixelFree();
				return FALSE;
			}
			cacheChanged = false;
		}
		
		crX += dx;
		crY += dy;
		
		//LONG crXsnap = crX;
		//LONG crYsnap = crY;
		ViPx = NULL;
		
		minDist_pow2 = 1234567L;
		crXsnap_tmp = crX;
		crYsnap_tmp = crY;
		for( i=0L; i != sceneAArC->GetCount(); ++i)
		{
			aimObj_tmp = (BaseObject*)(sceneAArC->GetIndex( i));
			ViPx_tmp = ViSeAll->GetNearestPoint( aimObj_tmp, crXsnap_tmp, crYsnap_tmp, NEAREST_POINT_DIST);
			if( ViPx_tmp)
			{
				dist_pow2 = ( crXsnap_tmp - crX) * ( crXsnap_tmp - crX) + ( crYsnap_tmp - crY) * ( crYsnap_tmp - crY);
				if( dist_pow2 < minDist_pow2)
				{
					minDist_pow2 = dist_pow2;
					ViPx = ViPx_tmp;
					aimObj = aimObj_tmp;
				}
			}
		}
		objInData = -1L;
		for( i=0L; i != objsCnt; ++i)
		{
			ViPx_tmp = ViSeAct->GetNearestPoint( objs[i].pob, crXsnap_tmp, crYsnap_tmp, NEAREST_POINT_DIST);
			if( ViPx_tmp)
			{
				dist_pow2 = ( crXsnap_tmp - crX) * ( crXsnap_tmp - crX) + ( crYsnap_tmp - crY) * ( crYsnap_tmp - crY);
				if( dist_pow2 < minDist_pow2)
				{
					minDist_pow2 = dist_pow2;
					ViPx = ViPx_tmp;
					aimObj = objs[i].pob;
					objInData = i;
				}
			}
		}
		if( ViPx)
		{
			if( aimObj != aimObj_prev || ViPx->i != ind_prev || ctrl != ctrl_old)
			{
				StatusClear();
				StatusSetText( GeLoadString( IDS_AMa_1DSNAP_STR1) + aimObj->GetName() + GeLoadString( IDS_AMa_1DSNAP_STR2) + LongToString( ViPx->i) + "  ");
				aimed = true;
				aimObj_prev = aimObj;
				ind_prev = ViPx->i;
				aimPoObj = ToPoint(aimObj);
				if( objInData != -1L && objs[objInData].bs->IsSelected( ViPx->i))
				{
					vcAim = vecCentrGlo;
					for( i=0L; i != objs[objInData].sel_cnt; ++i)
					{
						if( objs[objInData].poiData[i].ind == ViPx->i)
						{
							vcAim = objs[objInData].poiData[i].pos * objs[objInData].pob->GetMg();
							//GePrint( "ok it is finded !!!");
							break;
						}
					}
				}
				else
					vcAim = (aimPoObj->GetPointR())[ViPx->i] * aimPoObj->GetMg();
				
				veDiffer = ( vcAim - vecCentrGlo) ^ M_Axis_Inv;
				switch( selectedAxis)
				{
					case 0:
						veDiffer.y = 0.0;
						veDiffer.z = 0.0;
						break; 
					case 1:
						veDiffer.x = 0.0;
						veDiffer.z = 0.0;
						break;
					case 2:
						veDiffer.x = 0.0;
						veDiffer.y = 0.0;
						break;
				}
				veDifferGlobal = veDiffer ^ M_Axis;
				
				if( ! ctrl)
				{
					for( i=0L; i != objsCnt; ++i)
					{
						veDifferLocal = veDifferGlobal ^ ! objs[i].pob->GetMg();
						for( j=0L; j != objs[i].sel_cnt; ++j)
							objs[i].v_Pnt[ objs[i].poiData[j].ind] = objs[i].poiData[j].pos + veDifferLocal;
					}
				}
				else
				{
					for( i=0L; i != objsCnt; ++i)
					{
						switch( selectedAxis)
						{
							case 0:
								maAxI_m_Mg =		M_Axis_Inv * objs[i].pob->GetMg();
								maMgI_m_Ax =		!objs[i].pob->GetMg() * M_Axis;
								rAim_m_AxI_val =	( vcAim ^ M_Axis_Inv).x;
								for( j=0L; j != objs[i].sel_cnt; ++j)
								{
									veTmp = objs[i].poiData[j].pos * maAxI_m_Mg;
									veTmp.x = rAim_m_AxI_val;
									objs[i].v_Pnt[ objs[i].poiData[j].ind] = veTmp * maMgI_m_Ax;
								}
								break;
							case 1:
								maAxI_m_Mg = M_Axis_Inv * objs[i].pob->GetMg();
								maMgI_m_Ax = ! objs[i].pob->GetMg() * M_Axis;
								rAim_m_AxI_val = ( vcAim ^ M_Axis_Inv).y;
								for( j=0L; j != objs[i].sel_cnt; ++j)
								{
									veTmp = objs[i].poiData[j].pos * maAxI_m_Mg;
									veTmp.y = rAim_m_AxI_val;
									objs[i].v_Pnt[ objs[i].poiData[j].ind] = veTmp * maMgI_m_Ax;
								}
								break;
							case 2:
								maAxI_m_Mg = M_Axis_Inv * objs[i].pob->GetMg();
								maMgI_m_Ax = ! objs[i].pob->GetMg() * M_Axis;
								rAim_m_AxI_val = ( vcAim ^ M_Axis_Inv).z;
								for( j=0L; j != objs[i].sel_cnt; ++j)
								{
									veTmp = objs[i].poiData[j].pos * maAxI_m_Mg;
									veTmp.z = rAim_m_AxI_val;
									objs[i].v_Pnt[ objs[i].poiData[j].ind] = veTmp * maMgI_m_Ax;
								}
						}
					}
				}
				
				vecNewCentrGlo = vecCentrGlo + veDifferGlobal;
				ctrl_old = ctrl;
				cacheChanged = true;
			}
		}
		else
		{
			aimed = false;
			ind_prev = NULL;
			ind_prev = -1L;
			StatusClear();
		}
		DrawViews( DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_ANIMATION|DRAWFLAGS_NO_THREAD|DRAWFLAGS_ONLY_HIGHLIGHT|DRAWFLAGS_INDRAG);
	} // -- while
	if( win->MouseDragEnd() == MOUSEDRAGRESULT_FINISHED)
	{
		for( i=0L; i != objsCnt; ++i)
			objs[i].pob->Message( MSG_UPDATE);
		EventAdd();
		vecCentrGlo = vecNewCentrGlo;
	}
	else
	{
		chk = doc->DoUndo();
		if( ! chk)
		{
			ScenePixelFree();
			ActivePixelFree();
			return FALSE;
		}
	}
	isdragging = false;
	StatusClear();
	ScenePixelFree();
	ActivePixelFree();
	DrawViews( DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_ANIMATION|DRAWFLAGS_NO_THREAD|DRAWFLAGS_ONLY_HIGHLIGHT|DRAWFLAGS_INDRAG);
	Free_My_Data();
	chk = Init_My_Data( doc, false);
	return chk;
}

Bool D1Snap::MouseInput( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg)
{
	if (!doc) return FALSE;
	if( doc->GetMode() != Mpoints)	return TRUE;
	chk = CheckForDirty( doc);
	if( ! chk) return FALSE;
	///////////////////////////////////////////////////////_click____click____click____click____click____click____click
	//if (!(overR && overG && overB)) 
	if (! overR && ! overG && ! overB)
	{
		if( overPix != -1L)
		{
			vecCentrGlo = setOrigObj->GetPointR()[overPix] * setOrigObj->GetMg();
			draw_from_GetCursorInfo = true;
			DrawViews( DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_ANIMATION|DRAWFLAGS_NO_THREAD|DRAWFLAGS_ONLY_HIGHLIGHT);
			draw_from_GetCursorInfo = false;
			return TRUE;
		}
		else
			return TRUE;
	}
	///////////////////////////_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG_<=>_DRAG
	BaseContainer* pdata = GetToolData( doc, ID_AMa_1D_SNAP);
	if( ! pdata) return FALSE;

	chk = doc->StartUndo();
	if( ! chk) return FALSE;
	for( LONG i=0L; i != objsCnt; ++i)
	{
		chk = doc->AddUndo( UNDOTYPE_CHANGE, objs[i].pob);
		if( ! chk) return FALSE;
	}
	chk = doc->EndUndo();
	if( ! chk) return FALSE;
	
	return Drag_Interactive( doc, pdata, win, bd, &msg);
}

Bool Register_AMa_1dSnap()
{
#if API_VERSION >= 12000
	return RegisterToolPlugin(ID_AMa_1D_SNAP, GeLoadString( IDS_AMa_1DSNAP_MENU), 0L, AutoBitmap("AMa_1D_Snap.tif"), GeLoadString( IDS_AMa_1DSNAP_HLP), gNew D1Snap);
#else
	return RegisterToolPlugin( ID_AMa_1D_SNAP, GeLoadString( IDS_AMa_1DSNAP_MENU), 0 /*PLUGINFLAG_TOOL_TWEAK*/, "AMa_1D_Snap.tif", GeLoadString( IDS_AMa_1DSNAP_HLP), gNew D1Snap);
#endif
}
