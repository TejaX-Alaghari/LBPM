/*
  Copyright 2013--2018 James E. McClure, Virginia Polytechnic & State University
  Copyright Equnior ASA

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
#ifndef COMMUNICATION_H_INC
#define COMMUNICATION_H_INC

#include "common/MPI.h"
#include "common/Utilities.h"
#include "common/Array.h"

#include <array>

// ********** COMMUNICTION **************************************
/*
 //..............................................................
 // Communication helper routines for MPI
 //..............................................................
 */

using namespace std;

/*!
 * @brief  Rank info structure
 * @details  Structure used to hold the ranks for the current process and it's neighbors
 */
struct RankInfoStruct {
    int nx;            //!<  The number of processors in the x direction
    int ny;            //!<  The number of processors in the y direction
    int nz;            //!<  The number of processors in the z direction
    int ix;            //!<  The index of the current process in the x direction
    int jy;            //!<  The index of the current process in the y direction
    int kz;            //!<  The index of the current process in the z direction
    int rank[3][3][3]; //!<  The rank for the neighbor [i][j][k]
    RankInfoStruct();
    RankInfoStruct(int rank, int nprocx, int nprocy, int nprocz);
    int getRankForBlock(int i, int j, int k) const;
};

//! Redistribute domain data (dst may be smaller than the src)
template <class TYPE>
Array<TYPE>
redistribute(const RankInfoStruct &src_rank, const Array<TYPE> &src_data,
             const RankInfoStruct &dst_rank, std::array<int, 3> dst_size,
             const Utilities::MPI &comm);

/*!
 * @brief  Communicate halo
 * @details  Fill the halo cells in an array from the neighboring processes
 */
template <class TYPE> class fillHalo {
public:
    /*!
     * @brief  Default constructor
     * @param[in] comm          Communicator to use
     * @param[in] info          Rank and neighbor rank info
     * @param[in] n             Number of local cells
     * @param[in] ng            Number of ghost cells
     * @param[in] tag           Initial tag to use for the communication (we will require tag:tag+26)
     * @param[in] depth         Maximum depth to support
     * @param[in] fill          Fill {faces,edges,corners}
     * @param[in] periodic      Periodic dimensions
     */
    fillHalo(const Utilities::MPI &comm, const RankInfoStruct &info,
             std::array<int, 3> n, std::array<int, 3> ng, int tag, int depth,
             std::array<bool, 3> fill = {true, true, true},
             std::array<bool, 3> periodic = {true, true, true});

    //!  Destructor
    ~fillHalo();

    fillHalo() = delete;
    fillHalo(const fillHalo &) = delete;
    fillHalo &operator=(const fillHalo &) = delete;

    /*!
     * @brief  Communicate the halos
     * @param[in] array         The array on which we fill the halos
     */
    void fill(Array<TYPE> &array);

    /*!
     * @brief  Copy data from the src array to the dst array
     * @param[in] src           The src array with or without halos
     * @param[in] dst           The dst array with or without halos
     */
    template <class TYPE1, class TYPE2>
    void copy(const Array<TYPE1> &src, Array<TYPE2> &dst);

private:
    Utilities::MPI comm;
    RankInfoStruct info;
    std::array<int, 3> n, ng;
    int depth;
    bool fill_pattern[3][3][3];
    int tag[3][3][3];
    int N_send_recv[3][3][3];
    TYPE *mem;
    TYPE *send[3][3][3], *recv[3][3][3];
    MPI_Request send_req[3][3][3], recv_req[3][3][3];
    void pack(const Array<TYPE> &array, int i, int j, int k, TYPE *buffer);
    void unpack(Array<TYPE> &array, int i, int j, int k, const TYPE *buffer);
};

//***************************************************************************************
inline void PackMeshData(const int *list, int count, double *sendbuf,
                         double *data) {
    // Fill in the phase ID values from neighboring processors
    // This packs up the values that need to be sent from one processor to another
    int idx, n;
    for (idx = 0; idx < count; idx++) {
        n = list[idx];
        sendbuf[idx] = data[n];
    }
}
inline void UnpackMeshData(const int *list, int count, double *recvbuf,
                           double *data) {
    // Fill in the phase ID values from neighboring processors
    // This unpacks the values once they have been recieved from neighbors
    int idx, n;

    for (idx = 0; idx < count; idx++) {
        n = list[idx];
        data[n] = recvbuf[idx];
    }
}

// Initialize the ranks (this is deprecated, see RankInfoStruct)
void InitializeRanks(const int rank, const int nprocx, const int nprocy,
                     const int nprocz, int &iproc, int &jproc, int &kproc,
                     int &rank_x, int &rank_y, int &rank_z, int &rank_X,
                     int &rank_Y, int &rank_Z, int &rank_xy, int &rank_XY,
                     int &rank_xY, int &rank_Xy, int &rank_xz, int &rank_XZ,
                     int &rank_xZ, int &rank_Xz, int &rank_yz, int &rank_YZ,
                     int &rank_yZ, int &rank_Yz);

//***************************************************************************************
inline void CommunicateSendRecvCounts(
    const Utilities::MPI &comm, int sendtag, int recvtag, int rank_x,
    int rank_y, int rank_z, int rank_X, int rank_Y, int rank_Z, int rank_xy,
    int rank_XY, int rank_xY, int rank_Xy, int rank_xz, int rank_XZ,
    int rank_xZ, int rank_Xz, int rank_yz, int rank_YZ, int rank_yZ,
    int rank_Yz, int sendCount_x, int sendCount_y, int sendCount_z,
    int sendCount_X, int sendCount_Y, int sendCount_Z, int sendCount_xy,
    int sendCount_XY, int sendCount_xY, int sendCount_Xy, int sendCount_xz,
    int sendCount_XZ, int sendCount_xZ, int sendCount_Xz, int sendCount_yz,
    int sendCount_YZ, int sendCount_yZ, int sendCount_Yz, int &recvCount_x,
    int &recvCount_y, int &recvCount_z, int &recvCount_X, int &recvCount_Y,
    int &recvCount_Z, int &recvCount_xy, int &recvCount_XY, int &recvCount_xY,
    int &recvCount_Xy, int &recvCount_xz, int &recvCount_XZ, int &recvCount_xZ,
    int &recvCount_Xz, int &recvCount_yz, int &recvCount_YZ, int &recvCount_yZ,
    int &recvCount_Yz) {
    MPI_Request req1[18], req2[18];
    req1[0] = comm.Isend(&sendCount_x, 1, rank_x, sendtag + 0);
    req2[0] = comm.Irecv(&recvCount_X, 1, rank_X, recvtag + 0);
    req1[1] = comm.Isend(&sendCount_X, 1, rank_X, sendtag + 1);
    req2[1] = comm.Irecv(&recvCount_x, 1, rank_x, recvtag + 1);
    req1[2] = comm.Isend(&sendCount_y, 1, rank_y, sendtag + 2);
    req2[2] = comm.Irecv(&recvCount_Y, 1, rank_Y, recvtag + 2);
    req1[3] = comm.Isend(&sendCount_Y, 1, rank_Y, sendtag + 3);
    req2[3] = comm.Irecv(&recvCount_y, 1, rank_y, recvtag + 3);
    req1[4] = comm.Isend(&sendCount_z, 1, rank_z, sendtag + 4);
    req2[4] = comm.Irecv(&recvCount_Z, 1, rank_Z, recvtag + 4);
    req1[5] = comm.Isend(&sendCount_Z, 1, rank_Z, sendtag + 5);
    req2[5] = comm.Irecv(&recvCount_z, 1, rank_z, recvtag + 5);

    req1[6] = comm.Isend(&sendCount_xy, 1, rank_xy, sendtag + 6);
    req2[6] = comm.Irecv(&recvCount_XY, 1, rank_XY, recvtag + 6);
    req1[7] = comm.Isend(&sendCount_XY, 1, rank_XY, sendtag + 7);
    req2[7] = comm.Irecv(&recvCount_xy, 1, rank_xy, recvtag + 7);
    req1[8] = comm.Isend(&sendCount_Xy, 1, rank_Xy, sendtag + 8);
    req2[8] = comm.Irecv(&recvCount_xY, 1, rank_xY, recvtag + 8);
    req1[9] = comm.Isend(&sendCount_xY, 1, rank_xY, sendtag + 9);
    req2[9] = comm.Irecv(&recvCount_Xy, 1, rank_Xy, recvtag + 9);

    req1[10] = comm.Isend(&sendCount_xz, 1, rank_xz, sendtag + 10);
    req2[10] = comm.Irecv(&recvCount_XZ, 1, rank_XZ, recvtag + 10);
    req1[11] = comm.Isend(&sendCount_XZ, 1, rank_XZ, sendtag + 11);
    req2[11] = comm.Irecv(&recvCount_xz, 1, rank_xz, recvtag + 11);
    req1[12] = comm.Isend(&sendCount_Xz, 1, rank_Xz, sendtag + 12);
    req2[12] = comm.Irecv(&recvCount_xZ, 1, rank_xZ, recvtag + 12);
    req1[13] = comm.Isend(&sendCount_xZ, 1, rank_xZ, sendtag + 13);
    req2[13] = comm.Irecv(&recvCount_Xz, 1, rank_Xz, recvtag + 13);

    req1[14] = comm.Isend(&sendCount_yz, 1, rank_yz, sendtag + 14);
    req2[14] = comm.Irecv(&recvCount_YZ, 1, rank_YZ, recvtag + 14);
    req1[15] = comm.Isend(&sendCount_YZ, 1, rank_YZ, sendtag + 15);
    req2[15] = comm.Irecv(&recvCount_yz, 1, rank_yz, recvtag + 15);
    req1[16] = comm.Isend(&sendCount_Yz, 1, rank_Yz, sendtag + 16);
    req2[16] = comm.Irecv(&recvCount_yZ, 1, rank_yZ, recvtag + 16);
    req1[17] = comm.Isend(&sendCount_yZ, 1, rank_yZ, sendtag + 17);
    req2[17] = comm.Irecv(&recvCount_Yz, 1, rank_Yz, recvtag + 17);
    comm.waitAll(18, req1);
    comm.waitAll(18, req2);
    comm.barrier();
}

//***************************************************************************************
inline void CommunicateRecvLists( const Utilities::MPI& comm, int sendtag, int recvtag, 
		int *sendList_x, int *sendList_y, int *sendList_z, int *sendList_X, int *sendList_Y, int *sendList_Z,
		int *sendList_xy, int *sendList_XY, int *sendList_xY, int *sendList_Xy,
		int *sendList_xz, int *sendList_XZ, int *sendList_xZ, int *sendList_Xz,
		int *sendList_yz, int *sendList_YZ, int *sendList_yZ, int *sendList_Yz,
		int sendCount_x, int sendCount_y, int sendCount_z, int sendCount_X, int sendCount_Y, int sendCount_Z,
		int sendCount_xy, int sendCount_XY, int sendCount_xY, int sendCount_Xy,
		int sendCount_xz, int sendCount_XZ, int sendCount_xZ, int sendCount_Xz,
		int sendCount_yz, int sendCount_YZ, int sendCount_yZ, int sendCount_Yz,
		int *recvList_x, int *recvList_y, int *recvList_z, int *recvList_X, int *recvList_Y, int *recvList_Z,
		int *recvList_xy, int *recvList_XY, int *recvList_xY, int *recvList_Xy,
		int *recvList_xz, int *recvList_XZ, int *recvList_xZ, int *recvList_Xz,
		int *recvList_yz, int *recvList_YZ, int *recvList_yZ, int *recvList_Yz,
		int recvCount_x, int recvCount_y, int recvCount_z, int recvCount_X, int recvCount_Y, int recvCount_Z,
		int recvCount_xy, int recvCount_XY, int recvCount_xY, int recvCount_Xy,
		int recvCount_xz, int recvCount_XZ, int recvCount_xZ, int recvCount_Xz,
		int recvCount_yz, int recvCount_YZ, int recvCount_yZ, int recvCount_Yz,
		int rank_x, int rank_y, int rank_z, int rank_X, int rank_Y, int rank_Z, int rank_xy, int rank_XY, int rank_xY,
		int rank_Xy, int rank_xz, int rank_XZ, int rank_xZ, int rank_Xz, int rank_yz, int rank_YZ, int rank_yZ, int rank_Yz)
{
	MPI_Request req1[18], req2[18];
	req1[0] = comm.Isend(sendList_x,sendCount_x,rank_x,sendtag+0);
	req2[0] = comm.Irecv(recvList_X,recvCount_X,rank_X,recvtag+0);
	req1[1] = comm.Isend(sendList_X,sendCount_X,rank_X,sendtag+1);
	req2[1] = comm.Irecv(recvList_x,recvCount_x,rank_x,recvtag+1);
	req1[2] = comm.Isend(sendList_y,sendCount_y,rank_y,sendtag+2);
	req2[2] = comm.Irecv(recvList_Y,recvCount_Y,rank_Y,recvtag+2);
	req1[3] = comm.Isend(sendList_Y,sendCount_Y,rank_Y,sendtag+3);
	req2[3] = comm.Irecv(recvList_y,recvCount_y,rank_y,recvtag+3);
	req1[4] = comm.Isend(sendList_z,sendCount_z,rank_z,sendtag+4);
	req2[4] = comm.Irecv(recvList_Z,recvCount_Z,rank_Z,recvtag+4);
	req1[5] = comm.Isend(sendList_Z,sendCount_Z,rank_Z,sendtag+5);
	req2[5] = comm.Irecv(recvList_z,recvCount_z,rank_z,recvtag+5);

	req1[6] = comm.Isend(sendList_xy,sendCount_xy,rank_xy,sendtag+6);
	req2[6] = comm.Irecv(recvList_XY,recvCount_XY,rank_XY,recvtag+6);
	req1[7] = comm.Isend(sendList_XY,sendCount_XY,rank_XY,sendtag+7);
	req2[7] = comm.Irecv(recvList_xy,recvCount_xy,rank_xy,recvtag+7);
	req1[8] = comm.Isend(sendList_Xy,sendCount_Xy,rank_Xy,sendtag+8);
	req2[8] = comm.Irecv(recvList_xY,recvCount_xY,rank_xY,recvtag+8);
	req1[9] = comm.Isend(sendList_xY,sendCount_xY,rank_xY,sendtag+9);
	req2[9] = comm.Irecv(recvList_Xy,recvCount_Xy,rank_Xy,recvtag+9);

	req1[10] = comm.Isend(sendList_xz,sendCount_xz,rank_xz,sendtag+10);
	req2[10] = comm.Irecv(recvList_XZ,recvCount_XZ,rank_XZ,recvtag+10);
	req1[11] = comm.Isend(sendList_XZ,sendCount_XZ,rank_XZ,sendtag+11);
	req2[11] = comm.Irecv(recvList_xz,recvCount_xz,rank_xz,recvtag+11);
	req1[12] = comm.Isend(sendList_Xz,sendCount_Xz,rank_Xz,sendtag+12);
	req2[12] = comm.Irecv(recvList_xZ,recvCount_xZ,rank_xZ,recvtag+12);
	req1[13] = comm.Isend(sendList_xZ,sendCount_xZ,rank_xZ,sendtag+13);
	req2[13] = comm.Irecv(recvList_Xz,recvCount_Xz,rank_Xz,recvtag+13);

	req1[14] = comm.Isend(sendList_yz,sendCount_yz,rank_yz,sendtag+14);
	req2[14] = comm.Irecv(recvList_YZ,recvCount_YZ,rank_YZ,recvtag+14);
	req1[15] = comm.Isend(sendList_YZ,sendCount_YZ,rank_YZ,sendtag+15);
	req2[15] = comm.Irecv(recvList_yz,recvCount_yz,rank_yz,recvtag+15);
	req1[16] = comm.Isend(sendList_Yz,sendCount_Yz,rank_Yz,sendtag+16);
	req2[16] = comm.Irecv(recvList_yZ,recvCount_yZ,rank_yZ,recvtag+16);
	req1[17] = comm.Isend(sendList_yZ,sendCount_yZ,rank_yZ,sendtag+17);
	req2[17] = comm.Irecv(recvList_Yz,recvCount_Yz,rank_Yz,recvtag+17);
    comm.waitAll( 18, req1 );
    comm.waitAll( 18, req2 );
}

//***************************************************************************************
inline void CommunicateMeshHalo(
    DoubleArray &Mesh, const Utilities::MPI &comm, double *sendbuf_x,
    double *sendbuf_y, double *sendbuf_z, double *sendbuf_X, double *sendbuf_Y,
    double *sendbuf_Z, double *sendbuf_xy, double *sendbuf_XY,
    double *sendbuf_xY, double *sendbuf_Xy, double *sendbuf_xz,
    double *sendbuf_XZ, double *sendbuf_xZ, double *sendbuf_Xz,
    double *sendbuf_yz, double *sendbuf_YZ, double *sendbuf_yZ,
    double *sendbuf_Yz, double *recvbuf_x, double *recvbuf_y, double *recvbuf_z,
    double *recvbuf_X, double *recvbuf_Y, double *recvbuf_Z, double *recvbuf_xy,
    double *recvbuf_XY, double *recvbuf_xY, double *recvbuf_Xy,
    double *recvbuf_xz, double *recvbuf_XZ, double *recvbuf_xZ,
    double *recvbuf_Xz, double *recvbuf_yz, double *recvbuf_YZ,
    double *recvbuf_yZ, double *recvbuf_Yz, int *sendList_x, int *sendList_y,
    int *sendList_z, int *sendList_X, int *sendList_Y, int *sendList_Z,
    int *sendList_xy, int *sendList_XY, int *sendList_xY, int *sendList_Xy,
    int *sendList_xz, int *sendList_XZ, int *sendList_xZ, int *sendList_Xz,
    int *sendList_yz, int *sendList_YZ, int *sendList_yZ, int *sendList_Yz,
    int sendCount_x, int sendCount_y, int sendCount_z, int sendCount_X,
    int sendCount_Y, int sendCount_Z, int sendCount_xy, int sendCount_XY,
    int sendCount_xY, int sendCount_Xy, int sendCount_xz, int sendCount_XZ,
    int sendCount_xZ, int sendCount_Xz, int sendCount_yz, int sendCount_YZ,
    int sendCount_yZ, int sendCount_Yz, int *recvList_x, int *recvList_y,
    int *recvList_z, int *recvList_X, int *recvList_Y, int *recvList_Z,
    int *recvList_xy, int *recvList_XY, int *recvList_xY, int *recvList_Xy,
    int *recvList_xz, int *recvList_XZ, int *recvList_xZ, int *recvList_Xz,
    int *recvList_yz, int *recvList_YZ, int *recvList_yZ, int *recvList_Yz,
    int recvCount_x, int recvCount_y, int recvCount_z, int recvCount_X,
    int recvCount_Y, int recvCount_Z, int recvCount_xy, int recvCount_XY,
    int recvCount_xY, int recvCount_Xy, int recvCount_xz, int recvCount_XZ,
    int recvCount_xZ, int recvCount_Xz, int recvCount_yz, int recvCount_YZ,
    int recvCount_yZ, int recvCount_Yz, int rank_x, int rank_y, int rank_z,
    int rank_X, int rank_Y, int rank_Z, int rank_xy, int rank_XY, int rank_xY,
    int rank_Xy, int rank_xz, int rank_XZ, int rank_xZ, int rank_Xz,
    int rank_yz, int rank_YZ, int rank_yZ, int rank_Yz) {
    int sendtag, recvtag;
    sendtag = recvtag = 7;
    double *MeshData = Mesh.data();
    PackMeshData(sendList_x, sendCount_x, sendbuf_x, MeshData);
    PackMeshData(sendList_X, sendCount_X, sendbuf_X, MeshData);
    PackMeshData(sendList_y, sendCount_y, sendbuf_y, MeshData);
    PackMeshData(sendList_Y, sendCount_Y, sendbuf_Y, MeshData);
    PackMeshData(sendList_z, sendCount_z, sendbuf_z, MeshData);
    PackMeshData(sendList_Z, sendCount_Z, sendbuf_Z, MeshData);
    PackMeshData(sendList_xy, sendCount_xy, sendbuf_xy, MeshData);
    PackMeshData(sendList_Xy, sendCount_Xy, sendbuf_Xy, MeshData);
    PackMeshData(sendList_xY, sendCount_xY, sendbuf_xY, MeshData);
    PackMeshData(sendList_XY, sendCount_XY, sendbuf_XY, MeshData);
    PackMeshData(sendList_xz, sendCount_xz, sendbuf_xz, MeshData);
    PackMeshData(sendList_Xz, sendCount_Xz, sendbuf_Xz, MeshData);
    PackMeshData(sendList_xZ, sendCount_xZ, sendbuf_xZ, MeshData);
    PackMeshData(sendList_XZ, sendCount_XZ, sendbuf_XZ, MeshData);
    PackMeshData(sendList_yz, sendCount_yz, sendbuf_yz, MeshData);
    PackMeshData(sendList_Yz, sendCount_Yz, sendbuf_Yz, MeshData);
    PackMeshData(sendList_yZ, sendCount_yZ, sendbuf_yZ, MeshData);
    PackMeshData(sendList_YZ, sendCount_YZ, sendbuf_YZ, MeshData);
    //......................................................................................
    comm.sendrecv(sendbuf_x, sendCount_x, rank_x, sendtag, recvbuf_X,
                  recvCount_X, rank_X, recvtag);
    comm.sendrecv(sendbuf_X, sendCount_X, rank_X, sendtag, recvbuf_x,
                  recvCount_x, rank_x, recvtag);
    comm.sendrecv(sendbuf_y, sendCount_y, rank_y, sendtag, recvbuf_Y,
                  recvCount_Y, rank_Y, recvtag);
    comm.sendrecv(sendbuf_Y, sendCount_Y, rank_Y, sendtag, recvbuf_y,
                  recvCount_y, rank_y, recvtag);
    comm.sendrecv(sendbuf_z, sendCount_z, rank_z, sendtag, recvbuf_Z,
                  recvCount_Z, rank_Z, recvtag);
    comm.sendrecv(sendbuf_Z, sendCount_Z, rank_Z, sendtag, recvbuf_z,
                  recvCount_z, rank_z, recvtag);
    comm.sendrecv(sendbuf_xy, sendCount_xy, rank_xy, sendtag, recvbuf_XY,
                  recvCount_XY, rank_XY, recvtag);
    comm.sendrecv(sendbuf_XY, sendCount_XY, rank_XY, sendtag, recvbuf_xy,
                  recvCount_xy, rank_xy, recvtag);
    comm.sendrecv(sendbuf_Xy, sendCount_Xy, rank_Xy, sendtag, recvbuf_xY,
                  recvCount_xY, rank_xY, recvtag);
    comm.sendrecv(sendbuf_xY, sendCount_xY, rank_xY, sendtag, recvbuf_Xy,
                  recvCount_Xy, rank_Xy, recvtag);
    comm.sendrecv(sendbuf_xz, sendCount_xz, rank_xz, sendtag, recvbuf_XZ,
                  recvCount_XZ, rank_XZ, recvtag);
    comm.sendrecv(sendbuf_XZ, sendCount_XZ, rank_XZ, sendtag, recvbuf_xz,
                  recvCount_xz, rank_xz, recvtag);
    comm.sendrecv(sendbuf_Xz, sendCount_Xz, rank_Xz, sendtag, recvbuf_xZ,
                  recvCount_xZ, rank_xZ, recvtag);
    comm.sendrecv(sendbuf_xZ, sendCount_xZ, rank_xZ, sendtag, recvbuf_Xz,
                  recvCount_Xz, rank_Xz, recvtag);
    comm.sendrecv(sendbuf_yz, sendCount_yz, rank_yz, sendtag, recvbuf_YZ,
                  recvCount_YZ, rank_YZ, recvtag);
    comm.sendrecv(sendbuf_YZ, sendCount_YZ, rank_YZ, sendtag, recvbuf_yz,
                  recvCount_yz, rank_yz, recvtag);
    comm.sendrecv(sendbuf_Yz, sendCount_Yz, rank_Yz, sendtag, recvbuf_yZ,
                  recvCount_yZ, rank_yZ, recvtag);
    comm.sendrecv(sendbuf_yZ, sendCount_yZ, rank_yZ, sendtag, recvbuf_Yz,
                  recvCount_Yz, rank_Yz, recvtag);
    //........................................................................................
    UnpackMeshData(recvList_x, recvCount_x, recvbuf_x, MeshData);
    UnpackMeshData(recvList_X, recvCount_X, recvbuf_X, MeshData);
    UnpackMeshData(recvList_y, recvCount_y, recvbuf_y, MeshData);
    UnpackMeshData(recvList_Y, recvCount_Y, recvbuf_Y, MeshData);
    UnpackMeshData(recvList_z, recvCount_z, recvbuf_z, MeshData);
    UnpackMeshData(recvList_Z, recvCount_Z, recvbuf_Z, MeshData);
    UnpackMeshData(recvList_xy, recvCount_xy, recvbuf_xy, MeshData);
    UnpackMeshData(recvList_Xy, recvCount_Xy, recvbuf_Xy, MeshData);
    UnpackMeshData(recvList_xY, recvCount_xY, recvbuf_xY, MeshData);
    UnpackMeshData(recvList_XY, recvCount_XY, recvbuf_XY, MeshData);
    UnpackMeshData(recvList_xz, recvCount_xz, recvbuf_xz, MeshData);
    UnpackMeshData(recvList_Xz, recvCount_Xz, recvbuf_Xz, MeshData);
    UnpackMeshData(recvList_xZ, recvCount_xZ, recvbuf_xZ, MeshData);
    UnpackMeshData(recvList_XZ, recvCount_XZ, recvbuf_XZ, MeshData);
    UnpackMeshData(recvList_yz, recvCount_yz, recvbuf_yz, MeshData);
    UnpackMeshData(recvList_Yz, recvCount_Yz, recvbuf_Yz, MeshData);
    UnpackMeshData(recvList_yZ, recvCount_yZ, recvbuf_yZ, MeshData);
    UnpackMeshData(recvList_YZ, recvCount_YZ, recvbuf_YZ, MeshData);
}

#endif

#include "common/Communication.hpp"
