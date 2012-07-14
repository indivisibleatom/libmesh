// $Id: cell_hex.h,v 1.7 2006-12-27 07:21:27 roystgnr Exp $

// The libMesh Finite Element Library.
// Copyright (C) 2002-2005  Benjamin S. Kirk, John W. Peterson
  
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



#ifndef __cell_hex_h__
#define __cell_hex_h__

// C++ includes

// Local includes
#include "cell.h"




/**
 * The \p Hex is an element in 3D with 6 sides.
 */

// ------------------------------------------------------------
// Hex class definition
class Hex : public Cell
{
public:

  /**
   * Default brick element, takes number of nodes and 
   * parent. Derived classes implement 'true' elements.
   */
  Hex(const unsigned int nn, Elem* p);

  /**
   * @returns 6
   */
  unsigned int n_sides() const { return 6; }

  /**
   * @returns 8.  All hexahedrals have 8 vertices.
   */
  unsigned int n_vertices() const { return 8; }

  /**
   * @returns 12.  All hexahedrals have 12 edges.
   */
  unsigned int n_edges() const { return 12; }

  /**
   * @returns 6.  All hexahedrals have 6 faces.
   */
  unsigned int n_faces() const { return 6; }
  
  /**
   * @returns 8
   */
  unsigned int n_children() const { return 8; }
  
  /**
   * @returns an id associated with the \p s side of this element.
   * The id is not necessariy unique, but should be close.  This is
   * particularly useful in the \p MeshBase::find_neighbors() routine.
   */
  unsigned int key (const unsigned int s) const;

  /**
   * @returns a primitive (4-noded) quad for 
   * face i.
   */
  AutoPtr<DofObject> side (const unsigned int i) const;

  /**
   * Based on the quality metric q specified by the user,
   * returns a quantitative assessment of element quality.
   */
  Real quality (const ElemQuality q) const;

  /**
   * Returns the suggested quality bounds for
   * the hex based on quality measure q.  These are
   * the values suggested by the CUBIT User's Manual.
   */
  std::pair<Real, Real> qual_bounds (const ElemQuality q) const;


  
protected:


  
  /**
   * Matrix that tells which vertices define the location
   * of mid-side (or second-order) nodes.  This matrix
   * is kept here, since the matrix (for the first 12
   * higher-order nodes) is identical for \p Hex20 and
   * \p Hex27.
   */
  static const unsigned short int _second_order_adjacent_vertices[12][2];

  /**
   * Vector that names a child sharing each second order node. 
   */
  static const unsigned short int _second_order_vertex_child_number[27];

  /**
   * Vector that names the child vertex index for each second order node.
   */
  static const unsigned short int _second_order_vertex_child_index[27];
};



// ------------------------------------------------------------
// Hex class member functions
inline
Hex::Hex(const unsigned int nn, Elem* p) :
  Cell(nn, Hex::n_sides(), p) 
{
}

#endif