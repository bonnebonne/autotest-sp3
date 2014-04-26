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
  double n;
  double sum;
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
  average = sum / 2;
  cout << average << endl;
  }

