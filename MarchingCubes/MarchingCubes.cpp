#include "MarchingCubes.h"
#include <Math.h>

int triTable[256][16] =
{ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} };

MarchingCubes::MarchingCubes(ID3D11DeviceContext* context,Vector3 pos, sEdges edges, float pointDistance, float frequency, int GridSize, bool interpolate) : Pos(pos)
{
	mpContext = context;
	
	EdgeState = edges;
	gridSize = GridSize;
	Interpolate = interpolate;

	mpContext = context;

	ID3D11Device* device;
	mpContext->GetDevice(&device);

	//States = std::make_unique<DirectX::CommonStates>(device);

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &mpConstantBuffer);

	ID3DBlob* VertexCode;

	LoadVertexShader(device, L"simple_vs.hlsl", &mpVertexShader, &VertexCode);
	LoadPixelShader(device, L"simple_ps.hlsl", &mpPixelShader);
	LoadPixelShader(device, L"Water_ps.hlsl", &mpWaterPixelShader);

	D3D11_INPUT_ELEMENT_DESC VertexDesc[] =
	{
		// Data Type,  Type Index,  Data format                      Slot  Offset    Other values can be ignored for now 
		{ "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    0,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	int VertexDescCount = sizeof(VertexDesc) / sizeof(VertexDesc[0]); // This gives a count of rows in the array above

	device->CreateInputLayout(VertexDesc, VertexDescCount, VertexCode->GetBufferPointer(), VertexCode->GetBufferSize(), &mpVertexLayout);
	generate(pointDistance, frequency, GridSize, interpolate,20);

}

void MarchingCubes::generate(float pointDistance,float frequency,int GridSize, bool interpolate, float surfaceLevel)
{
	gridSize = GridSize;
	Interpolate = interpolate;
	Points.resize(GridSize);
	for (int x = 0; x < GridSize; x++)
	{
		Points[x].resize(GridSize);
		for (int y = 0; y < GridSize; y++)
		{
			Points[x][y].resize(GridSize);
		}
	}

	Vector3 convertedPos = Pos * (gridSize - 1);

	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			for (int z = 0; z < GridSize; z++)
			{
				Points[x][y][z].pos = { (float)(x + (convertedPos.x)) * pointDistance,(float)(y + (convertedPos.y )) * pointDistance,(float)(z + (convertedPos.z)) * pointDistance};

				if ((y >= GridSize - 1 && EdgeState.yMax)
					|| (x == 0 && EdgeState.xMin)
					|| (x >= GridSize - 1 && EdgeState.xMax)
					|| (z == 0 && EdgeState.zMin)
					|| (z >= GridSize - 1 && EdgeState.zMax))
				{
					Points[x][y][z].value[earth] = -1;
					Points[x][y][z].value[water] = -1;
				}
				else if (y == 0 && EdgeState.yMin)
				{
					Points[x][y][z].value[earth] = 1;
					Points[x][y][z].value[water] = -1;
				}
				else
				{
					bool surface = true; //y + convertedPos.y > 20;
					float noise;

					float noise1 = Noise.GetPerlin((x + convertedPos.x) * frequency, (y + convertedPos.y) * frequency, (z + convertedPos.z) * frequency);
					float noise2 = Noise.GetPerlin((x + convertedPos.x) * frequency * 1.05, (y + convertedPos.y) * frequency, (z + convertedPos.z) * frequency * 1.05) * 0.5;
					float noise3 = Noise.GetPerlin((x + convertedPos.x) * frequency * 2.5, (y + convertedPos.y) * frequency, (z + convertedPos.z) * frequency * 2.5) * 0.25;
					float noise4 = Noise.GetPerlin((x + convertedPos.x) * frequency * 5.5, (y + convertedPos.y) * frequency, (z + convertedPos.z) * frequency * 5.5) * 0.125;
					noise = noise1 + noise2 + noise3 + noise4;

					if (surface)
					{
						
						noise = (noise + 2)  - (y + convertedPos.y) / surfaceLevel;
					}
					
					Points[x][y][z].value[earth] = noise;
					Points[x][y][z].value[water] = -0.02;
				}
			}
		}
	}
	
}

void MarchingCubes::CreateMesh(int type)
{
	//mesh changed is optimisation for water so it doesnt recalculate when nothing has changed
	MeshChanged = true;
	CalculateCubeNormals(type);
	if (type == earth)
		vertices.clear();
	else
		WaterVertices.clear();
		
	for (int x = 0; x < Points.size() - 1; x++)
	{
		for (int y = 0; y < Points[x].size() - 1; y++)
		{
			for (int z = 0; z < Points[x][y].size() - 1; z++)
			{
				//get a cube
				PointData edge[8];
				edge[0] = Points[x][y][z + 1];
				edge[1] = Points[x + 1][y][z + 1];
				edge[2] = Points[x + 1][y][z];
				edge[3] = Points[x][y][z];
				edge[4] = Points[x][y + 1][z + 1];
				edge[5] = Points[x + 1][y + 1][z + 1];
				edge[6] = Points[x + 1][y + 1][z];
				edge[7] = Points[x][y + 1][z];
				CalculateCubesVerticies(edge, type);
			}
		}
	}
	if (type == earth)
	{
		if (vertices.size() > 0)
			SetBuffer(type);
	}
	else
	{
		if (WaterVertices.size() > 0)
			SetBuffer(type);
	}
	
}

void MarchingCubes::UpdateWater()
{
	if (!MeshChanged)
		return;

	bool change = false;
	for (int x = 0; x < Points.size(); x++)
	{
		for (int y = 0; y < Points[x].size(); y++)
		{
			for (int z = 0; z < Points[x][y].size(); z++)
			{
				if ((y >= gridSize - 1 && EdgeState.yMax)
					|| (x == 0 && EdgeState.xMin)
					|| (x >= gridSize - 1 && EdgeState.xMax)
					|| (z == 0 && EdgeState.zMin)
					|| (z >= gridSize - 1 && EdgeState.zMax)
					|| (y == 0 && EdgeState.yMin))

				{
					Points[x][y][z].value[water] = -1;
				}
				else
				{
					bool FlownDown = false;

					if (Points[x][y][z].value[water] > 0.2)
						Points[x][y][z].value[water] = 0.2;

					//y values

					if (Points[x][y][z].value[water] > SurfaceLevel) //then its water
					{
						if (y > 0)
						{
							if (Points[x][y - 1][z].value[water] < 0.2 && Points[x][y - 1][z].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								Points[x][y - 1][z].value[water] += 0.01;
								if (y == 1 && !EdgeState.yMin)
								{
									neighbours.Down->Points[x][gridSize - 1][z].value[water] = Points[x][y -1][z].value[water];
									neighbours.Down->MeshChanged = true;
								}
								change = true;
								FlownDown = true;
							}
						}
						else
						{
							if (neighbours.Down->Points[x][gridSize - 2][z].value[water] < 0.2 && neighbours.Down->Points[x][gridSize - 2][z].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								neighbours.Down->Points[x][gridSize - 1][z].value[water] = Points[x][y][z].value[water];
								neighbours.Down->Points[x][gridSize - 2][z].value[water] += 0.01;
								neighbours.Down->MeshChanged = true;
								change = true;
								FlownDown = true;
							}
						}
					}


					//x values
					if (Points[x][y][z].value[water] > SurfaceLevel + 0.01 && !FlownDown)
					{
						if (x < gridSize - 1)
						{
							if (Points[x + 1][y][z].value[water] < 0.2 && Points[x + 1][y][z].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								Points[x + 1][y][z].value[water] += 0.01;
								if (x == gridSize - 2 && !EdgeState.xMax)
								{
									neighbours.Left->Points[0][y][z].value[water] = Points[x + 1][y][z].value[water];
									neighbours.Left->MeshChanged = true;
								}
								change = true;
							}

						}
						else
						{
							if (neighbours.Left->Points[1][y][z].value[water] < 0.2 && neighbours.Left->Points[1][y][z].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								neighbours.Left->Points[0][y][z].value[water] = Points[x][y][z].value[water];
								neighbours.Left->Points[1][y][z].value[water] += 0.01;
								neighbours.Left->MeshChanged = true;
								change = true;
							}
						}
					}

					if (Points[x][y][z].value[water] > SurfaceLevel && !FlownDown)
					{
						if (x > 0)
						{
							if (Points[x - 1][y][z].value[water] < 0.2 && Points[x - 1][y][z].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								Points[x - 1][y][z].value[water] += 0.01;
								if (x == 0)
								{
									neighbours.Right->Points[gridSize - 1][y][z].value[water] = Points[x - 1][y][z].value[water];
									neighbours.Right->MeshChanged = true;
								}
								change = true;
							}
						}
						else
						{
							if (neighbours.Right->Points[gridSize - 2][y][z].value[water] < 0.2 && neighbours.Right->Points[gridSize - 2][y][z].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								neighbours.Right->Points[gridSize - 1][y][z].value[water] = Points[x][y][z].value[water];
								neighbours.Right->Points[gridSize - 2][y][z].value[water] += 0.01;
								neighbours.Right->MeshChanged = true;
								change = true;
							}

						}
					}
					//end of x

					//z values
					if (Points[x][y][z].value[water] > SurfaceLevel + 0.01 && !FlownDown)
					{
						if (z < gridSize - 1)
						{
							if (Points[x][y][z + 1].value[water] < 0.2 && Points[x][y][z + 1].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								Points[x][y][z + 1].value[water] += 0.01;
								if (z == gridSize - 2 && !EdgeState.zMax)
								{
									neighbours.Back->Points[x][y][0].value[water] = Points[x][y][z + 1].value[water];
									neighbours.Back->MeshChanged = true;
								}
								change = true;
							}

						}
						else
						{
							if (neighbours.Back->Points[x][y][1].value[water] < 0.2 && neighbours.Back->Points[x][y][1].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								neighbours.Back->Points[x][y][0].value[water] = Points[x][y][z].value[water];
								neighbours.Back->Points[x][y][1].value[water] += 0.01;
								neighbours.Back->MeshChanged = true;
								change = true;
							}
						}
					}


					if (Points[x][y][z].value[water] > SurfaceLevel && !FlownDown)
					{
						if (z > 0)
						{
							if (Points[x][y][z - 1].value[water] < 0.2 && Points[x][y][z - 1].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								Points[x][y][z - 1].value[water] += 0.01;
								if (z == 0)
								{
									neighbours.Forward->Points[x][y][gridSize - 1].value[water] = Points[x][y][z -1].value[water];
									neighbours.Forward->MeshChanged = true;
								}
								change = true;
							}
						}
						else
						{
							if (neighbours.Forward->Points[x][y][gridSize - 2].value[water] < 0.2 && neighbours.Forward->Points[x][y][gridSize - 2].value[earth] < SurfaceLevel)
							{
								//Points[x][y][z].value[water] -= 0.01;
								neighbours.Forward->Points[x][y][gridSize - 1].value[water] = Points[x][y][z].value[water];
								neighbours.Forward->Points[x][y][gridSize - 2].value[water] += 0.01;
								neighbours.Forward->MeshChanged = true;
								change = true;
							}

						}
					}
					//end of z values										
				}
			}
		}
	}

	if (change)
		CreateMesh(water);
	else
		MeshChanged = false;
}

void MarchingCubes::AffectPoints(Vector3 pos, int direction, float radius, int type)
{
	for (int x = 0; x < Points.size(); x++)
	{
		for (int y = 0; y < Points[x].size(); y++)
		{
			for (int z = 0; z < Points[x][y].size(); z++)
			{
				if ((y >= gridSize - 1 && EdgeState.yMax)
					|| (x == 0 && EdgeState.xMin)
					|| (x >= gridSize - 1 && EdgeState.xMax)
					|| (z == 0 && EdgeState.zMin)
					|| (z >= gridSize - 1 && EdgeState.zMax))
				{
					Points[x][y][z].value[type] = -1;
				}
				else
				{
					float length = (Points[x][y][z].pos - pos).Length();
					if (length < radius)
					{
						Points[x][y][z].value[type] += (0.04 * direction) / length;

						if (Points[x][y][z].value[type] > MaxValue)
							Points[x][y][z].value[type] = MaxValue;

						if (Points[x][y][z].value[type] < MinValue)
							Points[x][y][z].value[type] = MinValue;

					}
				}
			}
		}
	}
	CreateMesh(type);
}

void MarchingCubes::Smooth(Vector3 pos, float radius,int type)
{
	for (int x = 0; x < Points.size(); x++)
	{
		for (int y = 0; y < Points[x].size(); y++)
		{
			for (int z = 0; z < Points[x][y].size(); z++)
			{
				if ((y >= gridSize - 1 && EdgeState.yMax)
					|| (x == 0 && EdgeState.xMin)
					|| (x >= gridSize - 1 && EdgeState.xMax)
					|| (z == 0 && EdgeState.zMin)
					|| (z >= gridSize - 1 && EdgeState.zMax))
				{
					Points[x][y][z].value[type] = -1;
				}
				else
				{
					float average =0;
					int totals = 6;
					float length = (Points[x][y][z].pos - pos).Length();
					
					if (length < radius)
					{
						//left right connectors
						if (x < gridSize - 1)
							average += Points[x + 1][y][z].value[type];
						else
							average += neighbours.Left->Points[1][y][z].value[type];
							

						if (x > 0)
							average += Points[x - 1][y][z].value[type];
						else
							average += neighbours.Right->Points[gridSize - 2][y][z].value[type];

						////up down connectors
						if (y < gridSize - 1)
							average += Points[x][y + 1][z].value[type];
						else
							average += neighbours.Up->Points[x][1][z].value[type];
							

						if (y > 0)
							average += Points[x][y - 1][z].value[type];
						else if (!EdgeState.yMin)
							average += neighbours.Down->Points[x][gridSize - 2][z].value[type];
						else
							totals--;

						//forward back connectors
						if (z < gridSize - 1)
							average += Points[x][y][z + 1].value[type];
						else
							average += neighbours.Back->Points[x][y][1].value[type];

						if (z > 0)
							average += Points[x][y][z - 1].value[type];
						else
							average += neighbours.Forward->Points[x][y][gridSize - 2].value[type];
							

						average /= totals;
						if (abs(Points[x][y][z].value[type] - average) > 0.02)
						{
							if (Points[x][y][z].value[type] < average)
								Points[x][y][z].value[type] += 0.04;
							else
								Points[x][y][z].value[type] -= 0.04;
						}
						

					}
				}
			}
		}
	}
	CreateMesh(type);
}

void MarchingCubes::Flatten(Vector3 pos, float radius, int type)
{
	for (int x = 0; x < Points.size(); x++)
	{
		for (int y = 0; y < Points[x].size(); y++)
		{
			for (int z = 0; z < Points[x][y].size(); z++)
			{				
				if ((y >= gridSize - 1 && EdgeState.yMax)
					|| (x == 0 && EdgeState.xMin)
					|| (x >= gridSize - 1 && EdgeState.xMax)
					|| (z == 0 && EdgeState.zMin)
					|| (z >= gridSize - 1 && EdgeState.zMax))
				{
					Points[x][y][z].value[type] = -1;
				}
				else
				{
					float length = (Points[x][y][z].pos - pos).Length();
					if (length < radius)
					{
						if (Points[x][y][z].pos.y <= pos.y)
						{
							Points[x][y][z].value[type] += (0.04) / length;

							if (Points[x][y][z].value[type] > MaxValue)
								Points[x][y][z].value[type] = MaxValue;
						}
						else
						{
							Points[x][y][z].value[type] -= (0.04) / length;

							if (Points[x][y][z].value[type] < MinValue)
								Points[x][y][z].value[type] = MinValue;
						}
					}
				}
			}
		}
	}
	CreateMesh(type);
}

bool MarchingCubes::CubeToSphere(Vector3 sPos, float radius)
{
	float dmin = 0;

	Vector3 center = sPos;
	Vector3 bmin = Points[0][0][0].pos;
	Vector3 bmax = Points[gridSize-1][gridSize-1][gridSize-1].pos;

	if (center.x < bmin.x) {
		dmin += pow(center.x - bmin.x, 2);
	}
	else if (center.x > bmax.x) {
		dmin += pow(center.x - bmax.x, 2);
	}

	if (center.y < bmin.y) {
		dmin += pow(center.y - bmin.y, 2);
	}
	else if (center.y > bmax.y) {
		dmin += pow(center.y - bmax.y, 2);
	}

	if (center.z < bmin.z) {
		dmin += pow(center.z - bmin.z, 2);
	}
	else if (center.z > bmax.z) {
		dmin += pow(center.z - bmax.z, 2);
	}

	return dmin <= pow(radius + 10.f, 2);
	
}

bool MarchingCubes::GetSurfacePoint(Vector3& pos, float Radius, int type)
{
	for (int x = 0; x < Points.size(); x++)
	{
		for (int y = 0; y < Points[x].size(); y++)
		{
			for (int z = 0; z < Points[x][y].size(); z++)
			{
				float length = (Points[x][y][z].pos - pos).Length();
				if (length < Radius)
				{
					if (Points[x][y][z].value[type] > SurfaceLevel)
					{
						pos = Points[x][y][z].pos;
						return true;
					}
				}
			}
		}
	}
	return false;
}

void MarchingCubes::SetBuffer(int type)
{
	if (mVertexBuffer[type])
		mVertexBuffer[type]->Release();

	HRESULT hr = S_OK;
	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if(type == earth)
		bufferDesc.ByteWidth = sizeof(CUSTOMVERTEX) * vertices.size();
	else
		bufferDesc.ByteWidth = sizeof(CUSTOMVERTEX) * WaterVertices.size();

	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	if (type == earth)
		InitData.pSysMem = &vertices[0];
	else
		InitData.pSysMem = &WaterVertices[0];
	
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	ID3D11Device* device;
	mpContext->GetDevice(&device);
	hr = device->CreateBuffer(&bufferDesc, &InitData, &mVertexBuffer[type]);

}

void MarchingCubes::RenderEarth(ID3D11RasterizerState* state, ID3D11BlendState* BlendState, ID3D11DepthStencilState* DepthState)
{
	if (vertices.size() > 0)
	{
		mpContext->RSSetState(state);

		UINT stride = sizeof(CUSTOMVERTEX);
		UINT offset = 0;

		mpContext->IASetVertexBuffers(0, 1, &mVertexBuffer[earth], &stride, &offset);


		// 2a) Indicate the layout of the vertex buffer
		mpContext->IASetInputLayout(mpVertexLayout);

		// 2b) Also indicate the primitive topology of the buffer. Our buffer holds a triangle list - each set of 3 vertices
		//     will be connected into a triangle. There are other topologies and we will see them shortly.
		mpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		// 3) Select which shaders to use when rendering
		mpContext->VSSetShader(mpVertexShader, nullptr, 0);
		mpContext->PSSetShader(mpPixelShader, nullptr, 0);
		mpContext->OMSetBlendState(BlendState, DirectX::Colors::Black, 0xFFFFFFFF);
		mpContext->OMSetDepthStencilState(DepthState, 0);

		// 4) Draw 3 vertices, starting at vertex 0. This will draw a triangle using the vertex data and shaders selected
		mpContext->Draw(vertices.size(), 0);
	}
}
void MarchingCubes::RenderWater(ID3D11RasterizerState* state, ID3D11BlendState* BlendState, ID3D11DepthStencilState* DepthState)
{
	//draw water
	if (WaterVertices.size() > 0)
	{
		mpContext->RSSetState(state);

		UINT stride = sizeof(CUSTOMVERTEX);
		UINT offset = 0;

		mpContext->IASetVertexBuffers(0, 1, &mVertexBuffer[water], &stride, &offset);


		// 2a) Indicate the layout of the vertex buffer
		mpContext->IASetInputLayout(mpVertexLayout);

		// 2b) Also indicate the primitive topology of the buffer. Our buffer holds a triangle list - each set of 3 vertices
		//     will be connected into a triangle. There are other topologies and we will see them shortly.
		mpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		// 3) Select which shaders to use when rendering
		mpContext->VSSetShader(mpVertexShader, nullptr, 0);
		mpContext->PSSetShader(mpWaterPixelShader, nullptr, 0);
		mpContext->OMSetBlendState(BlendState, DirectX::Colors::Black, 0xFFFFFFFF);
		mpContext->OMSetDepthStencilState(DepthState, 0);

		// 4) Draw 3 vertices, starting at vertex 0. This will draw a triangle using the vertex data and shaders selected
		mpContext->Draw(WaterVertices.size(), 0);
	}
}

void MarchingCubes::CalculateCubesVerticies(PointData edge[8], int type)
{
	int ActivePointsbyte = 0;

	//get the value for the lookup table
	if (edge[0].value[type] > SurfaceLevel) ActivePointsbyte |= 1;
	if (edge[1].value[type] > SurfaceLevel) ActivePointsbyte |= 2;
	if (edge[2].value[type] > SurfaceLevel) ActivePointsbyte |= 4;
	if (edge[3].value[type] > SurfaceLevel) ActivePointsbyte |= 8;
	if (edge[4].value[type] > SurfaceLevel) ActivePointsbyte |= 16;
	if (edge[5].value[type] > SurfaceLevel) ActivePointsbyte |= 32;
	if (edge[6].value[type] > SurfaceLevel) ActivePointsbyte |= 64;
	if (edge[7].value[type] > SurfaceLevel) ActivePointsbyte |= 128;

	std::vector<SVertices> triangle;

	for (int x = 0; x < 16; x++)
	{
		switch (triTable[ActivePointsbyte][x])
		{
		case 0:
			triangle.push_back(CalculateMid(edge[0], edge[1],type));
			break;
		case 1:
			triangle.push_back(CalculateMid(edge[1], edge[2], type));
			break;
		case 2:
			triangle.push_back(CalculateMid(edge[2], edge[3], type));
			break;
		case 3:
			triangle.push_back(CalculateMid(edge[3], edge[0], type));
			break;
		case 4:
			triangle.push_back(CalculateMid(edge[4], edge[5], type));
			break;
		case 5:
			triangle.push_back(CalculateMid(edge[5], edge[6], type));
			break;
		case 6:
			triangle.push_back(CalculateMid(edge[6], edge[7], type));
			break;
		case 7:
			triangle.push_back(CalculateMid(edge[7], edge[4], type));
			break;
		case 8:
			triangle.push_back(CalculateMid(edge[0], edge[4], type));
			break;
		case 9:
			triangle.push_back(CalculateMid(edge[1], edge[5], type));
			break;
		case 10:
			triangle.push_back(CalculateMid(edge[2], edge[6], type));
			break;
		case 11:
			triangle.push_back(CalculateMid(edge[3], edge[7], type));
			break;
		case -1:
			return;
			break;
		}
		if (x % 3 == 2)
		{
			Vector3 v1 = triangle[1].pos - triangle[0].pos;
			Vector3 v2 = triangle[2].pos - triangle[0].pos;

			Vector3 normal = Normalise(Cross(v2,v1));
			normal.x *= -1; //flip x because its the only one the wrong way
			if (type == earth)
			{
				for (auto vert : triangle)
				{
					vertices.push_back({ vert.pos,vert.normal });
				}
				triangle.clear();
			}
			else
			{
				for (auto vert : triangle)
				{
					WaterVertices.push_back({ vert.pos,vert.normal });
				}
				triangle.clear();
			}
			
		}
	}
}

void MarchingCubes::CalculateCubeNormals(int type)
{
	for (int x = 0; x < Points.size(); x++)
	{
		for (int y = 0; y < Points[x].size(); y++)
		{
			for (int z = 0; z < Points[x][y].size(); z++)
			{
				if ((y >= gridSize - 1 && EdgeState.yMax)
					|| (x == 0 && EdgeState.xMin)
					|| (x >= gridSize - 1 && EdgeState.xMax)
					|| (z == 0 && EdgeState.zMin)
					|| (z >= gridSize - 1 && EdgeState.zMax)
					|| (y == 0 && EdgeState.yMin))
				{
					Points[x][y][z].normal = { 0,0,0 };
				}
				else
				{
					

					//left right connectors
					if (x >= gridSize - 1)
					{
						Points[x][y][z].normal.x = Points[x - 1][y][z].value[type] -  neighbours.Left->Points[1][y][z].value[type];
					}
					else if(x <= 0)
					{
						Points[x][y][z].normal.x = neighbours.Right->Points[gridSize - 2][y][z].value[type] - Points[x + 1][y][z].value[type];
					}
					else
					{
						Points[x][y][z].normal.x = Points[x - 1][y][z].value[type] - Points[x + 1][y][z].value[type];
					}
						


					////up down connectors

					if (y >= gridSize - 1)
					{
						Points[x][y][z].normal.y = Points[x][y -1][z].value[type] - neighbours.Up->Points[x][1][z].value[type];
					}
					else if (y <= 0)
					{
						Points[x][y][z].normal.y = neighbours.Down->Points[x][gridSize - 2][z].value[type] - Points[x][y + 1][z].value[type];
					}
					else
					{
						Points[x][y][z].normal.y = Points[x][y - 1][z].value[type] - Points[x][y + 1][z].value[type];
					}




					//forward back connectors
					if (z >= gridSize - 1)
					{
						Points[x][y][z].normal.z = Points[x ][y][z -1].value[type] - neighbours.Back->Points[x][y][1].value[type];
					}
					else if (z <= 0)
					{
						Points[x][y][z].normal.z = neighbours.Forward->Points[x][y][gridSize - 2].value[type] - Points[x][y][z + 1].value[type];
					}
					else
					{
						Points[x][y][z].normal.z = Points[x][y][z - 1].value[type] - Points[x][y][z + 1].value[type];
					}


					Points[x][y][z].normal.Normalize();

				}
			}
		}
	}
}

SVertices MarchingCubes::CalculateMid(const PointData& p1, const PointData& p2, int type)
{
	if (Interpolate)
	{
		/*if (abs(SurfaceLevel - p1.value) < 0.00001)
			return p1.pos;
		else if (abs(SurfaceLevel - p2.value) < 0.00001)
			return p2.pos;
		else if(abs(p1.value - p2.value) < 0.00001)
			return p1.pos;
		else
		*/
		{
			SVertices point;
			float multiplier = (SurfaceLevel - p1.value[type]) / (p2.value[type] - p1.value[type]);
			point.pos.x = p1.pos.x + multiplier * (p2.pos.x - p1.pos.x);
			point.pos.y = p1.pos.y + multiplier * (p2.pos.y - p1.pos.y);
			point.pos.z = p1.pos.z + multiplier * (p2.pos.z - p1.pos.z);

			point.normal.x = p1.normal.x + multiplier * (p2.normal.x - p1.normal.x);
			point.normal.y = p1.normal.y + multiplier * (p2.normal.y - p1.normal.y);
			point.normal.z = p1.normal.z + multiplier * (p2.normal.z - p1.normal.z);
			return point;
		}
	}	
	else
	{
		return { (p1.pos + p2.pos) / 2, };
	}


}
Vector3 MarchingCubes::Cross(const Vector3& v1,const Vector3& v2)
{
	return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}
Vector3 MarchingCubes::Normalise(const Vector3& v)
{
	float Length = sqrt( v.x * v.x + v.y * v.y + v.z * v.z);

	if(Length > 0)
		return Vector3(v.x / Length, v.y / Length, v.z / Length);
	return { 0,0,0 };
}