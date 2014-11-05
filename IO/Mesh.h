#ifndef MESH_INC
#define MESH_INC

#include <iostream>
#include <string.h>
#include <memory>
#include <vector>

#include "common/PointList.h"


namespace IO {


/*! \class Mesh
    \brief A base class for meshes
*/
class Mesh
{
public:
    //! Destructor
    virtual ~Mesh();
    //! Mesh class name (eg. PointList)
    virtual std::string className() const = 0;
    //! Pack the data
    virtual std::pair<size_t,void*> pack( int level ) const = 0;
    //! Unpack the data
    virtual void unpack( const std::pair<size_t,void*>& data ) = 0;
protected:
    //! Empty constructor
    Mesh();
    Mesh(const Mesh&);
    Mesh& operator=(const Mesh&);
};


/*! \class PointList
    \brief A class used to hold a list of verticies
*/
class PointList: public Mesh
{
public:
    //! Empty constructor
    PointList();
    //! Constructor for N points
    PointList( size_t N );
    //! Destructor
    virtual ~PointList();
    //! Mesh class name
    virtual std::string className() const { return "PointList"; }
    //! Pack the data
    virtual std::pair<size_t,void*> pack( int level ) const;
    //! Unpack the data
    virtual void unpack( const std::pair<size_t,void*>& data );
public:
    std::vector<Point>  points;  //!< List of points vertex
};


/*! \class TriMesh
    \brief A class used to hold a list of trianges specified by their vertex number and list of coordiantes
*/
class TriList;
class TriMesh: public Mesh
{
public:
    //! TriMesh constructor
    TriMesh();
    //! Constructor for Nt triangles and Np points
    TriMesh( size_t N_tri, size_t N_point );
    //! Constructor for Nt triangles and the given points
    TriMesh( size_t N_tri, std::shared_ptr<PointList> points );
    //! Constructor from TriList
    TriMesh( const TriList& );
    //! Destructor
    virtual ~TriMesh();
    //! Mesh class name
    virtual std::string className() const { return "TriMesh"; }
    //! Pack the data
    virtual std::pair<size_t,void*> pack( int level ) const;
    //! Unpack the data
    virtual void unpack( const std::pair<size_t,void*>& data );
public:
    std::shared_ptr<PointList> vertices;    //!< List of verticies
    std::vector<int>    A;                  //!< First vertex
    std::vector<int>    B;                  //!< Second vertex
    std::vector<int>    C;                  //!< Third vertex
};


/*! \class TriList
    \brief A class used to hold a list of triangles specified by their vertex coordinates
*/
class TriList: public Mesh
{
public:
    //! Empty constructor
    TriList();
    //! Constructor for N triangles
    TriList( size_t N_tri );
    //! Constructor from TriMesh
    TriList( const TriMesh& );
    //! Destructor
    virtual ~TriList();
    //! Mesh class name
    virtual std::string className() const { return "TriList"; }
    //! Pack the data
    virtual std::pair<size_t,void*> pack( int level ) const;
    //! Unpack the data
    virtual void unpack( const std::pair<size_t,void*>& data );
public:
    std::vector<Point>  A;      //!< First vertex
    std::vector<Point>  B;      //!< Second vertex
    std::vector<Point>  C;      //!< Third vertex
};



/*! \class Variable
    \brief A base class fore variables
*/
struct Variable
{
public:
    //! Internal variables
    int dim;
    std::string name;
    std::vector<double> data;
    //! Empty constructor
    Variable() {}
    //! Destructor
    virtual ~Variable() {}
protected:
    //! Empty constructor
    Variable(const Variable&);
    Variable& operator=(const Variable&);
};



/*! \class MeshDataStruct
    \brief A class used to hold database info for saving a mesh
*/
struct MeshDataStruct {
    std::string             meshName;
    std::shared_ptr<Mesh>   mesh;
    std::vector<std::shared_ptr<Variable> >  vars;
};


//! Convert the mesh to a TriMesh (will return NULL if this is invalid)
std::shared_ptr<PointList> getPointList( std::shared_ptr<Mesh> mesh );
std::shared_ptr<TriMesh> getTriMesh( std::shared_ptr<Mesh> mesh );
std::shared_ptr<TriList> getTriList( std::shared_ptr<Mesh> mesh );


} // IO namespace

#endif

