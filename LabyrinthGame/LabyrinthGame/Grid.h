#pragma once

class Cell 
{
public:
	bool top;
	bool right;
public:
	Cell();
};


class Grid
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
private:
	void GenerateLabyrynth();
};