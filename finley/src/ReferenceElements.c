
/*******************************************************
*
* Copyright (c) 2003-2010 by University of Queensland
* Earth Systems Science Computational Center (ESSCC)
* http://www.uq.edu.au/esscc
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
*******************************************************/


/**************************************************************/

/*   Finley: Reference elements */

/**************************************************************/

#include "ReferenceElements.h"
#include "esysUtils/mem.h"
#include <string.h>

#define MAX(X,Y) ((X)>(Y)?(X):(Y))

/**************************************************************

    this list has been generated by generateReferenceElementList.py:.
*/

Finley_ReferenceElementInfo Finley_ReferenceElement_InfoList[]={
{ Finley_Point1, "Point1", 1, 1, 1, { 0, 1 }, Finley_Point1,
    { 0 }, PointQuad, Point1Shape, Point1Shape,
    { 0 },
  1, { 0 },
  1, { 0 },
    { 0 },
    { -1 } },
{ Finley_Line2, "Line2", 2, 1, 1, { 0, 2 }, Finley_Line2,
    { 0, 1 }, LineQuad, Line2Shape, Line2Shape,
    { 0, 1 },
  2, { 0, 1 },
  2, { 0, 1 },
    { 1, 0 },
    { -1 } },
{ Finley_Line3, "Line3", 3, 1, 1, { 0, 3 }, Finley_Line2,
    { 0, 1 }, LineQuad, Line3Shape, Line3Shape,
    { 0, 1, 2 },
  3, { 0, 1, 2 },
  3, { 0, 1, 2 },
    { 1, 0, 2 },
    { -1 } },
{ Finley_Line4, "Line4", 4, 1, 1, { 0, 4 }, Finley_Line2,
    { 0, 1 }, LineQuad, Line4Shape, Line4Shape,
    { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
    { 1, 0, 3, 2 },
    { -1 } },
{ Finley_Tri3, "Tri3", 3, 1, 1, { 0, 3 }, Finley_Tri3,
    { 0, 1, 2 }, TriQuad, Tri3Shape, Tri3Shape,
    { 0, 1, 2 },
  3, { 0, 1, 2 },
  3, { 0, 1, 2 },
    { 1, 2, 0 },
    { 0, 2, 1 } },
{ Finley_Tri6, "Tri6", 6, 1, 1, { 0, 6 }, Finley_Tri3,
    { 0, 1, 2 }, TriQuad, Tri6Shape, Tri6Shape,
    { 0, 1, 2, 3, 4, 5 },
  6, { 0, 1, 2, 3, 4, 5 },
  6, { 0, 1, 2, 3, 4, 5 },
    { 1, 2, 0, 4, 5, 3 },
    { 0, 2, 1, 5, 4, 3 } },
{ Finley_Tri9, "Tri9", 9, 1, 1, { 0, 9 }, Finley_Tri3,
    { 0, 1, 2 }, TriQuad, Tri9Shape, Tri9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 1, 2, 0, 5, 6, 7, 8, 3, 4 },
    { 0, 2, 1, 8, 7, 6, 5, 4, 3 } },
{ Finley_Tri10, "Tri10", 10, 1, 1, { 0, 10 }, Finley_Tri3,
    { 0, 1, 2 }, TriQuad, Tri10Shape, Tri10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
    { 1, 2, 0, 5, 6, 7, 8, 3, 4, 9 },
    { 0, 2, 1, 8, 7, 6, 5, 4, 3, 9 } },
{ Finley_Rec4, "Rec4", 4, 1, 1, { 0, 4 }, Finley_Rec4,
    { 0, 1, 2, 3 }, RecQuad, Rec4Shape, Rec4Shape,
    { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
    { 1, 2, 3, 0 },
    { 0, 3, 2, 1 } },
{ Finley_Rec8, "Rec8", 8, 1, 1, { 0, 8 }, Finley_Rec4,
    { 0, 1, 2, 3 }, RecQuad, Rec8Shape, Rec8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  8, { 0, 1, 2, 3, 4, 5, 6, 7 },
  8, { 0, 1, 2, 3, 4, 5, 6, 7 },
    { 1, 2, 3, 0, 5, 6, 7, 4 },
    { 0, 3, 2, 1, 7, 6, 5, 4 } },
{ Finley_Rec9, "Rec9", 9, 1, 1, { 0, 9 }, Finley_Rec4,
    { 0, 1, 2, 3 }, RecQuad, Rec9Shape, Rec9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 1, 2, 3, 0, 5, 6, 7, 4, 8 },
    { 0, 3, 2, 1, 7, 6, 5, 4, 8 } },
{ Finley_Rec12, "Rec12", 12, 1, 1, { 0, 12 }, Finley_Rec4,
    { 0, 1, 2, 3 }, RecQuad, Rec12Shape, Rec12Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
  12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
  12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
    { 1, 2, 3, 0, 6, 7, 8, 9, 10, 11, 4, 5 },
    { 0, 3, 2, 1, 11, 10, 9, 8, 7, 6, 5, 4 } },
{ Finley_Rec16, "Rec16", 16, 1, 1, { 0, 16 }, Finley_Rec4,
    { 0, 1, 2, 3 }, RecQuad, Rec16Shape, Rec16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  16, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  16, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 1, 2, 3, 0, 6, 7, 8, 9, 10, 11, 4, 5, 13, 14, 15, 12 },
    { 0, 3, 2, 1, 11, 10, 9, 8, 7, 6, 5, 4, 12, 15, 14, 13 } },
{ Finley_Tet4, "Tet4", 4, 1, 1, { 0, 4 }, Finley_Tet4,
    { 0, 1, 2, 3 }, TetQuad, Tet4Shape, Tet4Shape,
    { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
    { -1 },
    { -1 } },
{ Finley_Tet10, "Tet10", 10, 1, 1, { 0, 10 }, Finley_Tet4,
    { 0, 1, 2, 3 }, TetQuad, Tet10Shape, Tet10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
    { -1 },
    { -1 } },
{ Finley_Tet16, "Tet16", 16, 1, 1, { 0, 16 }, Finley_Tet4,
    { 0, 1, 2, 3 }, TetQuad, Tet16Shape, Tet16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  16, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  16, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { -1 },
    { -1 } },
{ Finley_Hex8, "Hex8", 8, 1, 1, { 0, 8 }, Finley_Hex8,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, HexQuad, Hex8Shape, Hex8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  8, { 0, 1, 2, 3, 4, 5, 6, 7 },
  8, { 0, 1, 2, 3, 4, 5, 6, 7 },
    { -1 },
    { -1 } },
{ Finley_Hex20, "Hex20", 20, 1, 1, { 0, 20 }, Finley_Hex8,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, HexQuad, Hex20Shape, Hex20Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
    { -1 },
    { -1 } },
{ Finley_Hex27, "Hex27", 27, 1, 1, { 0, 27 }, Finley_Hex8,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, HexQuad, Hex27Shape, Hex27Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 },
  27, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 },
  27, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 },
    { -1 },
    { -1 } },
{ Finley_Hex32, "Hex32", 32, 1, 1, { 0, 32 }, Finley_Hex8,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, HexQuad, Hex32Shape, Hex32Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
  32, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
  32, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
    { -1 },
    { -1 } },
{ Finley_Line2Face, "Line2Face", 2, 1, 1, { 0, 2 }, Finley_Line2Face,
    { 0, 1 }, PointQuad, Line2Shape, Line2Shape,
    { 0, 1 },
  1, { 0 },
  1, { 0 },
    { 0, 1, 2 },
    { -1 } },
{ Finley_Line3Face, "Line3Face", 3, 1, 1, { 0, 3 }, Finley_Line2Face,
    { 0, 1 }, PointQuad, Line3Shape, Line3Shape,
    { 0, 1, 2 },
  1, { 0 },
  1, { 0 },
    { 0, 1, 2 },
    { -1 } },
{ Finley_Line4Face, "Line4Face", 4, 1, 1, { 0, 4 }, Finley_Line2Face,
    { 0, 1 }, PointQuad, Line4Shape, Line4Shape,
    { 0, 1, 2, 3 },
  1, { 0 },
  1, { 0 },
    { 0, 1, 2 },
    { -1 } },
{ Finley_Tri3Face, "Tri3Face", 3, 1, 1, { 0, 3 }, Finley_Tri3Face,
    { 0, 1, 2 }, LineQuad, Tri3Shape, Tri3Shape,
    { 0, 1, 2 },
  2, { 0, 1 },
  2, { 0, 1 },
    { 1, 0, 2 },
    { -1 } },
{ Finley_Tri6Face, "Tri6Face", 6, 1, 1, { 0, 6 }, Finley_Tri3Face,
    { 0, 1, 2 }, LineQuad, Tri6Shape, Tri6Shape,
    { 0, 1, 2, 3, 4, 5 },
  3, { 0, 1, 3 },
  3, { 0, 1, 3 },
    { 1, 0, 2, 3, 5, 4 },
    { -1 } },
{ Finley_Tri9Face, "Tri9Face", 9, 1, 1, { 0, 9 }, Finley_Tri3Face,
    { 0, 1, 2 }, LineQuad, Tri9Shape, Tri9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  4, { 0, 1, 3, 4 },
  4, { 0, 1, 3, 4 },
    { 1, 0, 2, 4, 3, 7, 8, 6, 5 },
    { -1 } },
{ Finley_Tri10Face, "Tri10Face", 10, 1, 1, { 0, 10 }, Finley_Tri3Face,
    { 0, 1, 2 }, LineQuad, Tri10Shape, Tri10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  4, { 0, 1, 3, 4 },
  4, { 0, 1, 3, 4 },
    { 1, 0, 2, 4, 3, 7, 8, 6, 5, 9 },
    { -1 } },
{ Finley_Rec4Face, "Rec4Face", 4, 1, 1, { 0, 4 }, Finley_Rec4Face,
    { 0, 1, 2, 3 }, LineQuad, Rec4Shape, Rec4Shape,
    { 0, 1, 2, 3 },
  2, { 0, 1 },
  2, { 0, 1 },
    { 1, 0, 3, 2 },
    { -1 } },
{ Finley_Rec8Face, "Rec8Face", 8, 1, 1, { 0, 8 }, Finley_Rec4Face,
    { 0, 1, 2, 3 }, LineQuad, Rec8Shape, Rec8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  3, { 0, 1, 4 },
  3, { 0, 1, 4 },
    { 1, 0, 3, 2, 4, 7, 6, 5 },
    { -1 } },
{ Finley_Rec9Face, "Rec9Face", 9, 1, 1, { 0, 9 }, Finley_Rec4Face,
    { 0, 1, 2, 3 }, LineQuad, Rec9Shape, Rec9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  3, { 0, 1, 4 },
  3, { 0, 1, 4 },
    { 1, 0, 3, 2, 4, 7, 6, 5, 8 },
    { -1 } },
{ Finley_Rec12Face, "Rec12Face", 12, 1, 1, { 0, 12 }, Finley_Rec4Face,
    { 0, 1, 2, 3 }, LineQuad, Rec12Shape, Rec12Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
  4, { 0, 1, 4, 5 },
  4, { 0, 1, 4, 5 },
    { 1, 0, 3, 2, 5, 4, 11, 10, 9, 8, 7, 6 },
    { -1 } },
{ Finley_Rec16Face, "Rec16Face", 16, 1, 1, { 0, 16 }, Finley_Rec4Face,
    { 0, 1, 2, 3 }, LineQuad, Rec16Shape, Rec16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  4, { 0, 1, 4, 5 },
  4, { 0, 1, 4, 5 },
    { 1, 0, 3, 2, 5, 4, 11, 10, 9, 8, 7, 6, 13, 12, 15, 14 },
    { -1 } },
{ Finley_Tet4Face, "Tet4Face", 4, 1, 1, { 0, 4 }, Finley_Tet4Face,
    { 0, 1, 2, 3 }, TriQuad, Tet4Shape, Tet4Shape,
    { 0, 1, 2, 3 },
  3, { 0, 1, 2 },
  4, { 0, 1, 2, 3 },
    { 1, 2, 0, 3 },
    { 0, 2, 1, 3 } },
{ Finley_Tet10Face, "Tet10Face", 10, 1, 1, { 0, 10 }, Finley_Tet4Face,
    { 0, 1, 2, 3 }, TriQuad, Tet10Shape, Tet10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  6, { 0, 1, 2, 4, 5, 6 },
  6, { 0, 1, 2, 4, 5, 6 },
    { 1, 2, 0, 3, 5, 6, 4, 8, 9, 7 },
    { 0, 2, 1, 3, 6, 7, 9, 8 } },
{ Finley_Tet16Face, "Tet16Face", 16, 1, 1, { 0, 16 }, Finley_Tet4Face,
    { 0, 1, 2, 3 }, TriQuad, Tet16Shape, Tet16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  9, { 0, 1, 2, 4, 5, 6, 7, 8, 9 },
  9, { 0, 1, 2, 4, 5, 6, 7, 8, 9 },
    { 1, 2, 0, 3, 6, 7, 8, 9, 4, 5, 11, 12, 10, 14, 15, 13 },
    { 0, 2, 1, 3, 9, 8, 7, 6, 5, 4, 9, 8, 7, 6, 10, 12, 11, 13, 15, 14 } },
{ Finley_Hex8Face, "Hex8Face", 8, 1, 1, { 0, 8 }, Finley_Hex8Face,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, RecQuad, Hex8Shape, Hex8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  4, { 0, 1, 2, 3 },
  4, { 0, 1, 2, 3 },
    { 1, 2, 3, 0, 5, 6, 7, 4 },
    { 0, 3, 2, 1, 4, 7, 6, 5 } },
{ Finley_Hex20Face, "Hex20Face", 20, 1, 1, { 0, 20 }, Finley_Hex8Face,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, RecQuad, Hex20Shape, Hex20Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  8, { 0, 1, 2, 3, 8, 9, 10, 11 },
  8, { 0, 1, 2, 3, 8, 9, 10, 11 },
    { 1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15, 12, 17, 18, 19, 16 },
    { 0, 3, 2, 1, 4, 7, 6, 5, 11, 10, 9, 8, 12, 15, 14, 13, 19, 18, 17, 16 } },
{ Finley_Hex27Face, "Hex27Face", 27, 1, 1, { 0, 27 }, Finley_Hex8Face,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, RecQuad, Hex27Shape, Hex27Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 },
  9, { 0, 1, 2, 3, 8, 9, 10, 11, 20 },
  9, { 0, 1, 2, 3, 8, 9, 10, 11, 20 },
    { 1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15, 12, 17, 18, 19, 16, 20, 22, 23, 24, 22, 25, 26 },
    { 0, 3, 2, 1, 4, 7, 6, 5, 11, 10, 9, 8, 12, 15, 14, 13, 19, 18, 17, 16, 20, 24, 23, 22, 21, 25, 26 } },
{ Finley_Hex32Face, "Hex32Face", 32, 1, 1, { 0, 32 }, Finley_Hex8Face,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, RecQuad, Hex32Shape, Hex32Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
  12, { 0, 1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 15 },
  12, { 0, 1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 1, 2, 3, 0, 5, 6, 7, 4, 10, 11, 12, 13, 14, 15, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20, 26, 27, 28, 29, 30, 31, 34, 25 },
    { 0, 3, 2, 1, 4, 7, 6, 5, 15, 14, 13, 12, 11, 10, 9, 8, 16, 19, 18, 17, 20, 23, 22, 21, 31, 30, 29, 28, 27, 26, 25, 24 } },
{ Finley_Point1_Contact, "Point1_Contact", 2, 1, 2, { 0, 1, 2 }, Finley_Point1_Contact,
    { 0, 1 }, PointQuad, Point1Shape, Point1Shape,
    { 0, 1 },
  1, { 0 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line2_Contact, "Line2_Contact", 4, 1, 2, { 0, 2, 4 }, Finley_Line2_Contact,
    { 0, 1, 2, 3 }, LineQuad, Line2Shape, Line2Shape,
    { 0, 1, 2, 3 },
  2, { 0, 1 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line3_Contact, "Line3_Contact", 6, 1, 2, { 0, 3, 6 }, Finley_Line2_Contact,
    { 0, 1, 3, 4 }, LineQuad, Line3Shape, Line3Shape,
    { 0, 1, 2, 3, 4, 5 },
  3, { 0, 1, 2 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line4_Contact, "Line4_Contact", 8, 1, 2, { 0, 4, 8 }, Finley_Line2_Contact,
    { 0, 1, 4, 5 }, LineQuad, Line4Shape, Line4Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  4, { 0, 1, 2, 3 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri3_Contact, "Tri3_Contact", 6, 1, 2, { 0, 3, 6 }, Finley_Tri3_Contact,
    { 0, 1, 2, 3, 4, 5 }, TriQuad, Tri3Shape, Tri3Shape,
    { 0, 1, 2, 3, 4, 5 },
  3, { 0, 1, 2 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri6_Contact, "Tri6_Contact", 12, 1, 2, { 0, 6, 12 }, Finley_Tri3_Contact,
    { 0, 1, 2, 6, 7, 8 }, TriQuad, Tri6Shape, Tri6Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
  6, { 0, 1, 2, 3, 4, 5 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri9_Contact, "Tri9_Contact", 18, 1, 2, { 0, 9, 18 }, Finley_Tri3_Contact,
    { 0, 1, 2, 9, 10, 11 }, TriQuad, Tri9Shape, Tri9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri10_Contact, "Tri10_Contact", 20, 1, 2, { 0, 10, 20 }, Finley_Tri3_Contact,
    { 0, 1, 2, 10, 11, 12 }, TriQuad, Tri10Shape, Tri10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec4_Contact, "Rec4_Contact", 8, 1, 2, { 0, 4, 8 }, Finley_Rec4_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, RecQuad, Rec4Shape, Rec4Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  4, { 0, 1, 2, 3 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec8_Contact, "Rec8_Contact", 16, 1, 2, { 0, 8, 16 }, Finley_Rec4_Contact,
    { 0, 1, 2, 3, 8, 9, 10, 11 }, RecQuad, Rec8Shape, Rec8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  8, { 0, 1, 2, 3, 4, 5, 6, 7 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec9_Contact, "Rec9_Contact", 18, 1, 2, { 0, 9, 18 }, Finley_Rec4_Contact,
    { 0, 1, 2, 3, 9, 10, 11, 12 }, RecQuad, Rec9Shape, Rec9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec12_Contact, "Rec12_Contact", 24, 1, 2, { 0, 12, 24 }, Finley_Rec4_Contact,
    { 0, 1, 2, 3, 12, 13, 14, 15 }, RecQuad, Rec12Shape, Rec12Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },
  12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec16_Contact, "Rec16_Contact", 32, 1, 2, { 0, 16, 32 }, Finley_Rec4_Contact,
    { 0, 1, 2, 3, 16, 17, 18, 19 }, RecQuad, Rec16Shape, Rec16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
  16, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line2Face_Contact, "Line2Face_Contact", 4, 1, 2, { 0, 2, 4 }, Finley_Line2Face_Contact,
    { 0, 1, 2, 3 }, PointQuad, Line2Shape, Line2Shape,
    { 0, 1, 2, 3 },
  1, { 0 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line3Face_Contact, "Line3Face_Contact", 6, 1, 2, { 0, 3, 6 }, Finley_Line2Face_Contact,
    { 0, 1, 3, 4 }, PointQuad, Line3Shape, Line3Shape,
    { 0, 1, 2, 3, 4, 5 },
  1, { 0 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line4Face_Contact, "Line4Face_Contact", 8, 1, 2, { 0, 4, 8 }, Finley_Line2Face_Contact,
    { 0, 1, 4, 5 }, PointQuad, Line4Shape, Line4Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  1, { 0 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri3Face_Contact, "Tri3Face_Contact", 6, 1, 2, { 0, 3, 6 }, Finley_Tri3Face_Contact,
    { 0, 1, 2, 3, 4, 5 }, LineQuad, Tri3Shape, Tri3Shape,
    { 0, 1, 2, 3, 4, 5 },
  2, { 0, 1 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri6Face_Contact, "Tri6Face_Contact", 12, 1, 2, { 0, 6, 12 }, Finley_Tri3Face_Contact,
    { 0, 1, 2, 6, 7, 8 }, LineQuad, Tri6Shape, Tri6Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
  3, { 0, 1, 3 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri9Face_Contact, "Tri9Face_Contact", 18, 1, 2, { 0, 9, 18 }, Finley_Tri3Face_Contact,
    { 0, 1, 2, 9, 10, 11 }, LineQuad, Tri9Shape, Tri9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
  4, { 0, 1, 3, 4 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tri10Face_Contact, "Tri10Face_Contact", 20, 1, 2, { 0, 10, 20 }, Finley_Tri3Face_Contact,
    { 0, 1, 2, 10, 11, 12 }, LineQuad, Tri10Shape, Tri10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  4, { 0, 1, 3, 4 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec4Face_Contact, "Rec4Face_Contact", 8, 1, 2, { 0, 4, 8 }, Finley_Rec4Face_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, LineQuad, Rec4Shape, Rec4Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  2, { 0, 1 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec8Face_Contact, "Rec8Face_Contact", 16, 1, 2, { 0, 8, 16 }, Finley_Rec4Face_Contact,
    { 0, 1, 2, 3, 8, 9, 10, 11 }, LineQuad, Rec8Shape, Rec8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  3, { 0, 1, 4 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec9Face_Contact, "Rec9Face_Contact", 18, 1, 2, { 0, 9, 18 }, Finley_Rec4Face_Contact,
    { 0, 1, 2, 3, 9, 10, 11, 12 }, LineQuad, Rec9Shape, Rec9Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
  3, { 0, 1, 4 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec12Face_Contact, "Rec12Face_Contact", 24, 1, 2, { 0, 12, 24 }, Finley_Rec4Face_Contact,
    { 0, 1, 2, 3, 12, 13, 14, 15 }, LineQuad, Rec12Shape, Rec12Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },
  4, { 0, 1, 4, 5 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Rec16Face_Contact, "Rec16Face_Contact", 32, 1, 2, { 0, 16, 32 }, Finley_Rec4Face_Contact,
    { 0, 1, 2, 3, 16, 17, 18, 19 }, LineQuad, Rec16Shape, Rec16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
  4, { 0, 1, 4, 5 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tet4Face_Contact, "Tet4Face_Contact", 8, 1, 2, { 0, 4, 8 }, Finley_Tet4Face_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, TriQuad, Tet4Shape, Tet4Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7 },
  3, { 0, 1, 2 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tet10Face_Contact, "Tet10Face_Contact", 20, 1, 2, { 0, 10, 20 }, Finley_Tet4Face_Contact,
    { 0, 1, 2, 3, 10, 11, 12, 13 }, TriQuad, Tet10Shape, Tet10Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  6, { 0, 1, 2, 4, 5, 6 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Tet16Face_Contact, "Tet16Face_Contact", 32, 1, 2, { 0, 16, 32 }, Finley_Tet4Face_Contact,
    { 0, 1, 2, 3, 16, 17, 18, 19 }, TriQuad, Tet16Shape, Tet16Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
  9, { 0, 1, 2, 4, 5, 6, 7, 8, 9 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Hex8Face_Contact, "Hex8Face_Contact", 16, 1, 2, { 0, 8, 16 }, Finley_Hex8Face_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }, RecQuad, Hex8Shape, Hex8Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
  4, { 0, 1, 2, 3 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Hex20Face_Contact, "Hex20Face_Contact", 40, 1, 2, { 0, 20, 40 }, Finley_Hex8Face_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7, 20, 21, 22, 23, 24, 25, 26, 27 }, RecQuad, Hex20Shape, Hex20Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  8, { 0, 1, 2, 3, 8, 9, 10, 11 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Hex27Face_Contact, "Hex27Face_Contact", 54, 1, 2, { 0, 27, 54 }, Finley_Hex8Face_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7, 27, 28, 29, 30, 31, 32, 33, 34 }, RecQuad, Hex27Shape, Hex27Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53 },
  9, { 0, 1, 2, 3, 8, 9, 10, 11, 20 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Hex32Face_Contact, "Hex32Face_Contact", 64, 1, 2, { 0, 32, 64 }, Finley_Hex8Face_Contact,
    { 0, 1, 2, 3, 4, 5, 6, 7, 32, 33, 34, 35, 36, 37, 38, 39 }, RecQuad, Hex32Shape, Hex32Shape,
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 },
  12, { 0, 1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 15 },
  -1, { -1 },
    { -1 },
    { -1 } },
{ Finley_Line3Macro, "Line3Macro", 3, 2, 1, { 0, 3 }, Finley_Line2,
    { 0, 1 }, LineQuad, Line3Shape, Line2Shape,
    { 0, 2, 2, 1 },
  3, { 0, 1, 2 },
  3, { 0, 1, 2 },
    { 1, 0, 2 },
    { -1 } },
{ Finley_Tri6Macro, "Tri6Macro", 6, 4, 1, { 0, 6 }, Finley_Tri3,
    { 0, 1, 2 }, TriQuad, Tri6Shape, Tri3Shape,
    { 0, 3, 5,  5, 4, 2,  3, 1, 4,  4, 5, 3},
  6, { 0, 1, 2, 3, 4, 5 },
  6, { 0, 1, 2, 3, 4, 5 },
    { 1, 2, 0, 4, 5, 3 },
    { 0, 2, 1, 5, 4, 3 } },
{ Finley_Rec9Macro, "Rec9Macro", 9, 4, 1, { 0, 9 }, Finley_Rec4,
    { 0, 1, 2, 3 }, RecQuad, Rec9Shape, Rec4Shape,
    { 0, 4, 8, 7, 7, 8, 6, 3, 4, 1, 5, 8, 8, 5, 2, 6 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  9, { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 1, 2, 3, 0, 5, 6, 7, 4, 8 },
    { 0, 3, 2, 1, 7, 6, 5, 4, 8 } },
{ Finley_Tet10Macro, "Tet10Macro", 10, 8, 1, { 0, 10 }, Finley_Tet4,
    { 0, 1, 2, 3 }, TetQuad, Tet10Shape, Tet4Shape,
    { 0, 4, 6, 7, 4, 1, 5, 8, 6, 5, 2, 9, 7, 8, 9, 3, 4, 5, 6, 8, 5, 9, 8, 6, 9, 7, 6, 8, 7, 4, 6, 8 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
  10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
    { -1 },
    { -1 } },
{ Finley_Hex27Macro, "Hex27Macro", 27, 8, 1, { 0, 27 }, Finley_Hex8,
    { 0, 1, 2, 3, 4, 5, 6, 7 }, HexQuad, Hex27Shape, Hex8Shape,
    { 0, 8, 20, 11, 12, 21, 26, 24, 8, 1, 9, 20, 21, 13, 22, 26, 11, 20, 10, 3, 24, 26, 23, 15, 20, 9, 2, 10, 26, 22, 14, 23, 12, 21, 26, 24, 4, 16, 25, 19, 21, 13, 22, 26, 16, 5, 17, 25, 24, 26, 23, 15, 19, 25, 18, 7, 26, 22, 14, 23, 25, 17, 6, 18 },
  27, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 },
  27, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 },
    { -1 },
    { -1 } },
{ Finley_NoRef, "noElement", 0, 0, 0, { 0 }, Finley_NoRef,
    { 0 }, NoQuad, NoShape, NoShape,
    { 0 },
  -1, { 0 },
  -1, { 0 },
    { 0 },
    { 0 } }

};
/**************************************************************
  
  creates a ReferenceElement of type id and a given integration order 

  */


Finley_ReferenceElement* Finley_ReferenceElement_alloc(Finley_ElementTypeId id, int order) 
{
	dim_t nsub, numQuadNodes, numQuadNodes2;
	double *quadWeights=NULL, *quadNodes=NULL, *quadWeights2=NULL, *quadNodes2=NULL;
	Finley_ReferenceElement *out=NULL;
	Finley_QuadInfo* quadscheme;
	Finley_ShapeFunctionInfo* parametrization, *basisfunction, *linearbasisfunction;
	Finley_ReferenceElementInfo *type, *linear_type;

        type=Finley_ReferenceElement_getInfo(id);
        if (type == NULL) { 
             Finley_setError(VALUE_ERROR,"Finley_ReferenceElement_alloc: unable to identify element type.");
            return NULL;
        }
        linear_type=Finley_ReferenceElement_getInfo(type->LinearTypeId);
        if (linear_type == NULL) {
            Finley_setError(VALUE_ERROR,"Finley_ReferenceElement_alloc: unable to identify linear element type.");
            return NULL;
        }
        
	
	
	out=MEMALLOC(1,Finley_ReferenceElement);
    if (Finley_checkPtr(out)) return NULL;
	
	out->reference_counter=0;
	out->Parametrization=NULL;
	out->BasisFunctions=NULL;
	out->LinearBasisFunctions=NULL;
	out->Type=type;
	out->numNodes=out->Type->numNodes;
	out->LinearType=linear_type;
	out->numLinearNodes=out->LinearType->numNodes;
        out->integrationOrder=-1;
        out->DBasisFunctionDv=NULL;
        out->DBasisFunctionDvShared=TRUE;

	quadscheme=Finley_QuadInfo_getInfo(out->Type->Quadrature);
	parametrization=Finley_ShapeFunction_getInfo(out->Type->Parametrization);
	basisfunction=Finley_ShapeFunction_getInfo(out->Type->BasisFunctions);
	linearbasisfunction=Finley_ShapeFunction_getInfo(Finley_ReferenceElement_InfoList[out->Type->LinearTypeId].BasisFunctions);
	nsub=out->Type->numSubElements;
        out->numLocalDim=quadscheme->numDim;
        
	
	/*  set up the basic integration scheme 
	    note that quadscheme->numDim is not necessarily the dimension of the element 
	*/
	
	if (order<0) order=MAX(2*basisfunction->numOrder,0);
        out->integrationOrder=order;

    numQuadNodes=quadscheme->getNumQuadNodes(order);
	
    quadNodes=MEMALLOC(numQuadNodes*quadscheme->numDim*nsub,double);
    quadWeights=MEMALLOC(numQuadNodes*nsub,double);	
	if ( !( Finley_checkPtr(quadNodes) || Finley_checkPtr(quadWeights) ) ) {
	
		quadscheme->getQuadNodes(numQuadNodes, quadNodes, quadWeights);
	
		/*  set the basis functions on the quadrature points:
		 *  note: Finley_ShapeFunction_alloc will introduce 0. If the dimensions of the quadrature scheme and the dimension of the element don't match.
		 */


		/*
	     *	  before we can set the shape function for the parametrization the quadrature scheme needs to be replicated :
		 */
	
		if (nsub>1) {
                        out->DBasisFunctionDv=MEMALLOC(numQuadNodes*nsub* (basisfunction->numShapes) * (basisfunction->numDim), double );
                        out->DBasisFunctionDvShared=FALSE;
                        
		        out->BasisFunctions=Finley_ShapeFunction_alloc(basisfunction->TypeId, quadscheme->numDim, numQuadNodes, quadNodes, quadWeights);
			quadNodes2=MEMALLOC(numQuadNodes*quadscheme->numDim*nsub,double);
			quadWeights2=MEMALLOC(numQuadNodes*nsub,double);
			if ( !( Finley_checkPtr(quadNodes2) || Finley_checkPtr(quadWeights2) || Finley_checkPtr(out->DBasisFunctionDv)) ) {

				numQuadNodes2=quadscheme->getMacro(nsub,out->BasisFunctions->numQuadNodes,
                                                                        out->BasisFunctions->QuadNodes,
                                                                        out->BasisFunctions->QuadWeights,
                                                                        out->BasisFunctions->Type->numShapes, out->BasisFunctions->dSdv,
                                                                        numQuadNodes*nsub, quadNodes2, quadWeights2,
                                                                        out->DBasisFunctionDv);
				if (Finley_noError()) {
					out->Parametrization=Finley_ShapeFunction_alloc(parametrization->TypeId, quadscheme->numDim, numQuadNodes2, quadNodes2, quadWeights2);
					out->LinearBasisFunctions=Finley_ShapeFunction_alloc(linearbasisfunction->TypeId, quadscheme->numDim, numQuadNodes2, quadNodes2, quadWeights2); 
				}
				
			}
			MEMFREE(quadNodes2);
			MEMFREE(quadWeights2);
			
		} else {
			out->Parametrization=Finley_ShapeFunction_alloc(parametrization->TypeId, quadscheme->numDim, numQuadNodes*nsub, quadNodes, quadWeights);
		        out->BasisFunctions=Finley_ShapeFunction_alloc(basisfunction->TypeId, quadscheme->numDim, numQuadNodes, quadNodes, quadWeights);
			out->LinearBasisFunctions=Finley_ShapeFunction_alloc(linearbasisfunction->TypeId, quadscheme->numDim, numQuadNodes, quadNodes, quadWeights);
                        out->DBasisFunctionDv=out->BasisFunctions->dSdv;
                        out->DBasisFunctionDvShared=TRUE;
		}
	}
	MEMFREE(quadNodes);
	MEMFREE(quadWeights);
    if (Finley_noError()) {
		return Finley_ReferenceElement_reference(out);
	} else {
        Finley_ReferenceElement_dealloc(out);
        return NULL;
	} 
}

/**************************************************************/

void Finley_ReferenceElement_dealloc(Finley_ReferenceElement* in) {
	if (in!=NULL) {
		in->reference_counter--;
		if (in->reference_counter<1) {
			Finley_ShapeFunction_dealloc(in->Parametrization);
			Finley_ShapeFunction_dealloc(in->BasisFunctions);
			Finley_ShapeFunction_dealloc(in->LinearBasisFunctions);
                        if (! in->DBasisFunctionDvShared) MEMFREE(in->DBasisFunctionDv);
			MEMFREE(in);
		}
	}

}

/**************************************************************/

Finley_ElementTypeId Finley_ReferenceElement_getTypeId(char* element_type) {
    int ptr=0;
    Finley_ElementTypeId out=Finley_NoRef;
    while (Finley_ReferenceElement_InfoList[ptr].TypeId!=Finley_NoRef && out==Finley_NoRef) {
       if (strcmp(element_type,Finley_ReferenceElement_InfoList[ptr].Name)==0) out=Finley_ReferenceElement_InfoList[ptr].TypeId;
       ptr++;
    }
    return out;
}

Finley_ReferenceElement* Finley_ReferenceElement_reference(Finley_ReferenceElement* in) {
     if (in!=NULL) ++(in->reference_counter);
     return in;
}

Finley_ReferenceElementInfo* Finley_ReferenceElement_getInfo(Finley_ElementTypeId id)
{
    int ptr=0;
    Finley_ReferenceElementInfo* out=NULL;
    while (Finley_ReferenceElement_InfoList[ptr].TypeId!=Finley_NoRef && out==NULL) {
       if (Finley_ReferenceElement_InfoList[ptr].TypeId==id) out=&(Finley_ReferenceElement_InfoList[ptr]);
       ptr++;
    }
    if (out==NULL) {
        Finley_setError(VALUE_ERROR,"Finley_ReferenceElement_getInfo: cannot find requested reference element.");
    }
    return out;
}


