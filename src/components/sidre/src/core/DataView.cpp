/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */

/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   Implementation file for DataView class.
 *
 ******************************************************************************
 */

// Associated header file
#include "DataView.hpp"

// Other toolkit project headers
#include "common/CommonTypes.hpp"
#include "slic/slic.hpp"

// SiDRe project headers
#include "DataBuffer.hpp"
#include "DataGroup.hpp"
#include "DataStore.hpp"

namespace asctoolkit
{
namespace sidre
{

/*
 *************************************************************************
 *
 * Allocate data for view, previously described.
 *
 *************************************************************************
 */
DataView * DataView::allocate()
{

  if ( !isAllocateValid() )
  {
    SLIC_CHECK_MSG( isAllocateValid(),
                    "View " << this->getName() << "'s state " <<
        getStateStringName(m_state) << " does not allow data allocation");
    return this;
  }

  if ( m_data_buffer == ATK_NULLPTR )
  {
    m_data_buffer = m_owning_group->getDataStore()->createBuffer();
    m_data_buffer->attachView(this);
  }

  if ( m_data_buffer->getNumViews() == 1 )
  {
    TypeID type = static_cast<TypeID>(m_schema.dtype().id());
    SidreLength num_elems = m_schema.dtype().number_of_elements();
    m_data_buffer->allocate(type, num_elems);
    m_state = ALLOCATED;

    apply();
  }

  return this;
}

/*
 *************************************************************************
 *
 * Allocate data for view with type and number of elements.
 *
 *************************************************************************
 */
DataView * DataView::allocate( TypeID type, SidreLength num_elems)
{
  if ( num_elems < 0 )
  {
    SLIC_CHECK(num_elems >= 0);
    return this;
  }

  declare(type, num_elems);
  allocate();

  return this;
}

/*
 *************************************************************************
 *
 * Allocate data for view described by a Conduit data type object.
 *
 *************************************************************************
 */
DataView * DataView::allocate(const DataType& dtype)
{
  declare(dtype);
  allocate();

  return this;
}

/*
 *************************************************************************
 *
 * Allocate data for view described by a Conduit schema object.
 *
 *************************************************************************
 */
DataView * DataView::allocate(const Schema& schema)
{
  declare(schema);
  allocate();

  return this;
}

/*
 *************************************************************************
 *
 * Reallocate data for view to given number of elements.
 * This function requires that the view is already described.
 *
 *************************************************************************
 */
DataView * DataView::reallocate(SidreLength num_elems)
{
  TypeID vtype = static_cast<TypeID>(m_schema.dtype().id());

  // If we don't have an allocated buffer, we can just call allocate.
  if ( !isAllocated() )
  {
    return allocate( vtype, num_elems);
  }

  if ( !isAllocateValid() || num_elems < 0 )
  {
    SLIC_CHECK_MSG( isAllocateValid(),
                    "View " << this->getName() << "'s state " <<
        getStateStringName(m_state) << " does not allow data re-allocation");
    SLIC_CHECK(num_elems >= 0);

    return this;
  }

  declare(vtype, num_elems);
  m_data_buffer->reallocate(num_elems);
  m_state = ALLOCATED;
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Reallocate data for view using a Conduit data type object.
 *
 *************************************************************************
 */
DataView * DataView::reallocate(const DataType& dtype)
{
  // If we don't have an allocated buffer, we can just call allocate.
  if ( !isAllocated() )
  {
    return allocate(dtype);
  }

  TypeID type = static_cast<TypeID>(dtype.id());
  TypeID view_type = static_cast<TypeID>(m_schema.dtype().id());

  if (!isAllocateValid() || type != view_type)
  {
    SLIC_CHECK_MSG( isAllocateValid(),
                    "View " << this->getName() << "'s state " <<
        getStateStringName(m_state) << " does not allow data re-allocation");
    SLIC_CHECK_MSG( type == view_type,
                    "View " << this->getName() <<
        " attempting to re-allocate with different type.");
    return this;
  }

  declare(dtype);
  SidreLength num_elems = dtype.number_of_elements();
  m_data_buffer->reallocate(num_elems);
  m_state = ALLOCATED;
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Reallocate data for view using a Conduit schema object.
 *
 *************************************************************************
 */
DataView * DataView::reallocate(const Schema& schema)
{
  // If we don't have an allocated buffer, we can just call allocate.
  if ( !isAllocated() )
  {
    return allocate(schema);
  }

  TypeID type = static_cast<TypeID>(schema.dtype().id());
  TypeID view_type = static_cast<TypeID>(m_schema.dtype().id());

  if ( !isAllocateValid() || type != view_type )
  {
    SLIC_CHECK_MSG( isAllocateValid(),
                    "View " << this->getName() << "'s state " <<
        getStateStringName(m_state) << " does not allow data re-allocation");
    SLIC_CHECK_MSG( type == view_type,
                    "View " << this->getName() <<
        " attempting to re-allocate with different type.");
    return this;
  }

  declare(schema);
  SidreLength num_elems = schema.dtype().number_of_elements();
  m_data_buffer->reallocate(num_elems);
  m_state = ALLOCATED;
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Attach buffer to view.
 *
 *************************************************************************
 */
DataView * DataView::attachBuffer(DataBuffer * buff)
{
  if ( !isAttachBufferValid() || buff == ATK_NULLPTR)
  {
    SLIC_CHECK_MSG( isAttachBufferValid(),
                    "View state " << getStateStringName(
                      m_state) << " does not allow attaching buffer");
    SLIC_CHECK( buff != ATK_NULLPTR );
    return this;
  }

  buff->attachView(this);
  m_data_buffer = buff;
  m_state = BUFFER_ATTACHED;
  m_is_applied = false;

  if (isDescribed())
  {
    SLIC_CHECK_MSG(
      m_schema.total_bytes() > m_data_buffer->getTotalBytes(),
      "Unable to apply description to view's data, # of bytes in description is > # of bytes in buffer's data.");
    if ( m_schema.total_bytes() <= m_data_buffer->getTotalBytes() )
    {
      apply();
    }
  }
  {
    //
    // RDH -- What if the view is not described and the buffer is not
    //        declared and allocated???
    //
  }

  return this;
}

/*
 *************************************************************************
 *
 * Apply data description to data.
 *
 *************************************************************************
 */
DataView * DataView::apply()
{
  if ( !isApplyValid() )
  {
    SLIC_CHECK_MSG( isApplyValid(),
                    "View state, '" << getStateStringName(
                      m_state) << "', does not allow apply operation");
    return this;
  }

  void * data_pointer = ATK_NULLPTR;

  if ( hasBuffer() )
  {
    data_pointer = m_data_buffer->getVoidPtr();
  }
  else
  {
    SLIC_ASSERT( m_state == EXTERNAL );

    // Get the undescribed (opaque) external pointer value out of node.  We are
    // going to set the pointer again in the node, this time using
    // set_external(), with a schema, so conduit recognizes it as a
    // pointer.
    data_pointer = (void *)m_node.as_uint64();
  }

  m_node.set_external(m_schema, data_pointer);
  m_is_applied = true;

  return this;
}

/*
 *************************************************************************
 *
 * Apply given # elems, offset, stride description to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(SidreLength num_elems,
                           SidreLength offset,
                           SidreLength stride)
{
  if ( num_elems < 0 || offset < 0 )
  {
    SLIC_CHECK(num_elems >= 0);
    SLIC_CHECK(offset >= 0);

    return this;
  }

  DataType dtype(m_schema.dtype());
  if ( dtype.is_empty() )
  {
    dtype = conduit::DataType::default_dtype(m_data_buffer->getTypeID());
  }

  dtype.set_number_of_elements(num_elems);
  dtype.set_offset(offset * dtype.element_bytes() );
  dtype.set_stride(stride * dtype.element_bytes() );

  declare(dtype);

  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply given type, # elems, offset, stride desscription to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(TypeID type, SidreLength num_elems,
                           SidreLength offset,
                           SidreLength stride)
{
  if ( num_elems < 0 || offset < 0)
  {
    SLIC_CHECK(num_elems >= 0);
    SLIC_CHECK(offset >= 0);

    return this;
  }

  DataType dtype = conduit::DataType::default_dtype(type);

  size_t bytes_per_elem = dtype.element_bytes();

  dtype.set_number_of_elements(num_elems);
  dtype.set_offset(offset * bytes_per_elem);
  dtype.set_stride(stride * bytes_per_elem);

  declare(dtype);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply given type, number of dimensions and shape to data view.
 * If ndims is 1 then do not save in m_shape.
 *
 *************************************************************************
 */
DataView * DataView::apply(TypeID type, int ndims, SidreLength * shape)
{
  if ( ndims < 1 || shape == ATK_NULLPTR )
  {
    SLIC_CHECK(ndims >= 1);
    SLIC_CHECK(shape != ATK_NULLPTR);

    return this;
  }

  declare(type, ndims, shape);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply a data type description to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(const DataType &dtype)
{
  declare(dtype);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply a Conduit Schema to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(const Schema& schema)
{
  declare(schema);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Set data view to hold external data.
 *
 *************************************************************************
 */
DataView * DataView::setExternalDataPtr(void * external_ptr)
{
  SLIC_ASSERT_MSG( isSetExternalDataPtrValid(),
                   "View state " << getStateStringName(
                     m_state) <<
      " does not allow setting external data pointer");

  if ( isSetExternalDataPtrValid() )
  {
    // todo, conduit should provide a check for if uint64 is a
    // good enough type to rep void *

    // TODO - Store the pointer in conduit node.
    m_node.set( (detail::sidre_uint64)external_ptr);
    m_state = EXTERNAL;

    if ( isDescribed() )
    {
      apply();
    }
  }

  return this;
}

/*
 *************************************************************************
 *
 * Return true if view contains a buffer with allocated data.
 *
 * Note: Most of our isXXX functions are implemented in the header.
 * This one is in not, because we are only forward declaring the buffer
 * class in the view header.
 *************************************************************************
 */
bool DataView::isAllocated() const
{
  return ( (m_data_buffer != ATK_NULLPTR) && (m_data_buffer->isAllocated() ) );
}


/*
 *************************************************************************
 *
 * Return number of dimensions and fill in shape information.
 *
 *************************************************************************
 */
int DataView::getShape(int ndims, SidreLength * shape) const
{
  if (m_shape == ATK_NULLPTR)
  {
    if (ndims > 0)
    {
      shape[0] = getNumElements();
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if (static_cast<unsigned>(ndims) < m_shape->size())
    {
      return -1;
    }
    else
    {
#if 0
      for(std::vector<SidreLength>::iterator it = v.begin() ; it != v.end() ;
          ++it)
      {
        *shape++ = it.
      }
#else
      for(std::vector<SidreLength>::size_type i = 0 ; i != m_shape->size() ;
          ++i)
      {
        shape[i] = (*m_shape)[i];
      }
#endif
    }
    return m_shape->size();
  }
}

/*
 *************************************************************************
 *
 * Print JSON description of data view to stdout.
 *
 *************************************************************************
 */
void DataView::print() const
{
  print(std::cout);
}

/*
 *************************************************************************
 *
 * Print JSON description of data view to an  ostream.
 *
 *************************************************************************
 */
void DataView::print(std::ostream& os) const
{
  Node n;
  info(n);
  n.to_json_stream(os);
}

/*
 *************************************************************************
 *
 * Copy data view description to given Conduit node.
 *
 *************************************************************************
 */
void DataView::info(Node &n) const
{
  n["name"] = m_name;
  n["schema"] = m_schema.to_json();
  n["node"] = m_node.to_json();
  n["state"] = getStateStringName(m_state);
  n["is_applied"] = m_is_applied;
}

/*
 *************************************************************************
 *
 * PRIVATE ctor for DataView not associated with any data.
 *
 *************************************************************************
 */
DataView::DataView( const std::string& name,
                    DataGroup * const owning_group)
  :   m_name(name),
  m_owning_group(owning_group),
  m_data_buffer(ATK_NULLPTR),
  m_schema(),
  m_node(),
  m_shape(ATK_NULLPTR),
  m_state(EMPTY),
  m_is_applied(false)
{}

/*
 *************************************************************************
 *
 * PRIVATE dtor.
 *
 *************************************************************************
 */
DataView::~DataView()
{
  if (m_data_buffer != ATK_NULLPTR)
  {
    m_data_buffer->detachView(this);
  }
  if (m_shape != ATK_NULLPTR)
  {
    delete m_shape;
  }
}

/*
 *************************************************************************
 *
 * PRIVATE method to declare data view with type and number of elements.
 *
 *************************************************************************
 */
DataView * DataView::declare(TypeID type, SidreLength num_elems)
{
  SLIC_ASSERT_MSG(num_elems >= 0, "Must give number of elements >= 0");

  if ( num_elems >= 0 )
  {
    DataType dtype = conduit::DataType::default_dtype(type);
    dtype.set_number_of_elements(num_elems);
    m_schema.set(dtype);

    if ( m_state == EMPTY )
    {
      m_state = DESCRIBED;
    }

    m_is_applied = false;
  }
  return this;
}

/*
 *************************************************************************
 *
 * PRIVATE method to declare data view with type, number of dimensions,
 *         and number of elements per dimension.
 *         Only use m_shape if ndims > 1.
 *
 *************************************************************************
 */
DataView * DataView::declare(TypeID type, int ndims, SidreLength * shape)
{
  SLIC_ASSERT(ndims >= 0);
  SLIC_ASSERT(shape != ATK_NULLPTR);
  SidreLength num_elems = 0;

  if ( ndims > 0 && shape != ATK_NULLPTR)
  {
    if (ndims == 1)
    {
      if (m_shape != ATK_NULLPTR)
      {
        delete m_shape;
        m_shape = ATK_NULLPTR;
      }
    }
    else
    {
      if (m_shape != ATK_NULLPTR)
      {
        m_shape->resize(ndims);
      }
      else
      {
        m_shape = new std::vector<SidreLength>(ndims);
      }
    }

    num_elems = 1;
    for (int i=0 ; i < ndims ; i++)
    {
      num_elems *= shape[i];
      if (m_shape != ATK_NULLPTR)
      {
        (*m_shape)[i] = shape[i];
      }
    }
  }
  else
  {
    delete m_shape;
    m_shape = ATK_NULLPTR;
  }
  declare(type, num_elems);
  return this;
}

/*
 *************************************************************************
 *
 * PRIVATE method to declare data view with a Conduit data type object.
 *
 *************************************************************************
 */
DataView * DataView::declare(const DataType& dtype)
{
  m_schema.set(dtype);

  if ( m_state == EMPTY )
  {
    m_state = DESCRIBED;
  }

  m_is_applied = false;

  return this;
}

/*
 *************************************************************************
 *
 * PRIVATE method to declare data view with a Conduit schema object.
 *
 *************************************************************************
 */
DataView * DataView::declare(const Schema& schema)
{
  m_schema.set(schema);

  if ( m_state == EMPTY )
  {
    m_state = DESCRIBED;
  }

  m_is_applied = false;

  return this;
}

/*
 *************************************************************************
 *
 * PRIVATE method returns true if view can allocate data; else false.
 *
 *************************************************************************
 */
bool DataView::isAllocateValid() const
{
  if ( !isDescribed() || m_state == EXTERNAL || m_state == SCALAR ||
       m_state == STRING ||
       (m_data_buffer != ATK_NULLPTR && m_data_buffer->getNumViews() != 1) )
  {
    SLIC_CHECK_MSG(
      isDescribed(),
      "Allocate is not valid, view has no data description.");
    SLIC_CHECK_MSG( m_state != EXTERNAL,
                    "Allocate is not valid, view contains an external data pointer.");
    SLIC_CHECK_MSG( m_state != SCALAR,
                    "Allocate is not valid, view contains a scalar.");
    SLIC_CHECK_MSG( m_state != STRING,
                    "Allocate is not valid, view contains a string.");
    SLIC_CHECK_MSG(
      m_data_buffer != ATK_NULLPTR && m_data_buffer->getNumViews() != 1, ""
                                                                         "Allocate is not valid, buffer does not contain exactly one view.");

    return false;
  }

  return true;
}

/*
 *************************************************************************
 *
 * PRIVATE method returns true if attaching buffer to view is valid;
 * else false.
 *
 *************************************************************************
 */
bool DataView::isAttachBufferValid() const
{
  return ( m_state == EMPTY || m_state == DESCRIBED );
}

/*
 *************************************************************************
 *
 * PRIVATE method returns true if setting external data pointer on view
 * is valid; else false.
 *
 *************************************************************************
 */
bool DataView::isSetExternalDataPtrValid() const
{
  return ( m_state == EMPTY || m_state == DESCRIBED || m_state == EXTERNAL );
}

/*
 *************************************************************************
 *
 * PRIVATE method returns true if apply is a valid operation on view;
 * else false.
 *
 *************************************************************************
 */
bool DataView::isApplyValid() const
{
  SLIC_CHECK_MSG(
    isDescribed(), "Apply not valid, no description in view to apply.");
  SLIC_CHECK_MSG(
    !hasBuffer() && m_state != EXTERNAL,
    "Apply not valid, no buffer or external data to apply description to.");
  bool success = ( isDescribed() && ( hasBuffer() || m_state == EXTERNAL ) );

  if (hasBuffer() && m_data_buffer->isDescribed() )
  {
    SLIC_CHECK_MSG(
      getTotalBytes() <= m_data_buffer->getTotalBytes(),
      "Apply not valid, # of bytes required by view description exceeds bytes in buffer.");
    success = success && ( getTotalBytes() <= m_data_buffer->getTotalBytes() );
  }

  return success;
}

/*
 *************************************************************************
 *
 * PRIVATE method returns string name of given view state enum value.
 *
 *************************************************************************
 */
char const * DataView::getStateStringName(State state) const
{
  char const * ret_string = NULL;

  switch ( state )
  {
  case EMPTY:
  {
    ret_string = "EMPTY";
    break;
  }

  case DESCRIBED:
  {
    ret_string = "DESCRIBED";
    break;
  }

  case ALLOCATED:
  {
    ret_string = "ALLOCATED";
    break;
  }

  case BUFFER_ATTACHED:
  {
    ret_string = "BUFFER_ATTACHED";
    break;
  }

  case EXTERNAL:
  {
    ret_string = "EXTERNAL";
    break;
  }

  case SCALAR:
  {
    ret_string = "SCALAR";
    break;
  }

  case STRING:
  {
    ret_string = "STRING";
    break;
  }

  default:
  {
    ret_string = "/0";
  }
  }

  return( ret_string );
}


} /* end namespace sidre */
} /* end namespace asctoolkit */
