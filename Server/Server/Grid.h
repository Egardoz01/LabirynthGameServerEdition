#pragma once

class Cell : public CObject
{
public:
	bool top;
	bool right;
public:
	virtual void Serialize(CArchive& ar);
	Cell();
};


class Grid : public CObject
{
public:
	Cell **grid;
	int nRows;
	int nColumns;

public:
	void Initialize(int nRows, int nColumns);
	void FillGrid(int _nRows, int _nColumns, char * str);
	void ClearGrid();
	Grid();
	virtual void Serialize(CArchive& ar);
private:
	void GenerateLabyrynth();
};