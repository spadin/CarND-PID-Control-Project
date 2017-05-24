#include "Twiddle.h"
#include <stdio.h>

using namespace std;

Twiddle::Twiddle() {}
Twiddle::~Twiddle() {}

void Twiddle::Init(PID &pid, int stepsPerRun)
{
    this->pid = pid;
    this->step = 0;
    this->totalError = 0.0;
    this->stepsPerRun = stepsPerRun;
    this->bestError = 1e10;

    this->p = new double[3]{0.05, 0.0005, 1.2};
    this->dp = new double[3]{0.5, 0.5, 0.5};
    this->pidx = 0;
    this->tol = 0.2;

    ReInitPid();
}

void Twiddle::UpdateError(double cte, uWS::WebSocket<uWS::SERVER> &ws)
{
    if((dp[0] + dp[1] + dp[2]) < tol)
    {
        printf("Optimized (%f, %f, %f)\n", p[0], p[1], p[2]);
        return;
    }

    totalError += (cte * cte);

    if(step == 0)
    {
        p[pidx] += dp[pidx];
        printf("pidx: %d\n", pidx);
        ReInitPid();
        ResetSimulator(ws);
    }

    step++;

    if(step == stepsPerRun)
    {
        double error = AverageError();
        if (error < bestError)
        {
            bestError = error;
            dp[pidx] *= 1.1;
            IncrementPidx();
            Reset();
        }
        else
        {
            p[pidx] -= 2 * dp[pidx];
        }

        printf("1: p=(%f, %f, %f) dp=(%f, %f, %f), error=%f, bestError=%f\n", p[0], p[1], p[2], dp[0], dp[1], dp[2], error, bestError);
        ReInitPid();
        ResetSimulator(ws);
    }

    if(step == 2 * stepsPerRun) {
        double error = AverageError();
        if(error < bestError)
        {
            bestError = error;
            dp[pidx] *= 1.1;
        }
        else
        {
            p[pidx] += dp[pidx];
            dp[pidx] *= 0.9;
        }

        IncrementPidx();
        Reset();

        printf("2: p=(%f, %f, %f) dp=(%f, %f, %f), error=%f, bestError=%f\n", p[0], p[1], p[2], dp[0], dp[1], dp[2], error, bestError);
        ReInitPid();
        ResetSimulator(ws);
    }
}

void Twiddle::ReInitPid()
{
    pid.Init(p[0], p[1], p[2]);
}

void Twiddle::IncrementPidx()
{
    pidx += 1;
    pidx %= 3;
}

void Twiddle::ResetSimulator(uWS::WebSocket<uWS::SERVER> &ws)
{
    std::string msg("42[\"reset\", {}]");
    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
}

void Twiddle::Reset()
{
    step = 0;
    totalError = 0.0;
}

double Twiddle::AverageError()
{
    if (step > 0)
    {
        return totalError / step;
    }
    else
    {
        return 0.0;
    }
}