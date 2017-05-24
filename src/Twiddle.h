#ifndef Twiddle_H
#define Twiddle_H

#include <uWS/uWS.h>
#include "PID.h"

class Twiddle {
private:
  PID pid;
  double *dp;
  double *p;
  double bestError;
  double tol;
  double totalError;
  int pidx;
  int step;
  int stepsPerRun;

public:
  Twiddle();
  virtual ~Twiddle();

  double AverageError();
  void IncrementPidx();
  void IncrementStep();
  void Init(PID&, int);
  void ReInitPid();
  void Reset();
  void ResetSimulator(uWS::WebSocket<uWS::SERVER> &);
  void UpdateError(double, uWS::WebSocket<uWS::SERVER> &);
};

#endif /* Twiddle_H */
