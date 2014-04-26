///////////////////////////////////////////////////////
//   Find average
//   Testing example for CS 470
//   This version is not correct
///////////////////////////////////////////////////////
#include <iostream>
using namespace std;

int main()
  {
  double average;
  int    n;
  int    sum;
  double x;

  sum = 0;
  n = 0;
  while (true)
    {
    cin >> x;
    if (cin . fail())
      break;
    n ++;
    sum += x;
    }
  average = sum / n;
  cout << average << endl;
  }

