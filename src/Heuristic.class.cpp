#include "Heuristic.class.hpp"

Puzzle          Heuristic::goal;
Array<Position> Heuristic::goalMap;

void Heuristic::init()
{
	int size = Puzzle::getSize();
	Heuristic::goal = Puzzle::getGoal();
	Heuristic::goalMap = Array<Position>(size * size);
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
			Heuristic::goalMap[Heuristic::goal.getAt(y, x)] = { y, x };
	}
}

int Heuristic::manhattan(const Puzzle &puzzle)
{
	int size = Puzzle::getSize();
	int total = 0;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			Position &goalPos = Heuristic::goalMap[puzzle.getAt(y, x)];
			total += std::abs(goalPos.y - y) + abs(goalPos.x - x);
		}
	}
	return total;
}

// Aim of the function:
// Returns the position of the highest element in the Array
int getHighestConflict(Array<int> array, int size)
{
	int iHighest = 0;
	int highest = 0;
	for (int i = 0; i < size; i++)
	{
		if (array[i] > highest)
		{
			highest = array[i];
			iHighest = i;
		}
	}
	return iHighest;
}

// Aim of the function:
// Returns true if array still contains elements > 0
bool rowConflictsRemain(Array<int> array, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i] > 0)
		{
			return true;
		}
	}
	return false;
}

// Aim of the function:
// Returns the position of the conflicting tiles, for a given row and a given tile
std::vector<int> listConflictsRow(const Puzzle &puzzle, int row, int itj, int size)
{
	std::vector<int> vec;
	int              tj = puzzle.getAt(row, itj);

	Position &goalPosTj = Heuristic::goalMap[tj];
	// If the current tile is positionned, in the goal puzzle, inside the same row
	if (goalPosTj.y == row)
	{
		// Loop on all tiles in current row of the puzzle
		for (int itk = 0; itk < size; itk++)
		{
			int       tk = puzzle.getAt(row, itk);
			Position &goalPosTk = Heuristic::goalMap[tk];
			// if tj and tk are different, not equal to 0 (empty tile) and tk is at same row as tj in goal puzzle
			if (tk != tj && tk != 0 && tj != 0 && goalPosTk.y == row)
			{
				// if tj is on the right side of tk and tj's goal pos on the left of tk's goal pos OR contrary
				if ((itj > itk && goalPosTj.x < goalPosTk.x) || (itj < itk && goalPosTj.x > goalPosTk.x))
				{
					vec.push_back(itk);
				}
			}
		}
	}
	return vec;
}

// Transposed function :
// We now work on columns, and check for the x instead of the y
std::vector<int> listConflictsCol(const Puzzle &puzzle, int col, int itj, int size)
{
	std::vector<int> vec;
	int              tj = puzzle.getAt(itj, col);

	Position &goalPosTj = Heuristic::goalMap[tj];
	// If the current tile is positionned, in the goal puzzle, inside the same row
	if (goalPosTj.x == col)
	{
		// Loop on all tiles in current row of the puzzle
		for (int itk = 0; itk < size; itk++)
		{
			int       tk = puzzle.getAt(itk, col);
			Position &goalPosTk = Heuristic::goalMap[tk];
			// if tj and tk are different, not equal to 0 (empty tile) and tk is at same row as tj in goal puzzle
			if (tk != tj && tk != 0 && tj != 0 && goalPosTk.x == col)
			{
				// if tj is on the right side of tk and tj's goal pos on the left of tk's goal pos OR contrary
				if ((itj > itk && goalPosTj.y < goalPosTk.y) || (itj < itk && goalPosTj.y > goalPosTk.y))
				{
					vec.push_back(itk);
				}
			}
		}
	}
	return vec;
}

int Heuristic::linearConflicts(const Puzzle &puzzle)
{
	int        size = Puzzle::getSize();
	int        h = Heuristic::manhattan(puzzle);
	int        nbLc = 0;    // score linear conflicts
	Array<int> crn(size, 0); // conflicts row number
	Array<int> ccn(size, 0); // conflicts column number
	Array<std::vector<int>> crnVec(size); // conflicts row number
	Array<std::vector<int>> ccnVec(size); // conflicts row number

	// Loop on rows
	for (int row = 0; row < size; row++)
	{
		// Loop on tiles, called tilesj (tj), and
		// store nb of conflict for each tile on this row
		for (int itj = 0; itj < size; itj++)
		{
			crnVec[itj] = listConflictsRow(puzzle, row, itj, size);
			crn[itj] = crnVec[itj].size();
		}
		while (rowConflictsRemain(crn, size))
		{
			// Pop the tile with the biggest amount of conflict
			int itk = getHighestConflict(crn, size);
			// Set it at 0
			crn[itk] = 0;
			// Loop list of position the tile is in conflict with and remove one when processed
			for (int el: crnVec[itk])
				crn[el] -= 1; // Remove one to related conflict
			nbLc += 1; // Each removed conflicted equals one conflict
		}
	}
	// Transpose to columns
	// Need to implement same, but with columns
	for (int col = 0; col < size; col++)
	{
		// Loop on tiles, called tilesj (tj), and
		// store nb of conflict for each tile on this row
		for (int itj = 0; itj < size; itj++)
		{
			ccnVec[itj] = listConflictsCol(puzzle, col, itj, size);
			ccn[itj] = ccnVec[itj].size();
		}
		while (rowConflictsRemain(ccn, size))
		{
			// Pop the tile with the biggest amount of conflict
			int itk = getHighestConflict(ccn, size);
			// Set it at 0
			ccn[itk] = 0;
			// Loop list of position the tile is in conflict with and remove one when processed
			for (int el: ccnVec[itk])
				ccn[el] -= 1; // Remove one to related conflict
			nbLc += 1; // Each removed conflicted equals one conflict
		}
	}
	return h + 2 * nbLc;
}

int Heuristic::hamming(const Puzzle &puzzle)
{
	int size = Puzzle::getSize();
	int total = 0;

	for (int i = 0; i < size * size; i++)
	{
		if (puzzle[i] != Heuristic::goal[i])
			total += 1;
	}

	return total;
}
