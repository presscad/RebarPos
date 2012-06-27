//-----------------------------------------------------------------------------
//----- GenericTable.cpp : Implementation of CGenericTable
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "GenericTable.h"
#include "Utility.h"

#include <algorithm>

Adesk::UInt32 CGenericTable::kCurrentVersionNumber = 1;

//*************************************************************************
// Code for the Class Body. 
//*************************************************************************

ACRX_DXF_DEFINE_MEMBERS(CGenericTable, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kAllAllowedBits, GENERICTABLE,
	"OZOZRebarPos\
	|Product Desc:     Generic Table Entity\
	|Company:          OZOZ");

//*************************************************************************
// Constructors and destructors 
//*************************************************************************

CGenericTable::CGenericTable(void) : 
	m_BasePoint(0, 0, 0), geomInit(false), ucs(AcGeMatrix3d::kIdentity),
	m_Direction(1, 0, 0), m_Up(0, 1, 0),
	m_Rows(0), m_Columns(0), m_Cells(0),
	columnWidths(), rowHeights(), isModified(true),
	m_CellMargin(0.2), m_MaxHeight(0), m_TableSpacing(2.0),
	m_Width(0), m_Height(0)
{
}

CGenericTable::~CGenericTable(void)
{
	Clear();
	ResetDrawParams();
}

//*************************************************************************
// Properties
//*************************************************************************
const AcGeVector3d& CGenericTable::DirectionVector(void) const
{
	assertReadEnabled();
	return m_Direction;
}

const AcGeVector3d& CGenericTable::UpVector(void) const
{
	assertReadEnabled();
	return m_Up;
}

const AcGeVector3d CGenericTable::NormalVector(void) const
{
	assertReadEnabled();
	AcGeVector3d norm = m_Direction.crossProduct(m_Up);
	norm = norm.normal() * m_Direction.length();
	return norm;
}

const double CGenericTable::Scale(void) const
{
	assertReadEnabled();
	return m_Direction.length();
}
Acad::ErrorStatus CGenericTable::setScale(const double newVal)
{
	assertWriteEnabled();
	AcGeMatrix3d xform = AcGeMatrix3d::scaling(newVal / m_Direction.length(), m_BasePoint);
	return transformBy(xform);
}

const int CGenericTable::Columns(void) const
{
	assertReadEnabled();
	return m_Columns;
}
const int CGenericTable::Rows(void) const
{
	assertReadEnabled();
	return m_Rows;
}

const double CGenericTable::MaxHeight(void) const
{
	assertReadEnabled();
	return m_MaxHeight * m_Direction.length();
}
Acad::ErrorStatus CGenericTable::setMaxHeight(const double newVal)
{
	assertWriteEnabled();
	m_MaxHeight = newVal / m_Direction.length();
	isModified = true;
	return Acad::eOk;
}

const double CGenericTable::TableSpacing(void) const
{
	assertReadEnabled();
	return m_TableSpacing * m_Direction.length();
}
Acad::ErrorStatus CGenericTable::setTableSpacing(const double newVal)
{
	assertWriteEnabled();
	m_TableSpacing = newVal / m_Direction.length();
	isModified = true;
	return Acad::eOk;
}

const double CGenericTable::CellMargin(void) const
{
	assertReadEnabled();
	return m_CellMargin;
}
Acad::ErrorStatus CGenericTable::setCellMargin(const double newVal)
{
	assertWriteEnabled();
	for(std::vector<CTableCell*>::iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		(*it)->setMargin(newVal);
	}
	isModified = true;
	return Acad::eOk;
}

const AcGePoint3d& CGenericTable::BasePoint(void) const
{
	assertReadEnabled();
	return m_BasePoint;
}
Acad::ErrorStatus CGenericTable::setBasePoint(const AcGePoint3d& newVal)
{
	assertWriteEnabled();
	m_BasePoint = newVal;
	return Acad::eOk;
}

const double CGenericTable::Width(void) const
{
	assertReadEnabled();
	if(isModified)
	{
		Calculate();
	}
	return m_Width * m_Direction.length();
}

const double CGenericTable::Height(void) const
{
	assertReadEnabled();
	if(isModified)
	{
		Calculate();
	}
	return m_Height * m_Direction.length();
}

//*************************************************************************
// Methods
//*************************************************************************

void CGenericTable::SetSize(int rows, int columns)
{
	assertWriteEnabled();

	Clear();

	m_Cells.resize(rows * columns);
	minColumnWidths.resize(columns);
	minRowHeights.resize(rows);

	// Create new cells
	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < columns; j++)
		{
			m_Cells[i * columns + j] = new CTableCell();
		}
	}

	m_Rows = rows;
	m_Columns = columns;

	isModified = true;
}

void CGenericTable::Clear()
{
	assertWriteEnabled();

	for(std::vector<CTableCell*>::iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
		delete (*it);

	m_Cells.clear();

	isModified = true;
}

void CGenericTable::setMinimumColumnWidth(const int j, const double newVal)
{
	assertWriteEnabled();
	minColumnWidths[j] = newVal;
	isModified = true;
}

void CGenericTable::setMinimumRowHeight(const int i, const double newVal)
{
	assertWriteEnabled();
	minRowHeights[i] = newVal;
	isModified = true;
}

//*************************************************************************
// Cell Access Methods
//*************************************************************************
void CGenericTable::setCellText(const int i, const int j, const ACHAR* newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setText(newVal);
	isModified = true;
}
void CGenericTable::setCellShapeId(const int i, const int j, const AcDbObjectId& newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setShapeId(newVal);
	isModified = true;
}
void CGenericTable::setCellShapeText(const int i, const int j, const ACHAR* a, const ACHAR* b, const ACHAR* c, const ACHAR* d, const ACHAR* e, const ACHAR* f)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setShapeText(a, b, c, d, e, f);
	isModified = true;
}
void CGenericTable::setCellTextColor(const int i, const int j, const unsigned short newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setTextColor(newVal);
	isModified = true;
}
void CGenericTable::setCellTextStyleId(const int i, const int j, const AcDbObjectId& newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setTextStyleId(newVal);
	isModified = true;
}
void CGenericTable::setCellTextHeight(const int i, const int j, const double newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setTextHeight(newVal);
	isModified = true;
}

void CGenericTable::setCellLeftBorder(const int i, const int j, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setLeftBorder(hasBorder);
	cell->setLeftBorderColor(borderColor);
	cell->setLeftBorderDouble(isdouble);
	isModified = true;
}
void CGenericTable::setCellRightBorder(const int i, const int j, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setRightBorder(hasBorder);
	cell->setRightBorderColor(borderColor);
	cell->setRightBorderDouble(isdouble);
	isModified = true;
}
void CGenericTable::setCellTopBorder(const int i, const int j, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setTopBorder(hasBorder);
	cell->setTopBorderColor(borderColor);
	cell->setTopBorderDouble(isdouble);
	isModified = true;
}
void CGenericTable::setCellBottomBorder(const int i, const int j, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setBottomBorder(hasBorder);
	cell->setBottomBorderColor(borderColor);
	cell->setBottomBorderDouble(isdouble);
	isModified = true;
}

void CGenericTable::setRowTopBorder(const int i, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	for(int j = 0; j < m_Columns; j++)
	{
		CTableCell* cell = m_Cells[i * m_Columns + j];
		cell->setTopBorder(hasBorder);
		cell->setTopBorderColor(borderColor);
		cell->setTopBorderDouble(isdouble);
	}
	isModified = true;
}
void CGenericTable::setRowBottomBorder(const int i, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	for(int j = 0; j < m_Columns; j++)
	{
		CTableCell* cell = m_Cells[i * m_Columns + j];
		cell->setBottomBorder(hasBorder);
		cell->setBottomBorderColor(borderColor);
		cell->setBottomBorderDouble(isdouble);
	}
	isModified = true;
}

void CGenericTable::setColumnLeftBorder(const int j, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	for(int i = 0; i < m_Rows; i++)
	{
		CTableCell* cell = m_Cells[i * m_Columns + j];
		cell->setLeftBorder(hasBorder);
		cell->setLeftBorderColor(borderColor);
		cell->setLeftBorderDouble(isdouble);
	}
	isModified = true;
}
void CGenericTable::setColumnRightBorder(const int j, const bool hasBorder, const unsigned short borderColor, bool isdouble)
{
	for(int i = 0; i < m_Rows; i++)
	{
		CTableCell* cell = m_Cells[i * m_Columns + j];
		cell->setRightBorder(hasBorder);
		cell->setRightBorderColor(borderColor);
		cell->setRightBorderDouble(isdouble);
	}
	isModified = true;
}

void CGenericTable::setCellHorizontalAlignment(const int i, const int j, const CTableCell::Alignment newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setHorizontalAlignment(newVal);
	isModified = true;
}
void CGenericTable::setCellVerticalAlignment(const int i, const int j, const CTableCell::Alignment newVal)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setVerticalAlignment(newVal);
	isModified = true;
}

void CGenericTable::MergeAcross(const int i, const int j, const int span)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setMergeRight(span);
	
	if(span > 1)
	{
		// Clear texts
		for(int k = j + 1; k < j + span; k++)
		{
			m_Cells[i * m_Columns + k]->setText(L"");
		}
		// Clear interior borders
		for(int k = j + 1; k < j + span; k++)
		{
			m_Cells[i * m_Columns + k]->setLeftBorder(false);
		}
		for(int k = j; k < j + span - 1; k++)
		{
			m_Cells[i * m_Columns + k]->setRightBorder(false);
		}
	}
	isModified = true;
}
void CGenericTable::MergeDown(const int i, const int j, const int span)
{
	CTableCell* cell = m_Cells[i * m_Columns + j];
	cell->setMergeDown(span);
	
	if(span > 1)
	{
		// Clear texts
		for(int k = i + 1; k < i + span; k++)
		{
			m_Cells[k * m_Columns + j]->setText(L"");
		}
		// Clear interior borders
		for(int k = i + 1; k < i + span; k++)
		{
			m_Cells[k * m_Columns + j]->setTopBorder(false);
		}
		for(int k = i; k < i + span - 1; k++)
		{
			m_Cells[k * m_Columns + j]->setBottomBorder(false);
		}
	}
	isModified = true;
}

//*************************************************************************
// Helper Methods
//*************************************************************************
const void CGenericTable::ResetDrawParams(void) const
{
	columnWidths.clear();
	rowHeights.clear();
}

const void CGenericTable::Calculate(void) const
{
	if(!isModified)
	{
		return;
	}

	assertReadEnabled();

	// Current UCS
	if(!geomInit)
	{
		acdbUcsMatrix(ucs);
		geomInit = true;
	}

	// Reset old params
	ResetDrawParams();

	// Set minimum column sizes
	columnWidths.resize(m_Columns);
	for(int j = 0; j < m_Columns; j++)
	{
		columnWidths[j] = 2.0 * m_CellMargin;
	}
	for(int j = 0; j < m_Columns; j++)
	{
		for(int i = 0; i < m_Rows; i++)
		{
			CTableCell* cell = m_Cells[i * m_Columns + j];

			AcGePoint2d pt = cell->MeasureContents();
			double w = pt.x;
			double h = pt.y;
			
			if(cell->MergeRight() == 0)
			{
				columnWidths[j] = max(columnWidths[j], w);
			}
			else
			{
				int span = cell->MergeRight();
				if(j + span - 1 > m_Columns - 1) span = m_Columns - j;
				w /= (double)span;
				for(int k = j; k < j + span; k++)
				{
					columnWidths[k] = max(columnWidths[k], w);
				}
			}
		}
		columnWidths[j] = max(columnWidths[j], minColumnWidths[j]);
	}

	// Set minimum row sizes
	rowHeights.resize(m_Rows);
	for(int i = 0; i < m_Rows; i++)
	{
		rowHeights[i] = 2.0 * m_CellMargin;
	}
	for(int i = 0; i < m_Rows; i++)
	{
		for(int j = 0; j < m_Columns; j++)
		{
			CTableCell* cell = m_Cells[i * m_Columns + j];

			AcGePoint2d pt = cell->MeasureContents();
			double w = pt.x;
			double h = pt.y;

			if(cell->MergeDown() == 0)
			{
				rowHeights[i] = max(rowHeights[i], h);
			}
			else
			{
				int span = cell->MergeDown();
				if(i + span - 1 > m_Rows - 1) span = m_Rows - i;
				h /= (double)span;
				for(int k = i; k < i + span; k++)
				{
					rowHeights[k] = max(rowHeights[k], h);
				}
			}
		}
		rowHeights[i] = max(rowHeights[i], minRowHeights[i]);
	}

	// Calculate table size
	m_Width = 0;
	for(std::vector<double>::iterator it = columnWidths.begin(); it != columnWidths.end(); it++)
	{
		m_Width += (*it);
	}
	m_Height = 0;
	for(std::vector<double>::iterator it = rowHeights.begin(); it != rowHeights.end(); it++)
	{
		m_Height += (*it);
	}

	// Calculate rows to divide table at
	int ntables = 1;
	std::vector<bool> dividers;
	dividers.resize(m_Rows);
	for(std::vector<bool>::iterator it = dividers.begin(); it != dividers.end(); it++)
	{
		(*it) = false;
	}

	if(m_MaxHeight > 0.001)
	{
		double y = m_MaxHeight;
		int sr = DivideAt(y);
		while(sr > 0 && sr < m_Rows)
		{
			dividers[sr] = true;
			y += m_MaxHeight;
			sr = DivideAt(y);
		}
	}

	// Set cell sizes and location
	double tableoffset = 0;
	double x = 0;
	double y = 0;
	for(int i = 0; i < m_Rows; i++)
	{
		if(dividers[i])
		{
			ntables++;
			tableoffset += m_Width + m_TableSpacing;
			x = tableoffset;
			y = 0;
		}

		for(int j = 0; j < m_Columns; j++)
		{
			CTableCell* cell = m_Cells[i * m_Columns + j];

			double h = rowHeights[i];
			double w = columnWidths[j];
			if(cell->MergeRight() != 0)
			{
				int span =cell->MergeRight();
				if(j + span - 1 > m_Columns - 1) span = m_Columns - j;
				w = 0;
				for(int k = j ; k < j + span; k++)
				{
					w += columnWidths[k];
				}
			}
			if(cell->MergeDown() != 0)
			{
				int span = cell->MergeDown();
				if(i + span - 1 > m_Rows - 1) span = m_Rows - i;
				h = 0;
				for(int k = i ; k < i + span; k++)
				{
					h += rowHeights[k];
				}
			}

			cell->setBasePoint(AcGePoint3d(x, y, 0));
			cell->setWidth(w);
			cell->setHeight(h);

			x += columnWidths[j];
		}

		x = tableoffset;
		y -= rowHeights[i];
	}

	// Transform cells
	AcGeMatrix3d trans = AcGeMatrix3d::kIdentity;
	trans.setCoordSystem(m_BasePoint, m_Direction, m_Up, NormalVector());
	for(std::vector<CTableCell*>::const_iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->transformBy(trans);
	}

	// Recalculate table size
	tableoffset = 0;
	x = 0;
	y = 0;
	m_Width = 0;
	m_Height = 0;
	for(int i = 0; i < m_Rows; i++)
	{
		double h = rowHeights[i];

		if(dividers[i])
		{
			ntables++;
			tableoffset += m_Width + m_TableSpacing;
			x = tableoffset;
			y = 0;
		}

		for(int j = 0; j < m_Columns; j++)
		{
			double w = columnWidths[j];

			m_Width = max(m_Width, x + columnWidths[j]);
			m_Height = max(m_Height, abs(y - rowHeights[i]));

			x += columnWidths[j];
		}
		x = tableoffset;
		y -= rowHeights[i];
	}

	// Done update
	isModified = false;
}

const int CGenericTable::DivideAt(double& y) const
{
	int r = 0;
	double yt = 0;
	for(int i = 0; i < (int)rowHeights.size(); i++)
	{
		yt += rowHeights[i];
		if(yt > y)
		{
			if(i <= 1) return 0;
			r = i - 1;
			yt -= rowHeights[i];
			break;
		}
	}
	if(r > m_Rows - 1) return 0;
	for(int j = 0; j < m_Columns; j++)
	{
		for(int i = 0; i < r; i++)
		{
			CTableCell* cell = m_Cells[i * m_Columns + j];
			int span = cell->MergeDown();
			if(span > 0)
			{
				if(i + span > r) r = i;
			}
		}
	}
	y = yt;
	return r;
}

//*************************************************************************
// Overridden methods from AcDbEntity
//*************************************************************************
Acad::ErrorStatus CGenericTable::subGetOsnapPoints(
    AcDb::OsnapMode       osnapMode,
    Adesk::GsMarker       gsSelectionMark,
    const AcGePoint3d&    pickPoint,
    const AcGePoint3d&    lastPoint,
    const AcGeMatrix3d&   viewXform,
    AcGePoint3dArray&     snapPoints,
    AcDbIntArray&         geomIds) const
{
	assertReadEnabled();

	if(isModified)
	{
		Calculate();
	}

	if(osnapMode == AcDb::kOsModeIns)
	{
		snapPoints.append(m_BasePoint);
	}

	for(std::vector<CTableCell*>::const_iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
	}

	return Acad::eOk;
}

Acad::ErrorStatus CGenericTable::subGetGripPoints(
    AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes,
    AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_BasePoint);
	return Acad::eOk;
}

Acad::ErrorStatus CGenericTable::subMoveGripPointsAt(
    const AcDbIntArray& indices,
    const AcGeVector3d& offset)
{
    if(indices.length() == 0 || offset.isZeroLength())
        return Acad::eOk;

	assertWriteEnabled();

	transformBy(AcGeMatrix3d::translation(offset));

	return Acad::eOk;
}

Acad::ErrorStatus CGenericTable::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	
	AcGeMatrix3d cellTrans(xform);

	m_BasePoint.transformBy(xform);
	m_Direction.transformBy(xform);
	m_Up.transformBy(xform);

	// Get UCS vectors
	AcGeVector3d ucsdir(m_Direction);
	ucsdir.transformBy(ucs);
	AcGeVector3d ucsup(m_Up);
	ucsup.transformBy(ucs);

	// Set mirror base point to middle of entity
	AcGePoint3d pt(m_Width / 2.0, -m_Height / 2.0, 0);
	
	// Transform to WCS
	AcGeMatrix3d trans = AcGeMatrix3d::kIdentity;
	trans.setCoordSystem(m_BasePoint, m_Direction, m_Up, NormalVector());
	pt.transformBy(trans);

	// Text always left to right
	if(ucsdir.x < 0)
	{
		// Mirror around vertical axis
		AcGeMatrix3d mirror = AcGeMatrix3d::kIdentity;
		mirror.setToMirroring(AcGeLine3d(pt, m_Up));
		cellTrans.preMultBy(mirror);
		m_BasePoint.transformBy(mirror);
		m_Direction.transformBy(mirror);
		m_Up.transformBy(mirror);
	}

	// Text always upright
	if(ucsup.y < 0)
	{
		// Mirror around horizontal axis
		AcGeMatrix3d mirror = AcGeMatrix3d::kIdentity;
		mirror.setToMirroring(AcGeLine3d(pt, m_Direction));
		cellTrans.preMultBy(mirror);
		m_BasePoint.transformBy(mirror);
		m_Direction.transformBy(mirror);
		m_Up.transformBy(mirror);
	}

	// Transform cells
	for(std::vector<CTableCell*>::iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->transformBy(cellTrans);
	}

	// Correct direction vectors
	double scale = m_Direction.length();
	m_Direction = m_Direction.normal() * scale;
	m_Up = m_Up.normal() * scale;

	return Acad::eOk;
}

Acad::ErrorStatus CGenericTable::subExplode(AcDbVoidPtrArray& entitySet) const
{
    assertReadEnabled();

	if(isModified)
	{
		Calculate();
	}

	for(std::vector<CTableCell*>::const_iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->explode(entitySet);
	}

    return Acad::eOk;
}

Adesk::Boolean CGenericTable::subWorldDraw(AcGiWorldDraw* worldDraw)
{
    assertReadEnabled();
	
    if(worldDraw->regenAbort())
	{
        return Adesk::kTrue;
    }

	// Update if modified
	if(!worldDraw->isDragging() && isModified)
	{
		Calculate();
	}

	for(std::vector<CTableCell*>::iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->worldDraw(worldDraw);
	}

    return Adesk::kTrue; // Don't call viewportDraw().
}

Acad::ErrorStatus CGenericTable::subGetGeomExtents(AcDbExtents& extents) const
{
    assertReadEnabled();

	if(isModified)
	{
		Calculate();
	}

	// Get ECS extents
	AcGePoint3d pt1(0, 0, 0);
	AcGePoint3d pt2(m_Width, 0, 0);
	AcGePoint3d pt3(m_Width, -m_Height, 0);
	AcGePoint3d pt4(0, -m_Height, 0);

	// Transform to WCS
	AcGeMatrix3d trans = AcGeMatrix3d::kIdentity;
	trans.setCoordSystem(m_BasePoint, m_Direction, m_Up, NormalVector());
	pt1.transformBy(trans);
	pt2.transformBy(trans);
	pt3.transformBy(trans);
	pt4.transformBy(trans);

	extents.addPoint(pt1);
	extents.addPoint(pt2);
	extents.addPoint(pt3);
	extents.addPoint(pt4);
	
	return Acad::eOk;
}

//*************************************************************************
// Overridden methods from AcDbObject
//*************************************************************************

Acad::ErrorStatus CGenericTable::dwgOutFields(AcDbDwgFiler* pFiler) const
{
	assertReadEnabled();

	if(isModified)
	{
		Calculate();
	}

	// Save parent class information first.
	Acad::ErrorStatus es;
	if((es = AcDbEntity::dwgOutFields(pFiler)) != Acad::eOk)
		return es;

	// Object version number
	pFiler->writeItem(CGenericTable::kCurrentVersionNumber);

	pFiler->writePoint3d(m_BasePoint);
	pFiler->writeVector3d(m_Direction);
	pFiler->writeVector3d(m_Up);

	pFiler->writeDouble(m_CellMargin);

	pFiler->writeInt32(m_Rows);
	pFiler->writeInt32(m_Columns);

	pFiler->writeDouble(m_MaxHeight);
	pFiler->writeDouble(m_TableSpacing);

	pFiler->writeDouble(m_Width);
	pFiler->writeDouble(m_Height);

	for(std::vector<CTableCell*>::const_iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->dwgOutFields(pFiler);
	}

	return pFiler->filerStatus();
}

Acad::ErrorStatus CGenericTable::dwgInFields(AcDbDwgFiler* pFiler)
{
	assertWriteEnabled();

	// Read parent class information first.
	Acad::ErrorStatus es;
	if((es = AcDbEntity::dwgInFields(pFiler)) != Acad::eOk)
		return es;

	// Object version number needs to be read first
	Adesk::UInt32 version = 0;
	pFiler->readItem(&version);
	if (version > CGenericTable::kCurrentVersionNumber)
		return Acad::eMakeMeProxy;


	pFiler->readPoint3d(&m_BasePoint);
	pFiler->readVector3d(&m_Direction);
	pFiler->readVector3d(&m_Up);

	pFiler->readDouble(&m_CellMargin);

	Adesk::Int32 rows;
	Adesk::Int32 columns;
	pFiler->readInt32(&rows);
	pFiler->readInt32(&columns);
	pFiler->readDouble(&m_MaxHeight);
	pFiler->readDouble(&m_TableSpacing);
	SetSize(rows, columns);

	pFiler->readDouble(&m_Width);
	pFiler->readDouble(&m_Height);

	for(int i = 0; i < m_Rows * m_Columns; i++)
	{
		CTableCell* cell = m_Cells[i];
		cell->dwgInFields(pFiler);
	}


	isModified = true;

	return pFiler->filerStatus();
}

Acad::ErrorStatus CGenericTable::dxfOutFields(AcDbDxfFiler* pFiler) const
{
	assertReadEnabled();

	if(isModified)
	{
		Calculate();
	}

	// Save parent class information first.
	Acad::ErrorStatus es;
	if((es = AcDbEntity::dxfOutFields(pFiler)) != Acad::eOk)
		return es;

	// Subclass
	pFiler->writeItem(AcDb::kDxfSubclass, _T("GenericTable"));

	pFiler->writePoint3d(AcDb::kDxfXCoord, m_BasePoint);
	// Use max precision when writing out direction vectors
	pFiler->writeVector3d(AcDb::kDxfXCoord + 1, m_Direction, 16);
	pFiler->writeVector3d(AcDb::kDxfXCoord + 2, m_Up, 16);

	pFiler->writeDouble(AcDb::kDxfReal, m_CellMargin);

	pFiler->writeInt32(AcDb::kDxfInt32, m_Rows);
	pFiler->writeInt32(AcDb::kDxfInt32, m_Columns);
	pFiler->writeDouble(AcDb::kDxfReal, m_MaxHeight);
	pFiler->writeDouble(AcDb::kDxfReal, m_TableSpacing);

	pFiler->writeDouble(AcDb::kDxfReal, m_Width);
	pFiler->writeDouble(AcDb::kDxfReal, m_Height);

	for(std::vector<CTableCell*>::const_iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->dxfOutFields(pFiler);
	}

	return pFiler->filerStatus();
}

Acad::ErrorStatus CGenericTable::dxfInFields(AcDbDxfFiler* pFiler)
{
	assertWriteEnabled();

	// Read parent class information first.
	Acad::ErrorStatus es;
	if(((es = AcDbEntity::dxfInFields(pFiler)) != Acad::eOk) || !pFiler->atSubclassData(_T("GenericTable")))
		return es;

	resbuf rb;
	// Object version number
    Adesk::UInt32 version;
    pFiler->readItem(&rb);
    if (rb.restype != AcDb::kDxfInt32) 
    {
        pFiler->pushBackItem();
        pFiler->setError(Acad::eInvalidDxfCode, _T("\nError: expected group code %d (version)"), AcDb::kDxfInt32);
        return pFiler->filerStatus();
    }
    version = rb.resval.rlong;
	if (version > CGenericTable::kCurrentVersionNumber)
		return Acad::eMakeMeProxy;

	// Read params
	AcGeVector3d t_Direction, t_Up;
	AcGePoint3d t_BasePoint;
	double t_CellMargin;
	int t_Rows;
	int t_Columns;
	double t_MaxHeight;
	double t_TableSpacing;
	double t_Width;
	double t_Height;
	std::vector<CTableCell*> t_Cells;

	if((es = Utility::ReadDXFPoint(pFiler, AcDb::kDxfXCoord, _T("base point"), t_BasePoint)) != Acad::eOk) return es;
	if((es = Utility::ReadDXFVector(pFiler, AcDb::kDxfXCoord + 1, _T("direction vector"), t_Direction)) != Acad::eOk) return es;
	if((es = Utility::ReadDXFVector(pFiler, AcDb::kDxfXCoord + 2, _T("up vector"), t_Up)) != Acad::eOk) return es;

	if((es = Utility::ReadDXFReal(pFiler, AcDb::kDxfReal, _T("cell margin"), t_CellMargin)) != Acad::eOk) return es;

	if((es = Utility::ReadDXFLong(pFiler, AcDb::kDxfInt32, _T("row count"), t_Rows)) != Acad::eOk) return es;
	if((es = Utility::ReadDXFLong(pFiler, AcDb::kDxfInt32, _T("column count"), t_Columns)) != Acad::eOk) return es;
	if((es = Utility::ReadDXFReal(pFiler, AcDb::kDxfReal, _T("max height"), t_MaxHeight)) != Acad::eOk) return es;
	if((es = Utility::ReadDXFReal(pFiler, AcDb::kDxfReal, _T("table spacing"), t_TableSpacing)) != Acad::eOk) return es;

	if((es = Utility::ReadDXFReal(pFiler, AcDb::kDxfReal, _T("table width"), t_Width)) != Acad::eOk) return es;
	if((es = Utility::ReadDXFReal(pFiler, AcDb::kDxfReal, _T("table height"), t_Height)) != Acad::eOk) return es;

	for(int i = 0; i < t_Rows * t_Columns; i++)
    {
		CTableCell* cell = new CTableCell();
		cell->dxfInFields(pFiler);
		t_Cells.push_back(cell);
    }

	// Successfully read DXF codes; set object properties.
	m_BasePoint = t_BasePoint;
	m_Direction = t_Direction;
	m_Up = t_Up;
	m_CellMargin = t_CellMargin;
	SetSize(t_Rows, t_Columns);
	m_MaxHeight = t_MaxHeight;
	m_TableSpacing = t_TableSpacing;
	m_Width = t_Width;
	m_Height = t_Height;
	m_Cells = t_Cells;

	isModified = true;

    return es;
}


void CGenericTable::saveAs(AcGiWorldDraw *worldDraw, AcDb::SaveType saveType)
{
    assertReadEnabled();

    if(worldDraw->regenAbort())
	{
        return;
    }

	// Update if modified
	if(isModified)
	{
		Calculate();
	}

	for(std::vector<CTableCell*>::const_iterator it = m_Cells.begin(); it != m_Cells.end(); it++)
	{
		CTableCell* cell = (*it);
		cell->saveAs(worldDraw, saveType);
	}
}