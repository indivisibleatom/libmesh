// $Id: mesh_data.h,v 1.13 2003-08-04 17:23:48 ddreyer Exp $

// The Next Great Finite Element Library.
// Copyright (C) 2002  Benjamin S. Kirk, John W. Peterson
  
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
  
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



#ifndef __mesh_data_h__
#define __mesh_data_h__

// C++ includes
#include <map>
#include <vector>
#include <string>
#include <fstream>


// Local Includes
#include "libmesh.h"
#include "node.h"
#include "elem.h"
#include "enum_xdr_mode.h"



// Forward Declarations
class MeshBase;
class UnvMeshInterface;
class XdrInterface;
class MeshDataUnvHeader;



/**
 * Yet another Mesh-something class...  What's this good for: 
 * \p MeshData handles actual data on entities (nodes, elements)
 * of meshes.  \p MeshBase owns a \p MeshData for dealing with files
 * that contain nodal or element-oriented data, numbered in the same 
 * format as the corresponding mesh file.
 *
 * @author Daniel Dreyer, 2003
 */

// ------------------------------------------------------------
// MeshData class definition
class MeshData 
{
public:

  /**
   * Default Constructor.  Takes const reference
   * to the mesh it belongs to.
   */
  MeshData (const MeshBase& m);

  /**
   * Destructor.
   */
  ~MeshData ();

  /**
   * When \p MeshData should be used, it has to be activated
   * first, @e prior to reading in a mesh with the \p Mesh::read()
   * methods.  Optionally takes a string that should help the user
   * in identifying the data later on.
   */
  void activate (const std::string& descriptor="");

  /**
   * Clears the data fields, but leaves the id maps
   * untouched.  Useful for clearing data for a new
   * data file.  Use \p slim() to delete the maps.
   */
  void clear ();

  /**
   * Once the data is properly read from file, the id 
   * maps can safely be cleared.  However, if this object
   * should remain able to @e write nodal or element oriented 
   * data to file, this method should better @e not be used.
   * Use the appropriate \p bool to select the id map that
   * should be cleared.  By default, both id maps are deleted.
   */
  void slim (const bool node_id_map = true,
	     const bool elem_id_map = true);

  /**
   * Translates the @e nodal data contained in this object
   * to \p data_values and \p data_names.  These two
   * vectors are particularly suitable for use with
   * the \p MeshBase::write method that takes nodal
   * data.  E.g., the export method may be used for
   * inspecting boundary conditions.  A reference
   * to the mesh for which the data should be written
   * has to be provided.  Note that this mesh @e has 
   * to contain the nodes for which this \p MeshData 
   * holds data.  I.e., \p out_mesh may only refer to 
   * the \p MeshBase itself (that this \p MeshData belongs 
   * to), or its \p BoundaryMesh, cf. \p Mesh.  
   */
  void translate (const MeshBase& out_mesh,
		  std::vector<Number>& data_values,
		  std::vector<std::string>& data_names) const;

  /**
   * Read mesh data from file named \p name.  
   * Guess format from the file extension.  Note that
   * prior to this you have to at least either
   * \p close_node_map() or \p close_elem_map().
   */
  void read (const std::string& name);

  /**
   * Write mesh data to file named \p name.  
   * Guess format from the file extension.
   */
  void write (const std::string& name);

  /**
   * @returns a string containing relevant information
   * about the mesh.
   */
  std::string get_info () const;

  /**
   * Prints relevant information about the mesh.
   */
  void print_info () const;

  //----------------------------------------------------------
  // Node-associated data
  /**
   * @returns the \f$ i^{th} \f$ value (defaults to 0) associated 
   * with node \p node.  Returns \p libMesh::zero when there
   * is no such \p node in the map.
   */
  Number operator() (const Node* node, 
		     const unsigned int i=0) const;

  /**
   * @returns \p true when the node \p node has data,
   * \p false otherwise.
   */
  bool has_data (const Node* node) const;

  /**
   * @returns a const reference to the values associated with 
   * the node \p node.  @e Beware: this method will crash
   * when there is no data associated with the node \p node!
   * Check existence through \p has_data() first.
   */
  const std::vector<Number>& get_data (const Node* node) const;

  /**
   * @returns the number of \p Number -type data 
   * (i.e., the size of the \p std::vector<Number>
   * returned through the \p operator() methods)
   * associated with a node.  Returns 0 when no
   * nodal data exists.
   */
  unsigned int n_val_per_node () const;

  /**
   * @returns the number of nodes for which this
   * \p MeshData has data stored.
   */
  unsigned int n_node_data () const;

  /**
   * For the desperate user, nodal boundary conditions 
   * may be inserted directly through the map \p nd.
   * It is mandatory that there does not yet exist any
   * other node data in this object, that the id maps
   * are closed, that the size of the std::vector's of 
   * each map have identical length and that the Node* 
   * point to nodes of the associated mesh.  
   * Note that this method takes a non-const reference 
   * and essentially clears the passed-in data.
   * If \p close_elem_data is \p true (default), then
   * this \p MeshData is ready for use: write to file,
   * use the operator() methods etc. If \p false, the 
   * user @e has to add element-associated data, too.
   */
  void insert_node_data (std::map<const Node*,
			          std::vector<Number> >& nd,
			 const bool close_elem_data = true);


  //----------------------------------------------------------
  // Element-associated data
  /**
   * @returns the \f$ i^{th} \f$ value (defaults to 0) associated 
   * with element \p elem.  Returns \p libMesh::zero when there
   * is no data for \p elem in the map.
   */
  Number operator() (const Elem* elem, 
		     const unsigned int i=0) const;

  /**
   * @returns \p true when the element \p elem has data,
   * \p false otherwise.
   */
  bool has_data (const Elem* elem) const;

  /**
   * @returns a const reference to the values associated with 
   * the element \p elem.  @e Beware: this method will crash
   * when there is no data associated with the element \p elem!
   * Check existence through \p has_data() first.
   */
  const std::vector<Number>& get_data (const Elem* elem) const;

  /**
   * @returns the number of \p Number -type data 
   * (i.e., the size of the \p std::vector<Number>
   * returned through the \p operator() methods)
   * associated with an element.  Returns 0 when
   * there is no element-associated data.
   */
  unsigned int n_val_per_elem () const;

  /**
   * @returns the number of elements for which this
   * \p MeshData has data stored.
   */
  unsigned int n_elem_data () const;

  /**
   * For the desperate user, element-associated boundary 
   * conditions may be inserted directly through the 
   * map \p ed.  Similar to the version for nodal data, 
   * it is imperative that the local \p _elem_data is empty, 
   * that the id maps are closed, that the size of the 
   * \p std::vector's of each map have identical length 
   * and that the \p Elem* point to elements of the 
   * associated mesh.  
   * Note that this method takes a non-const reference 
   * and essentially clears the passed-in data.
   * If \p close_node_data is \p true (default), then
   * this \p MeshData is ready for use: write to file,
   * use the operator() methods etc. If \p false, the 
   * user @e has to add nodal data, too.
   */
  void insert_elem_data (std::map<const Elem*,
			          std::vector<Number> >& ed,
			 const bool close_node_data = true);


  //----------------------------------------------------------
  /**
   * @returns \p true when this object is active and working.
   * Use \p activate() to bring this object alive.
   */
  bool active () const;

  /**
   * @returns \p true when this object is properly initialized
   * and ready for use for @e element associated data, \p false 
   * otherwise.
   */
  bool elem_initialized () const;

  /**
   * @returns \p true when this object is properly initialized
   * and ready for use for @e nodal data, \p false otherwise.
   */
  bool node_initialized () const;


  //----------------------------------------------------------
  // Methods for accessing the node and element maps.
  // Heavily used by the \p read() and \p write() methods.
  /**
   * @returns the \p Node* that this foreign id maps to.
   */
  const Node* foreign_id_to_node (const unsigned int fid) const;

  /**
   * @returns the \p Elem* that this foreign id maps to.
   */
  const Elem* foreign_id_to_elem (const unsigned int fid) const;

  /**
   * @returns the foreign id this \p Node* maps to.
   */
  unsigned int node_to_foreign_id (const Node* n) const;

  /**
   * @returns the foreign id this \p Elem* maps to.
   */
  unsigned int elem_to_foreign_id (const Elem* n) const;

  //----------------------------------------------------------
  // Methods for the header information in universal formated
  // datasets.

  /**
   * Read access to the MeshDataUnvHeader data structure.
   */
  const MeshDataUnvHeader & get_unv_header() const;

  /**
   * Set the MeshDataUnvHeader data structure that will be
   * used for output.
   */
  void set_unv_header(MeshDataUnvHeader& unv_header);


protected:


  //----------------------------------------------------------
  // Methods used by mesh importes to communicate node/element
  // labels to this \p MeshData
  /**
   * In general, \p MeshData gathers nodal data
   * from a file, but it needs to relate this data
   * with the \p Node* of the current mesh.  Mesh
   * importers simply use this method to add such
   * a map.
   */
  void add_foreign_node_id (const Node* node, 
			    const unsigned int foreign_node_id);

  /**
   * In general, \p MeshData gathers element-associated
   * data from file, but it needs to relate this data
   * with the \p Elem* of the current mesh.  Mesh
   * importers simply use this method to add such
   * a map.
   */
  void add_foreign_elem_id (const Elem* elem, 
			    const unsigned int foreign_elem_id);

  /**
   * Signal to this object that the mesh importer finished
   * adding node and element foreign-id maps.
   */
  void close_foreign_id_maps ();



  //----------------------------------------------------------
  // read/write Methods
  /**
   * Read nodal/element oriented data in UNV format.
   */
  void read_unv (const std::string& name);

  /**
   * Write nodal/element oriented data in UNV format.
   */
  void write_unv (const std::string& name);

  /**
   * Read nodal/element oriented data using the
   * \p Xdr class that enables both ASCII and
   * binary format through the same interface.  
   * By default uses ASCII format, but may easily
   * be changed setting \p mode to \p DECODE.
   */
  void read_xdr (const std::string& name,
		 const XdrMODE mode = READ);

  /**
   * Write nodal data in format comparable to
   * the XDR format already known from \p Mesh.
   * By default uses ASCII format, but may easily
   * be changed setting \p mode to \p ENCODE.
   */
  void write_xdr (const std::string& name,
		  const XdrMODE mode = WRITE);


  /**
   * The mesh this object belongs to
   */
  const MeshBase& _mesh;

  /**
   * Some name the user gave to the data when this
   * object got activated
   */
  std::string _data_descriptor;


  //--------------------------------------------------
  // node associated data & maps
  /**
   * The map containing pointers to nodes in the mesh
   * and the corresponding data.
   */
  std::map<const Node*,
           std::vector<Number> > _node_data;

  /**
   * Maps node pointers to node numbers in the @e foreign
   * format.  
   */
  std::map<const Node*,
           unsigned int> _node_id;

  /**
   * Maps @e foreign node ids to node pointers of the
   * current mesh.
   */
  std::map<unsigned int,
           const Node*> _id_node;



  //--------------------------------------------------
  // element associated data & maps
  /**
   * Maps element pointers to the element-associated data
   */
  std::map<const Elem*,
           std::vector<Number> > _elem_data;

  /**
   * Maps element pointers to element labels in the @e foreign
   * format.  
   */
  std::map<const Elem*,
           unsigned int> _elem_id;
  /**
   * Maps @e foreign element labels to element pointers of the
   * current mesh.
   */
  std::map<unsigned int,
           const Elem*> _id_elem;



  //--------------------------------------------------------
  /**
   * \p true when the mesh importer finished adding
   * node-foreign-id maps, and the node-foreign-id maps 
   * exist.  Note that these maps may be deleted through
   * \p slim() to save memory.  Then the data is
   * still accessible through the \p Node* or \p Elem*,
   * but the foreign id's are lost.
   */
  bool _node_id_map_closed;

  /**
   * \p true when the nodal data are properly initialized,
   * false otherwise.
   */
  bool _node_data_closed;


  //--------------------------------------------------------
  /**
   * \p true when the mesh importer finished adding
   * element-id maps, and the element-id maps exist.  
   * Note that these maps may be deleted through
   * \p slim() to save memory.  Then the data is
   * still accessible through the \p Elem*,
   * but the foreign element id's are lost.
   */
  bool _elem_id_map_closed;

  /**
   * \p true when the element based data are properly initialized,
   * false otherwise.
   */
  bool _elem_data_closed;


  //--------------------------------------------------------
  /**
   * \p true when this object is set active (to gather data
   * during mesh import).
   */
  bool _active;

  /**
   * A pointer to the header information of universal files.
   */
  MeshDataUnvHeader* _unv_header;

  /**
   * Make the mesh importer class \p UnvInterface friend, so
   * that it can communicate foreign node ids to this class.
   */
  friend class UnvMeshInterface;

  /**
   * Make the mesh importer class \p XdrInterface friend, so
   * that it can communicate foreign node ids to this class.
   */
  friend class XdrInterface;

  /**
   * Make the MeshDataUnvHeader class a friend.
   */
  friend class MeshDataUnvHeader;

};



//-----------------------------------------------------------
// MeshDataUnvHeader class definition

/**
 * Class \p MeshDataUnvHeader handles the data specified at
 * the @e beginning of a dataset 2414 in a universal file.
 * This header is structured in records 1 to 13.  For more
 * details we refer to the general description of the I-DEAS
 * universal file format.
 */
class MeshDataUnvHeader
{
public:

  /**
   * Default Constructor.  Initializes the respective
   * data.
   */
  MeshDataUnvHeader ();

  /**
   * Destructor.
   */
  ~MeshDataUnvHeader ();

  /**
   * Read the header information from the stream \p in_file.
   */
  void read (std::ifstream& in_file);

  /**
   * Write the header information to the stream \p out_file.
   */
  void write (std::ofstream& out_file);


  /**
   * Record 1.  User specified analysis dataset label.
   */
  unsigned int dataset_label;

  /**
   * Record 2. User specified analysis dataset name.
   */
  std::string dataset_name;

  /**
   * Record 3. The dataset location (e.g. data at nodes,
   * data on elements, etc.). 
   */
  unsigned int dataset_location;

  /**
   * Record 4 trough 8 are ID lines.
   */
  std::string id_line_1,
              id_line_2,
              id_line_3,
              id_line_4,
              id_line_5;

  /**
   * Record 9. This record contains data specifying
   * the model type (e.g. unknown, structural, etc.),
   * the analysis type (e.g. unknown, static, transient,
   * normal mode, etc.),
   * the data characteristics (such as scalar, 3 dof global
   * translation vector, etc.),
   * the result type (e.g. stress, strain, velocity, etc.),
   * the data type (e.g. integer, single precision floating
   * point, double precision floating point, etc.),
   * and the number of data values for the mesh data.
   */
  unsigned int model_type,          
               analysis_type,
               data_characteristic,
               result_type,
               data_type,
               nvaldc;

  /**
   * Record 10 and 11 are analysis specific data of
   * type integer.
   */
  std::vector<int> record_10,
                   record_11;

  /**
   * Record 12 and 13 are analysis specific data of
   * type Real.
   */
  std::vector<Real> record_12,
                    record_13;

  /**
   * Make the MeshDataUnvHeader class a friend.
   */
  friend class MeshData;


private:

  /**
   * @returns \p true when the string \p number
   * has a 'D' that needs to be replaced by 'e',
   * \p false otherwise.  Also actually replaces
   * the 'D' by an 'e'.
   */
  static bool need_D_to_e (std::string& number);

};



// ------------------------------------------------------------
// MeshData inline methods
inline
Number MeshData::operator() (const Node* node, 
			     const unsigned int i) const
{
  assert (_active);
  assert (_node_data_closed);

  std::map<const Node*, 
           std::vector<Number> >::const_iterator pos = _node_data.find(node);

  if (pos == _node_data.end())
      return libMesh::zero;

  // we only get here when pos != _node_data.end()
  assert (i < (*pos).second.size());
  return (*pos).second[i];
}



inline
bool MeshData::has_data (const Node* node) const
{
  assert (_active);
  assert (_node_data_closed);

  std::map<const Node*, 
           std::vector<Number> >::const_iterator pos = _node_data.find(node);

  return (pos != _node_data.end());
}



inline
const std::vector<Number>& MeshData::get_data (const Node* node) const
{
  assert (_active);
  assert (_node_data_closed);

  std::map<const Node*, 
           std::vector<Number> >::const_iterator pos = _node_data.find(node);

#ifdef DEBUG
  if (pos == _node_data.end())
    {
      std::cerr << "ERROR: No data for this node.  Use has_data() first!" << std::endl;
      error();
    }
#endif

  return (*pos).second;
}



inline
Number MeshData::operator() (const Elem* elem, 
			     const unsigned int i) const
{
  assert (_active);
  assert (_elem_data_closed);

  std::map<const Elem*, 
           std::vector<Number> >::const_iterator pos = _elem_data.find(elem);

  if (pos == _elem_data.end())
    return libMesh::zero;
  
  // we only get here when pos != _elem_data.end()  
  assert (i < (*pos).second.size());
  return (*pos).second[i];
}



inline
bool MeshData::has_data (const Elem* elem) const
{
  assert (_active);
  assert (_elem_data_closed);

  std::map<const Elem*, 
           std::vector<Number> >::const_iterator pos = _elem_data.find(elem);

  return (pos != _elem_data.end());
}



inline
const std::vector<Number>& MeshData::get_data (const Elem* elem) const
{
  assert (_active);
  assert (_elem_data_closed);

  std::map<const Elem*, 
           std::vector<Number> >::const_iterator pos = _elem_data.find(elem);

#ifdef DEBUG
  if (pos == _elem_data.end())
    {
      std::cerr << "ERROR: No data for this element.  Use has_data() first!" << std::endl;
      error();
    }
#endif

  return pos->second;
}



inline
bool MeshData::active() const
{
  return _active;
}



inline
bool MeshData::elem_initialized() const
{
  return (_active && _elem_data_closed);
}



inline
bool MeshData::node_initialized() const
{
  return (_active && _node_data_closed);
}



inline  
void MeshData::add_foreign_node_id (const Node* node, 
				    const unsigned int foreign_node_id)
{
  if (_active)
    {
      assert (!_node_id_map_closed);
      assert (node                             != NULL);
      assert (_node_id.find(node)              == _node_id.end());
      assert (_id_node.find(foreign_node_id)   == _id_node.end());

      /*
       * _always_ insert in _id_node and _node_id.  If we would 
       * use the mesh.node(unsigned int) method or the node.id()
       * to get Node* and unsigned int, respectively, we would not
       * be safe any more when the mesh gets refined or re-numbered
       * within libMesh. And we could get in big trouble that would
       * be hard to find when importing data _after_ having refined...
       */
      _node_id.insert(std::make_pair(node, foreign_node_id));
      _id_node.insert(std::make_pair(foreign_node_id, node));
    }
}



inline  
void MeshData::add_foreign_elem_id (const Elem* elem, 
				    const unsigned int foreign_elem_id)
{
  if (_active)
    {
      assert (!_elem_id_map_closed);
      assert (elem                             != NULL);
      assert (_elem_id.find(elem)              == _elem_id.end());
      assert (_id_elem.find(foreign_elem_id)   == _id_elem.end());

      _elem_id.insert(std::make_pair(elem, foreign_elem_id));
      _id_elem.insert(std::make_pair(foreign_elem_id, elem));
    }
}


inline
const MeshDataUnvHeader & MeshData::get_unv_header () const
{
  return *_unv_header;
}


inline
void MeshData::set_unv_header (MeshDataUnvHeader& unv_header)
{
  this->_unv_header = & unv_header;
}


//-----------------------------------------------------------
// MeshDataUnvHeader inline methods


#endif
