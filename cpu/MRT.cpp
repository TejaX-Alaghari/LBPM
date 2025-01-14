/*
  Copyright 2013--2018 James E. McClure, Virginia Polytechnic & State University

  This file is part of the Open Porous Media project (OPM).
  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

extern "C" void INITIALIZE(char *ID, double *f_even, double *f_odd, int Nx,
                           int Ny, int Nz) {
    int n, N;
    N = Nx * Ny * Nz;

    for (n = 0; n < N; n++) {

        if (ID[n] > 0) {
            f_even[n] = 0.3333333333333333;
            f_odd[n] = 0.055555555555555555;          //double(100*n)+1.f;
            f_even[N + n] = 0.055555555555555555;     //double(100*n)+2.f;
            f_odd[N + n] = 0.055555555555555555;      //double(100*n)+3.f;
            f_even[2 * N + n] = 0.055555555555555555; //double(100*n)+4.f;
            f_odd[2 * N + n] = 0.055555555555555555;  //double(100*n)+5.f;
            f_even[3 * N + n] = 0.055555555555555555; //double(100*n)+6.f;
            f_odd[3 * N + n] = 0.0277777777777778;    //double(100*n)+7.f;
            f_even[4 * N + n] = 0.0277777777777778;   //double(100*n)+8.f;
            f_odd[4 * N + n] = 0.0277777777777778;    //double(100*n)+9.f;
            f_even[5 * N + n] = 0.0277777777777778;   //double(100*n)+10.f;
            f_odd[5 * N + n] = 0.0277777777777778;    //double(100*n)+11.f;
            f_even[6 * N + n] = 0.0277777777777778;   //double(100*n)+12.f;
            f_odd[6 * N + n] = 0.0277777777777778;    //double(100*n)+13.f;
            f_even[7 * N + n] = 0.0277777777777778;   //double(100*n)+14.f;
            f_odd[7 * N + n] = 0.0277777777777778;    //double(100*n)+15.f;
            f_even[8 * N + n] = 0.0277777777777778;   //double(100*n)+16.f;
            f_odd[8 * N + n] = 0.0277777777777778;    //double(100*n)+17.f;
            f_even[9 * N + n] = 0.0277777777777778;   //double(100*n)+18.f;
        } else {
            for (int q = 0; q < 9; q++) {
                f_even[q * N + n] = -1.0;
                f_odd[q * N + n] = -1.0;
            }
            f_even[9 * N + n] = -1.0;
        }
    }
}

extern "C" void Compute_VELOCITY(char *ID, double *disteven, double *distodd,
                                 double *vel, int Nx, int Ny, int Nz) {
    int n, N;
    // distributions
    double f1, f2, f3, f4, f5, f6, f7, f8, f9;
    double f10, f11, f12, f13, f14, f15, f16, f17, f18;
    double vx, vy, vz;

    N = Nx * Ny * Nz;
    for (n = 0; n < N; n++) {

        if (ID[n] > 0) {
            //........................................................................
            // Registers to store the distributions
            //........................................................................
            f2 = disteven[N + n];
            f4 = disteven[2 * N + n];
            f6 = disteven[3 * N + n];
            f8 = disteven[4 * N + n];
            f10 = disteven[5 * N + n];
            f12 = disteven[6 * N + n];
            f14 = disteven[7 * N + n];
            f16 = disteven[8 * N + n];
            f18 = disteven[9 * N + n];
            //........................................................................
            f1 = distodd[n];
            f3 = distodd[1 * N + n];
            f5 = distodd[2 * N + n];
            f7 = distodd[3 * N + n];
            f9 = distodd[4 * N + n];
            f11 = distodd[5 * N + n];
            f13 = distodd[6 * N + n];
            f15 = distodd[7 * N + n];
            f17 = distodd[8 * N + n];
            //.................Compute the velocity...................................
            vx = f1 - f2 + f7 - f8 + f9 - f10 + f11 - f12 + f13 - f14;
            vy = f3 - f4 + f7 - f8 - f9 + f10 + f15 - f16 + f17 - f18;
            vz = f5 - f6 + f11 - f12 - f13 + f14 + f15 - f16 - f17 + f18;
            //..................Write the velocity.....................................
            vel[n] = vx;
            vel[N + n] = vy;
            vel[2 * N + n] = vz;
            //........................................................................
        }
    }
}

//*************************************************************************
extern "C" void ScaLBL_D3Q19_MRT(char *ID, double *disteven, double *distodd,
                                 int Nx, int Ny, int Nz, double rlx_setA,
                                 double rlx_setB, double Fx, double Fy,
                                 double Fz) {

    int n, N;
    // distributions
    double f0, f1, f2, f3, f4, f5, f6, f7, f8, f9;
    double f10, f11, f12, f13, f14, f15, f16, f17, f18;

    // conserved momemnts
    double rho, jx, jy, jz;
    // non-conserved moments
    double m1, m2, m4, m6, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18;

    N = Nx * Ny * Nz;

    char id;
    for (n = 0; n < N; n++) {
        id = ID[n];

        if (id > 0) {
            //........................................................................
            // Registers to store the distributions - read based on swap convention
            //........................................................................
            f2 = distodd[n];
            f4 = distodd[N + n];
            f6 = distodd[2 * N + n];
            f8 = distodd[3 * N + n];
            f10 = distodd[4 * N + n];
            f12 = distodd[5 * N + n];
            f14 = distodd[6 * N + n];
            f16 = distodd[7 * N + n];
            f18 = distodd[8 * N + n];
            //........................................................................
            f0 = disteven[n];
            f1 = disteven[N + n];
            f3 = disteven[2 * N + n];
            f5 = disteven[3 * N + n];
            f7 = disteven[4 * N + n];
            f9 = disteven[5 * N + n];
            f11 = disteven[6 * N + n];
            f13 = disteven[7 * N + n];
            f15 = disteven[8 * N + n];
            f17 = disteven[9 * N + n];
            //........................................................................
            //....................compute the moments...............................................
            rho = f0 + f2 + f1 + f4 + f3 + f6 + f5 + f8 + f7 + f10 + f9 + f12 +
                  f11 + f14 + f13 + f16 + f15 + f18 + f17;
            m1 = -30 * f0 - 11 * (f2 + f1 + f4 + f3 + f6 + f5) +
                 8 * (f8 + f7 + f10 + f9 + f12 + f11 + f14 + f13 + f16 + f15 +
                      f18 + f17);
            m2 = 12 * f0 - 4 * (f2 + f1 + f4 + f3 + f6 + f5) + f8 + f7 + f10 +
                 f9 + f12 + f11 + f14 + f13 + f16 + f15 + f18 + f17;
            jx = f1 - f2 + f7 - f8 + f9 - f10 + f11 - f12 + f13 - f14;
            m4 = 4 * (-f1 + f2) + f7 - f8 + f9 - f10 + f11 - f12 + f13 - f14;
            jy = f3 - f4 + f7 - f8 - f9 + f10 + f15 - f16 + f17 - f18;
            m6 = -4 * (f3 - f4) + f7 - f8 - f9 + f10 + f15 - f16 + f17 - f18;
            jz = f5 - f6 + f11 - f12 - f13 + f14 + f15 - f16 - f17 + f18;
            m8 = -4 * (f5 - f6) + f11 - f12 - f13 + f14 + f15 - f16 - f17 + f18;
            m9 = 2 * (f1 + f2) - f3 - f4 - f5 - f6 + f7 + f8 + f9 + f10 + f11 +
                 f12 + f13 + f14 - 2 * (f15 + f16 + f17 + f18);
            m10 = -4 * (f1 + f2) + 2 * (f4 + f3 + f6 + f5) + f8 + f7 + f10 +
                  f9 + f12 + f11 + f14 + f13 - 2 * (f16 + f15 + f18 + f17);
            m11 =
                f4 + f3 - f6 - f5 + f8 + f7 + f10 + f9 - f12 - f11 - f14 - f13;
            m12 = -2 * (f4 + f3 - f6 - f5) + f8 + f7 + f10 + f9 - f12 - f11 -
                  f14 - f13;
            m13 = f8 + f7 - f10 - f9;
            m14 = f16 + f15 - f18 - f17;
            m15 = f12 + f11 - f14 - f13;
            m16 = f7 - f8 + f9 - f10 - f11 + f12 - f13 + f14;
            m17 = -f7 + f8 + f9 - f10 + f15 - f16 + f17 - f18;
            m18 = f11 - f12 - f13 + f14 - f15 + f16 + f17 - f18;
            //..............incorporate external force................................................
            //jx += 0.5*Fx;
            //jy += 0.5*Fy;
            //jz += 0.5*Fz;
            //..............carry out relaxation process...............................................
            m1 = m1 + rlx_setA * ((19 * (jx * jx + jy * jy + jz * jz) / rho -
                                   11 * rho) -
                                  m1);
            m2 = m2 + rlx_setA * ((3 * rho -
                                   5.5 * (jx * jx + jy * jy + jz * jz) / rho) -
                                  m2);
            m4 = m4 + rlx_setB * ((-0.6666666666666666 * jx) - m4);
            m6 = m6 + rlx_setB * ((-0.6666666666666666 * jy) - m6);
            m8 = m8 + rlx_setB * ((-0.6666666666666666 * jz) - m8);
            m9 = m9 +
                 rlx_setA * (((2 * jx * jx - jy * jy - jz * jz) / rho) - m9);
            m10 = m10 +
                  rlx_setA *
                      (-0.5 * ((2 * jx * jx - jy * jy - jz * jz) / rho) - m10);
            m11 = m11 + rlx_setA * (((jy * jy - jz * jz) / rho) - m11);
            m12 = m12 + rlx_setA * (-0.5 * ((jy * jy - jz * jz) / rho) - m12);
            m13 = m13 + rlx_setA * ((jx * jy / rho) - m13);
            m14 = m14 + rlx_setA * ((jy * jz / rho) - m14);
            m15 = m15 + rlx_setA * ((jx * jz / rho) - m15);
            m16 = m16 + rlx_setB * (-m16);
            m17 = m17 + rlx_setB * (-m17);
            m18 = m18 + rlx_setB * (-m18);
            //.................inverse transformation......................................................
            f0 = 0.05263157894736842 * rho - 0.012531328320802 * m1 +
                 0.04761904761904762 * m2;
            f1 = 0.05263157894736842 * rho - 0.004594820384294068 * m1 -
                 0.01587301587301587 * m2 + 0.1 * (jx - m4) +
                 0.05555555555555555 * (m9 - m10);
            f2 = 0.05263157894736842 * rho - 0.004594820384294068 * m1 -
                 0.01587301587301587 * m2 + 0.1 * (m4 - jx) +
                 0.05555555555555555 * (m9 - m10);
            f3 = 0.05263157894736842 * rho - 0.004594820384294068 * m1 -
                 0.01587301587301587 * m2 + 0.1 * (jy - m6) +
                 0.02777777777777778 * (m10 - m9) +
                 0.08333333333333333 * (m11 - m12);
            f4 = 0.05263157894736842 * rho - 0.004594820384294068 * m1 -
                 0.01587301587301587 * m2 + 0.1 * (m6 - jy) +
                 0.02777777777777778 * (m10 - m9) +
                 0.08333333333333333 * (m11 - m12);
            f5 = 0.05263157894736842 * rho - 0.004594820384294068 * m1 -
                 0.01587301587301587 * m2 + 0.1 * (jz - m8) +
                 0.02777777777777778 * (m10 - m9) +
                 0.08333333333333333 * (m12 - m11);
            f6 = 0.05263157894736842 * rho - 0.004594820384294068 * m1 -
                 0.01587301587301587 * m2 + 0.1 * (m8 - jz) +
                 0.02777777777777778 * (m10 - m9) +
                 0.08333333333333333 * (m12 - m11);
            f7 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                 0.003968253968253968 * m2 + 0.1 * (jx + jy) +
                 0.025 * (m4 + m6) + 0.02777777777777778 * m9 +
                 0.01388888888888889 * m10 + 0.08333333333333333 * m11 +
                 0.04166666666666666 * m12 + 0.25 * m13 + 0.125 * (m16 - m17);
            f8 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                 0.003968253968253968 * m2 - 0.1 * (jx + jy) -
                 0.025 * (m4 + m6) + 0.02777777777777778 * m9 +
                 0.01388888888888889 * m10 + 0.08333333333333333 * m11 +
                 0.04166666666666666 * m12 + 0.25 * m13 + 0.125 * (m17 - m16);
            f9 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                 0.003968253968253968 * m2 + 0.1 * (jx - jy) +
                 0.025 * (m4 - m6) + 0.02777777777777778 * m9 +
                 0.01388888888888889 * m10 + 0.08333333333333333 * m11 +
                 0.04166666666666666 * m12 - 0.25 * m13 + 0.125 * (m16 + m17);
            f10 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jy - jx) +
                  0.025 * (m6 - m4) + 0.02777777777777778 * m9 +
                  0.01388888888888889 * m10 + 0.08333333333333333 * m11 +
                  0.04166666666666666 * m12 - 0.25 * m13 - 0.125 * (m16 + m17);
            f11 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jx + jz) +
                  0.025 * (m4 + m8) + 0.02777777777777778 * m9 +
                  0.01388888888888889 * m10 - 0.08333333333333333 * m11 -
                  0.04166666666666666 * m12 + 0.25 * m15 + 0.125 * (m18 - m16);
            f12 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 - 0.1 * (jx + jz) -
                  0.025 * (m4 + m8) + 0.02777777777777778 * m9 +
                  0.01388888888888889 * m10 - 0.08333333333333333 * m11 -
                  0.04166666666666666 * m12 + 0.25 * m15 + 0.125 * (m16 - m18);
            f13 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jx - jz) +
                  0.025 * (m4 - m8) + 0.02777777777777778 * m9 +
                  0.01388888888888889 * m10 - 0.08333333333333333 * m11 -
                  0.04166666666666666 * m12 - 0.25 * m15 - 0.125 * (m16 + m18);
            f14 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jz - jx) +
                  0.025 * (m8 - m4) + 0.02777777777777778 * m9 +
                  0.01388888888888889 * m10 - 0.08333333333333333 * m11 -
                  0.04166666666666666 * m12 - 0.25 * m15 + 0.125 * (m16 + m18);
            f15 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jy + jz) +
                  0.025 * (m6 + m8) - 0.05555555555555555 * m9 -
                  0.02777777777777778 * m10 + 0.25 * m14 + 0.125 * (m17 - m18);
            f16 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 - 0.1 * (jy + jz) -
                  0.025 * (m6 + m8) - 0.05555555555555555 * m9 -
                  0.02777777777777778 * m10 + 0.25 * m14 + 0.125 * (m18 - m17);
            f17 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jy - jz) +
                  0.025 * (m6 - m8) - 0.05555555555555555 * m9 -
                  0.02777777777777778 * m10 - 0.25 * m14 + 0.125 * (m17 + m18);
            f18 = 0.05263157894736842 * rho + 0.003341687552213868 * m1 +
                  0.003968253968253968 * m2 + 0.1 * (jz - jy) +
                  0.025 * (m8 - m6) - 0.05555555555555555 * m9 -
                  0.02777777777777778 * m10 - 0.25 * m14 - 0.125 * (m17 + m18);
            //.......................................................................................................
            // incorporate external force
            f1 += 0.16666666 * Fx;
            f2 -= 0.16666666 * Fx;
            f3 += 0.16666666 * Fy;
            f4 -= 0.16666666 * Fy;
            f5 += 0.16666666 * Fz;
            f6 -= 0.16666666 * Fz;
            f7 += 0.08333333333 * (Fx + Fy);
            f8 -= 0.08333333333 * (Fx + Fy);
            f9 += 0.08333333333 * (Fx - Fy);
            f10 -= 0.08333333333 * (Fx - Fy);
            f11 += 0.08333333333 * (Fx + Fz);
            f12 -= 0.08333333333 * (Fx + Fz);
            f13 += 0.08333333333 * (Fx - Fz);
            f14 -= 0.08333333333 * (Fx - Fz);
            f15 += 0.08333333333 * (Fy + Fz);
            f16 -= 0.08333333333 * (Fy + Fz);
            f17 += 0.08333333333 * (Fy - Fz);
            f18 -= 0.08333333333 * (Fy - Fz);
            //.......................................................................................................
            // Write data based on un-swapped convention
            disteven[n] = f0;
            disteven[N + n] = f2;
            disteven[2 * N + n] = f4;
            disteven[3 * N + n] = f6;
            disteven[4 * N + n] = f8;
            disteven[5 * N + n] = f10;
            disteven[6 * N + n] = f12;
            disteven[7 * N + n] = f14;
            disteven[8 * N + n] = f16;
            disteven[9 * N + n] = f18;

            distodd[n] = f1;
            distodd[N + n] = f3;
            distodd[2 * N + n] = f5;
            distodd[3 * N + n] = f7;
            distodd[4 * N + n] = f9;
            distodd[5 * N + n] = f11;
            distodd[6 * N + n] = f13;
            distodd[7 * N + n] = f15;
            distodd[8 * N + n] = f17;
            //.......................................................................................................
        }
    }
}
