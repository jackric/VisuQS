/****************************************************************************
Program:        VisuQS.LX / VQS.exe
File:           quickSort.cpp / quickSort.h
Description:    Efficient sorting algorithm
Source:         http://linux.wku.edu/~lamonml/algor/sort/quick.html
Notes:          Used to sort the probabilities for 'fog' visualisation
Revision:       $Revision: 1.2 $
****************************************************************************/

/*
 * Source: http://linux.wku.edu/~lamonml/algor/sort/quick.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program demonstrates the use of the quick sort algorithm.  For
 * more information about this and other sorting algorithms, see
 * http://linux.wku.edu/~lamonml/kb.html
 *
 */


#include "quickSort.h"

using namespace std;



int q_sort(int left, int right, vector<float>& probs, vector<vector<int> >& coords)
{
    int l_hold, r_hold;
    float pivot;
    int pivotx, pivoty, pivotz;

    l_hold = left;
    r_hold = right;

    pivot = probs[left];
    pivotx = coords[left][0];
    pivoty = coords[left][1];
    pivotz = coords[left][2];

    while (left < right)
    {
        while ((probs[right] >= pivot) && (left < right))
            right--;
        if (left != right)
        {
            probs[left] = probs[right];

            coords[left][0] = coords[right][0];
            coords[left][1] = coords[right][1];
            coords[left][2] = coords[right][2];

            left++;
        }
        while ((probs[left] <= pivot) && (left < right))
            left++;
        if (left != right)
        {
            probs[right] = probs[left];

            coords[right][0] = coords[left][0];
            coords[right][1] = coords[left][1];
            coords[right][2] = coords[left][2];

            right--;
        }
    }
    probs[left] = pivot;
    coords[left][0] = pivotx;
    coords[left][1] = pivoty;
    coords[left][2] = pivotz;

    int temp = left;
    left = l_hold;
    right = r_hold;
    if (left < temp)
        q_sort(left, temp-1, probs, coords);
    if (right > temp)
        q_sort(temp+1, right, probs, coords);
}


void quickSort(vector<float>& localProbs, vector<vector<int> >& localCoords, int array_size)
{
#ifdef _DEBUG
    cout << "\nquickSort(): $Revision: 1.2 $";
#endif

    q_sort(0, array_size - 1, localProbs, localCoords);
}

