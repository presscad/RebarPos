//-----------------------------------------------------------------------------
//----- PosShape.cpp : Implementation of PosShape
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "MgPosShape.h"
#include "Marshal.h"

using namespace OZOZ::RebarPosWrapper;

//*************************************************************************
// Constructors and destructors 
//*************************************************************************
PosShape::PosShape()
: Autodesk::AutoCAD::GraphicsInterface::Drawable(IntPtr(new CPosShape()), true)
{
	m_Shapes = gcnew PosShape::ShapeCollection(this);
}

PosShape::PosShape(System::IntPtr unmanagedPointer, bool autoDelete)
: Autodesk::AutoCAD::GraphicsInterface::Drawable(unmanagedPointer, autoDelete)
{
	m_Shapes = gcnew PosShape::ShapeCollection(this);
}

//*************************************************************************
// Properties
//*************************************************************************
String^ PosShape::Name::get()
{
	return Marshal::WcharToString(GetImpObj()->Name());
}
void PosShape::Name::set(String^ value)
{
	GetImpObj()->setName(Marshal::StringToWchar(value));
}

int PosShape::Fields::get()
{
	return GetImpObj()->Fields();
}
void PosShape::Fields::set(int value)
{
	GetImpObj()->setFields(value);
}

String^ PosShape::Formula::get()
{
	return Marshal::WcharToString(GetImpObj()->Formula());
}
void PosShape::Formula::set(String^ value)
{
	GetImpObj()->setFormula(Marshal::StringToWchar(value));
}

String^ PosShape::FormulaBending::get()
{
    return Marshal::WcharToString(GetImpObj()->FormulaBending());
}
void PosShape::FormulaBending::set(String^ value)
{
	GetImpObj()->setFormulaBending(Marshal::StringToWchar(value));
}

PosShape::ShapeCollection^ PosShape::Items::get()
{
	return m_Shapes;
}

int PosShape::Priority::get()
{
	return GetImpObj()->Priority();
}
void PosShape::Priority::set(int value)
{
	GetImpObj()->setPriority(value);
}

bool PosShape::IsBuiltIn::get()
{
	return (GetImpObj()->IsBuiltIn() == Adesk::kTrue);
}

bool PosShape::IsUnknown::get()
{
    return (GetImpObj()->IsUnknown() == Adesk::kTrue);
}

bool PosShape::IsInternal::get()
{
    return (GetImpObj()->IsInternal() == Adesk::kTrue);
}

void PosShape::SetShapeTexts(String^ a, String^ b, String^ c, String^ d, String^ e, String^ f)
{
	GetImpObj()->setShapeTexts(Marshal::StringToWchar(a), Marshal::StringToWchar(b), Marshal::StringToWchar(c), Marshal::StringToWchar(d), Marshal::StringToWchar(e), Marshal::StringToWchar(f));
}
void PosShape::ClearShapeTexts(void)
{
	GetImpObj()->clearShapeTexts();
}

System::Drawing::Bitmap^ PosShape::ToBitmap(System::Drawing::Color backColor, int width, int height)
{
	AcGsColor color;
	color.m_red = backColor.R;
	color.m_green = backColor.G;
	color.m_blue = backColor.B;
	color.m_filler = backColor.A;
	return System::Drawing::Bitmap::FromHbitmap(System::IntPtr(GetImpObj()->ToBitmap(color, width, height)));
}

System::Drawing::Bitmap^ PosShape::ToBitmap(Device^ device, View^ view, Model^ model, System::Drawing::Color backColor, int width, int height)
{
	AcGsColor color;
	color.m_red = backColor.R;
	color.m_green = backColor.G;
	color.m_blue = backColor.B;
	color.m_filler = backColor.A;

	AcGsDevice* pdevice = static_cast<AcGsDevice*>(device->UnmanagedObject.ToPointer());
	AcGsView* pview = static_cast<AcGsView*>(view->UnmanagedObject.ToPointer());
	AcGsModel* pmodel = static_cast<AcGsModel*>(model->UnmanagedObject.ToPointer());

	return System::Drawing::Bitmap::FromHbitmap(System::IntPtr(GetImpObj()->ToBitmap(pdevice, pview, pmodel, color, width, height)));
}

//*************************************************************************
// Drawable implementation
//*************************************************************************
ObjectId PosShape::Id::get()
{ 
	//return Marshal::ToObjectId(GetImpObj()->id()); 
	return ObjectId::Null;
}
bool PosShape::IsPersistent::get() 
{ 
	//return (GetImpObj()->isPersistent() == Adesk::kTrue); 
	return false;
}

int PosShape::SubSetAttributes(Autodesk::AutoCAD::GraphicsInterface::DrawableTraits^ traits) 
{
	return GetImpObj()->subSetAttributes(static_cast<AcGiDrawableTraits*>(traits->UnmanagedObject.ToPointer()));
}
void PosShape::SubViewportDraw(Autodesk::AutoCAD::GraphicsInterface::ViewportDraw^ vd) 
{
	GetImpObj()->subViewportDraw(static_cast<AcGiViewportDraw*>(vd->UnmanagedObject.ToPointer()));
}
int PosShape::SubViewportDrawLogicalFlags(Autodesk::AutoCAD::GraphicsInterface::ViewportDraw^ vd) 
{
	return GetImpObj()->subViewportDrawLogicalFlags(static_cast<AcGiViewportDraw*>(vd->UnmanagedObject.ToPointer()));
}
bool PosShape::SubWorldDraw(Autodesk::AutoCAD::GraphicsInterface::WorldDraw^ wd) 
{
	return (GetImpObj()->subWorldDraw(static_cast<AcGiWorldDraw*>(wd->UnmanagedObject.ToPointer())) == Adesk::kTrue);
}

//*************************************************************************
// Static Methods
//*************************************************************************
void PosShape::AddPosShape(PosShape^ shape)
{
	assert(!shape->IsBuiltIn);
	assert(!shape->IsInternal);
	CPosShape::AddPosShape(shape->GetImpObj());
}

PosShape^ PosShape::GetPosShape(String^ name)
{
	CPosShape* shape = CPosShape::GetPosShape(Marshal::StringToWstring(name));
	return gcnew PosShape(IntPtr(shape), false);
}

PosShape^ PosShape::GetUnknownPosShape()
{
	CPosShape* shape = CPosShape::GetUnknownPosShape();
	return gcnew PosShape(IntPtr(shape), true);
}

bool PosShape::HasPosShape(String^ name)
{
	return CPosShape::HasPosShape(Marshal::StringToWstring(name));
}

int PosShape::GetPosShapeCount()
{
	return CPosShape::GetPosShapeCount(true, false, true);
}

System::Collections::Generic::List<String^>^ PosShape::GetAllPosShapes()
{
	System::Collections::Generic::List<String^>^ dict = gcnew System::Collections::Generic::List<String^>();
	std::vector<std::wstring> map = CPosShape::GetAllShapes(true, false, true);
	for(std::vector<std::wstring>::iterator it = map.begin(); it != map.end(); it++)
	{
		dict->Add(Marshal::WstringToString((*it)));
	}
	return dict;
}

void PosShape::ClearPosShapes()
{
	CPosShape::ClearPosShapes(false, false, true);
}

void PosShape::ReadPosShapesFromFile(String^ source)
{
	CPosShape::ReadPosShapesFromFile(Marshal::StringToWstring(source));
}

void PosShape::SavePosShapesToFile(String^ source)
{
	CPosShape::SavePosShapesToFile(Marshal::StringToWstring(source));
}
