//-----------------------------------------------------------------------------
//----- BOQTable.h : Declaration of the CBOQTable
//-----------------------------------------------------------------------------
#pragma once

#pragma warning( push )
#pragma warning( disable : 4100 )
#include "acgi.h"
#pragma warning( pop )

#include <vector>
#include <map>
#include "BOQRow.h"
#include "DrawParams.h"
#include "GenericTable.h"

// The following is part of the code used to export an API
// and/or use the exported API.
//
#pragma warning( disable: 4275 4251 )
#ifdef REBARPOS_MODULE
#define DLLIMPEXP __declspec( dllexport )
#else
#define DLLIMPEXP
#endif

typedef std::vector<CBOQRow*> RowList;
typedef std::vector<CBOQRow*>::size_type RowListSize;
typedef std::vector<CBOQRow*>::iterator RowListIt;
typedef std::vector<CBOQRow*>::const_iterator RowListConstIt;

/// ---------------------------------------------------------------------------
/// The CBOQTable represents the BOQ list of rebar markers in the drawing.
/// ---------------------------------------------------------------------------
class DLLIMPEXP CBOQTable : public  CGenericTable
{
public:
	/// Define additional RTT information for AcRxObject base type.
    ACRX_DECLARE_MEMBERS(CBOQTable);
    
protected:
	/// Object version number serialized in the drawing database.
	static Adesk::UInt32 kCurrentVersionNumber;

public:
    /// Constructors and destructor    
    CBOQTable();
    virtual ~CBOQTable();
       
public:
	enum ColumnType
	{ 
		NONE = 0,
		POS = 1,
		POSDD = 2,
		COUNT = 3,
		DIAMETER = 4,
		LENGTH = 5,
		SHAPE = 6,
		TOTALLENGTH = 7
	};

	enum DrawingUnits
	{ 
		MM = 0,
		CM = 1,
		DM = 2,
		M = 3,
	};

private:
	/// Property backing fields
	Adesk::Int32 m_Multiplier;

	ACHAR* m_Heading;
	ACHAR* m_Footing;
	ACHAR* m_Note;

	Adesk::Int32 m_Precision;

	DrawingUnits m_DisplayUnit;

	ACHAR* m_Columns;

	Adesk::UInt16 m_TextColor;
	Adesk::UInt16 m_PosColor;
	Adesk::UInt16 m_LineColor;
	Adesk::UInt16 m_SeparatorColor;
	Adesk::UInt16 m_BorderColor;
	Adesk::UInt16 m_HeadingColor;
	Adesk::UInt16 m_FootingColor;

	ACHAR* m_PosLabel;
	ACHAR* m_CountLabel;
	ACHAR* m_DiameterLabel;
	ACHAR* m_LengthLabel;
	ACHAR* m_ShapeLabel;
	ACHAR* m_TotalLengthLabel;
	ACHAR* m_DiameterListLabel;
	ACHAR* m_DiameterLengthLabel;
	ACHAR* m_UnitWeightLabel;
	ACHAR* m_WeightLabel;
	ACHAR* m_GrossWeightLabel;
	ACHAR* m_MultiplierHeadingLabel;

    AcDbHardPointerId m_TextStyleId;
    AcDbHardPointerId m_HeadingStyleId;
    AcDbHardPointerId m_FootingStyleId;

	double m_HeadingScale;
	double m_FootingScale;
	double m_RowSpacing;

	Adesk::UInt32 m_MaxRows;
	double m_TableSpacing;

	RowList m_List;

private:
	int suspendCount;
	bool needsUpdate;

protected:
	/// Calculates table when modified.
	void UpdateTable(void);

private:
	/// Parses column definition text
	const std::vector<ColumnType> ParseColumns(const ACHAR* columns) const;

public:
	virtual void SuspendUpdate(void);
	virtual void ResumeUpdate(void);

public:
	/// Gets or sets the BOQ multiplier
	const Adesk::Int32 Multiplier(void) const;
	Acad::ErrorStatus setMultiplier(const Adesk::Int32 newVal);

	/// Gets or sets heading text
	const ACHAR* Heading(void) const;
	Acad::ErrorStatus setHeading(const ACHAR* newVal);

	/// Gets or sets heading text
	const ACHAR* Footing(void) const;
	Acad::ErrorStatus setFooting(const ACHAR* newVal);

	/// Gets or sets note text
	const ACHAR* Note(void) const;
	Acad::ErrorStatus setNote(const ACHAR* newVal);

	/// Gets or sets the display precision
    const Adesk::Int32 Precision(void) const;
	Acad::ErrorStatus setPrecision(const Adesk::Int32 newVal);

	/// Gets or sets the display unit
    const DrawingUnits DisplayUnit(void) const;
	Acad::ErrorStatus setDisplayUnit(const DrawingUnits newVal);

	/// Gets or sets the column definition.
    const ACHAR* ColumnDef(void) const;
	Acad::ErrorStatus setColumnDef(const ACHAR* newVal);

	/// Gets or sets the text color.
	const Adesk::UInt16 TextColor(void) const;
	Acad::ErrorStatus setTextColor(const Adesk::UInt16 newVal);

	/// Gets or sets the pos text color.
	const Adesk::UInt16 PosColor(void) const;
	Acad::ErrorStatus setPosColor(const Adesk::UInt16 newVal);

	/// Gets or sets the line color.
	const Adesk::UInt16 LineColor(void) const;
	Acad::ErrorStatus setLineColor(const Adesk::UInt16 newVal);

	/// Gets or sets the separator line color.
	const Adesk::UInt16 SeparatorColor(void) const;
	Acad::ErrorStatus setSeparatorColor(const Adesk::UInt16 newVal);

	/// Gets or sets the border color.
	const Adesk::UInt16 BorderColor(void) const;
	Acad::ErrorStatus setBorderColor(const Adesk::UInt16 newVal);

	/// Gets or sets the heading text color.
	const Adesk::UInt16 HeadingColor(void) const;
	Acad::ErrorStatus setHeadingColor(const Adesk::UInt16 newVal);

	/// Gets or sets the footing text color.
	const Adesk::UInt16 FootingColor(void) const;
	Acad::ErrorStatus setFootingColor(const Adesk::UInt16 newVal);

	// Get labels
	const ACHAR* PosLabel(void) const;
	const ACHAR* CountLabel(void) const;
	const ACHAR* DiameterLabel(void) const;
	const ACHAR* LengthLabel(void) const;
	const ACHAR* ShapeLabel(void) const;
	const ACHAR* TotalLengthLabel(void) const;
	const ACHAR* DiameterListLabel(void) const;
	const ACHAR* DiameterLengthLabel(void) const;
	const ACHAR* UnitWeightLabel(void) const;
	const ACHAR* WeightLabel(void) const;
	const ACHAR* GrossWeightLabel(void) const;
	// Set labels
	Acad::ErrorStatus setPosLabel(const ACHAR* newVal);
	Acad::ErrorStatus setCountLabel(const ACHAR* newVal);
	Acad::ErrorStatus setDiameterLabel(const ACHAR* newVal);
	Acad::ErrorStatus setLengthLabel(const ACHAR* newVal);
	Acad::ErrorStatus setShapeLabel(const ACHAR* newVal);
	Acad::ErrorStatus setTotalLengthLabel(const ACHAR* newVal);
	Acad::ErrorStatus setDiameterLengthLabel(const ACHAR* newVal);
	Acad::ErrorStatus setDiameterListLabel(const ACHAR* newVal);
	Acad::ErrorStatus setUnitWeightLabel(const ACHAR* newVal);
	Acad::ErrorStatus setWeightLabel(const ACHAR* newVal);
	Acad::ErrorStatus setGrossWeightLabel(const ACHAR* newVal);

	/// Gets or sets the heading for tables with multiplier > 1
	const ACHAR* MultiplierHeadingLabel(void) const;
	Acad::ErrorStatus setMultiplierHeadingLabel(const ACHAR* newVal);

	/// Gets or sets pointer to the text style.
	const AcDbObjectId& TextStyleId(void) const;
	Acad::ErrorStatus setTextStyleId(const AcDbObjectId& newVal);

	/// Gets or sets pointer to the heading style.
	const AcDbObjectId& HeadingStyleId(void) const;
	Acad::ErrorStatus setHeadingStyleId(const AcDbObjectId& newVal);

	/// Gets or sets pointer to the footing style.
	const AcDbObjectId& FootingStyleId(void) const;
	Acad::ErrorStatus setFootingStyleId(const AcDbObjectId& newVal);

	/// Gets or sets the heading scale relative to text height.
	const double HeadingScale(void) const;
	Acad::ErrorStatus setHeadingScale(const double newVal);

	/// Gets or sets the footing scale relative to text height.
	const double FootingScale(void) const;
	Acad::ErrorStatus setFootingScale(const double newVal);

	/// Gets or sets the row spacing.
	const double RowSpacing(void) const;
	Acad::ErrorStatus setRowSpacing(const double newVal);

	/// Gets or sets the maximum number of rows per table.
    const Adesk::UInt32 MaxRows(void) const;
	Acad::ErrorStatus setMaxRows(const Adesk::UInt32 newVal);

	/// Gets or sets the spacing between tables.
	const double TableSpacing(void) const;
	Acad::ErrorStatus setTableSpacing(const double newVal);

public:
	/// Adds a row.
	void AddRow(CBOQRow* const row);

	/// Gets the row at the given index.
	const CBOQRow* GetRow(const RowListSize index) const;

	/// Sets the row at the given index.
	void SetRow(const RowListSize index, CBOQRow* const row);

	/// Gets the count of rows.
	const RowListSize GetRowCount() const;

	/// Removes the row at the given index.
	void RemoveRow(const RowListSize index);

	/// Clears all rows.
	void ClearRows();

public:
	/// Forces a view update.
	const void Update(void);

public:
	/// AcDbEntity overrides: database    
    virtual Acad::ErrorStatus	dwgInFields(AcDbDwgFiler* filer);
    virtual Acad::ErrorStatus	dwgOutFields(AcDbDwgFiler* filer) const;
    
    virtual Acad::ErrorStatus	dxfInFields(AcDbDxfFiler* filer);
    virtual Acad::ErrorStatus	dxfOutFields(AcDbDxfFiler* filer) const;

protected:
	/// AcDbEntity overrides: geometry
    virtual Acad::ErrorStatus subGetOsnapPoints(
        AcDb::OsnapMode       osnapMode,
        Adesk::GsMarker       gsSelectionMark,
        const AcGePoint3d&    pickPoint,
        const AcGePoint3d&    lastPoint,
        const AcGeMatrix3d&   viewXform,
        AcGePoint3dArray&     snapPoints,
        AcDbIntArray&         geomIds) const;

    virtual Acad::ErrorStatus   subGetGripPoints(AcGePoint3dArray&     gripPoints,
        AcDbIntArray&  osnapModes,
        AcDbIntArray&  geomIds) const;

    virtual Acad::ErrorStatus   subMoveGripPointsAt(const AcDbIntArray& indices,
        const AcGeVector3d&     offset);

    virtual Acad::ErrorStatus   subTransformBy(const AcGeMatrix3d& xform);

    virtual void                subList() const;

    virtual Acad::ErrorStatus	subExplode(AcDbVoidPtrArray& entitySet) const;

    virtual Adesk::Boolean      subWorldDraw(AcGiWorldDraw*	mode);

    /// Overridden methods from AcDbObject    
    virtual Acad::ErrorStatus subDeepClone(AcDbObject* pOwnerObject,
        AcDbObject*& pClonedObject,
        AcDbIdMapping& idMap,
        Adesk::Boolean isPrimary
        = Adesk::kTrue) const;
    
    virtual Acad::ErrorStatus subWblockClone(AcRxObject* pOwnerObject,
        AcDbObject*& pClonedObject,
        AcDbIdMapping& idMap,
        Adesk::Boolean isPrimary
        = Adesk::kTrue) const;
    
    virtual Acad::ErrorStatus subGetClassID(CLSID* pClsid) const;

private:
    // These are here because otherwise dllexport tries to export the
    // private methods of AcDbObject.  They're private in AcDbObject
    // because vc5 and vc6 do not properly support array new and delete.
    // The "vector deleting dtor" gets optimized into a scalar deleting
    // dtor by the linker if the server dll does not call vector delete.
    // The linker shouldn't do that, because it doesn't know that the
    // object won't be passed to some other dll which *does* do vector
    // delete.
    //
#ifdef MEM_DEBUG
#undef new
#undef delete
#endif
    void *operator new[](size_t /* nSize */) { return 0;}
    void operator delete[](void* /* p */) {};
    void *operator new[](size_t /* nSize */, const TCHAR* /* file*/ , int /* line */) { return 0;}
#ifdef MEM_DEBUG
#define new DEBUG_NEW
#define delete DEBUG_DELETE
#endif
};
